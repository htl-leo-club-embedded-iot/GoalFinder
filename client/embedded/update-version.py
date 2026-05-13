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
VERSION_PATTERN = re.compile(
    r'^([ \t]*#\s*define\s+FIRMWARE_VERSION\s+")([^"]+)(")',
    re.MULTILINE,
)

def read_current_version():
    """Read the current FIRMWARE_VERSION from Version.h"""
    current_version = None

    if VERSION_FILE.exists():
        content = VERSION_FILE.read_text()
        match = VERSION_PATTERN.search(content)
        if match:
            current_version = match.group(2)

    return current_version

def update_version(new_version):
    """Update FIRMWARE_VERSION in Version.h with the new version"""
    updated = False

    if VERSION_FILE.exists():
        content = VERSION_FILE.read_text()
        updated_content, count = VERSION_PATTERN.subn(
            lambda match: f"{match.group(1)}{new_version}{match.group(3)}",
            content,
            count=1,
        )

        if count == 0:
            print(f"Warning: No FIRMWARE_VERSION definition found in {VERSION_FILE}")
        else:
            VERSION_FILE.write_text(updated_content)
            updated = True
    else:
        print(f"Error: {VERSION_FILE} not found")
        sys.exit(1)

    return updated

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
