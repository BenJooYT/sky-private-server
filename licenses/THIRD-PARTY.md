# Third-party components

This repository redistributes or derives from the following projects.

| Component | Where | License | Copyright |
|---|---|---|---|
| HTML-Sky / HT's Mod Loader | `installer/payload/winhttp.dll`, `loader/` | MIT (`HTML-Sky-MIT.txt`) | (c) 2025 HTMonkeyG |
| sslpatch | account-redirect logic in `loader/patches/0001-*.patch` | AGPL-3.0 (`AGPL-3.0.txt`) | Thexiaoyu / that-sky-project |
| LevelDB / leveldb-mcpe | storage library linked into the loader | BSD-3-Clause (`LevelDB-BSD.txt`) | (c) 2011 The LevelDB Authors |
| zlib | compression linked into the loader | zlib (`zlib.txt`) | Jean-loup Gailly, Mark Adler |

## Licensing consequence

The account-redirect implementation is derived from sslpatch, which is licensed
under AGPL-3.0. Combined with the MIT-licensed HTML-Sky loader, the resulting
loader — and therefore `installer/payload/winhttp.dll` as distributed here — is
licensed under **AGPL-3.0**.

To satisfy the AGPL's source requirement, this repository ships:

- the complete modifications (`loader/patches/0001-loader-account-redirect.patch`),
- the exact upstream commit they apply to (`ccccbd5`, HTMonkeyG/HTML-Sky),
- the build steps needed to reproduce the binary (`loader/README.md`).

If you redistribute a built `winhttp.dll`, keep this repository (or equivalent
source) available to whoever receives the binary.
