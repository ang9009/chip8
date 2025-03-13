#ifndef CYCLE_H_INCLUDED
#define CYCLE_H_INCLUDED

/**
 * Executes one iteration of the chip8 fetch-decode-execute cycle.
 */
bool execute_cycle(chip8_t *chip8, bool debug);

#endif