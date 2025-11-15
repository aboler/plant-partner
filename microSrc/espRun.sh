#!/usr/bin/env bash
set -euo pipefail

# espRun - convenience script to install ESP-IDF tools (if needed), source the env,
# build the project and flash to a specified serial port.
# Usage: ./espRun [PORT]
# Example: ./espRun /dev/ttyUSB0

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IDF_DIR="$SCRIPT_DIR/submodules/esp-idf"

if [ ! -d "$IDF_DIR" ]; then
  echo "ERROR: esp-idf directory not found at $IDF_DIR"
  exit 1
fi

echo "[espRun] Running esp-idf install script (may prompt or take time)..."
# Run install.sh - it's safe to run multiple times; it will skip already installed parts.
# If you prefer to skip install, set SKIP_INSTALL=1 in the environment before calling this script.
if [ "${SKIP_INSTALL:-0}" != "1" ]; then
  "$IDF_DIR/install.sh"
else
  echo "[espRun] SKIP_INSTALL set; skipping install.sh"
fi

# Source the export script so idf.py and toolchain are on PATH for this shell
echo "[espRun] Sourcing export.sh to set up environment..."
# shellcheck source=/dev/null
. "$IDF_DIR/export.sh"

# Check idf.py availability
if ! command -v idf.py >/dev/null 2>&1; then
  echo "ERROR: idf.py not found after sourcing export.sh"
  echo "Make sure export.sh completed successfully or run it manually:" 
  echo "  . $IDF_DIR/export.sh"
  exit 1
fi

# Move to project root (SCRIPT_DIR) and build
cd "$SCRIPT_DIR"
echo "[espRun] Running: idf.py build"
idf.py build

# Flash if port provided
PORT="${1:-}" 
if [ -z "$PORT" ]; then
  echo "[espRun] No serial port provided; build finished. To flash run: $0 /dev/ttyUSB0"
  exit 0
fi

echo "[espRun] Flashing project to port $PORT and opening monitor..."
echo "[espRun] To close monitor, press Ctrl+]"
idf.py -p "$PORT" flash monitor

echo "[espRun] Flash completed. To monitor logs run: idf.py -p $PORT monitor"
exit 0
