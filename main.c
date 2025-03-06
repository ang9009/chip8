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
    uint32_t pixel_color; // The color of each pixel in the foreground
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

// Sets up the renderer with the specified pixel color.
bool setup_renderer(config config) {
    // Convert from hex to RGB
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
    config config = {8, 0xFF};
    
    // Initialize SDL
    if (!init_sdl(&sdl, config)) {
        exit(EXIT_FAILURE);
    }
    
    while (true) {
        // Get rid of this lols
        SDL_SetRenderDrawColor(sdl.renderer, 255, 0, 0, 255);
        SDL_RenderClear(sdl.renderer);

        SDL_Delay(1000);
        SDL_RenderPresent(sdl.renderer);

        SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, 255);
        SDL_RenderClear(sdl.renderer);

        SDL_Delay(1000);
        SDL_RenderPresent(sdl.renderer);
    }

    cleanup(sdl);
    exit(EXIT_SUCCESS);
}