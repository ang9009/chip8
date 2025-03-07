#include <stdio.h>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "chip8.h"
#include "init.h"


int main(int argc, char **argv) {
    if (argc < 2) {
        SDL_Log(
            "Invalid no. of arguments. Usage: %s %s <rom_file> %s <hz>", 
            ROM_FLAG,
            INSNS_FLAG,
            argv[0]
        );
        exit(EXIT_FAILURE);
    }
    SDL_Log("Rom file: %s", argv[2]);

    sdl_t sdl = {0};
    config_t config = {0};
    
    // Initialize config object, then SDL
    if (!init_config(&config) || !init_sdl(&sdl, config)) {
        exit(EXIT_FAILURE);
    }

    chip8 chip8 = {};
    init_chip8(&chip8);
    
    while (chip8.state != STOPPED) {
        handle_input(&chip8);
        clear_screen(sdl.renderer);
    }

    cleanup_sdl(sdl);
    exit(EXIT_SUCCESS);
}