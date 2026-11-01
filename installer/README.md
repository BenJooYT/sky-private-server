# SkySwitch (installer)

A small WinForms app that installs/removes the modified loader in your Sky
folder and keeps the server address in one place.

## Build

```powershell
dotnet publish SkySwitch.csproj -c Release -r win-x64 `
  --self-contained false -p:PublishSingleFile=true -o ../dist
```

Output: `dist/SkySwitch.exe` (~2.7 MB, single file).

The two files in `payload/` are embedded into the executable and written out at
install time, so the shipped `.exe` is self-contained apart from the
[.NET 8 Desktop Runtime](https://dotnet.microsoft.com/download/dotnet/8.0).

## What Install / Remove do

**Install**

1. Reads the Sky install path from
   `HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 2325290`.
2. Backs up any existing `winhttp.dll`, `html-config.json` and
   `htmodloader\options.json` to `*.skyswitch.bak`.
3. Writes `winhttp.dll` and `html-config.json` next to `Sky.exe`.
4. Merges a `redirect` block into `htmodloader\options.json` (leaving any other
   keys untouched).
5. Records what it did in `<game>\.skyswitch-state.json`.

**Remove** deletes only the files listed in that state file, restores the
backups, and removes the folders it created if they are empty.

Re-running Install when already installed just updates the address; it never
creates a second set of backups.

## Settings

- GUI settings (last game path / server) → `%LOCALAPPDATA%\SkySwitch\gui.json`
- Redirect settings (used by the loader) → `<game>\htmodloader\options.json`

```json
{
  "redirect": {
    "enabled": true,
    "force_http": false,
    "account_from": "live.radiance.thatgamecompany.com",
    "host": "my-server.example.com:9000"
  }
}
```

`host` is `host` or `host:port`, never a URL — a pasted `https://` prefix and
trailing slashes are stripped by the GUI.
