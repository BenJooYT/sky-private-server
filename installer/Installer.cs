using System.Reflection;
using System.Text.Json;
using System.Text.Json.Nodes;
using Microsoft.Win32;

namespace SkySwitch;

internal static class Installer
{
    public const string OptionsRelPath = "htmodloader/options.json";
    public const string StateFileName = ".skyswitch-state.json";
    public const string DefaultAccountHost = "skynet.radiance.thatgamecompany.com";
    private const string SteamAppId = "2325290";

    private static readonly JsonSerializerOptions Json = new() { WriteIndented = true };

    public sealed class Status
    {
        public bool Installed { get; init; }
        public bool Enabled { get; init; }
        public string Host { get; init; } = "";
    }

    public static bool LooksLikeGame(string? dir) =>
        !string.IsNullOrWhiteSpace(dir) &&
        Directory.Exists(dir) &&
        File.Exists(Path.Combine(dir, "Sky.exe"));

    // Read the install location Steam records for Sky.
    public static string? DetectGameFolder()
    {
        try
        {
            using RegistryKey? key = Registry.LocalMachine.OpenSubKey(
                $@"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App {SteamAppId}");
            if (key?.GetValue("InstallLocation") is string path && LooksLikeGame(path))
                return path;
        }
        catch { }
        return null;
    }

    public static Status GetStatus(string game)
    {
        string statePath = Path.Combine(game, StateFileName);
        string optionsPath = Path.Combine(game, OptionsRelPath.Replace('/', Path.DirectorySeparatorChar));
        bool installed = File.Exists(statePath);

        bool enabled = false;
        string host = "";
        if (installed && File.Exists(optionsPath))
        {
            JsonObject root = ReadOptions(optionsPath);
            if (root["redirect"] is JsonObject redirect)
            {
                enabled = redirect["enabled"]?.GetValue<bool>() ?? false;
                host = redirect["host"]?.GetValue<string>() ?? "";
            }
        }

        return new Status { Installed = installed, Enabled = enabled, Host = host };
    }

    public static void Install(string game, string host, bool forceHttp, Action<string> log)
    {
        string statePath = Path.Combine(game, StateFileName);

        if (File.Exists(statePath))
        {
            WriteRedirect(game, enabled: true, host, forceHttp);
            log("Updated the server address.");
            log("Start Sky, then use Settings -> Server Redirect if you want to change it.");
            return;
        }

        var state = new InstallState
        {
            InstalledAt = DateTimeOffset.Now.ToString("o"),
            GamePath = game
        };

        try
        {
            Backup(game, "winhttp.dll", state, log);
            Backup(game, "html-config.json", state, log);
            Backup(game, OptionsRelPath, state, log);

            WriteResource("payload.winhttp.dll", Path.Combine(game, "winhttp.dll"));
            WriteResource("payload.html-config.json", Path.Combine(game, "html-config.json"));
            state.Files.Add("winhttp.dll");
            state.Files.Add("html-config.json");

            WriteRedirect(game, enabled: true, host, forceHttp);
            state.Files.Add(OptionsRelPath);
            state.Dirs.Add("htmodloader");

            File.WriteAllText(statePath, JsonSerializer.Serialize(state, Json));

            log("Installed.");
            log($"Server set to: {(string.IsNullOrWhiteSpace(host) ? "(none yet)" : host)}");
            log("Start Sky from Steam to use it.");
        }
        catch
        {
            Rollback(game, state, log);
            throw;
        }
    }

    public static void Uninstall(string game, Action<string> log)
    {
        string statePath = Path.Combine(game, StateFileName);
        if (!File.Exists(statePath))
        {
            log("Nothing to remove - it is not installed.");
            return;
        }

        InstallState? state;
        try { state = JsonSerializer.Deserialize<InstallState>(File.ReadAllText(statePath)); }
        catch (Exception ex) { throw new InvalidOperationException($"Could not read the install record: {ex.Message}"); }
        if (state is null) throw new InvalidOperationException("The install record is empty.");

        Rollback(game, state, log);
        log("Removed. Sky will start normally again.");
    }

