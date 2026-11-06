#include "nrom.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>

// Standalone mapper test; it calls NROM directly without a CPU or PPU. It is used to verify that the mapper implementation is correct and that the ROM is loaded correctly.
int main(void) {
    try {
        Nrom cartridge("tests/roms/test.nes");
        
        std::cout << "PRG at $8000: "
                  << static_cast<int>(cartridge.cpu_read(0x8000)) 
                  << std::endl;
        std::cout << "PRG at $C000: "
                  << static_cast<int>(cartridge.cpu_read(0xC000)) 
                  << std::endl;

        // Verify CHR RAM when ROM has no CHR ROM.
        std::cout << "Writing to CHR RAM at $0010: 0x42" << std::endl;
        cartridge.ppu_write(0x0010, 0x42);
        const auto chr_value = cartridge.ppu_read(0x0010);
        std::cout << "CHR RAM at $0010: "
                  << static_cast<int>(chr_value)
                  << std::endl;
        if (chr_value != 0x42) {
            throw std::runtime_error("CHR RAM write was not retained");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}