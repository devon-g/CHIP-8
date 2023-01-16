//
// Created by devon on 6/22/22.
//

#include "CHIP8.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

CHIP8::CHIP8() {
  // Initialize standalone registers to zero
  this->PC = CHIP8::program_start_address;
  this->SP = 0;
  this->I = 0;
  this->DT = 0;
  this->ST = 0;
  this->legacy_shift = false;

  // Load fontset
  std::array<uint8_t, CHIP8::fontset_size> font{
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
  for (int i = 0; i < CHIP8::fontset_size; i++)
    this->memory[CHIP8::fontset_start_address + i] = font[i];
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

    // Reserve space for the rom data
    std::vector<uint8_t> rom_data;
    rom_data.reserve(rom_size);

    // Get rom data
    rom_data.insert(rom_data.begin(), std::istream_iterator<uint8_t>(rom),
                    std::istream_iterator<uint8_t>());
    rom.close();

    for (int i = 0; i < rom_size; i++) {
      this->memory[CHIP8::program_start_address + i] = rom_data[i];
    }

  } else {
    std::cerr << "Failed to load rom" << std::endl;
  }
}

void CHIP8::toggle_legacy() { this->legacy_shift = !this->legacy_shift; }

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
    uint16_t KK = instruction & 0x00FF;
    uint16_t NNN = instruction & 0x0FFF;

    // Decode current instruction
    switch (T) {
    case 0x0:
      switch (KK) {
      case 0xE0: // 00E0 - CLS
        // Clear the display
        std::fill(this->screen.begin(), this->screen.end(), false);
        break;
      case 0xEE: // 00EE - RET
        // Return from a subroutine
        this->PC = this->stack[this->SP];
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
      this->SP++;
      this->stack[this->SP] = this->PC;
      this->PC = NNN;
      break;
    case 0x3:
      // 3XKK - SE VX, byte
      // Skip next instruction if VX = KK
      if (this->V[X] == KK)
        this->PC += 2;
      break;
    case 0x4:
      // 4XKK - SNE VX, byte
      // Skip next instruction if VX != KK
      if (this->V[X] != KK)
        this->PC += 2;
      break;
    case 0x5:
      // 5XY0 - SE VX, VY
      // Skip next instruction if VX = VY
      if (this->V[X] == this->V[Y])
        this->PC += 2;
      break;
    case 0x6:
      // 6XKK - LD VX, byte
      // Set VX = KK
      this->V[X] = KK;
      break;
    case 0x7:
      // 7XKK - ADD VX, byte
      // Set VX = VX + KK
      this->V[X] += KK;
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
        this->V[0xF] =
            ((uint16_t)this->V[X] + (uint16_t)this->V[Y] > 0xFF ? 1 : 0);
        // Only store low byte of the result
        this->V[X] += this->V[Y];
        break;
      case 0x5: // 8XY5 - SUB VX, VY
        // If VX > VY, VF = 1 else VF = 0
        // Set VX = VX - VY, set VF = NOT borrow
        this->V[0xF] = (this->V[X] > this->V[Y] ? 1 : 0);
        this->V[X] -= this->V[Y];
        break;
      case 0x6: // 8XY6 - SHR VX {, VY}
        // Set VX = VX SHR 1
        // If least-significant bit of VX is 1, VF = 1 else VF = 0, then VX >>
        // 1
        if (this->legacy_shift) {
          this->V[X] = this->V[Y];
        }
        // Flag if least-significant bit will be lost
        this->V[0xF] = (this->V[X] & 0b1);
        // Shift VX right once
        this->V[X] >>= 1;
        break;
      case 0x7: // 8XY7 - SUBN VX, VY
        // Set VX = VY - VX, set VF = NOT borrow
        // IF VY > VX, then VF = 1 else VF = 0
        this->V[0xF] = (this->V[Y] > this->V[X] ? 1 : 0);
        this->V[X] = this->V[Y] - this->V[X];
        break;
      case 0xE: // 8XYE - SHL VX {, VY}
        // Set VX = VY SHL 1
        // OR
        // Set VX = VX SHL 1
        // If most-significant bit of VX is 1, VF = 1 else VF = 0, then VX <<
        // 1
        if (this->legacy_shift) {
          this->V[X] = this->V[Y];
        }
        // Flag if most-significant bit will be lost
        this->V[0xF] = ((this->V[X] >> 7) & 0b1);
        // Shift VX left once
        this->V[X] <<= 1;
        break;
      default:
        break;
      }
      break;
    case 0x9: // 9XY0 - SNE VX, VY
      // Skip next instruction if VX != VY
      if (this->V[X] != this->V[Y]) {
        this->PC += 2;
      }
      break;
    case 0xA: // ANNN - LD I, addr
      // Set I = NNN
      this->I = NNN;
      break;
    case 0xB: // BNNN - JP V0, addr
      // Jump to location NNN + V0
      this->PC = NNN + this->V[0];
      break;
    case 0xC: // CXKK - RND VX, byte
      // Set VX = random byte AND KK
      this->V[X] = (rand() % 256) & KK;
      break;
    case 0xD: // DXYN - DRW VX, VY, nibble
      // TODO: Go over this implementation again to verify correctness
      // Display n-byte sprite starting at memory location I at (VX, VY),
      // set VF = collision
      // Reset VF flag
      this->V[0xF] = 0;

      int x;
      int y;

      // Go through each byte of sprite
      for (int n = 0; n < N; n++) {
        // Wrap bottom to top
        y = (this->V[Y] + n) % CHIP8::screen_height;
        // Grab a byte from the sprite
        uint8_t sprite_byte = this->memory[this->I + n];
        // Go through each bit of sprite byte
        for (int b = 7; b >= 0; b--) {
          x = (this->V[X] + (7 - b)) % CHIP8::screen_width;
          uint8_t sprite_bit = (sprite_byte >> b) & 0b1;
          // When screen and sprite pixels are on, set collision flag
          if (this->screen[CHIP8::screen_width * y + x] && sprite_bit) {
            this->V[0xF] = 1;
          }
          // XOR bit onto screen
          this->screen[CHIP8::screen_width * y + x] =
              (this->screen[CHIP8::screen_width * y + x] != sprite_bit);
        }
      }
      break;
    case 0xE:
      switch (KK) {
      case (0x9E): // EX9E - SKP
        // TODO: Need to implement keyboard handling first
        // Skip next instruction if key with the value of VX is pressed
        std::cerr << "Instruction EX9E not implemented..." << std::endl;
        break;
      case (0xA1): // EXA1 - SKNP VX
        // TODO: Need to implement keyboard handling first
        // Skip next instruction if key with the value of VX is not pressed
        std::cerr << "Instruction EXA1 not implemented..." << std::endl;
        break;
      default:
        break;
      }
      break;
    case 0xF:
      switch (KK) {
      case 0x07: // FX07 - LD VX, DT
        // Set VX = delay timer value
        this->V[X] = this->DT;
        break;
      case 0x0A: // FX0A - LD VX, K
        // TODO: Need to implement keyboard handling first
        // Wait for key press then save value to VX
        std::cerr << "Instruction FX0A not implemented..." << std::endl;
        break;
      case 0x15: // FX15 - LD DT, VX
        // FX15 - LD DT, VX
        // Set delay timer to VX
        this->DT = this->V[X];
        break;
      case 0x18: // FX18 - LD ST, VX
        // Set sound timer to VX
        this->ST = this->V[X];
        break;
      case 0x1E: // FX1E - ADD I, VX
        // Set I to I + VX
        this->I += this->V[X];
        break;
      case 0x29: // FX29 - LD F, VX
        // Set I to location of sprite for VX
        // TODO: Need to verify that this indexing is correct
        this->I = this->memory[CHIP8::fontset_start_address + 5 * this->V[X]];
        break;
      case 0x33: // FX33 - LD B, VX
        // Store BCD representation of VX in memory locations I, I+1, and I+2
        this->memory[this->I] = this->V[X] / 100;            // Hundreds digit
        this->memory[this->I + 1] = (this->V[X] % 100) / 10; // Tens digit
        this->memory[this->I + 2] = (this->V[X] % 10);       // Ones digit
        break;
      case 0x55: // FX55 - LD [I], VX
        // Store registers V0 through VX in memory starting at location I
        for (int i = 0; i < X; i++) {
          this->memory[this->I + i] = this->V[i];
        }
        break;
      case 0x65: // FX65 - LD VX, [I]
        // Read registers V0 through VX from memory starting at location I
        for (int i = 0; i < X; i++) {
          this->V[i] = this->memory[this->I + i];
        }
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
