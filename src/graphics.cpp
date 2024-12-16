#include "graphics.hpp"
#include "SDL3/SDL_render.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include <iostream>

SDL init_sdl(std::string title, uint w, uint h) {
  // Initialize video and event subsystems (event implicitly due to video)
  if (!SDL_Init(SDL_INIT_VIDEO))
    std::cerr << "[ERROR] SDL_Init: " << SDL_GetError() << std::endl;

  // Window to render things onto
  SDL_Window *window = SDL_CreateWindow("CHIP-8", w, h, SDL_WINDOW_RESIZABLE);
  if (window == nullptr)
    std::cerr << "[ERROR] SDL_CreateWindow: " << SDL_GetError() << std::endl;

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, nullptr);
  if (renderer == nullptr)
    std::cerr << "[ERROR] SDL_CreateRenderer: " << SDL_GetError() << std::endl;

  std::cout << "[INFO] SDL_Renderer: " << SDL_GetRendererName(renderer)
            << std::endl;

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
