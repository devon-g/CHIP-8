//
// Created by devon on 5/2/22.
//

#include "CHIP8.h"
#include <SDL2/SDL.h>
#include <iomanip>
#include <iostream>

int main(int argc, char **argv) {
  CHIP8 c8;
  c8.load_rom(argv[1]);

  bool quit = false;
  int idx = 0;
  while (!quit) {
    // Draw screen contents to terminal
    for (int i = 0; i < 50; i++) {
      std::cout << '\n';
    }
    for (int y = 0; y < CHIP8::screen_height; y++) {
      for (int x = 0; x < CHIP8::screen_width; x++)
        std::cout << (c8.display[CHIP8::screen_width * y + x] ? "█" : " ");
      std::cout << '\n';
    }
    std::cout << std::flush;
    c8.advance();
  }

  // int stride = 16;
  // std::cout << std::hex << std::setfill('0') << std::uppercase;
  // for (int i = 0; i < c8.memory.size(); i += stride) {
  //   std::cout << "0x" << std::setw(3) << i << ":\t";
  //   for (int j = 0; j < stride; j++) {
  //     std::cout << "0x" << std::setw(2)
  //               << unsigned(c8.memory[i + j])
  //               << '\t';
  //   }
  //   std::cout << std::endl;
  // }

  return 0;
}
