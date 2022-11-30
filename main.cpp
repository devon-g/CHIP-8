//
// Created by devon on 5/2/22.
//

#include "CHIP8.h"
#include <SDL2/SDL.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

Uint32 decrement(Uint32 interval, void *param) {
  uint8_t *val = static_cast<uint8_t *>(param);
  if (*val > 0)
    *val -= 1;
  return interval;
}

int main(int argc, char **argv) {
  // Init random seed
  srand(time(nullptr));

  // Create a CHIP-8
  CHIP8 c8;

  // CHIP-8 display information
  const int CELL_SIZE = 20;

  // Compute SDL window size
  const int SCREEN_WIDTH = CELL_SIZE * CHIP8::SCREEN_WIDTH;
  const int SCREEN_HEIGHT = CELL_SIZE * CHIP8::SCREEN_HEIGHT;

  // Collection of SDL scancodes associated with CHIP8 keys
  const int N_KEYS = 16;
  const SDL_Scancode VALID_KEYS[N_KEYS] = {
      SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_C,
      SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_D,
      SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_E,
      SDL_SCANCODE_A, SDL_SCANCODE_0, SDL_SCANCODE_B, SDL_SCANCODE_F};

  // Window to display CHIP8 screen information
  SDL_Window *window;
  // Renderer for drawing CHIP8 screen information to window
  SDL_Renderer *renderer;
  SDL_Event event;
  // Timers for CHIP8 delay and sound
  SDL_TimerID delay_timer;
  SDL_TimerID sound_timer;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0)
    printf("Failed to initialize SDL: %s", SDL_GetError());
  window =
      SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // Attempt to load rom given through command line argument
  c8.load_rom(argv[1]);

  // Clear SDL window to black initially
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  // TODO: Implement sound and delay timers

  SDL_bool running = SDL_TRUE;
  while (running) {
    // Listen for events
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: // Stop main loop when a quit event is encountered
        running = SDL_FALSE;
        break;
      default: // Do nothing for events that aren't used
        break;
      }
    }

    // TODO: Handle key presses
    // const uint8_t *pressed_key = SDL_GetKeyboardState(nullptr);
    // bool valid_key_pressed = false;
    // for (SDL_Scancode k : VALID_KEYS)
    //   if (pressed_key[k] == 1)
    //     valid_key_pressed = true;
    //
    // c8.set_pressed_key(pressed_key);

    // Handle next instruction
    c8.advance();
  }

  // Clean up SDL objects
  SDL_RemoveTimer(delay_timer);
  SDL_RemoveTimer(sound_timer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  // Exit the program
  SDL_Quit();
  return 0;
}
