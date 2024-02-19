#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "chip8.hpp"
#include "display.hpp"
#include "graphics.hpp"
#include "imgui.h"
#include "keyboard.hpp"
#include <SDL.h>
#include <iostream>
#include <unordered_map>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

static const std::unordered_map<char, Key> sdl_to_key{
    {'0', Key::ZERO},  {'1', Key::ONE},  {'2', Key::TWO}, {'3', Key::THREE},
    {'4', Key::FOUR},  {'5', Key::FIVE}, {'6', Key::SIX}, {'7', Key::SEVEN},
    {'8', Key::EIGHT}, {'9', Key::NINE}, {'a', Key::A},   {'b', Key::B},
    {'c', Key::C},     {'d', Key::D},    {'e', Key::E},   {'f', Key::F},
};

int main(int argc, char **argv) {
  int cell_size;
  int *current_width = new int;
  SDL sdl = init_sdl("CHIP-8", 1280, 720);

  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ImGui::StyleColorsDark();

  Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  Keyboard keyboard;
  CHIP8 c8(&display, &keyboard);
  c8.load_rom(argv[1]);

  SDL_Event event;
  bool quit = false;
  while (!quit) {
    // Non blocking polling for sdl2 events
    // (e.g. key presses, x button to quit, etc)
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT ||
          (event.type == SDL_WINDOWEVENT &&
           event.window.event == SDL_WINDOWEVENT_CLOSE &&
           event.window.windowID == SDL_GetWindowID(sdl.window)))
        quit = true;
      if (!ImGui::GetIO().WantCaptureKeyboard) {
        if (event.type == SDL_KEYDOWN) {
          if (sdl_to_key.count(event.key.keysym.sym)) {
            keyboard.set_pressed_key(sdl_to_key.at(event.key.keysym.sym));
            std::cout << "[DEBUG] Key pressed: " << (char)event.key.keysym.sym
                      << std::endl;
          }
        }
        if (event.type == SDL_KEYUP)
          keyboard.set_pressed_key(Key::NONE);
      }
    }

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow();

    {
      if (ImGui::Begin("Debug")) {
        if (ImGui::CollapsingHeader("CHIP8")) {
          if (ImGui::BeginTable("Registers", 2,
                                ImGuiTableFlags_Borders +
                                    ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("Register");
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();
            for (int row = 0; row < c8.V.size(); row++) {
              ImGui::TableNextRow();
              ImGui::TableNextColumn();
              ImGui::Text("V%X", row);
              ImGui::TableNextColumn();
              ImGui::Text("0x%X", c8.V[row]);
            }
          }
          ImGui::EndTable();

          ImGui::Text("Opcode: 0x%x", c8.opcode);
          ImGui::Text("PC: 0x%x", c8.PC);
          ImGui::Text("SP: 0x%x", c8.SP);
          ImGui::Text("I: 0x%x", c8.I);
          ImGui::Text("DT: 0x%x", c8.DT);
          ImGui::Text("ST: 0x%x", c8.ST);
        }
        if (ImGui::CollapsingHeader("Display")) {
        }
        if (ImGui::CollapsingHeader("Keyboard")) {
          ImGui::Text("Pressed: %3x", keyboard.get_pressed_key());
        }
      }

      ImGui::End();
    }

    SDL_GetWindowSize(sdl.window, current_width, nullptr);
    cell_size = *current_width / DISPLAY_WIDTH;
    SDL_SetWindowSize(sdl.window, *current_width, cell_size * DISPLAY_HEIGHT);
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
    SDL_RenderSetScale(sdl.renderer, ImGui::GetIO().DisplayFramebufferScale.x,
                       ImGui::GetIO().DisplayFramebufferScale.y);
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
  delete current_width;
  return 0;
}
