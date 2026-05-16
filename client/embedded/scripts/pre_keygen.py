#!/usr/bin/env python3
"""Minimal PlatformIO extra_script wrapper to run the shell key generator.
This file is intentionally tiny; the heavy lifting happens in the shell script.
"""
import os
import subprocess
import sys

script = os.path.join(os.path.dirname(__file__), 'generate_device_keys.sh')
if not os.path.exists(script):
    print('Key generation script not found:', script, file=sys.stderr)
    sys.exit(1)

try:
    subprocess.check_call(['bash', script])
except subprocess.CalledProcessError as e:
    print('Key generation script failed:', e, file=sys.stderr)
    sys.exit(e.returncode)
