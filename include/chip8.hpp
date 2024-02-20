#ifndef CHIP8_H
#define CHIP8_H

#include "display.hpp"
#include "keyboard.hpp"
#include <array>
#include <cstdint>

class CHIP8 {
public:
  static const unsigned int fontset_size = 80;
  static const uint16_t fontset_start_address = 0x50;
  static const uint16_t program_start_address = 0x200;

  void init(Display *display, Keyboard *keyboard);
  void load_rom(const char *filename);
  void step();

  // Periferals
  Display *display;
  Keyboard *keyboard;
  // System memory (4kbs)
  std::array<uint8_t, 4096> memory{};
  // Miscellaneous Registers
  std::array<uint8_t, 16> V{};
  // Stack (for returning from subroutines)
  std::array<uint16_t, 16> stack{};

  // Current instruction
  uint16_t opcode;
  // Program counter (starts at memory address 0x200)
  uint16_t PC;
  // Stack pointer
  uint8_t SP;
  // Addressing Register
  uint16_t I;
  // Special timers
  uint8_t DT; // Delay
  uint8_t ST; // Sound
};

#endif
