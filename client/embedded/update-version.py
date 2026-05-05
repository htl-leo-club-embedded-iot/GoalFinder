#!/usr/bin/env python3
"""
Update the FIRMWARE_VERSION in Version.h before building with PlatformIO.

This script allows specifying a version tag that will be injected into Version.h
before the build process. If no version is provided, it preserves the current value.

Usage:
    python3 update-version.py <version>
    python3 update-version.py  # No change to Version.h

Example:
    python3 update-version.py v0.4.1
    python3 update-version.py v0.5.0-beta
"""

import sys
import re
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent
VERSION_FILE = SCRIPT_DIR / "src" / "Version.h"

def read_current_version():
    """Read the current FIRMWARE_VERSION from Version.h"""
    if not VERSION_FILE.exists():
        return None
    
    with open(VERSION_FILE, 'r') as f:
        content = f.read()
    
    match = re.search(r'#define\s+FIRMWARE_VERSION\s+"([^"]+)"', content)
    return match.group(1) if match else None

def update_version(new_version):
    """Update FIRMWARE_VERSION in Version.h with the new version"""
    if not VERSION_FILE.exists():
        print(f"Error: {VERSION_FILE} not found")
        sys.exit(1)
    
    with open(VERSION_FILE, 'r') as f:
        content = f.read()
    
    # Replace the version string
    updated_content = re.sub(
        r'(#define\s+FIRMWARE_VERSION\s+)"[^"]+"',
        rf'\1"{new_version}"',
        content
    )
    
    if updated_content == content:
        print(f"Warning: No FIRMWARE_VERSION definition found in {VERSION_FILE}")
        return False
    
    with open(VERSION_FILE, 'w') as f:
        f.write(updated_content)
    
    return True

def main():
    """Main entry point"""
    current_version = read_current_version()
    
    if len(sys.argv) > 1:
        new_version = sys.argv[1]
        if update_version(new_version):
            print(f"✓ Updated FIRMWARE_VERSION: {current_version} → {new_version}")
        else:
            print(f"✗ Failed to update FIRMWARE_VERSION")
            sys.exit(1)
    else:
        if current_version:
            print(f"Current FIRMWARE_VERSION: {current_version}")
        else:
            print("No FIRMWARE_VERSION found in Version.h")

if __name__ == "__main__":
    main()
