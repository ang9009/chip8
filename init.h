#ifndef INIT_H_INCLUDED
#define INIT_H_INCLUDED

#include <stdbool.h>
#include "SDL2/SDL.h"
#include "chip8.h"

// Command line flags
#define ROM_FLAG "--rom"
#define INSNS_FLAG "--insns-per-sec"
#define VERSION_FLAG "--v"

#define FONT_ENTRYPOINT 0x050

// Used to differentiate between different versions of chip8 (quirks)
typedef enum {
  UNINITIALIZED,
  COSMAC,
  CHIP48_SCHIP,  // Chip48 or Superchip variants
} chip8_ver_t;

typedef struct {
  char* rom_name;          // The name of the rom file
  uint32_t insns_per_sec;  // The number of instructions per second (hz)
  chip8_ver_t version;
} flags_t;

typedef struct {
  uint32_t scale;          // The scaling factor of each pixel
  uint32_t pixel_color;    // The color of each pixel in the foreground
  uint32_t insns_per_sec;  // The number of instructions per second (hz)
  chip8_ver_t version;
  bool debug;  // If debug statements should be printed for each instruction
} config_t;

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_AudioDeviceID audio_dev_id;
} sdl_t;

// Initializes the SDL library with video, audio, and timer subsystems
bool init_sdl(sdl_t* sdl, config_t config);

// Initializes the chip8 structure
bool init_chip8(chip8_t* chip8, char* rom_name);

// Gets flags from the command line
bool init_flags(flags_t* flags, int argc, char** argv);

/**
 * Reads a specified ROM into the RAM of the given chip8 struct at the given RAM entrypoint.
 * Returns whether the operation was successful, and logs an error message if it was not.
 */
bool read_rom_into_ram(chip8_t* chip8, char* rom_name, size_t entrypoint);

#endif