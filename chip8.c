#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "SDL.h"

/**
 * Initializes the SDL library with vidoe, audio, and timer subsystems.
 */
bool init_sdl(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void final_cleanup(void) {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    
    // Initialize SDL
    if (!init_sdl()) {
        exit(EXIT_FAILURE);
    }

    final_cleanup();

    exit(EXIT_SUCCESS);
}