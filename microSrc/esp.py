#!/usr/bin/env python3
import argparse
import os
import platform
import subprocess
import sys

# Paths
ROOT = os.path.dirname(os.path.abspath(__file__))
SCRIPTS_DIR = os.path.join(ROOT, "scripts")
IDF_DIR = os.path.join(ROOT, "submodules", "esp-idf")
INSTALL_FLAG = os.path.join(SCRIPTS_DIR, ".idf_install_done")
PROJECT_DIR = os.path.join(ROOT, "src")  # Project folder containing CMakeLists.txt

# Run a shell command
def run_cmd(cmd, shell=True):
    result = subprocess.run(cmd, shell=shell)

    # If command failed, exit
    if result.returncode != 0:
        print(f"[esp.py] Error running: {cmd}")
        sys.exit(result.returncode)

# Determine OS and installer
def get_installer_cmd():
    system = platform.system()

    # Check which setup script to run
    if system == "Linux":
        return os.path.join(SCRIPTS_DIR, "linux_setup.sh")
    elif system == "Darwin":
        return os.path.join(SCRIPTS_DIR, "mac_setup.sh")
    elif system == "Windows":
        return os.path.join(SCRIPTS_DIR, "windows_setup.ps1")
    else:
        print(f"[esp.py] Unsupported OS: {system}")
        sys.exit(1)

# Ensure ESP-IDF installed (./install.sh or install.bat has been run or not)
def ensure_installed():
    # Check for install flag
    if not os.path.exists(INSTALL_FLAG):
        # Get setup script command
        installer = get_installer_cmd()
        print(f"[esp.py] ESP-IDF not installed. Running installer: {installer}")

        # Run setup scripts for respective OS
        if platform.system() == "Windows":
            run_cmd(f"powershell -ExecutionPolicy Bypass -File \"{installer}\"")
        else:
            run_cmd(f"bash \"{installer}\"")

    # If Install flag exists, skip
    else:
        print("[esp.py] ESP-IDF already installed.")

# Build, flash, or monitor using idf.py
def run_idf(command, port=None):
    system = platform.system()

    # Prepare commands depending on OS
    if system == "Windows":
        # Setup export.bat and run idf.py
        export_file = os.path.join(IDF_DIR, "export.bat")
        idf = os.path.join(IDF_DIR, "tools", "idf.py")

        # If port specified, include in command
        if port:
            cmd = f'cd "{PROJECT_DIR}" && "{export_file}" && "{idf}" -p {port} {command} "monitor"'
        else:
            cmd = f'cd "{PROJECT_DIR}" && "{export_file}" && "{idf}" {command}'

        # Run command in cmd.exe
        run_cmd(f'cmd.exe /c {cmd}')

    else:
        # Setup export.sh and run idf.py
        export_file = os.path.join(IDF_DIR, "export.sh")
        idf = os.path.join(IDF_DIR, "tools", "idf.py")

        # If port specified, include in command
        if port:
            cmd = f'cd "{PROJECT_DIR}" && source "{export_file}" && "{idf}" -p {port} {command} "monitor"'
        else:
            cmd = f'cd "{PROJECT_DIR}" && source "{export_file}" && "{idf}" {command}'
        
        # Run command in bash
        run_cmd(f'bash -c "{cmd}"')

def main():
    # Parse arguments
    parser = argparse.ArgumentParser(description="Cross-platform ESP-IDF build/flash/monitor script")
    parser.add_argument("action", nargs="?", choices=["build", "flash", "clean", "monitor"],
                        help="Action to perform")
    parser.add_argument("-p", "--port", help="Serial port for flashing or monitoring")
    args = parser.parse_args()

    # Ensure ESP-IDF installed (setup script has been run)
    ensure_installed()

    # Default: Build + flash + monitor if port provided
    if args.action is None:
        print("[esp.py] No action specified, running default: build, flash, and monitor if port provided")

        # Build, Flash, and Monitor if port provided
        if args.port:
            # Flash automatically runs build so only need to call flash
            run_idf("flash", port=args.port)
        else:
            print("[esp.py] No port provided, skipping flash and monitor")
    else:
        # If parameter provided, run specified action
        if args.action == "build":
            run_idf("build")
        elif args.action == "clean":
            run_idf("clean")
        elif args.action == "flash":
            if not args.port:
                print("[esp.py] Flash requires -p PORT argument")
                sys.exit(1)
            # This will build and flash
            run_idf("flash", port=args.port)
        elif args.action == "monitor":
            if not args.port:
                print("[esp.py] Monitor requires -p PORT argument")
                sys.exit(1)
            run_idf("monitor", port=args.port)

if __name__ == "__main__":
    main()
