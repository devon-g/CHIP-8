#include "interpreter.hpp"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "graphics.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>

static const std::array<SDL_Scancode, 16> keys{
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V,
};

Interpreter::Interpreter() {
  std::cout << "[INFO] Interpreter: Initializing SDL" << std::endl;
  init_sdl("CHIP-8", &this->window, &this->renderer);
  std::cout << "[INFO] Interpreter: Initializing ImGui" << std::endl;
  init_imgui(this->window, this->renderer);
  int window_width;
  SDL_GetWindowSize(this->window, &window_width, nullptr);
  this->cell_size = window_width / this->display.get_width();
}

Interpreter::~Interpreter() {
  std::cout << "[INFO] Interpreter: Shutting down ImGui" << std::endl;
  shutdown_imgui();
  std::cout << "[INFO] Interpreter: Shutting down SDL" << std::endl;
  shutdown_sdl(this->window, this->renderer);
}

void Interpreter::load_rom(const char *filename) {
  // Open the rom
  std::streampos rom_size;
  std::ifstream rom(filename, std::ios::binary);

  if (rom.is_open()) {
    // Get rom size
    rom.seekg(0, std::ios::end);
    rom_size = rom.tellg();
    rom.seekg(0, std::ios::beg);

    // Reserve space for the rom data
    auto *rom_data = new uint8_t[rom_size];
    rom.read((char *)rom_data, rom_size);
    rom.close();

    for (int i = 0; i < rom_size; i++)
      this->c8.memory[CHIP8::program_start_address + i] = rom_data[i];

    delete[] rom_data;
  } else {
    std::cerr << "[ERROR] Failed to load rom" << std::endl;
  }
}

void Interpreter::handle_events() {
  // Non blocking polling for sdl2 events
  // (e.g. key presses, x button to quit, etc)
  while (SDL_PollEvent(&this->event)) {
    ImGui_ImplSDL2_ProcessEvent(&this->event);
    // Stop interpreter if user closes program
    if (this->event.type == SDL_QUIT)
      this->running = false;
    // Send keypresses to the emulated keyboard and ImGui
    if (!ImGui::GetIO().WantCaptureKeyboard) {
      if (this->event.type == SDL_KEYDOWN) {
        // Look for pressed key in CHIP8 keymap
        auto iterator = std::find(keys.begin(), keys.end(),
                                  this->event.key.keysym.scancode);
        if (iterator != keys.end()) {
          this->keyboard.set_key(std::distance(keys.begin(), iterator));
          this->keyboard.set_key_pressed(true);
          std::cout << "[DEBUG] Key pressed: " << std::hex << std::uppercase
                    << keys[this->keyboard.get_key()] << std::dec
                    << std::nouppercase << std::endl;
        }
      }
      if (this->event.type == SDL_KEYUP)
        this->keyboard.set_key_pressed(false);
    }
  }
}

void Interpreter::pre_render() {
  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
  SDL_RenderSetScale(this->renderer, ImGui::GetIO().DisplayFramebufferScale.x,
                     ImGui::GetIO().DisplayFramebufferScale.y);
}

void Interpreter::render() {
  // ImGui::ShowDemoWindow();
  // Custom ImGui Window
  {
    if (ImGui::Begin("Debug")) {
      if (ImGui::CollapsingHeader("CHIP8")) {
        if (ImGui::BeginTable("Registers", 2,
                              ImGuiTableFlags_Borders +
                                  ImGuiTableFlags_Resizable)) {
          ImGui::TableSetupColumn("Register");
          ImGui::TableSetupColumn("Value");
          ImGui::TableHeadersRow();
          for (int row = 0; row < this->c8.V.size(); row++) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("V%X", row);
            ImGui::TableNextColumn();
            ImGui::Text("0x%X", this->c8.V[row]);
          }
        }
        ImGui::EndTable();

        ImGui::Text("Opcode: 0x%x", this->c8.opcode);
        ImGui::Text("PC: 0x%X", this->c8.PC);
        ImGui::Text("SP: 0x%X", this->c8.SP);
        ImGui::Text("I: 0x%X", this->c8.I);
        ImGui::Text("DT: 0x%X", this->c8.DT);
        ImGui::Text("ST: 0x%X", this->c8.ST);
      }
      if (ImGui::CollapsingHeader("Display")) {
      }
      if (ImGui::CollapsingHeader("Keyboard")) {
        ImGui::Text("Pressed: %3x", this->keyboard.get_key());
      }
    }

    ImGui::End();
  }

  // Convert chip8 display pixels to sdl rectangles
  auto display_buffer = this->display.get_buffer();
  // Collect rectangles to be drawn
  std::vector<SDL_Rect> rects;
  for (int i = 0; i < display_buffer.size(); i++) {
    if (display_buffer[i]) {
      rects.push_back(
          SDL_Rect{(i % (int)this->display.get_width()) * this->cell_size,
                   (i / (int)this->display.get_width()) * this->cell_size,
                   this->cell_size, this->cell_size}); // {x * w, y * w, w, h}
    }
  }

  ImGui::Render();
  // Clear old pixels
  if (!SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE))
    std::cerr << "[ERROR] SDL_SetDrawColor: " << SDL_GetError() << std::endl;
  if (!SDL_RenderClear(renderer))
    std::cerr << "[ERROR] SDL_RenderClear: " << SDL_GetError() << std::endl;
  // Render white rectangles representing pixels
  if (!SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE))
    std::cerr << "[ERROR] SDL_SetDrawColor: " << SDL_GetError() << std::endl;
  if (!SDL_RenderFillRects(renderer, rects.data(), rects.size()))
    std::cerr << "[ERROR] SDL_RenderFillRects: " << SDL_GetError() << std::endl;

  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

void Interpreter::post_render() {
  // Update window to match rendered shapes
  SDL_RenderPresent(renderer);

  // Interpret next instruction from rom
  c8.step();
}
