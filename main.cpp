#include "CHIP8.h"
#include "graphics.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  const int cell_size = 20;
  const int window_width = cell_size * CHIP8::screen_width;
  const int window_height = cell_size * CHIP8::screen_height;

  SDL sdl = init_sdl("CHIP-8", window_width, window_height);
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

    // Convert chip8 display pixels to sdl rectangles
    auto display = c8.get_display();
    // Collect rectangles to be drawn
    std::vector<SDL_Rect> rects;
    for (int i = 0; i < display.size(); i++) {
      if (display[i]) {
        rects.push_back(SDL_Rect{(i % (int)CHIP8::screen_width) * cell_size,
                                 (i / (int)CHIP8::screen_width) * cell_size,
                                 cell_size, cell_size}); // {x * w, y * w, w, h}
      }
    }
    // Clear old pixels
    SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(sdl.renderer);
    // Render white rectangles representing pixels
    SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(sdl.renderer, rects.data(), rects.size());

    // Update window to match rendered shapes
    SDL_RenderPresent(sdl.renderer);

    // Interpret next instruction from rom
    c8.step();
  }

  std::cout << "[INFO] Quitting..." << std::endl;
  shutdown_sdl(sdl);
  return 0;
}
