#include "nrom.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// Format an address or byte as uppercase hexadecimal.
std::string hex_value(std::uint16_t value, int width) {
    std::ostringstream out;
    out << "$" 
        << std::uppercase 
        << std::hex 
        << std::setfill('0')
        << std::setw(width)
        << value;
    return out.str();
}

void draw(const std::string& last_operation,
     const std::string& last_result) {
    // Clear the terminal and move the cursor to the top-left.
    std::cout << "\033[2J\033[H";
    std::cout << "NROM Mapper 0 - interactive test\n" 
              << "================================\n"
              << "CPU address map:\n"
              << "\t$0000-$FFFF\t PRG ROM (16 KiB mirrors; 32 KiB does not)\n\n"
              << "Last operation: " << last_operation << "\n"
              << "Last result: "    << last_result << "\n\n"
              << "Commands:\n"
              << "\tcr <hex-address>            CPU read, e.g cr 8000\n"
              << "\tpr <hex-address>            PPU read, e.g pr 0010\n"
              << "\tpw <hex-address> <value>    PPU write, e.g pw 0010 42\n"
              << "\tls                          List all NROM addresses\n"
              << "\tq                           Quit\n\n"
              << "> " << std::flush;
}


int main(void) {
    try {
        Nrom cartridge("tests/roms/test.nes");

        std::string last_operation = "None";
        std::string last_result = "No operation yet";
        std::string line;

        while (true) {
            draw(last_operation, last_result);

            if (!std::getline(std::cin, line)) {
                break;
            }

            std::istringstream input(line);
            std::string command;
            unsigned int address = 0;
            unsigned int value = 0;

            input >> command;

            try {
                if (command == "q") {
                    break;
                } else if (command == "ls") {
                    std::cout << "\n";
                    cartridge.list_address_space(std::cout);
                    std::cout << "\nPress Enter to return to the TUI...";
                    std::string pause;
                    std::getline(std::cin, pause);
                } else if (command == "cr" && input >> std::hex >> address) {
                    const auto byte =
                        cartridge.cpu_read(static_cast<std::uint16_t>(address));
                    last_operation = "CPU read " + 
                        hex_value(static_cast<std::uint16_t>(address), 4);
                    last_result = hex_value(byte, 2);
                } else if (command == "pr" && input >> std::hex >> address) {
                    const auto byte =
                        cartridge.ppu_read(static_cast<std::uint16_t>(address));
                    last_operation = "PPU read " + 
                        hex_value(static_cast<std::uint16_t>(address), 4);
                    last_result = hex_value(byte, 2);
                } else if (command == "pw" && input >> std::hex >> address >> value) {
                    cartridge.ppu_write(
                        static_cast<std::uint16_t>(address),
                        static_cast<std::uint8_t>(value));
                    last_operation = "PPU write " +
                        hex_value(static_cast<std::uint16_t>(address), 4) +
                        " <- " + 
                        hex_value(static_cast<std::uint8_t>(value), 2);
                    last_result = "Write completed";
                } else {
                    last_operation = "Invalid command";
                    last_result = 
                        "Use cr <addr>, pr <addr>, pw <addr> <value>, ls or q";
                }
            } catch (const std::exception& e) {
                last_operation = "Operation failed";
                last_result = e.what();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Could not start NROM TUI: " << e.what() << "\n";
    }
}