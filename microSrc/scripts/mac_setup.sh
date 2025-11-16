#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_FLAG="$SCRIPT_DIR/.idf_install_done"
ESP_DIR="$SCRIPT_DIR/../submodules/esp-idf"

# Check if setup already done
if [ -f "$INSTALL_FLAG" ]; then
    echo "[install] ESP-IDF already installed."
    exit 0
fi

# Check if Homebrew is installed
echo "[install] Installing macOS dependencies via Homebrew..."
if ! command -v brew >/dev/null 2>&1; then
    echo "[install] Homebrew not found. Please install Homebrew: https://brew.sh/"
    exit 1
fi

# Install dependencies for MacOS
brew update
brew install git cmake ninja dfu-util python3 wget

# Run ESP-IDF installer
echo "[install] Running ESP-IDF installer..."
cd "$ESP_DIR"
./install.sh

touch "$INSTALL_FLAG"
echo "[install] Installation complete. IDF install marker created at $INSTALL_FLAG."
echo "[install] You can now use esp.py to build and flash your project."
