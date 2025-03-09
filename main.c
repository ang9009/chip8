#include <stdio.h>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "chip8.h"
#include "init.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        SDL_Log(
            "Invalid no. of arguments. Usage: %s %s <rom_file> %s <hz>", 
            argv[0],
            ROM_FLAG,
            INSNS_FLAG
        );
        exit(EXIT_FAILURE);
    }

    sdl_t sdl = {0};
    config_t config = {0};
    flags_t flags = get_flags(argc, argv);
    
    SDL_Log("Rom file: %s, instructions/sec: %d", flags.rom_file, flags.insns_per_sec);
    // Initialize config object, then SDL
    if (!init_config(&config, flags.insns_per_sec) || !init_sdl(&sdl, config)) {
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