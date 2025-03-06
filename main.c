#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "SDL2/SDL.h"
#include "components.h"


typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl;

typedef struct {
    int scale; // The scaling factor of each pixel
} config;

// Initializes the SDL library with vidoe, audio, and timer subsystems.
bool init_sdl(sdl *sdl, config config) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow(
        "Chip8 Emulator", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        SCREEN_W * config.scale,
        SCREEN_H * config.scale,
        0
    );
    if (!sdl->window) {
        SDL_Log("Could not initialize window: %s\n", SDL_GetError());
        return false;
    }

    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
    if (!sdl->renderer) {
        SDL_Log("Could not initialize renderer: %s\n", SDL_GetError());
    }

    return true;
}

// Cleans up all sdl related processes.
void cleanup(const sdl sdl) {
    SDL_DestroyWindow(sdl.window);
    SDL_DestroyRenderer(sdl.renderer);
    SDL_Quit();
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    sdl sdl = {};
    config config = {64, 32, 4};
    
    // Initialize SDL
    if (!init_sdl(&sdl, config)) {
        exit(EXIT_FAILURE);
    }

    SDL_RenderClear(sdl.renderer);

    while (true) {
    }

    cleanup(sdl);
    exit(EXIT_SUCCESS);
}