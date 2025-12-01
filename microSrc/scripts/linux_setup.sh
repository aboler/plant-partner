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

# Install dependencies for Ubuntu
echo "[install] Installing Ubuntu dependencies..."
sudo apt update
sudo apt install -y git wget flex bison gperf python3 python3-pip python3-venv \
    cmake ninja-build ccache libffi-dev libssl-dev dfu-util

# Run ESP-IDF installer
echo "[install] Running ESP-IDF installer..."
cd "$ESP_DIR"
./install.sh

touch "$INSTALL_FLAG"
echo "[install] Installation complete. IDF install marker created at $INSTALL_FLAG."
echo "[install] You can now use esp.py to build and flash your project."
