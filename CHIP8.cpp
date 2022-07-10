//
// Created by devon on 6/22/22.
//

#include "CHIP8.h"

CHIP8::CHIP8()
{
    // Initialize memory blocks to zero
    this->memory = new uint8_t[CHIP8::MEMORY_SIZE]{0};
    this->V = new uint8_t[CHIP8::N_REGISTERS]{0};
    this->stack = new uint16_t[CHIP8::STACK_SIZE]{0};
    this->screen = new uint8_t[CHIP8::SCREEN_WIDTH*CHIP8::SCREEN_HEIGHT]{0};

    // Initialize standalone registers to zero
    this->PC = CHIP8::PROGRAM_START_ADDRESS;
    this->SP = 0;
    this->I = 0;
    this->DT = 0;
    this->ST = 0;

    // Load fontset
    uint8_t font[]{
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // Apply above font
    for (int i = 0; i < CHIP8::FONTSET_SIZE; i++)
        this->memory[i + CHIP8::FONTSET_START_ADDRESS] = font[i];
}

CHIP8::~CHIP8()
{
    delete[] this->memory;
    delete[] this->V;
    delete[] this->stack;
    delete[] this->screen;
}

void CHIP8::dump()
{
    printf("CHIP-8 Memory:\n");
    for (int i = 0; i < CHIP8::MEMORY_SIZE; i++)
        printf("0x%03X: 0x%02X\n", i, this->memory[i]);

    printf("\nCHIP-8 Data Registers:\n");
    for (int i = 0; i < CHIP8::N_REGISTERS; i++)
        printf("V%01X: 0x%04X\n", i, this->V[i]);

    printf("\nCHIP-8 Stack:\n");
    for (int i = 0; i < CHIP8::STACK_SIZE; i++)
        printf("S%d: 0x%03X\n", i, this->stack[i]);

    printf("\nCHIP-8 Screen:\n");
    for (int row = 0; row < CHIP8::SCREEN_HEIGHT; row++)
    {
        for (int col = 0; col < CHIP8::SCREEN_WIDTH; col++)
            printf("%d", this->screen[CHIP8::SCREEN_WIDTH*row + col]);
        printf("\n");
    }

    printf("\nCHIP-8 Program Counter:\nPC: 0x%03X\n", this->PC);
    printf("\nCHIP-8 Addressing Register:\nI: 0x%03X\n", this->I);
}

void CHIP8::load_rom(const char* filename)
{
    // Open the rom
    std::streampos rom_size;
    std::ifstream rom(filename, std::ios::binary);

    if (rom.is_open())
    {
        // Get rom size
        rom.seekg(0, std::ios::end);
        rom_size = rom.tellg();
        rom.seekg(0, std::ios::beg);

        // Read the data
        auto* rom_data = new uint8_t[rom_size];
        rom.read((char*) rom_data, rom_size);
        rom.close();

        for (int i = 0; i < rom_size; i++)
            this->memory[CHIP8::PROGRAM_START_ADDRESS + i] = rom_data[i];

        delete[] rom_data;
    }
    else
        printf("\nFailed to load rom\n");
}
