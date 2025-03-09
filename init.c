#include <stdbool.h>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "init.h"
#include "chip8.h"

// Initializes the SDL library with video, audio, and timer subsystems
bool init_sdl(sdl_t *sdl, config_t config, char *rom_name) {
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

// Initializes the chip8 struct
void init_chip8(chip8_t *chip8) {
    const uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0   
        0x20, 0x60, 0x20, 0x20, 0x70,   // 1  
        0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2 
        0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,   // 4    
        0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
        0xF0, 0x80, 0xF0, 0x80, 0x80,   // F
    };
    
    // memset();

    chip8->state = RUNNING;
}

// Reads a specified ROM into the RAM of the given chip8 struct at the given RAM entrypoint
void read_rom_into_ram(chip8_t *chip8, char *rom_name, uint8_t entrypoint) {
    // ! Lots of things that are wrong with this lol use reference please
    FILE *file_ptr = fopen(rom_name, "r");
    if (file_ptr == NULL) {
        SDL_Log("Could not open rom file \"%s\"", rom_name);
        exit(EXIT_FAILURE);
    }

    if (fseek(file_ptr, 0, SEEK_END) != 0) {
        SDL_Log("Could not find end of rom file \"%s\"", rom_name);
        exit(EXIT_FAILURE);
    }
    rewind(file_ptr);

    long file_size = ftell(file_ptr);
    if (file_size == -1L) {
        SDL_Log("Failed to get size of rom file \"%s\"", rom_name);
        exit(EXIT_FAILURE);
    } else if (file_size > sizeof(chip8->ram) - entrypoint) {
        SDL_Log("File size of rom \"%s\" is too big (%ld bytes)", rom_name, file_size);
    }

    size_t read_size = fread(chip8->ram[entrypoint], sizeof(uint8_t), file_size, file_ptr);
    if (read_size != file_size) {
        SDL_Log("Failed to read all contents of rom file \"%s\" into RAM", rom_name);
    }
}

// Intializes the config struct. This should be called before the config struct is used
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
void handle_input(chip8_t *chip8) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                SDL_Log("Window was closed, stopping program");
                chip8->state = STOPPED;
                break;
            // ! Incomplete
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
                exit(EXIT_FAILURE);
            }
            i++; // Skip flag value
        } else if (strcmp(argv[i], INSNS_FLAG) == 0) {
            if (i + 1 < argc) {
                flags.insns_per_sec = strtol(argv[i + 1], NULL, 10);
            } else {
                SDL_Log(
                    "Please specify the number of instructions/sec: %s <insns_per_sec>", 
                    INSNS_FLAG
                );
                exit(EXIT_FAILURE);
            }
            i++;
        } else {
            SDL_Log("Unknown flag: %s.", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    return flags;
}