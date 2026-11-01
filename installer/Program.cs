using System.Text.Json;

namespace SkySwitch;

internal static class Program
{
    [STAThread]
    private static void Main()
    {
        ApplicationConfiguration.Initialize();
        Application.Run(new MainForm());
    }
}

internal sealed class MainForm : Form
{
    private static readonly string SettingsPath = Path.Combine(
        Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "SkySwitch", "gui.json");

    private static readonly JsonSerializerOptions Json = new() { WriteIndented = true };

    private readonly TextBox _gameBox = new();
    private readonly Button _browse = new();
    private readonly TextBox _serverBox = new();
    private readonly CheckBox _plainHttp = new();
    private readonly Panel _statusPanel = new();
    private readonly Label _statusText = new();
    private readonly Button _install = new();
    private readonly Button _remove = new();
    private readonly TextBox _log = new();

    private sealed class Settings
    {
        public string? GamePath { get; set; }
        public string? Server { get; set; }
        public bool PlainHttp { get; set; }
    }

    public MainForm()
    {
        Text = "Sky Private Server";
        ClientSize = new Size(680, 648);
        FormBorderStyle = FormBorderStyle.FixedSingle;
        MaximizeBox = false;
        StartPosition = FormStartPosition.CenterScreen;
        Font = new Font("Segoe UI", 9f);
        AutoScaleMode = AutoScaleMode.Dpi;

        BuildUi();
        LoadSettings();
        RefreshStatus();
    }

    private void BuildUi()
    {
        var title = new Label
        {
            Text = "Play Sky on a private server",
            Font = new Font("Segoe UI", 16f, FontStyle.Bold),
            Location = new Point(24, 18),
            AutoSize = true
        };

        var subtitle = new Label
        {
            Text = "This tool connects your game to a server someone else runs.\n"
                 + "You can undo it at any time.",
            ForeColor = Color.DimGray,
            Location = new Point(26, 56),
            AutoSize = true
        };

        _statusPanel.Location = new Point(24, 102);
        _statusPanel.Size = new Size(632, 54);
        _statusPanel.BackColor = Color.FromArgb(242, 242, 242);
        _statusPanel.BorderStyle = BorderStyle.FixedSingle;

        _statusText.Location = new Point(14, 16);
        _statusText.AutoSize = true;
        _statusText.Font = new Font("Segoe UI", 10.5f, FontStyle.Bold);
        _statusPanel.Controls.Add(_statusText);

        var gameLabel = new Label { Text = "Game folder", Location = new Point(24, 176), AutoSize = true };

        _gameBox.Location = new Point(132, 172);
        _gameBox.Size = new Size(430, 25);
        _gameBox.ReadOnly = true;
        _gameBox.BackColor = SystemColors.Window;

        _browse.Text = "Change...";
        _browse.Location = new Point(568, 171);
        _browse.Size = new Size(88, 27);
        _browse.Click += Browse_Click;

        var serverLabel = new Label { Text = "Server address", Location = new Point(24, 216), AutoSize = true };

        _serverBox.Location = new Point(132, 212);
        _serverBox.Size = new Size(524, 25);
        _serverBox.MaxLength = 200;

        var serverHint = new Label
        {
            Text = "Ask the person hosting the server for this. Example:  my-server.example.com:9000",
            ForeColor = Color.DimGray,
            Location = new Point(134, 241),
            AutoSize = true
        };

        _plainHttp.Text = "My server does not use HTTPS (only if you were told so)";
        _plainHttp.Location = new Point(132, 268);
        _plainHttp.AutoSize = true;

        _install.Text = "Install";
        _install.Location = new Point(132, 300);
        _install.Size = new Size(180, 42);
        _install.Font = new Font("Segoe UI", 10f, FontStyle.Bold);
        _install.Click += Install_Click;

        _remove.Text = "Remove";
        _remove.Location = new Point(326, 300);
        _remove.Size = new Size(180, 42);
        _remove.Font = new Font("Segoe UI", 10f);
        _remove.Click += Remove_Click;

        var guide = new GroupBox
        {
            Text = "How to use",
            Location = new Point(24, 358),
            Size = new Size(632, 188)
        };

        var guideText = new Label
        {
            Text =
                "1.  Type the server address above, then click Install.\n\n" +
                "2.  Start Sky from Steam like you normally do.\n\n" +
                "3.  In the game, press the ` key (top-left, under Esc) to open the menu,\n" +
                "     then go to  Settings  ->  Server Redirect  and tick\n" +
                "     \"Enable account redirect\".\n\n" +
                "4.  To play on the official servers again: untick that box, or click\n" +
                "     Remove here. Nothing else on your PC is changed.",
            Location = new Point(16, 24),
            Size = new Size(600, 150),
            ForeColor = Color.FromArgb(45, 45, 45)
        };
        guide.Controls.Add(guideText);

        var logLabel = new Label { Text = "Activity", Location = new Point(24, 556), AutoSize = true };

        _log.Location = new Point(24, 578);
        _log.Size = new Size(632, 56);
        _log.Multiline = true;
        _log.ReadOnly = true;
        _log.ScrollBars = ScrollBars.Vertical;
        _log.BackColor = Color.FromArgb(250, 250, 250);
        _log.Font = new Font("Consolas", 8.5f);

        Controls.AddRange(new Control[]
        {
            title, subtitle, _statusPanel, gameLabel, _gameBox, _browse,
            serverLabel, _serverBox, serverHint, _plainHttp, _install, _remove,
            guide, logLabel, _log
        });
    }

