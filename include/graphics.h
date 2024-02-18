#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL.h>
#include <string>

struct SDL {
  SDL_Window *window;
  SDL_Renderer *renderer;
};

SDL init_sdl(std::string title, uint w, uint h);
void shutdown_sdl(SDL sdl);

#endif
