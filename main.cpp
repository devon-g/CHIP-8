#include "CHIP8.h"
#include "graphics.h"
#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char **argv) {
  const uint cell_size = 20;
  const uint window_width = cell_size * CHIP8::screen_width;
  const uint window_height = cell_size * CHIP8::screen_height;

  GraphicsSDL sdl("CHIP-8", window_width, window_height);
  SDL_Event event;

  CHIP8 c8;
  c8.load_rom(argv[1]);

  bool quit = false;
  while (!quit) {
    // Non blocking polling for sdl2 events
    // (e.g. key presses, x button to quit, etc)
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = true;
      }
    }

    // Interpret next instruction from rom
    c8.advance();
  }

  return 0;
}
