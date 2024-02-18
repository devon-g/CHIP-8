#include "CHIP8.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "display.h"
#include "graphics.h"
#include "imgui.h"
#include "keyboard.h"
#include <SDL.h>
#include <iostream>
#include <unordered_map>
#include <vector>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

std::unordered_map<char, Key> sdl_to_key{
    {'0', Key::ZERO},  {'1', Key::ONE},  {'2', Key::TWO}, {'3', Key::THREE},
    {'4', Key::FOUR},  {'5', Key::FIVE}, {'6', Key::SIX}, {'7', Key::SEVEN},
    {'8', Key::EIGHT}, {'9', Key::NINE}, {'a', Key::A},   {'b', Key::B},
    {'c', Key::C},     {'d', Key::D},    {'e', Key::E},   {'f', Key::F},
};

int main(int argc, char **argv) {
  const int cell_size = 20;
  const int window_width = cell_size * DISPLAY_WIDTH;
  const int window_height = cell_size * DISPLAY_HEIGHT;

  SDL sdl = init_sdl("CHIP-8", window_width, window_height);
  SDL_Event event;

  Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  Keyboard keyboard;
  CHIP8 c8(&display, &keyboard);
  c8.load_rom(argv[1]);

  bool quit = false;
  while (!quit) {
    // Non blocking polling for sdl2 events
    // (e.g. key presses, x button to quit, etc)
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_KEYDOWN:
        if (sdl_to_key.count(event.key.keysym.sym)) {
          keyboard.set_pressed_key(sdl_to_key.at(event.key.keysym.sym));
          std::cout << "[DEBUG] Key pressed: " << (char)event.key.keysym.sym
                    << std::endl;
        }
        break;
      case SDL_KEYUP:
        keyboard.set_pressed_key(Key::NONE);
      default:
        break;
      }
    }

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();

    // Convert chip8 display pixels to sdl rectangles
    auto display_buffer = display.get_buffer();
    // Collect rectangles to be drawn
    std::vector<SDL_Rect> rects;
    for (int i = 0; i < display_buffer.size(); i++) {
      if (display_buffer[i]) {
        rects.push_back(SDL_Rect{(i % (int)display.get_width()) * cell_size,
                                 (i / (int)display.get_width()) * cell_size,
                                 cell_size, cell_size}); // {x * w, y * w, w, h}
      }
    }
    ImGui::Render();
    SDL_RenderSetScale(sdl.renderer, sdl.io.DisplayFramebufferScale.x,
                       sdl.io.DisplayFramebufferScale.y);
    // Clear old pixels
    SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(sdl.renderer);
    // Render white rectangles representing pixels
    SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(sdl.renderer, rects.data(), rects.size());

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    // Update window to match rendered shapes
    SDL_RenderPresent(sdl.renderer);

    // Interpret next instruction from rom
    c8.step();
  }

  std::cout << "[INFO] Quitting..." << std::endl;
  shutdown_sdl(sdl);
  return 0;
}