    public static void SetEnabled(string game, bool enabled, Action<string> log)
    {
        string path = Path.Combine(game, OptionsRelPath.Replace('/', Path.DirectorySeparatorChar));
        if (!File.Exists(path)) throw new InvalidOperationException("Not installed.");

        JsonObject root = ReadOptions(path);
        JsonObject redirect = root["redirect"] as JsonObject ?? new JsonObject();
        redirect["enabled"] = enabled;
        root["redirect"] = redirect;
        File.WriteAllText(path, root.ToJsonString(Json));

        log(enabled ? "Redirect turned on." : "Redirect turned off - using the official servers.");
    }

    private static void Rollback(string game, InstallState state, Action<string> log)
    {
        foreach (string rel in state.Files.AsEnumerable().Reverse())
            TryDelete(Path.Combine(game, rel.Replace('/', Path.DirectorySeparatorChar)), log);

        foreach (BackupEntry backup in state.Backups)
        {
            string target = Path.Combine(game, backup.Path.Replace('/', Path.DirectorySeparatorChar));
            string source = Path.Combine(game, backup.Backup.Replace('/', Path.DirectorySeparatorChar));
            try
            {
                if (File.Exists(source))
                {
                    File.Move(source, target, true);
                    log($"  restored {backup.Path}");
                }
            }
            catch (Exception ex) { log($"  could not restore {backup.Path}: {ex.Message}"); }
        }

        foreach (string dir in state.Dirs.OrderByDescending(d => d.Count(c => c == '/')))
        {
            string full = Path.Combine(game, dir.Replace('/', Path.DirectorySeparatorChar));
            try
            {
                if (Directory.Exists(full) && !Directory.EnumerateFileSystemEntries(full).Any())
                    Directory.Delete(full);
            }
            catch (Exception ex) { log($"  could not remove {dir}: {ex.Message}"); }
        }

        TryDelete(Path.Combine(game, StateFileName), log);
    }

    private static JsonObject ReadOptions(string path)
    {
        try
        {
            JsonNode? node = JsonNode.Parse(File.ReadAllText(path));
            if (node is JsonObject obj) return obj;
        }
        catch { }
        return new JsonObject();
    }

    // Merge the redirect block into htmodloader/options.json without touching
    // any other keys the loader keeps there.
    private static void WriteRedirect(string game, bool enabled, string host, bool forceHttp)
    {
        string path = Path.Combine(game, OptionsRelPath.Replace('/', Path.DirectorySeparatorChar));
        JsonObject root = File.Exists(path) ? ReadOptions(path) : new JsonObject();

        root["redirect"] = new JsonObject
        {
            ["enabled"] = enabled,
            ["force_http"] = forceHttp,
            ["account_from"] = DefaultAccountHost,
            ["host"] = host
        };

        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        File.WriteAllText(path, root.ToJsonString(Json));
    }

    private static void Backup(string game, string relative, InstallState state, Action<string> log)
    {
        string target = Path.Combine(game, relative.Replace('/', Path.DirectorySeparatorChar));
        if (!File.Exists(target)) return;

        string candidate = target + ".skyswitch.bak";
        if (File.Exists(candidate)) candidate = target + ".skyswitch.bak." + DateTime.Now.ToString("yyyyMMddHHmmss");

        File.Move(target, candidate);
        log($"  backed up existing {relative}");
        state.Backups.Add(new BackupEntry { Path = relative, Backup = Path.GetFileName(candidate) });
    }

    private static void WriteResource(string name, string destination)
    {
        using Stream? src = Assembly.GetExecutingAssembly().GetManifestResourceStream(name);
        if (src is null) throw new InvalidOperationException($"Missing embedded file {name}");
        string? dir = Path.GetDirectoryName(destination);
        if (dir is not null) Directory.CreateDirectory(dir);
        using FileStream dst = File.Create(destination);
        src.CopyTo(dst);
    }

    private static void TryDelete(string path, Action<string> log)
    {
        try { if (File.Exists(path)) File.Delete(path); }
        catch (Exception ex) { log($"  could not delete {Path.GetFileName(path)}: {ex.Message}"); }
    }

    private sealed class InstallState
    {
        public string InstalledAt { get; set; } = "";
        public string GamePath { get; set; } = "";
        public List<string> Files { get; set; } = new();
        public List<string> Dirs { get; set; } = new();
        public List<BackupEntry> Backups { get; set; } = new();
    }

    private sealed class BackupEntry
    {
        public string Path { get; set; } = "";
        public string Backup { get; set; } = "";
    }
}
