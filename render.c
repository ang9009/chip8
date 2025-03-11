#include "SDL2/SDL.h"
#include "init.h"

// Clears the screen with black
void clear_screen(SDL_Renderer *renderer) {    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}

// Cleans up all sdl related processes
void cleanup_sdl(const sdl_t sdl) {
    SDL_DestroyWindow(sdl.window);
    SDL_DestroyRenderer(sdl.renderer);
    SDL_Quit();
}
