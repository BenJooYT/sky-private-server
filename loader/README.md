# Loader build — `winhttp.dll`

This directory contains everything needed to reproduce the loader binary that
`SkySwitch` installs. It is a patch on top of
[HT's Mod Loader (HTML-Sky)](https://github.com/HTMonkeyG/HTML-Sky) plus a small
patch to [leveldb-mcpe](https://github.com/extremeheat/leveldb-mcpe).

| File | Purpose |
|---|---|
| `patches/0001-loader-account-redirect.patch` | Adds the account redirect, the in-game settings panel, and wiring in `options.cpp` / `menu.cpp` / `setup.cpp`. |
| `patches/0002-leveldb-mingw-env_win.patch` | Makes leveldb's Windows backend compile under MinGW. |

## Prerequisites

- **MinGW-w64 15.1.0 with the win32 threading model, UCRT** — e.g.
  [niXman/mingw-builds-binaries](https://github.com/niXman/mingw-builds-binaries):
  `x86_64-15.1.0-release-win32-seh-ucrt-rt_v12-rev0.7z`
- **CMake**
- **git**
- zlib headers (only used to compile leveldb — the library it links is vendored)

> **The threading model is not optional.** A posix-threads MinGW build of this
> loader *deadlocks Sky at startup* — window opens, then zero CPU and every
> thread waiting. It builds and links fine, so the failure is easy to miss.
> Use the win32-threads toolchain.
>
> The prebuilt `libraries/leveldb/lib/libleveldb.a` shipped by upstream cannot be
> linked with MinGW at all: it references the MSVC-only import
> `__imp__snprintf_s`. That is why leveldb is rebuilt below.

## 1. Apply the loader patch

```bash
git clone https://github.com/HTMonkeyG/HTML-Sky.git
cd HTML-Sky
git checkout ccccbd5
git apply /path/to/sky-private-server/loader/patches/0001-loader-account-redirect.patch
```

Files touched:

- `src/redirect.cpp` *(new)* — hooks, URI rewriting, settings UI
- `src/htinternal.hpp` — `HTiRedirectSettings` + declarations
- `src/options.cpp` — load/save the `redirect` block in `options.json`
- `src/ui/menu.cpp` — **Settings → Server Redirect**
- `src/setup.cpp` — install the hooks during setup

## 2. Rebuild leveldb (with the same win32 toolchain)

```bash
git clone https://github.com/extremeheat/leveldb-mcpe.git
git clone https://github.com/madler/zlib.git          # headers only
cd leveldb-mcpe
git checkout 1bcc6ae
git apply /path/to/sky-private-server/loader/patches/0002-leveldb-mingw-env_win.patch

cmake -S . -B build-mingw -G "MinGW Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF \
  -DLEVELDB_BUILD_TESTS=OFF \
  -DLEVELDB_BUILD_BENCHMARKS=OFF \
  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
  -DZLIB_INCLUDE_DIR=/path/to/zlib \
  -DZLIB_LIBRARY=/path/to/HTML-Sky/libraries/leveldb/lib/libz.a

cmake --build build-mingw --parallel
cp build-mingw/libleveldb.a /path/to/HTML-Sky/libraries/leveldb/lib/libleveldb.a
```

## 3. Build the loader

```bash
cd HTML-Sky
mingw32-make all
# -> dist/winhttp.dll
```

## 4. Feed it to the installer

```bash
cp dist/winhttp.dll /path/to/sky-private-server/installer/payload/winhttp.dll
dotnet publish /path/to/sky-private-server/installer/SkySwitch.csproj \
  -c Release -r win-x64 --self-contained false \
  -p:PublishSingleFile=true -o /path/to/sky-private-server/dist
```

## How the redirect works

`src/redirect.cpp` installs two hooks with the loader's own signature scanner:

- `HttpClient::Request` — receives a pointer to
  `Net::Uri { char protocol[10]; char domain[128]; char port[6]; char path[256]; }`
  and rewrites `domain`/`port` when `domain` equals `account_from`.
- `AccountServerWebSocket::Connect` — receives the `/account/ws` URL as a string
  and rewrites host/port the same way.

Settings are stored in `htmodloader/options.json` under `"redirect"` and are
read on every request, so the in-game toggle and address edits apply immediately.
Default state is **disabled**.

> The hooks are installed with **MinHook directly**, not with
> `HTAsmHookCreateRaw()`. That API validates its owner handle with
> `HTiCheckHandleType(owner, HTHandleType_Mod)` and returns `HT_FAIL` for the
> loader's own module, since it is not a registered *mod*.

### Which host to rewrite

The account host is **`live.radiance.thatgamecompany.com`** (the CDN is
`live.sky.resources.cdn.thatgamecompany.com` and must be left alone). To confirm
on a new game build, the patch writes what it sees to
`<game>\htmodloader\redirect-debug.log` (`req: https://host/path` lines) on the
first ~60 requests — copy the account host from there into `account_from`.

### Signatures

The byte patterns in the patch were captured on **Sky 0.35.1**. After a game
update they may stop matching — `HTSigScan` then returns null and the loader
logs that the hooks could not be installed. Update the two `HTAsmSig`
definitions at the top of `src/redirect.cpp` against the new build.
