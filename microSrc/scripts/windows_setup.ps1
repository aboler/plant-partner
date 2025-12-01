# install_windows.ps1
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$InstallFlag = Join-Path $ScriptDir ".idf_install_done"
$EspDir = Join-Path $ScriptDir "..\submodules\esp-idf"

# Check if setup already done
if (Test-Path $InstallFlag) {
    Write-Host "[install] ESP-IDF already installed."
    exit 0
}

# Check if Git is installed
Write-Host "[install] Checking prerequisites..."
if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    Write-Host "Git is required. Install Git for Windows and rerun."
    exit 1
}

# Check if Python is installed
if (-not (Get-Command python -ErrorAction SilentlyContinue)) {
    Write-Host "Python is required. Install Python and rerun."
    exit 1
}

# Run ESP-IDF installer
Write-Host "[install] Running ESP-IDF installer..."
Set-Location $EspDir
cmd.exe /c "install.bat"

# Create install marker
New-Item -ItemType File -Path $InstallFlag | Out-Null
Write-Host "[install] Installation complete. IDF install marker created at $InstallFlag."
Write-Host "[install] You can now use esp.py to build and flash your project."
