#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <SDL.h>
#include <string>

void init_sdl(std::string title, SDL_Window **window, SDL_Renderer **renderer);
void shutdown_sdl(SDL_Window *window, SDL_Renderer *renderer);
void init_imgui(SDL_Window *window, SDL_Renderer *renderer);
void shutdown_imgui();

#endif
