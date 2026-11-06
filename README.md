# nes_emulator

## Mappers

## NROMs

The simplest mapper available for the NES.There are two formats available: NROM-128 and NROM-256. Each format is comprised of four banks.

| Name | Location | Lowest Address | Highest Address | Notes |
| --- | --- | --- | --- | --- |
| Unbanked PRG-RAM | CPU | $6000 | $7FFF | Mirrored as necessary to fill 8 KiB window. |
| First 16 KiB of PRG-ROM | CPU | $8000 | $BFFF | |
| Last 16 KiB of PRG-ROM | CPU | $C000 | $FFFF | Also functions as a mirror of $8000-$BFFF for NROM-128 format. |
| CHR-ROM | PPU | $0000 | $1FFF | 8 KiB available. |
