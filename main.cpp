//
// Created by devon on 5/2/22.
//

#include <iostream>
#include <bitset>

int main() {
    // System memory
    uint8_t memory[4096] = {0};
    size_t memory_size = sizeof(memory)/sizeof(memory[0]);

    // Data Registers
    uint8_t registers[16] = {0};
    size_t registers_size = sizeof(registers)/sizeof(registers[0]);

    // Addressing Register
    uint16_t I = 0;

    std::cout << "CHIP-8 Memory:" << std::endl;
    for (int i = 0; i < memory_size; i++)
        std::cout << "0x" << std::uppercase << std::hex << i << ": " << std::bitset<8>(memory[i]) << std::endl;

    std::cout << std::endl << "CHIP-8 Data Registers:" << std::endl;
    for (int i = 0; i < registers_size; i++) {
        std::cout << "V" << std::uppercase << std::hex << i << ": " << std::bitset<8>(registers[i]) << std::endl;
    }

    std::cout << std::endl << "CHIP-8 Addressing Register:" << std::endl << "I: " << std::bitset<16>(I) << std::endl;

    return 0;
}