    private void LoadSettings()
    {
        Settings settings = new();
        try
        {
            if (File.Exists(SettingsPath))
                settings = JsonSerializer.Deserialize<Settings>(File.ReadAllText(SettingsPath)) ?? new Settings();
        }
        catch { }

        _gameBox.Text = Installer.LooksLikeGame(settings.GamePath)
            ? settings.GamePath!
            : Installer.DetectGameFolder() ?? "";
        _serverBox.Text = settings.Server ?? "";
        _plainHttp.Checked = settings.PlainHttp;
    }

    private void SaveSettings()
    {
        try
        {
            var settings = new Settings
            {
                GamePath = _gameBox.Text,
                Server = _serverBox.Text.Trim(),
                PlainHttp = _plainHttp.Checked
            };
            Directory.CreateDirectory(Path.GetDirectoryName(SettingsPath)!);
            File.WriteAllText(SettingsPath, JsonSerializer.Serialize(settings, Json));
        }
        catch { }
    }

    private void Browse_Click(object? sender, EventArgs e)
    {
        using var dialog = new FolderBrowserDialog
        {
            Description = "Select the folder that contains Sky.exe",
            UseDescriptionForTitle = true,
            ShowNewFolderButton = false
        };

        if (Directory.Exists(_gameBox.Text))
            dialog.SelectedPath = _gameBox.Text;

        if (dialog.ShowDialog(this) != DialogResult.OK)
            return;

        if (!Installer.LooksLikeGame(dialog.SelectedPath))
        {
            MessageBox.Show(this,
                "That folder does not contain Sky.exe.\n\n" +
                "Usually it is inside Steam\\steamapps\\common\\Sky Children of the Light.",
                "Wrong folder", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        _gameBox.Text = dialog.SelectedPath;
        SaveSettings();
        RefreshStatus();
    }

    private void Install_Click(object? sender, EventArgs e)
    {
        string game = _gameBox.Text;
        if (!Installer.LooksLikeGame(game))
        {
            MessageBox.Show(this, "Please choose your Sky folder first.", "No game folder",
                MessageBoxButtons.OK, MessageBoxIcon.Warning);
            return;
        }

        string host = CleanHost(_serverBox.Text);
        if (host.Length == 0)
        {
            MessageBox.Show(this,
                "Please type the server address you were given first.\n\n" +
                "It looks like  my-server.example.com:9000",
                "Server address needed", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            _serverBox.Focus();
            return;
        }

        _serverBox.Text = host;
        SaveSettings();

        RunBusy(() =>
        {
            AppendLog("Installing...");
            Installer.Install(game, host, _plainHttp.Checked, AppendLog);
        });
    }

    private void Remove_Click(object? sender, EventArgs e)
    {
        string game = _gameBox.Text;
        if (!Installer.LooksLikeGame(game))
            return;

        DialogResult answer = MessageBox.Show(this,
            "Remove the private-server mod from Sky?\n\n" +
            "The game will start normally and connect to the official servers again.",
            "Remove", MessageBoxButtons.YesNo, MessageBoxIcon.Question);

        if (answer != DialogResult.Yes)
            return;

        RunBusy(() =>
        {
            AppendLog("Removing...");
            Installer.Uninstall(game, AppendLog);
        });
    }

    private void RunBusy(Action work)
    {
        Cursor = Cursors.WaitCursor;
        _install.Enabled = false;
        _remove.Enabled = false;
        try
        {
            work();
        }
        catch (Exception ex)
        {
            AppendLog("Error: " + ex.Message);
            MessageBox.Show(this, ex.Message, "Something went wrong",
                MessageBoxButtons.OK, MessageBoxIcon.Error);
        }
        finally
        {
            Cursor = Cursors.Default;
            RefreshStatus();
        }
    }

    private void RefreshStatus()
    {
        string game = _gameBox.Text;
        if (!Installer.LooksLikeGame(game))
        {
            SetStatus("No game folder found", "Choose your Sky folder to continue.",
                Color.FromArgb(242, 242, 242), Color.FromArgb(60, 60, 60));
            _install.Enabled = false;
            _remove.Enabled = false;
            return;
        }

        Installer.Status status = Installer.GetStatus(game);

        if (!status.Installed)
        {
            SetStatus("Not installed", "Sky starts normally.", Color.FromArgb(242, 242, 242), Color.FromArgb(60, 60, 60));
        }
        else if (status.Enabled)
        {
            string host = string.IsNullOrWhiteSpace(status.Host) ? "(no address set)" : status.Host;
            SetStatus("Installed - connecting to " + host,
                "Switch back to the official servers with Remove, or in game.",
                Color.FromArgb(232, 246, 236), Color.FromArgb(24, 96, 48));
        }
        else
        {
            SetStatus("Installed - using official servers",
                "The redirect is currently turned off in game.",
                Color.FromArgb(255, 244, 229), Color.FromArgb(120, 70, 0));
        }

        _install.Enabled = true;
        _remove.Enabled = status.Installed;
    }

    private void SetStatus(string headline, string detail, Color background, Color foreground)
    {
        _statusPanel.BackColor = background;
        _statusText.ForeColor = foreground;
        _statusText.Text = headline + "\n" + detail;
        _statusText.Font = new Font("Segoe UI", 10.5f, FontStyle.Bold);
    }

    private void AppendLog(string line)
    {
        _log.AppendText(line + Environment.NewLine);
    }

    // Accept "https://host:port/" and keep just "host:port".
    private static string CleanHost(string raw)
    {
        string host = raw.Trim();
        int scheme = host.IndexOf("://", StringComparison.Ordinal);
        if (scheme >= 0) host = host[(scheme + 3)..];
        host = host.TrimEnd('/');
        host = host.Trim();
        return host;
    }

    protected override void OnFormClosing(FormClosingEventArgs e)
    {
        SaveSettings();
        base.OnFormClosing(e);
    }
}
