#include "graphics.hpp"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <iostream>

SDL init_sdl(std::string title, uint w, uint h) {
  // Initialize video and event subsystems (event implicitly due to video)
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    std::cerr << "[ERROR] SDL_Init: " << SDL_GetError() << std::endl;

  // Window to render things onto
  SDL_Window *window =
      SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_RESIZABLE);
  if (window == nullptr)
    std::cerr << "[ERROR] SDL_CreateWindow: " << SDL_GetError() << std::endl;

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr)
    std::cerr << "[ERROR] SDL_CreateRenderer: " << SDL_GetError() << std::endl;

  SDL_RendererInfo info;
  SDL_GetRendererInfo(renderer, &info);
  std::cout << "[INFO] SDL_Renderer: " << info.name << std::endl;

  // Set up ImGui stuff
  ImGui::CreateContext();
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  return {window, renderer};
}

void shutdown_sdl(SDL sdl) {
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_DestroyRenderer(sdl.renderer);
  SDL_DestroyWindow(sdl.window);
  SDL_Quit();
}
