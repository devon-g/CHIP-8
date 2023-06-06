#include "graphics.h"

SDL init_sdl(std::string title, uint w, uint h) {
  // Initialize video and event subsystems (event implicitly due to video)
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Could not initialize SDL: %s",
                 SDL_GetError());
  }

  // Create a window to render things onto
  SDL_Window *window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, w, h, 0);
  if (window == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "Could not create window: %s",
                 SDL_GetError());
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Could not create renderer: %s",
                 SDL_GetError());
  }

  return {window, renderer};
}

void shutdown_sdl(SDL sdl) {
  SDL_DestroyRenderer(sdl.renderer);
  SDL_DestroyWindow(sdl.window);
  SDL_Quit();
}
