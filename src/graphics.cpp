#include "graphics.hpp"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <iostream>

void init_sdl(std::string title, SDL_Window **window, SDL_Renderer **renderer) {
  // Initialize video and event subsystems (event implicitly due to video)
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    std::cerr << "[ERROR] SDL_Init: " << SDL_GetError() << std::endl;

  // Window to render things onto
  *window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, 1280, 720, 0);
  if (window == nullptr)
    std::cerr << "[ERROR] SDL_CreateWindow: " << SDL_GetError() << std::endl;

  *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr)
    std::cerr << "[ERROR] SDL_CreateRenderer: " << SDL_GetError() << std::endl;
}

void shutdown_sdl(SDL_Window *window, SDL_Renderer *renderer) {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void init_imgui(SDL_Window *window, SDL_Renderer *renderer) {
  // Initialize
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  // Configure
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  ImGui::StyleColorsDark();
}

void shutdown_imgui() {
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}
