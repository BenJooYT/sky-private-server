// ----------------------------------------------------------------------------
// Account-server redirect, built into HT's Mod Loader.
//
// Rewrites the account HTTP + WebSocket destination so the game talks to a
// private server. Settings live in options.json and are edited from the loader
// menu (Settings -> Server Redirect). Default: disabled.
// ----------------------------------------------------------------------------
#include <windows.h>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>

#include "htinternal.hpp"
#include "htmodloader/htmodloader.h"
#include "MinHook.h"
#include "imgui.h"

namespace {

// Append a line to <game>\htmodloader\redirect-debug.log. Used to find where
// startup blocks if the game hangs.
void tracef(const char *fmt, ...) {
  char path[MAX_PATH];
  snprintf(path, sizeof(path), "%s\\redirect-debug.log", gPathData);
  FILE *f = fopen(path, "a");
  if (!f) return;
  va_list ap;
  va_start(ap, fmt);
  vfprintf(f, fmt, ap);
  va_end(ap);
  fputc('\n', f);
  fflush(f);
  fclose(f);
}

// Layout of the game's Net::Uri (mirrors ssl-patch):
//   protocol[10] +0, domain[128] +10, port[6] +138, path[256] +144.
struct NetUri {
  char protocol[10];
  char domain[128];
  char port[6];
  char path[256];
};
static_assert(sizeof(NetUri) == 400, "Net::Uri must be 400 bytes");
static_assert(offsetof(NetUri, domain) == 10, "domain offset");
static_assert(offsetof(NetUri, port) == 138, "port offset");
static_assert(offsetof(NetUri, path) == 144, "path offset");

// Signature of HttpClient::Request and the account WebSocket Connect.
const HTAsmSig kSigHttpRequest = {
  "4C 8D 8D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 85 ?? ?? ?? ?? 89 86",
  HTSigScanType_E8,
  0x07,
};
const HTAsmSig kSigWsConnect = {
  "55 41 57 41 56 56 57 53 48 81 EC B8 01 00 00 48 8D AC 24 80 00 00 00 "
  "4C 89 CE 4C 89 C7 48 89 D3 49 89 CE C5 F8 57 C0",
  HTSigScanType_Direct,
  0,
};

using RequestFn = void *(*)(void *thisptr, void *ret, NetUri *uri, void *args);
using WsConnectFn = char (*)(void *thisptr, const char *url, const char *host, void *args);

std::mutex gMutex;
HTiRedirectSettings gSettings;
RequestFn gOrigRequest = nullptr;
WsConnectFn gOrigWsConnect = nullptr;
unsigned long long gHttpHits = 0, gWsHits = 0;
bool gInstalled = false;

void setField(char *dst, size_t n, const std::string &s) {
  strncpy(dst, s.c_str(), n - 1);
  dst[n - 1] = '\0';
}

// Split "host[:port]" into parts. Returns true when a port was present.
bool splitTarget(const std::string &target, std::string &host, std::string &port) {
  const size_t colon = target.rfind(':');
  if (colon == std::string::npos) {
    host = target;
    port.clear();
    return false;
  }
  host = target.substr(0, colon);
  port = target.substr(colon + 1);
  return true;
}

// Rewrite a live Net::Uri when its domain matches the configured `from`.
void applyUri(NetUri *uri, const HTiRedirectSettings &s) {
  if (!uri) return;
  if (s.forceHttp) strcpy(uri->protocol, "http");

  const std::string domain(uri->domain);
  if (s.accountFrom.empty() || s.accountFrom != domain) return;

  std::string host, port;
  splitTarget(s.host, host, port);
  setField(uri->domain, sizeof(uri->domain), host);
  setField(uri->port, sizeof(uri->port), port);
}

// Rewrite an "https://host[:port]/path" string when the host matches.
bool applyUrl(const char *url, char *dst, size_t n, const HTiRedirectSettings &s) {
  if (!url || !dst || n == 0) return false;

  const char *schemeEnd = strstr(url, "://");
  if (!schemeEnd) {
    snprintf(dst, n, "%s", url);
    return false;
  }

  std::string scheme(url, schemeEnd);
  const char *hostBegin = schemeEnd + 3;
  const char *path = strchr(hostBegin, '/');
  std::string hostPort = path ? std::string(hostBegin, path) : std::string(hostBegin);
  if (!path) path = "";

  std::string host, port;
  const bool hasPort = splitTarget(hostPort, host, port);

  bool changed = false;
  if (s.forceHttp && scheme != "http") {
    scheme = "http";
    changed = true;
  }

  if (!s.wsFrom.empty() && s.wsFrom == host) {
    std::string toHost, toPort;
    const bool toHasPort = splitTarget(s.host, toHost, toPort);
    host = toHost;
    port = toHasPort ? toPort : std::string();
    changed = true;
  }

  (void)hasPort;
  snprintf(dst, n, "%s://%s%s%s%s", scheme.c_str(), host.c_str(),
           port.empty() ? "" : ":", port.c_str(), path);
  return changed;
}

void *detourRequest(void *thisptr, void *ret, NetUri *uri, void *args) {
  HTiRedirectSettings s;
  {
    std::lock_guard<std::mutex> lock(gMutex);
    s = gSettings;
  }
  if (uri && gHttpHits < 60)
    tracef("req: %s://%s:%s%s", uri->protocol, uri->domain, uri->port, uri->path);
  if (s.enabled) applyUri(uri, s);
  gHttpHits++;
  return gOrigRequest ? gOrigRequest(thisptr, ret, uri, args) : nullptr;
}

char detourWsConnect(void *thisptr, const char *url, const char *host, void *args) {
  tracef("ws: %s", url ? url : "");

  char rewritten[256];
  snprintf(rewritten, sizeof(rewritten), "%s", url ? url : "");

  HTiRedirectSettings s;
  {
    std::lock_guard<std::mutex> lock(gMutex);
    s = gSettings;
  }

  bool changed = false;
  if (s.enabled) changed = applyUrl(url, rewritten, sizeof(rewritten), s);
  tracef("ws -> %s (changed=%d)", rewritten, changed ? 1 : 0);

  char hostBuf[128];
  snprintf(hostBuf, sizeof(hostBuf), "%s", host ? host : "");
  if (changed) {
    const char *scheme = strstr(rewritten, "://");
    if (scheme) {
      scheme += 3;
      const char *end = strchr(scheme, '/');
      std::string hp = end ? std::string(scheme, end) : std::string(scheme);
      std::string h, p;
      splitTarget(hp, h, p);
      snprintf(hostBuf, sizeof(hostBuf), "%s", h.c_str());
    }
  }

  gWsHits++;
  return gOrigWsConnect ? gOrigWsConnect(thisptr, rewritten, hostBuf, args) : 0;
}

std::string trimmed(const char *text) {
  std::string v = text ? text : "";
  const char *ws = " \t\r\n";
  const size_t b = v.find_first_not_of(ws);
  if (b == std::string::npos) return {};
  const size_t e = v.find_last_not_of(ws);
  return v.substr(b, e - b + 1);
}

} // namespace

