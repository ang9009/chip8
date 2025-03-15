#ifndef CYCLE_H_INCLUDED
#define CYCLE_H_INCLUDED

#include <stdbool.h>

#include "chip8.h"
#include "init.h"

/**
 * Executes one iteration of the chip8 fetch-decode-execute cycle.
 */
bool execute_cycle(chip8_t* chip8, chip8_ver_t ver, bool debug);

#endif