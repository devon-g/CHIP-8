#include "graphics.hpp"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include <iostream>

SDL init_sdl(std::string title, uint w, uint h) {
  // Initialize video and event subsystems (event implicitly due to video)
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(0, "[ERROR] SDL_Init: %s\n", SDL_GetError());
  }

  // Window to render things onto
  SDL_Window *window = SDL_CreateWindow("CHIP-8", w, h, SDL_WINDOW_RESIZABLE);
  if (window == nullptr) {
    SDL_LogError(0, "[ERROR] SDL_CreateWindow: %s\n", SDL_GetError());
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
  if (renderer == nullptr) {
    SDL_LogError(0, "[ERROR] SDL_CreateRenderer: %s\n", SDL_GetError());
  }

  SDL_LogInfo(0, "[INFO] SDL_Renderer: %s\n", SDL_GetRendererName(renderer));

  // Set up ImGui stuff
  ImGui::CreateContext();
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  return {window, renderer};
}

void shutdown_sdl(SDL sdl) {
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyRenderer(sdl.renderer);
  SDL_DestroyWindow(sdl.window);
  SDL_Quit();
}
