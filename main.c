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
    flags_t flags = {0};
    if (!init_flags(&flags, argc, argv)) {
        exit(EXIT_FAILURE);
    }
    config_t config = {
        .scale = 8,
        .pixel_color = 0xfffc7f,
        .insns_per_sec = flags.insns_per_sec ? flags.insns_per_sec : 700,
    };
    
    SDL_Log("Rom file: %s, instructions/sec: %d", flags.rom_name, flags.insns_per_sec);
    // Initialize config object, then SDL
    if (!init_sdl(&sdl, config)) {
        exit(EXIT_FAILURE);
    }

    chip8_t chip8 = {0};
    if (!init_chip8(&chip8, flags.rom_name)) {
        exit(EXIT_FAILURE);
    }
    
    while (chip8.state != STOPPED) {
        handle_input(&chip8);
        clear_screen(sdl.renderer);
    }

    cleanup_sdl(sdl);
    exit(EXIT_SUCCESS);
}