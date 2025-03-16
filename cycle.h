#ifndef CYCLE_H_INCLUDED
#define CYCLE_H_INCLUDED

#include <stdbool.h>

#include "chip8.h"
#include "init.h"

/**
 * Executes one iteration of the chip8 fetch-decode-execute cycle.
 */
bool execute_cycle(chip8_t* chip8, chip8_ver_t ver, bool debug);

/**
 * Handles user input, and updates the given keypad based on what the user is currently 
 * pressing.
 */
void handle_input(bool* keypad);

#endif