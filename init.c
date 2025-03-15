#include <stdbool.h>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "chip8.h"
#include "init.h"

// Initializes the SDL library with video, audio, and timer subsystems
bool init_sdl(sdl_t* sdl, config_t config) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
    SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
    return false;
  }

  sdl->window = SDL_CreateWindow(
      "Chip8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      SCREEN_W * config.scale, SCREEN_H * config.scale, 0);
  if (!sdl->window) {
    SDL_Log("Could not initialize window: %s\n", SDL_GetError());
    return false;
  }

  // Open audio device
  const char* audio_dev_name;
  for (int i = 0; i < SDL_GetNumAudioDevices(0); i++) {
    const char* name = SDL_GetAudioDeviceName(i, 0);
    if (name != NULL) {
      audio_dev_name = name;
      break;
    }
  }
  if (audio_dev_name == NULL) {
    SDL_Log("Could not find valid audio device: %s\n", SDL_GetError());
    return false;
  }
  // ! Audio incomplete
  // sdl->audio_dev_id = SDL_OpenAudioDevice(audio_dev_name, 0, )

  // Initialize renderer
  sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
  if (!sdl->renderer) {
    SDL_Log("Could not initialize renderer: %s\n", SDL_GetError());
    return false;
  }

  return true;
}

/**
 *  Initializes the chip8 struct. Clears all values in struct (if there were any), then
 *  copies fonts to very beginning of RAM. Returns whether initialization was successful.
 */
bool init_chip8(chip8_t* chip8, char* rom_name) {
  const uint8_t font[] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
      0x20, 0x60, 0x20, 0x20, 0x70,  // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
      0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
      0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
      0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
      0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
      0xF0, 0x80, 0xF0, 0x80, 0x80,  // F
  };

  memset(chip8, 0, sizeof(chip8_t));

  // Copy fonts into RAM, read ROM into RAM
  memcpy(&(chip8->ram[0x050]), font, sizeof(font));
  const size_t entrypoint = 0x200;
  if (!read_rom_into_ram(chip8, rom_name, entrypoint)) {
    return false;
  }

  chip8->PC = entrypoint;
  chip8->state = RUNNING;
  chip8->SP = &chip8->stack[0];

  return true;
}

/**
 * Reads a specified ROM into the RAM of the given chip8 struct at the given RAM entrypoint.
 * Returns whether the operation was successful, and logs an error message if it was not.
 */
bool read_rom_into_ram(chip8_t* chip8, char* rom_name, size_t entrypoint) {
  if (entrypoint > sizeof(chip8->ram)) {
    SDL_Log("Invalid RAM access");
    return false;
  }

  FILE* file_ptr = fopen(rom_name, "r");
  if (file_ptr == NULL) {
    SDL_Log("Could not open rom file \"%s\"", rom_name);
    return false;
  }

  if (fseek(file_ptr, 0, SEEK_END) != 0) {
    SDL_Log("Could not find end of rom file \"%s\"", rom_name);
    return false;
  }
  const long file_size = ftell(file_ptr);
  rewind(file_ptr);

  if (file_size == -1L) {
    SDL_Log("Failed to get size of rom file \"%s\"", rom_name);
    return false;
  } else if ((size_t)file_size > sizeof(chip8->ram) - entrypoint) {
    SDL_Log("File size of rom \"%s\" is too big (%ld bytes)", rom_name,
            file_size);
    return false;
  }

  if (fread(&(chip8->ram[entrypoint]), file_size, 1, file_ptr) != 1) {
    SDL_Log("Failed to read all contents of rom file \"%s\" into RAM",
            rom_name);
    return false;
  }
  return true;
}

// Handles user input
void handle_input(chip8_t* chip8) {
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
bool init_flags(flags_t* flags, int argc, char** argv) {
  // Since 0 is a valid default enum value, we init to -1 first
  flags->version = UNINITIALIZED;

  for (int i = 1; i < argc - 1; i++) {
    if (strcmp(argv[i], ROM_FLAG) == 0) {
      flags->rom_name = argv[i + 1];
      i++;  // Skip flag value
    } else if (strcmp(argv[i], INSNS_FLAG) == 0) {
      char* endptr;
      flags->insns_per_sec = strtol(argv[i + 1], &endptr, 10);
      if (endptr == argv[i + 1]) {
        SDL_Log("Please enter a valid number for instructions per sec");
        return false;
      } else if (flags->insns_per_sec <= 60) {
        SDL_Log("Instructions per second must be > 60");
        return false;
      }
      i++;
    } else if (strcmp(argv[i], VERSION_FLAG) == 0) {
      if (strcmp(argv[i + 1], "COSMAC") == 0) {
        flags->version = COSMAC;
      } else if (strcmp(argv[i + 1], "CHIP48_SUPERCHIP") == 0) {
        flags->version = CHIP48_SUPERCHIP;
      } else {
        SDL_Log("Unrecognized chip8 version \"%s\" provided", argv[i + 1]);
        return false;
      }
      i++;
    } else {
      SDL_Log("Unknown flag: %s.", argv[i]);
      return false;
    }
  }

  //   Throw error if mandatory flags are missing
  if (flags->rom_name == 0 || flags->insns_per_sec == 0 ||
      flags->version == UNINITIALIZED) {
    SDL_Log("One or more flags are missing");
    return false;
  }

  return true;
}