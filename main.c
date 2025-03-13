#include <stdio.h>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "chip8.h"
#include "init.h"
#include "render.h"
#include "cycle.h"

int main(int argc, char **argv) {
    if (argc < 5) {
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
        .scale = 6,
        .pixel_color = 0xfffc7f,
        .insns_per_sec = flags.insns_per_sec ? flags.insns_per_sec : 700,
    };
    
    SDL_Log("Rom file: %s, instructions/sec: %d", flags.rom_name, flags.insns_per_sec);
    if (!init_sdl(&sdl, config)) {
        exit(EXIT_FAILURE);
    }

    chip8_t chip8 = {0};
    if (!init_chip8(&chip8, flags.rom_name)) {
        exit(EXIT_FAILURE);
    }

    // Set background to black
    // clear_screen(sdl.renderer);
    
    while (chip8.state != STOPPED) {
        handle_input(&chip8);

        const uint64_t start_count = SDL_GetPerformanceCounter();
        // We need to execute (insns per sec / 60) insns for every refresh to achieve 60Hz
        // (insns per sec / 60) * 60 refreshes per sec = insns per sec as desired
        for (int i = 0; i < ((int) flags.insns_per_sec) / 60; i++) {
            // Fetch decode execute helper
            execute_cycle(&chip8, true);
        }
        const uint64_t end_count = SDL_GetPerformanceCounter();
        const uint64_t count_per_sec = SDL_GetPerformanceFrequency();
        const double dt_ms = (((double)(end_count - start_count) / count_per_sec)) * 1000;

        // 1 / 60 = 16.67ms. We need to delay each frame by at least this amount to achieve 60hz
        const uint32_t delay = dt_ms > 16.67 ? 0 : 16.67 - dt_ms;
        SDL_Delay(delay);

        // Refresh window if needed
        if (chip8.update_screen) {
            update_screen(chip8, config, &sdl);
            chip8.update_screen = false;
        }

        // Update timer values
        if (chip8.delay_timer > 0) {
            chip8.delay_timer--;
        }
        if (chip8.sound_timer > 0) {
            chip8.sound_timer--;
        }
    }

    cleanup_sdl(sdl);
    exit(EXIT_SUCCESS);
    return 0; // Idk
}