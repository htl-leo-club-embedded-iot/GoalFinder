#!/usr/bin/python3

# Packs firmware.bin and littlefs.bin into a single .gfpkg combined update package.
#
# Package format (16-byte header + payload):
#   Bytes  0-3:   GFPK Magic
#   Byte   4:     Format version (1)
#   Bytes  5-8:   Firmware size  (uint32_t, little-endian)
#   Bytes  9-12:  Filesystem size (uint32_t, little-endian)
#   Bytes  13-15: Reserved (0x00)
#   --- payload ---
#   [firmware.bin bytes]
#   [littlefs.bin bytes]
#
# Usage:
#   PlatformIO custom target:  pio run -t package
#   Standalone:                python pack-gfpkg.py <firmware.bin> <littlefs.bin> <output.gfpkg>

import os
import struct
import subprocess
import sys

HEADER_SIZE = 16
MAGIC = b"GFPK"
FORMAT_VERSION = 1


def pack(firmware_path, filesystem_path, output_path):
    """Create a .gfpkg combined update package from firmware and filesystem images."""

    if not os.path.isfile(firmware_path):
        raise FileNotFoundError(f"Firmware binary not found: {firmware_path}")

    if not os.path.isfile(filesystem_path):
        raise FileNotFoundError(
            f"Filesystem image not found: {filesystem_path}\n"
            "  Build it first with:  pio run -t buildfs"
        )

    with open(firmware_path, "rb") as f:
        firmware_data = f.read()

    with open(filesystem_path, "rb") as f:
        filesystem_data = f.read()

    # Build 16-byte header
    header = MAGIC                                          # 4 bytes
    header += struct.pack("<B", FORMAT_VERSION)             # 1 byte
    header += struct.pack("<I", len(firmware_data))         # 4 bytes
    header += struct.pack("<I", len(filesystem_data))       # 4 bytes
    header += b"\x00" * 3                                   # 3 bytes reserved

    assert len(header) == HEADER_SIZE

    with open(output_path, "wb") as f:
        f.write(header)
        f.write(firmware_data)
        f.write(filesystem_data)

    total = HEADER_SIZE + len(firmware_data) + len(filesystem_data)
    print(
        f"[GFPKG] Created {output_path}\n"
        f"        Firmware  : {len(firmware_data):>10,} bytes\n"
        f"        Filesystem: {len(filesystem_data):>10,} bytes\n"
        f"        Total     : {total:>10,} bytes"
    )


# ---------------------------------------------------------------------------
# PlatformIO integration – register a custom target called "package"
# ---------------------------------------------------------------------------
try:
    Import("env")

    build_dir = env.subst("${BUILD_DIR}")
    firmware_bin = os.path.join(build_dir, "firmware.bin")
    filesystem_bin = os.path.join(build_dir, "littlefs.bin")
    output_gfpkg = os.path.join(build_dir, "system.gfpkg")

    # Path to the web client project (relative to the embedded project root)
    web_client_dir = os.path.abspath(os.path.join(env.subst("${PROJECT_DIR}"), "..", "web"))

    def package_action(source, target, env):
        # 1. Build the web app
        print("[GFPKG] Building web app...")
        npm_cmd = "npm.cmd" if sys.platform == "win32" else "npm"
        result = subprocess.run([npm_cmd, "run", "build"], cwd=web_client_dir)
        if result.returncode != 0:
            raise RuntimeError("Web app build failed")

        # 2. Build the LittleFS filesystem image
        print("[GFPKG] Building filesystem image...")
        env.Execute("pio run -t buildfs -e " + env.subst("${PIOENV}"))

        # 3. Pack firmware + filesystem into .gfpkg
        pack(firmware_bin, filesystem_bin, output_gfpkg)

    env.AddCustomTarget(
        name="package",
        dependencies=firmware_bin,
        actions=package_action,
        title="Package GFPKG",
        description="Pack firmware + filesystem into a single .gfpkg update file",
        always_build=True,
    )
except Exception:
    pass

# ---------------------------------------------------------------------------
# Standalone CLI usage
# ---------------------------------------------------------------------------
if __name__ == "__main__":
    if len(sys.argv) == 4:
        pack(sys.argv[1], sys.argv[2], sys.argv[3])
    elif len(sys.argv) == 1:
        # Default paths when run from the embedded directory
        build_dir = os.path.join(".pio", "build", "wemos_d1_mini32")
        pack(
            os.path.join(build_dir, "firmware.bin"),
            os.path.join(build_dir, "littlefs.bin"),
            os.path.join(build_dir, "system.gfpkg"),
        )
    else:
        print("Usage: python pack-gfpkg.py [<firmware.bin> <littlefs.bin> <output.gfpkg>]")
        sys.exit(1)
