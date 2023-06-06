#ifndef GRAPHICSSDL_H
#define GRAPHICSSDL_H

#include <SDL2/SDL.h>
#include <string>

class GraphicsSDL {
public:
  GraphicsSDL(std::string title, uint w, uint h);
  ~GraphicsSDL();

  const SDL_Window *get_window();
  const SDL_Renderer *get_renderer();

private:
  SDL_Window *window;
  SDL_Renderer *renderer;
};

#endif
