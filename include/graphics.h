#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "imgui.h"
#include <SDL.h>
#include <string>

struct SDL {
  SDL_Window *window;
  SDL_Renderer *renderer;
  ImGuiIO &io;
};

SDL init_sdl(std::string title, uint w, uint h);
void shutdown_sdl(SDL sdl);

#endif
