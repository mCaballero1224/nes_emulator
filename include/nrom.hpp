#ifndef NROM_HPP
#define NROM_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <iosfwd>

/**
 * NROM Overview
 * 
 * Banks:
 *  - CPU $8000 - $bfff: First 16KiB of PRG ROM.
 * 
 *  - CPU $6000 - $7fff: 8KiB unbanked PRG RAM, mirrored as nessessary to 
 *    fill entire 8KiB window, write protectable with an external switch.
 *    (Family BASIC only)
 * 
 *  - CPU $C000 - $ffff: Last 16KiB of PRG ROM (NROM-256) or mirror of the 
 *    first 16KiB (NROM-128).
 * 
 *  - PPU $0000 - $1fff: 8KiB of CHR ROM.
 * 
 * All banks are fixed. 
 * There are no registers.
 * 
 */

 /**
  * iNES Format Overview
  * 
  * 1. Header (16 bytes)
  *     [0-3] Signature: 4 bytes, "NES\x1A" 
  *         (ASCII characters "NES" followed by MS-DOS end-of-file)
  *     [4] PRG ROM size: 1 byte, in 16 KiB units
  *     [5] CHR ROM size: 1 byte, in 8 KiB units (0 means the board uses CHR RAM)
  *     [6] Flags 6: 1 byte, bitfield for mapper, mirroring, battery, trainer
  *     [7] Flags 7: 1 byte, bitfield for mapper, VS/Playchoice, NES 2.0
  *     [8] Flags 8: 1 byte, PRG RAM size (rarely used)
  *     [9] Flags 9: 1 byte, TV system (rarely used)
  *     [10] Flags 10: 1 byte, TV system, PRG RAM presence (rarely used)
  *     [11-15] Unused padding: 5 bytes, should be zero filled
  * 
  * 2. Optional Trainer: 0 or 512 bytes
  * 
  * 3. PRG ROM Data: 16 KiB * n bytes
  * 
  * 4. CHR ROM Data: 8 KiB * n bytes
  * 
  * 5. PlayChoice INST-ROM (if present): 0 or 8 KiB 
  * 
  * 6. PlayChoice PROM (if present): 16 bytes Data, 16 bytes CounterOut, 
  *     48 bytes Reserved; this is often missing
  * 
  */

// NROM Mapper class implented in src/mapper.cpp
class Nrom {
public:
    // Loads an iNES ROM file and inits its PRG and CHR memory.
    explicit Nrom(const std::string& path);

    // Reads PRG ROM data from the CPU address space ($8000-$ffff).
    std::uint8_t cpu_read(std::uint16_t addr) const;
    // Reads CHR ROM or CHR RAM mapped to PPU address
    std::uint8_t ppu_read(std::uint16_t addr) const;
    // Writes to CHR RAM. Writes are ignored when the cartridge has CHR ROM.
    void ppu_write(std::uint16_t addr, std::uint8_t data);
    // Prints the CPU PRG and PPU CHR address ranges and their mapped values.
    void list_address_space(std::ostream& out) const;

private:
    // Program ROM read by the CPU
    std::vector<uint8_t> prg_rom;
    // CHR ROM read by the PPU. Empty when the ROM uses CHR RAM.
    std::vector<uint8_t> chr_rom;
    // CHR RAM used when the ROM has no CHR ROM.
    std::vector<uint8_t> chr_ram;
};

#endif // NROM_HPP