#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED

#include "SDL2/SDL.h"

// Command line flags
#define ROM_FLAG "--rom"
#define INSNS_FLAG "--insns-per-sec"

typedef struct {
    char *rom_file; // The name of the rom file
    uint32_t insns_per_sec; // The number of instructions per second (hz). This is mandatory.
} flags_t;


typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

typedef struct {
    uint32_t scale; // The scaling factor of each pixel
    uint32_t pixel_color; // The color of each pixel in the foreground
    uint32_t insns_per_sec; // The number of instructions per second (hz)
} config_t;

// Initializes the SDL library with video, audio, and timer subsystems
bool init_sdl(sdl_t *sdl, config_t config);

// Clears the screen
void clear_screen(SDL_Renderer *renderer);

// Cleans up all sdl related processes
void cleanup_sdl(const sdl_t sdl);

// Initializes the chip8 structure
void init_chip8(chip8 *chip8);

// Intializes the config object. This should be called before the config object is used
bool init_config(config_t *config, char *insns_per_sec);

// Handles user input
void handle_input(chip8 *chip8);

// Gets flags from the command line
flags_t get_flags(int argc, char **argv);


#endif