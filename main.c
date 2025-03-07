#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "SDL2/SDL.h"
#include "chip8.h"


typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl;

typedef struct {
    uint32_t scale; // The scaling factor of each pixel
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

void clear_screen(SDL_Renderer *renderer) {    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // const u_int8_t r = (config.pixel_color) >> 24 & 0xFF;
    // const u_int8_t g = (config.pixel_color) >> 16 & 0xFF;
    // const u_int8_t b = (config.pixel_color) >> 8 & 0xFF;
    // const u_int8_t a = (config.pixel_color) & 0xFF;
    // SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

// Cleans up all sdl related processes.
void cleanup(const sdl sdl) {
    SDL_DestroyWindow(sdl.window);
    SDL_DestroyRenderer(sdl.renderer);
    SDL_Quit();
}

void init_chip8(chip8 *chip8) {
    chip8->state = RUNNING;
}

void handle_input(chip8 *chip8) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                SDL_Log("Window was closed, stopping program");
                chip8->state = STOPPED;
                break;
        }
    }
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    sdl sdl = {};
    config config = {8, 0xfffc7f};
    
    // Initialize SDL
    if (!init_sdl(&sdl, config)) {
        exit(EXIT_FAILURE);
    }

    chip8 chip8 = {};
    init_chip8(&chip8);
    
    while (chip8.state != STOPPED) {
        handle_input(&chip8);
        clear_screen(sdl.renderer);
    }

    cleanup(sdl);
    exit(EXIT_SUCCESS);
}