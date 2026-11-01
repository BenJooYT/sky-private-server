param(
    [string]$Configuration = "Release",
    [string]$Output = "dist"
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$project = Join-Path $root "installer\SkySwitch.csproj"
$outDir = Join-Path $root $Output

dotnet publish $project -c $Configuration -r win-x64 `
    --self-contained false -p:PublishSingleFile=true -o $outDir

Write-Host ""
Write-Host "Built: $(Join-Path $outDir 'SkySwitch.exe')"
