#include <stdbool.h>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "init.h"
#include "chip8.h"

// Initializes the SDL library with video, audio, and timer subsystems
bool init_sdl(sdl_t *sdl, config_t config) {
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

// Clears the screen
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

// Initializes the chip8 structure
void init_chip8(chip8 *chip8) {
    chip8->state = RUNNING;
}

// Intializes the config object. This should be called before the config object is used
bool init_config(config_t *config, uint32_t insns_per_sec) {
    // Set defaults
    *config = (config_t){
        .scale = 8,
        .pixel_color = 0xfffc7f,
        insns_per_sec = insns_per_sec ? insns_per_sec : 700,
    };

    return true;
}

// Handles user input
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

// Gets flags from the command line
flags_t get_flags(int argc, char **argv) {
    flags_t flags = {0};

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], ROM_FLAG) == 0) {
            if (i + 1 < argc) {
                flags.rom_file = argv[i + 1];
            } else {
                SDL_Log("Please specify a rom file: %s <rom>", ROM_FLAG);
            }
            i++; // Skip argument value
        } else if (strcmp(argv[i], INSNS_FLAG) == 0) {
            if (i + 1 < argc) {
                flags.insns_per_sec = strtol(argv[i + 1], NULL, 10);
            } else {
                SDL_Log(
                    "Please specify the number of instructions/sec: %s <insns_per_sec>", 
                    INSNS_FLAG
                );
            }
            i++;
        } else {
            SDL_Log("Unknown flag: %s.", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    return flags;
}
