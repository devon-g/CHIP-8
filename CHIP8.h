//
// Created by devon on 6/22/22.
//

#ifndef CHIP_8_CHIP8_H
#define CHIP_8_CHIP8_H

#include <array>
#include <cinttypes>

class CHIP8 {
public:
  static const unsigned int screen_width = 64;
  static const unsigned int screen_height = 32;
  static const unsigned int fontset_size = 80;

  static const unsigned int fontset_start_address = 0x50;
  static const unsigned int program_start_address = 0x200;

  CHIP8();
  void load_rom(const char *filename);
  void toggle_legacy();
  void advance();

//private:
  // System memory (4kbs)
  std::array<uint8_t, 4096> memory{};
  // Miscellaneous Registers
  std::array<uint8_t, 16> V{};
  // Stack (for returning from subroutines)
  std::array<uint16_t, 16> stack{};
  // Video buffer
  std::array<bool, CHIP8::screen_width * CHIP8::screen_height> screen{};

  // Program counter (starts at memory address 0x200)
  uint16_t PC;
  // Stack pointer
  uint8_t SP;
  // Addressing Register
  uint16_t I;
  // Special sound and delay timer V
  uint8_t DT;
  uint8_t ST;
  // Shift instructions differ based on time period
  // Use this toggle to switch between legacy and modern
  bool legacy_shift;
};

#endif // CHIP_8_CHIP8_H
