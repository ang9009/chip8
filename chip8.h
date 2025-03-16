#ifndef CHIP8_H_INCLUDED
#define CHIP8_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

// Chip8 specs
#define SCREEN_W 64
#define SCREEN_H 32

// chip8 state
typedef enum {
  RUNNING,
  PAUSED,
  STOPPED,
} state_t;

// chip8 components
typedef struct {
  uint8_t ram[4096];
  bool display[SCREEN_H][SCREEN_W];
  uint16_t I;
  uint16_t stack[12];
  uint16_t* SP;  // Stack pointer
  uint16_t PC;   // Program counter
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint8_t V[16];    // Registers
  state_t state;    // Current state
  bool keypad[16];  // Chip8's 16 digit keypad
  bool
      update_screen;  // Indicates if SDL should update the screen on the next tick
} chip8_t;

#endif