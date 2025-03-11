#include "chip8.h"

/**
 * Executes one iteration of the chip8 fetch-decode-execute cycle.
 */
bool execute_cycle(chip8_t *chip8) {
    const uint16_t opcode = chip8->ram[chip8->PC] << 8 | chip8->ram[chip8->PC + 1];
    chip8->PC += 2;

    const uint8_t X = (opcode >> 8) & 0xF; // Second nibble
    const uint8_t Y = (opcode >> 4) & 0xF; // Third nibble
    const uint8_t N = opcode & 0xF; // Fourth nibble
    const uint8_t NN = opcode & 0xFF; // Second byte
    const uint16_t NNN = opcode & 0xFFF; // Second, third, and fourth nibbles

    switch (opcode >> 12 & 0xF) {
        case 0x0:
            if (opcode == 0x00E0) {
                chip8->update_screen = true;
            }
            break;
        case 0x1:
            chip8->PC = NNN;
            break;
        case 0x6:
            chip8->V[X] = NN;
            break;
        case 0x7:
            chip8->V[X] += NN;
            break;
        case 0xA:
            chip8->I = NNN;
            break;
        case 0xD:
        default:
            SDL_Log("Unknown instruction: %x", opcode >> 12 & 0xF);
            return false;
    }

    return true;
}