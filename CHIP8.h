//
// Created by devon on 6/22/22.
//

#ifndef CHIP_8_CHIP8_H
#define CHIP_8_CHIP8_H

#include <cstdio>
#include <fstream>

class CHIP8
{
public:
    // Sizes of CHIP8 memory components
    static const unsigned int MEMORY_SIZE = 4096;
    static const unsigned int N_REGISTERS = 16;
    static const unsigned int STACK_SIZE = 16;
    static const unsigned int SCREEN_WIDTH = 64;
    static const unsigned int SCREEN_HEIGHT = 32;
    static const unsigned int FONTSET_SIZE = 80;
    static const unsigned int FONTSET_START_ADDRESS = 0x50;
    static const unsigned int PROGRAM_START_ADDRESS = 0x200;

    CHIP8();
    ~CHIP8();
    void dump();
    void load_rom(const char* filename);
    void advance();
    void set_pressed_key(uint8_t pressed_key);

private:
    // System memory (4kbs)
    uint8_t* memory;

    // Miscellaneous Registers
    uint8_t* V;

    // Stack (for returning from subroutines)
    uint16_t* stack;

    // Video buffer
    uint8_t* screen;
    // TODO: Keyboard stuff
    uint8_t pressed_key;

    // Program counter (starts at memory address 0x200)
    uint16_t PC;

    // Stack pointer
    uint16_t SP;

    // Addressing Register
    uint16_t I;

    // Special sound and delay timer V
    uint8_t DT;
    uint8_t ST;
};


#endif //CHIP_8_CHIP8_H
