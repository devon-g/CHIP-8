#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "chip8.hpp"
#include "display.hpp"
#include <SDL.h>

class Interpreter {
public:
  Interpreter();
  ~Interpreter();

  void load_rom(const char *filename);

  // Run CHIP8, handle events, render display
  void run() {
    this->running = true;
    while (this->running) {
      handle_events();
      pre_render();
      render();
      post_render();
    }
  }

  // Process user input/actions
  void handle_events();
  // Prepare to render a new frame
  void pre_render();
  // Generate frame of CHIP8 display and ImGui menus
  void render();
  // Present rendered frame
  void post_render();

private:
  bool running;
  bool debug;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Event event;
  int cell_size;
  Display display;
  Keyboard keyboard;
  CHIP8 c8;
};

#endif // !INTERPRETER_HPP
