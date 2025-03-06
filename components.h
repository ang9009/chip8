#include <stdint.h>
#include <stdbool.h>

// Screen dimensions
#define SCREEN_W 64
#define SCREEN_H 32

// Components
typedef struct {
    uint8_t ram[4000];
    bool display[SCREEN_W][SCREEN_H];
    uint16_t I;
    uint16_t stack[12]; 
    uint16_t *SP; // Stack pointer
    uint16_t PC; // Program counter
    uint8_t delay_timer; 
    uint8_t sound_timer;
    uint8_t V[16]; // Registers
} chip8;