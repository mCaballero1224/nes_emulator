#!/usr/bin/env python3
"""
Generate a small NROM file for the mapper tests.
"""

from pathlib import Path


OUTPUT = Path(__file__).resolve().parent.parent / "tests" / "roms" / "test.nes"
OUTPUT.parent.mkdir(parents=True, exist_ok=True)

# iNES header:
#   - one 16KiB PRG bank
#   - no CHR ROM (so the cartridge uses CHR RAM)
header = bytearray(16)
header[0:4] = b"NES\x1a"
header[4] = 1  # 1 PRG bank
header[5] = 0  # 0 CHR banks
header[6] = 0  # mapper 0

# Fille PRG with a known value to verify reads and 16KiB mirroring.
prg = bytearray([0x42] * 16 * 1024)

OUTPUT.write_bytes(header + prg)
print(f"Created test ROM: {OUTPUT}")