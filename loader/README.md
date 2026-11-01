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

- **MinGW-w64 15.1.0, posix threads, UCRT** — e.g. a WinLibs build:
  <https://github.com/brechtsanders/winlibs_mingw/releases>
  (`winlibs-x86_64-posix-seh-gcc-15.1.0-mingw-w64ucrt-*.zip`)
- **CMake** (the one bundled with WinLibs works)
- **git**
- zlib headers (only used to compile leveldb — the library it links is vendored)

> The threading model matters. Upstream ships `libleveldb.a` built with a
> **win32-threads** toolchain; linking it with a posix-threads build fails with
> `undefined reference to __gthr_win32_mutex_*`. That is why leveldb is rebuilt
> below instead of using the archive in the repository.

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

## 2. Rebuild leveldb

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

### Signatures

The byte patterns in the patch were captured on **Sky 0.35.1**. After a game
update they may stop matching — `HTSigScan` then returns null and the loader
logs that the hooks could not be installed. Update the two `HTAsmSig`
definitions at the top of `src/redirect.cpp` against the new build.
