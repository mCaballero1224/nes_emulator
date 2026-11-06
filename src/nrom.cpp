#include "nrom.hpp"

#include <fstream>
#include <stdexcept>
#include <ostream>
#include <iomanip>

// Loads the iNES header, skips an optional trainer, then reads PRG and CHR data.
// NROM supports either 16 or 32 KiB of PRG and either 8KiB of CHR ROM or CHR RAM.
Nrom::Nrom(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open ROM: " + path);
    }
    // The iNES header is 16B and starts with the signature "NES\x1A".
    std::uint8_t header[16]{};
    file.read(reinterpret_cast<char*>(header), sizeof(header));
    // Check the header signature.
    if (
        !file || 
        header[0] != 'N' ||
        header[1] != 'E' || 
        header[2] != 'S' || 
        header[3] != 0x1A
    ) {
        throw std::runtime_error("Invalid iNES ROM: " + path);
    }
    // This implementation supports the original iNES format, not NES 2.0.
    if ((header[7] & 0x0C) == 0x08) {
        throw std::runtime_error("NES 2.0 ROMs are not supported: " + path);
    }
    // Header flag 6, bit 2 indicates the presence of a 512B trainer following the header.
    const bool has_trainer = (header[6] & 0x04) != 0;
    // Header sizes are measured in 16KiB PRG units and 8Kib CHR units.
    const std::size_t prg_size = static_cast<std::size_t>(header[4]) * 16 * 1024;
    const std::size_t chr_size = static_cast<std::size_t>(header[5]) * 8 * 1024;
    // Skip the optional trainer to read the PRG ROM data.
    if (has_trainer) {
        file.seekg(512, std::ios::cur);
        if (!file) {
            throw std::runtime_error("Truncated ROM trainer");
        }
    }
    // Load PRG ROM, which is read with cpu_read()
    prg_rom.resize(prg_size);
    file.read(reinterpret_cast<char*>(prg_rom.data()), 
        static_cast<std::streamsize>(prg_rom.size()));
    if (!file) {
        throw std::runtime_error("Truncated ROM PRG data");
    }
    // A CHR ROM size of 0 indicates that the cartridge uses CHR RAM instead.
    if (chr_size > 0) {
        // Load CHR ROM, which is read with ppu_read()
        chr_rom.resize(chr_size);
        file.read(reinterpret_cast<char*>(chr_rom.data()), 
            static_cast<std::streamsize>(chr_rom.size()));
        if (!file) {
            throw std::runtime_error("Truncated CHR ROM");
        }
    } else {
        chr_ram.resize(8 * 1024);
    }
    // Reject sizes that are invalid for this NROM implementation. 
    if (prg_rom.size() != 16 * 1024 && prg_rom.size() != 32 * 1024) {
        throw std::runtime_error("Invalid PRG ROM size for NROM: " + std::to_string(prg_rom.size() / 1024) + "KiB");
    }
    if (!chr_rom.empty() && chr_rom.size() != 8 * 1024) {
        throw std::runtime_error("NROM must have 8KiB of CHR ROM");
    }
}


// Maps CPU addresses $8000-$ffff to the PRG ROM data. NROM supports either 16 or 32 KiB of PRG ROM.
// Modulo mapping mirrors a 16KiB PRG ROM into both halves of the address range.
std::uint8_t Nrom::cpu_read(std::uint16_t addr) const {
    if (addr < 0x8000) {
        throw std::out_of_range("Address is not NROM PRG ROM: " + std::to_string(addr));
    }
    // Calculate the offset of the address into PRG ROM.
    const std::size_t offset = addr - 0x8000;
    return prg_rom[offset % prg_rom.size()];
}


// Maps PPU pattern-table addresses $0000-$1fff into CHR ROM or CHR RAM.
std::uint8_t Nrom::ppu_read(std::uint16_t addr) const {
    if (addr > 0x1fff) {
        throw std::out_of_range("Address is not NROM CHR ROM/RAM: " + std::to_string(addr));
    }
    // Check if the cartridge has CHR ROM or CHR RAM. If it has CHR ROM, read from it, otherwise read from CHR RAM.
    if (!chr_rom.empty()) {
        return chr_rom.at(addr);
    }
    return chr_ram.at(addr);
}


// Writes to CHR RAM. The PPU can write to CHR RAM, CHR ROM is read-only.
// Writes to CHR ROM are ignored.
void Nrom::ppu_write(std::uint16_t addr, std::uint8_t data) {
    // Check if the address is within the CHR RAM range.
    if (addr > 0x1fff) {
        throw std::out_of_range("Address is not NROM CHR RAM: " + std::to_string(addr));
    }
    // CHR ROM is read-only; write only when cartridge has CHR RAM.
    if (!chr_rom.empty()) {
        chr_ram.at(addr) = data;
    }
    return;
}

// Lists all CPU PRG and PPU CHR addresses handled by NROM.
void Nrom::list_address_space(std::ostream& out) const {
    auto print_range = [&out](const char* title,
                              std::uint32_t first,
                              std::uint32_t last,
                              const auto& read) {
        out << title << '\n';

        for (std::uint32_t address = first; address <= last; address += 16) {
            out << '$' << std::uppercase << std::hex
                << std::setw(4) << std::setfill('0') << address << ": ";

            for (std::uint32_t offset = 0; offset < 16; ++offset) {
                const auto value =
                    read(static_cast<std::uint16_t>(address + offset));

                out << std::setw(2)
                    << static_cast<unsigned>(value)
                    << ' ';
            }
            out << '\n';
        }
    };

    print_range("CPU PRG ($8000-$FFFF)",
                0x8000,
                0xFFFF,
                [this](std::uint16_t address) {
                    return cpu_read(address);
                });

    out << '\n';

    print_range("PPU CHR ($0000-$1FFF)",
                0x0000,
                0x1FFF,
                [this](std::uint16_t address) {
                    return ppu_read(address);
                });
}