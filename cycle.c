#include "chip8.h"
#include "SDL2/SDL.h"

// Updates the display in the chip8 struct.
// ! There's something wrong with this and/or the render logic
void update_display(chip8_t *chip8, uint8_t X, uint8_t Y, uint8_t N) {
    int x_coord = chip8->V[X] % SCREEN_W;
    int y_coord = chip8->V[Y] % SCREEN_H;
    chip8->V[0xF] = 0;
    
    const uint16_t I = chip8->I;
    for (int k = 0; k < N; k++) {
        if (y_coord + k >= SCREEN_H) {
            break;
        }

        // Get nth byte of sprite data (1 row of the sprite)
        uint8_t data = chip8->ram[I + k];
        for (int j = 7; j >= 0; j--) {
            if (x_coord + (7 - j) >= SCREEN_W) {
                break;
            }

            bool *pixel_on = &(chip8->display[y_coord + k][x_coord + (7 - j)]);
            int curr_bit = (data >> j) & 0b1;
            if (curr_bit == 1) {
                if (*pixel_on) {
                    *pixel_on = false;
                    chip8->V[0xF] = 1;
                } else {
                    *pixel_on = true;
                }
            }
        }
    }
}


/**
 * Executes one iteration of the chip8 fetch-decode-execute cycle.
 */
bool execute_cycle(chip8_t *chip8, bool debug) {
    // Fetch insn from memory
    const uint16_t opcode = (chip8->ram[chip8->PC] << 8) | (chip8->ram[chip8->PC + 1]);
    chip8->PC += 2;

    const uint8_t X = (opcode >> 8) & 0xF; // Second nibble
    const uint8_t Y = (opcode >> 4) & 0xF; // Third nibble
    const uint8_t N = opcode & 0xF; // Fourth nibble
    const uint8_t NN = opcode & 0xFF; // Second byte
    const uint16_t NNN = opcode & 0xFFF; // Second, third, and fourth nibbles

    switch (opcode >> 12 & 0xF) {
        case 0x0:
            if (opcode == 0x00E0) {
                if (debug) SDL_Log("Clear screen");
                memset(&chip8->display[0], 0, sizeof chip8->display);
                chip8->update_screen = true;
            }
            break;
        case 0x1:
            if (debug) SDL_Log("Jump to 0x%04X", NNN);
            chip8->PC = NNN;
            break;
        case 0x6:
            if (debug) SDL_Log("Set register V%x to 0x%04X", X, NN);
            chip8->V[X] = NN;
            break;
        case 0x7:
            fprintf(stderr, "%x, %x", NN, X);
            if (debug) SDL_Log("Add 0x%04X to register V%x", NN, X);
            chip8->V[X] += NN;
            break;
        case 0xA:
            if (debug) SDL_Log("Set index register I to 0x%04X", NNN);
            chip8->I = NNN;
            break;
        case 0xD:
            if (debug) SDL_Log("Update display");
            update_display(chip8, X, Y, N);
            chip8->update_screen = true;
            break;
        default:
            SDL_Log("Unknown instruction: %x", opcode >> 12 & 0xF);
            return false;
    }

    return true;
}
