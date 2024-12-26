#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "chip8.hpp"
#include "display.hpp"
#include "graphics.hpp"
#include "imgui.h"
#include "keyboard.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <algorithm>
#include <unordered_map>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

static const std::unordered_map<SDL_Keycode, Key> sdl_to_key{
    {SDLK_0, Key::ZERO},  {SDLK_1, Key::ONE},   {SDLK_2, Key::TWO},
    {SDLK_3, Key::THREE}, {SDLK_4, Key::FOUR},  {SDLK_5, Key::FIVE},
    {SDLK_6, Key::SIX},   {SDLK_7, Key::SEVEN}, {SDLK_8, Key::EIGHT},
    {SDLK_9, Key::NINE},  {SDLK_A, Key::A},     {SDLK_B, Key::B},
    {SDLK_C, Key::C},     {SDLK_D, Key::D},     {SDLK_E, Key::E},
    {SDLK_F, Key::F},
};

int main(int argc, char **argv) {
  SDL sdl = init_sdl("CHIP-8", 1280, 640);

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.IniFilename = nullptr;
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
      ImGui_ImplSDL3_ProcessEvent(&event);
      if (event.type == SDL_EVENT_QUIT ||
          (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
           event.window.windowID == SDL_GetWindowID(sdl.window))) {
        quit = true;
      }
      // Only send input to chip8 if imgui isn't capturing already
      if (!ImGui::GetIO().WantCaptureKeyboard) {
        if (event.type == SDL_EVENT_KEY_DOWN) {
          if (sdl_to_key.count(event.key.key)) {
            keyboard.set_pressed_key(sdl_to_key.at(event.key.key));
            SDL_LogDebug(0, "[DEBUG] Key pressed: %c\n", event.key.key);
          }
        }
        if (event.type == SDL_EVENT_KEY_UP)
          keyboard.set_pressed_key(Key::NONE);
      }
    }

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
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

    // Convert chip8 display pixels to sdl rectangles
    auto display_buffer = display.get_buffer();
    // Collect rectangles to be drawn
    std::vector<uint32_t> pixels(display_buffer.size(), 0);
    // Convert true to white and false to black
    std::transform(
        display_buffer.cbegin(), display_buffer.cend(), pixels.begin(),
        [](bool state) -> uint32_t { return state ? 0xFFFFFFFF : 0xFF000000; });


    // Clear old pixels
    SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(sdl.renderer);

    // Turn pixel color data into a texture that fills the entire renderer
    auto texture = SDL_CreateTexture(sdl.renderer, SDL_PIXELFORMAT_ARGB8888,
                                     SDL_TEXTUREACCESS_STATIC, DISPLAY_WIDTH,
                                     DISPLAY_HEIGHT);
    SDL_UpdateTexture(texture, nullptr, pixels.data(), DISPLAY_WIDTH * 4);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_RenderTexture(sdl.renderer, texture, nullptr, nullptr);

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdl.renderer);
    SDL_RenderPresent(sdl.renderer);

    SDL_DestroyTexture(texture);

    // Interpret next instruction from rom
    c8.step();
  }

  SDL_LogInfo(0, "[INFO] Quitting...\n");
  shutdown_sdl(sdl);
  return 0;
}
