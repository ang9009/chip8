#ifndef CHIP8_H_INCLUDED
#define CHIP8_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

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
    bool display[SCREEN_W][SCREEN_H];
    uint16_t I;
    uint16_t stack[12]; 
    uint16_t *SP; // Stack pointer
    uint16_t PC; // Program counter
    uint8_t delay_timer; 
    uint8_t sound_timer;
    uint8_t V[16]; // Registers
    state_t state;
} chip8_t;

#endif