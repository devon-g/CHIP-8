//
// Created by devon on 6/22/22.
//

#include "CHIP8.h"

CHIP8::CHIP8() {
  // Initialize memory blocks to zero
  this->memory = new uint8_t[CHIP8::MEMORY_SIZE]{0};
  this->V = new uint8_t[CHIP8::N_REGISTERS]{0};
  this->stack = new uint16_t[CHIP8::STACK_SIZE]{0};
  this->screen = new uint8_t[CHIP8::SCREEN_WIDTH * CHIP8::SCREEN_HEIGHT]{0};

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

CHIP8::~CHIP8() {
  delete[] this->memory;
  delete[] this->V;
  delete[] this->stack;
  delete[] this->screen;
}

void CHIP8::dump() {
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
  for (int row = 0; row < CHIP8::SCREEN_HEIGHT; row++) {
    for (int col = 0; col < CHIP8::SCREEN_WIDTH; col++)
      printf("%d", this->screen[CHIP8::SCREEN_WIDTH * row + col]);
    printf("\n");
  }

  printf("\nCHIP-8 Program Counter:\nPC: 0x%03X\n", this->PC);
  printf("\nCHIP-8 Addressing Register:\nI: 0x%03X\n", this->I);
}

void CHIP8::load_rom(const char *filename) {
  // Open the rom
  std::streampos rom_size;
  std::ifstream rom(filename, std::ios::binary);

  if (rom.is_open()) {
    // Get rom size
    rom.seekg(0, std::ios::end);
    rom_size = rom.tellg();
    rom.seekg(0, std::ios::beg);

    // Read the data
    auto *rom_data = new uint8_t[rom_size];
    rom.read((char *)rom_data, rom_size);
    rom.close();

    for (int i = 0; i < rom_size; i++)
      this->memory[CHIP8::PROGRAM_START_ADDRESS + i] = rom_data[i];

    delete[] rom_data;
  } else
    printf("\nFailed to load rom\n");
}

void CHIP8::advance() {
  // Interpret CHIP8 program
  if (this->PC < 0xFFF) {
    // Fetch the current instruction
    // (0xHH << 8) | 0xLL = 0xHH00 | 0xLL = 0xHHLL
    int instruction =
        (this->memory[this->PC] << 8) | this->memory[this->PC + 1];

    // Increment to next instruction
    this->PC += 2;

    // Extract chunks of instruction
    uint16_t T = (instruction & 0xF000) >> 12;
    uint16_t X = (instruction & 0x0F00) >> 8;
    uint16_t Y = (instruction & 0x00F0) >> 4;
    uint16_t N = instruction & 0x000F;
    uint16_t NN = instruction & 0x00FF;
    uint16_t NNN = instruction & 0x0FFF;

    // Decode current instruction
    switch (T) {
    case 0x0:
      switch (NN) {
      case 0xE0: // 00E0 - CLS
        // Clear the display
        std::fill(this->screen,
                  this->screen + CHIP8::SCREEN_WIDTH * CHIP8::SCREEN_HEIGHT, 0);
        break;
      case 0xEE: // 00EE - RET
        // Return from a subroutine
        this->PC = this->stack[this->SP];
        if (this->SP > 0)
          this->SP--;
        break;
      default: // 0NNN - SYS addr
        // Ignore 0NNN instruction
        break;
      }
      break;
    case 0x1:
      // 1NNN - JP addr
      // Jump to location NNN
      this->PC = NNN;
      break;
    case 0x2:
      // 2NNN - CALL addr
      // Call subroutine at NNN
      if (this->SP < CHIP8::STACK_SIZE - 1)
        this->SP++;
      this->stack[this->SP] = this->PC;
      this->PC = NNN;
      break;
    case 0x3:
      // 3XNN - SE VX, byte
      // Skip next instruction if VX = NN
      if (this->V[X] == NN)
        this->PC += 2;
      break;
    case 0x4:
      // 4XNN - SNE VX, byte
      // Skip next instruction if VX != NN
      if (this->V[X] != NN)
        this->PC += 2;
      break;
    case 0x5:
      // 5XY0 - SE VX, VY
      // Skip next instruction if VX = VY
      if (this->V[X] == this->V[Y])
        this->PC += 2;
      break;
    case 0x6:
      // 6XNN - LD VX, byte
      // Set VX = NN
      this->V[X] = NN;
      break;
    case 0x7:
      // 7XNN - ADD VX, byte
      // Set VX = VX + NN
      this->V[X] += NN;
      break;
    case 0x8:
      switch (N) {
      case 0x0: // 8XY0 - LD VX, VY
        // Set VX = VY
        this->V[X] = this->V[Y];
        break;
      case 0x1: // 8XY1 - OR VX, VY
        // Set VX = VX OR VY
        // Bitwise or operation
        this->V[X] |= this->V[Y];
        break;
      case 0x2: // 8XY2 - AND VX, VY
        // Set VX = VX AND VY
        // Bitwise and operation
        this->V[X] &= this->V[Y];
        break;
      case 0x3: // 8XY3 - XOR VX, VY
        // Set VX = VX XOR VY
        // Bitwise exclusive or operation
        this->V[X] ^= this->V[Y];
        break;
      case 0x4: // 8XY4 - ADD VX, VY
        // Set VX = VX + VY, set VF = carry
        // If carry VF = 1 else VF = 0
        // Only store low byte of the result
        if ((uint16_t)this->V[X] + (uint16_t)this->V[Y] > 0xFF)
          this->V[0xF] = 1;
        else
          this->V[0xF] = 0;
        this->V[X] += this->V[Y];
        break;
      case 0x5: // 8XY5 - SUB VX, VY
        // If VX > VY, VF = 1 else VF = 0
        // Set VX = VX - VY, set VF = NOT borrow
        if (this->V[X] > this->V[Y])
          this->V[0xF] = 1;
        else
          this->V[0xF] = 0;
        this->V[X] -= this->V[Y];
        break;
      case 0x6: // 8XY6 - SHR VX {, VY}
        // Set VX = VY SHR 1
        // OR
        // Set VX = VX SHR 1
        // If least-significant bit of VX is 1, VF = 1 else VF = 0, then VX >>
        // 1
        this->V[X] = this->V[Y];

        // Check if least-significant bit will be lost
        if (this->V[X] & 0x01)
          this->V[0xF] = 1;
        else
          this->V[0xF] = 0;

        this->V[X] >>= 1;
        break;
      case 0x7: // 8XY7 - SUBN VX, VY
        // Set VX = VY - VX, set VF = NOT borrow
        // IF VY > VX, then VF = 1 else VF = 0
        this->V[X] = this->V[Y] - this->V[X];
        if (this->V[Y] > this->V[X])
          this->V[0xF] = 1;
        else
          this->V[0xF] = 0;
        break;
      case 0xE: // 8XYE - SHL VX {, VY}
        // Set VX = VY SHL 1
        // OR
        // Set VX = VX SHL 1
        // If most-significant bit of VX is 1, VF = 1 else VF = 0, then VX <<
        // 1
        this->V[X] = this->V[Y];

        // Check if most-significant bit will be lost
        if ((this->V[X] >> 7) & 0x01)
          this->V[0xF] = 1;
        else
          this->V[0xF] = 0;

        this->V[X] <<= 1;
        break;
      default:
        break;
      }
      break;
    case 0x9:
      // 9XY0 - SNE VX, VY
      if (this->V[X] != this->V[Y])
        this->PC += 2;
      break;
    case 0xA:
      // ANNN - LD I, addr
      // Set I = NNN
      this->I = NNN;
      break;
    case 0xB:
      // BNNN - JP V0, addr
      this->PC = NNN + this->V[0];
      break;
    case 0xC:
      // CXNN - RND VX, byte
      this->V[X] = (rand() % 256) & NN;
      break;
    case 0xD:
      // DXYN - DRW VX, VY, nibble
      // Reset VF flag
      this->V[0xF] = 0;

      // Get target x and y coordinates and wrap if necessary
      int x, y;
      y = this->V[Y] % 32;

      // Go through each byte of sprite
      for (int i = 0; i < N; i++) {
        // x = this->V[X] % 64;
        uint8_t sprite_byte = this->memory[this->I + i];
        // Go through each bit of sprite byte
        for (int d = 7; d >= 0; d--) {
          x = (this->V[X] + (7 - d)) % 64;
          uint8_t sprite_bit = (sprite_byte >> d) & 0b1;
          // If screen pixel is on and sprite pixel is on, turn screen pixel
          // off and set VF flag
          if (this->screen[CHIP8::SCREEN_WIDTH * y + x] == 1 &&
              sprite_bit == 1) {
            this->screen[CHIP8::SCREEN_WIDTH * y + x] = 0;
            this->V[0xF] = 1;
          }
          // Else if screen pixel is off and sprite pixel is on, turn screen
          // pixel on
          else if (this->screen[CHIP8::SCREEN_WIDTH * y + x] == 0 &&
                   sprite_bit == 1) {
            this->screen[CHIP8::SCREEN_WIDTH * y + x] = 1;
          }
        }
        // If we have reached the bottom of the screen stop drawing
        if (y == CHIP8::SCREEN_HEIGHT - 1)
          break;
        y++;
      }
      break;
    case 0xE:
      switch (NN) {
      case (0x9E):
        // EX9E - SKP VX
        // Skip next instruction if key with the value of VX is pressed
        if (this->V[X] == this->pressed_key)
          this->PC += 2;
        break;
      case (0xA1):
        // EXA1 - SKNP VX
        // Skip next instruction if key with the value of VX is not pressed
        if (this->V[X] != this->pressed_key)
          this->PC += 2;
        break;
      default:
        break;
      }
      break;
    case 0xF:
      switch (NN) {
      case 0x07:
        // LD VX, DT
        this->V[X] = this->DT;
        break;
      case 0x0A:
        // LD VX, K
        // Wait for key press then save value to VX
        // If key is being pressed store value in VX
        if (pressed_key <= 0xF)
          this->V[X] = this->pressed_key;
        else // If not being pressed repeat this instruction
          this->PC -= 2;
        break;
      case 0x15:
        // FX15 - LD DT, VX
        // Set delay timer to VX
        this->DT = this->V[X];
        break;
      case 0x18:
        // FX18 - LD ST, VX
        // Set sound timer to VX
        this->ST = this->V[X];
        break;
      case 0x1E:
        // FX1E - ADD I, VX
        // Set I to I + VX
        this->I += this->V[X];
        break;
      case 0x29:
        // FX29 - LD F, VX
        // Set I to location of sprite for VX
        this->I = this->memory[CHIP8::FONTSET_START_ADDRESS + this->V[X]];
        break;
      case 0x33:
        // FX33 - LD B, VX
        // Store BCD representation of VX in memory locations I, I+1, and I+2
        this->memory[this->I] = (this->V[X] % 1000) / 100;
        this->memory[this->I + 1] = (this->V[X] % 100) / 10;
        this->memory[this->I + 2] = (this->V[X] % 10);
        break;
      case 0x55:
        // FX55 - LD [I], VX
        // Store registers V0 through VX in memory starting at location I
        for (int i = 0; i < X; i++)
          this->memory[this->I + i] = this->V[i];
        break;
      case 0x65:
        // FX65 - LD VX, [I]
        // Read registers V0 through VX from memory starting at location I
        for (int i = 0; i < X; i++)
          this->V[i] = this->memory[this->I + i];
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }
}
