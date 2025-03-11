#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include "SDL2/SDL.h"
#include "init.h"

// Clears the screen with black
void clear_screen(SDL_Renderer *renderer);

// Cleans up all sdl related processes
void cleanup_sdl(const sdl_t sdl);

#endif