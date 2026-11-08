# Loader — `winhttp.dll`

The loader is [HT's Mod Loader (HTML-Sky)](https://github.com/HTMonkeyG/HTML-Sky)
with an account-redirect patch. The **complete modified source is vendored in
[`html-sky/`](html-sky)** — no external clone is needed to build this repo.

| Path | Purpose |
|---|---|
| `html-sky/` | Vendored HTML-Sky source (upstream commit `ccccbd5`) plus the redirect changes |
| `patches/0001-loader-account-redirect.patch` | The same changes as a diff, for reference / rebasing onto a newer upstream |
| `patches/0002-leveldb-mingw-env_win.patch` | Only needed if you rebuild leveldb |

## Files changed vs upstream

- `src/redirect.cpp` *(new)* — hooks, URI rewriting, settings UI, debug log
- `src/htinternal.hpp` — `HTiRedirectSettings` + declarations
- `src/options.cpp` — load/save the `redirect` block in `options.json`
- `src/ui/menu.cpp` — **Settings → Server Redirect**
- `src/setup.cpp` — install the hooks during setup

## Build

Prerequisites: **MinGW-w64 15.1.0, win32 threading model, UCRT** — e.g.
[niXman/mingw-builds-binaries](https://github.com/niXman/mingw-builds-binaries)
`x86_64-15.1.0-release-win32-seh-ucrt-rt_v12-rev0.7z`.

```bash
cd loader/html-sky
mingw32-make all
# -> dist/winhttp.dll
```

The leveldb archive (`libraries/leveldb/lib/libleveldb.a`), zlib and the Vulkan
import library are already in the tree.

> **Do not build with a posix-threads MinGW.** It links cleanly but deadlocks
> Sky right after the window appears — zero CPU, every thread waiting.
>
> The Makefile does not track `.hpp` dependencies. After editing a header run
> `mingw32-make clean` first, or you will get a stale binary.

Then update the installer payload and rebuild it:

```bash
cp dist/winhttp.dll ../../installer/payload/winhttp.dll
dotnet publish ../../installer/SkySwitch.csproj -c Release -r win-x64 \
  --self-contained false -p:PublishSingleFile=true -o ../../dist
```

## How the redirect works

`src/redirect.cpp` installs two hooks with the loader's own signature scanner:

- `HttpClient::Request` — receives a pointer to
  `Net::Uri { char protocol[10]; char domain[128]; char port[6]; char path[256]; }`
  and rewrites `domain`/`port` when `domain` equals `account_from`.
- `AccountServerWebSocket::Connect` — receives the `/account/ws` URL as a string
  and rewrites host/port when the host equals `ws_from`.

The two hosts are **different**:

| Channel | `from` key | Default value |
|---|---|---|
| REST | `account_from` | `live.radiance.thatgamecompany.com` |
| WebSocket | `ws_from` | `live.ws.radiance.thatgamecompany.com` |

Settings are stored in `htmodloader/options.json` under `"redirect"` and are read
on every request, so the in-game toggle and address edits apply immediately.
Default state is **disabled**.

> The hooks are installed with **MinHook directly**, not with
> `HTAsmHookCreateRaw()`. That API validates its owner handle with
> `HTiCheckHandleType(owner, HTHandleType_Mod)` and returns `HT_FAIL` for the
> loader's own module, since it is not a registered *mod*.

### Which host to rewrite

The hosts to rewrite are **`live.radiance.thatgamecompany.com`** (REST) and
**`live.ws.radiance.thatgamecompany.com`** (WebSocket); the CDN
(`live.sky.resources.cdn.thatgamecompany.com`) must be left alone. To confirm on
a new game build, the patch writes what it sees to
`<game>\htmodloader\redirect-debug.log` (`req: https://host/path` lines) for the
first ~60 requests — copy the REST host into `account_from` and the WS host into
`ws_from`.

### Signatures

The byte patterns in `src/redirect.cpp` were captured on **Sky 0.35.1**. After a
game update they may stop matching; `HTSigScan` then returns null, the loader
logs that, and the game simply keeps using the official servers. Update the two
`HTAsmSig` definitions against the new build.

## leveldb

`libraries/leveldb/lib/libleveldb.a` is built from
[leveldb-mcpe](https://github.com/extremeheat/leveldb-mcpe) commit `1bcc6ae`
with `patches/0002-leveldb-mingw-env_win.patch`, using the same win32 toolchain.
Upstream's prebuilt archive cannot be linked with MinGW at all — it references
the MSVC-only import `__imp__snprintf_s`.

To rebuild it:

```bash
git clone https://github.com/extremeheat/leveldb-mcpe && cd leveldb-mcpe
git checkout 1bcc6ae
git apply /path/to/loader/patches/0002-leveldb-mingw-env_win.patch
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF -DLEVELDB_BUILD_TESTS=OFF \
  -DLEVELDB_BUILD_BENCHMARKS=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  -DCMAKE_C_COMPILER=<mingw>/bin/gcc.exe -DCMAKE_CXX_COMPILER=<mingw>/bin/g++.exe \
  -DZLIB_INCLUDE_DIR=/path/to/zlib \
  -DZLIB_LIBRARY=/path/to/loader/html-sky/libraries/leveldb/lib/libz.a
cmake --build build --parallel
cp build/libleveldb.a /path/to/loader/html-sky/libraries/leveldb/lib/
```