HTiRedirectSettings HTiRedirectGetSettings() {
  std::lock_guard<std::mutex> lock(gMutex);
  return gSettings;
}

void HTiRedirectSetSettings(const HTiRedirectSettings &settings) {
  std::lock_guard<std::mutex> lock(gMutex);
  gSettings = settings;
}

void HTiRedirectLoadFromJson(const cJSON *root) {
  if (!root) return;
  const cJSON *r = cJSON_GetObjectItemCaseSensitive(root, "redirect");
  if (!r) return;

  HTiRedirectSettings s = HTiRedirectGetSettings();
  const cJSON *v;

  if ((v = cJSON_GetObjectItemCaseSensitive(r, "enabled")) && cJSON_IsBool(v))
    s.enabled = cJSON_IsTrue(v) != 0;
  if ((v = cJSON_GetObjectItemCaseSensitive(r, "force_http")) && cJSON_IsBool(v))
    s.forceHttp = cJSON_IsTrue(v) != 0;
  if ((v = cJSON_GetObjectItemCaseSensitive(r, "account_from")) && cJSON_IsString(v))
    s.accountFrom = cJSON_GetStringValue(v);
  if ((v = cJSON_GetObjectItemCaseSensitive(r, "ws_from")) && cJSON_IsString(v))
    s.wsFrom = cJSON_GetStringValue(v);
  if ((v = cJSON_GetObjectItemCaseSensitive(r, "host")) && cJSON_IsString(v))
    s.host = cJSON_GetStringValue(v);

  HTiRedirectSetSettings(s);
  LOGI("Redirect: enabled=%d force_http=%d http:'%s' ws:'%s' -> '%s'\n",
       s.enabled ? 1 : 0, s.forceHttp ? 1 : 0, s.accountFrom.c_str(),
       s.wsFrom.c_str(), s.host.c_str());
}

void HTiRedirectSaveToJson(cJSON *root) {
  if (!root) return;
  const HTiRedirectSettings s = HTiRedirectGetSettings();

  cJSON *r = cJSON_CreateObject();
  cJSON_AddBoolToObject(r, "enabled", s.enabled);
  cJSON_AddBoolToObject(r, "force_http", s.forceHttp);
  cJSON_AddStringToObject(r, "account_from", s.accountFrom.c_str());
  cJSON_AddStringToObject(r, "ws_from", s.wsFrom.c_str());
  cJSON_AddStringToObject(r, "host", s.host.c_str());
  cJSON_AddItemToObject(root, "redirect", r);
}

void HTiRedirectInit() {
  if (gInstalled) return;
  gInstalled = true;

  tracef("init: start");
  void *fnRequest = HTSigScan(&kSigHttpRequest);
  tracef("init: http sig -> %p", fnRequest);
  void *fnWs = HTSigScan(&kSigWsConnect);
  tracef("init: ws sig -> %p", fnWs);

  if (fnRequest) {
    // HTAsmHookCreateRaw() only accepts registered *mod* handles, so the loader
    // installs its own hooks through MinHook directly.
    MH_STATUS created = MH_CreateHook(fnRequest, (LPVOID)&detourRequest,
                                      (LPVOID *)&gOrigRequest);
    tracef("init: http create -> %d", (int)created);
    if (created == MH_OK) {
      tracef("init: http enable -> %d", (int)MH_EnableHook(fnRequest));
      LOGI("Redirect: HttpClient::Request hooked at 0x%p\n", fnRequest);
    } else {
      LOGI("Redirect: could not hook HttpClient::Request\n");
    }
  } else {
    LOGI("Redirect: HttpClient::Request signature not found\n");
  }

  if (fnWs) {
    MH_STATUS created = MH_CreateHook(fnWs, (LPVOID)&detourWsConnect,
                                      (LPVOID *)&gOrigWsConnect);
    tracef("init: ws create -> %d", (int)created);
    if (created == MH_OK) {
      tracef("init: ws enable -> %d", (int)MH_EnableHook(fnWs));
      LOGI("Redirect: AccountServerWebSocket::Connect hooked at 0x%p\n", fnWs);
    } else {
      LOGI("Redirect: could not hook AccountServerWebSocket::Connect\n");
    }
  } else {
    LOGI("Redirect: AccountServerWebSocket::Connect signature not found\n");
  }

  tracef("init: done");
}

void HTiRedirectRenderSettings() {
  HTiRedirectSettings s = HTiRedirectGetSettings();
  static char hostBuf[192] = {0};
  static bool ready = false;
  bool changed = false;

  if (!ready) {
    snprintf(hostBuf, sizeof(hostBuf), "%s", s.host.c_str());
    ready = true;
  }

  if (ImGui::Checkbox("Enable account redirect", &s.enabled))
    changed = true;

  ImGui::SetNextItemWidth(300);
  bool commit = ImGui::InputText("Server host", hostBuf, sizeof(hostBuf),
                                 ImGuiInputTextFlags_EnterReturnsTrue);
  ImGui::SameLine();
  if (ImGui::Button("Save host")) commit = true;
  if (commit) {
    s.host = trimmed(hostBuf);
    snprintf(hostBuf, sizeof(hostBuf), "%s", s.host.c_str());
    changed = true;
  }

  if (ImGui::Checkbox("force_http (plain http/ws)", &s.forceHttp))
    changed = true;

  if (changed) {
    HTiRedirectSetSettings(s);
    HTiOptionsMarkDirty();
  }

  ImGui::TextDisabled("host or host:port, no scheme");
  ImGui::Text("http from: %s", s.accountFrom.c_str());
  ImGui::Text("ws   from: %s", s.wsFrom.c_str());
  ImGui::Text("to:        %s", s.host.empty() ? "(unset)" : s.host.c_str());
  ImGui::Text("hits:  %llu http / %llu ws", gHttpHits, gWsHits);
}
