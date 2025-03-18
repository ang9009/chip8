#include "cycle.h"
#include "SDL2/SDL.h"
#include "chip8.h"
#include "init.h"

// Maps keypad indices to their scancode
const SDL_Scancode key_idx_to_code[16] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V};

// Maps keypad indices to their hex values
const uint8_t key_idx_to_hex[16] = {0x1, 0x2, 0x3, 0xC, 0x4, 0x5, 0x6, 0xD,
                                    0x7, 0x8, 0x9, 0xE, 0xA, 0x0, 0xB, 0xF};

/**
 * Handles user input, and updates the given keypad based on what the user is currently 
 * pressing.
 */
void handle_input(bool keypad[16], state_t* state) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        *state = STOPPED;
        break;
      default:
        const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);
        for (int i = 0; i < 16; i++) {
          keypad[i] = keyboard_state[key_idx_to_code[i]];
        }
        break;
    }
  }
}

// Updates the display in the chip8 struct.
void update_display(chip8_t* chip8, uint8_t X, uint8_t Y, uint8_t N) {
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

      bool* pixel_on = &(chip8->display[y_coord + k][x_coord + (7 - j)]);
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

// Handles opcodes prefixed with 0x0 (routine related instructions).
bool handle_routines(const uint16_t opcode, bool debug, chip8_t* chip8) {
  switch (opcode) {
    case 0x00E0:
      if (debug) {
        SDL_Log("Clear screen");
      }
      memset(&chip8->display[0], 0, sizeof chip8->display);
      chip8->update_screen = true;
      break;
    case 0x00EE:
      if (debug) {
        SDL_Log("Return from subroutine");
      }
      chip8->PC = *chip8->SP;
      chip8->SP--;  // Pop address from stack
      break;
    default:
      SDL_Log("Unknown instruction: %x", opcode);
      return false;
  }

  return true;
}

// Handles opcodes prefixed with 0x8.
bool handle_arithmetic(const uint8_t N, bool debug, const uint8_t X,
                       const uint8_t Y, chip8_t* chip8, chip8_ver_t ver,
                       const uint16_t opcode) {
  switch (N) {
    case 0x0:
      if (debug) {
        SDL_Log("Set V[%X] to V[%X]", X, Y);
      }
      chip8->V[X] = chip8->V[Y];
      break;
    case 0x1:
      if (debug) {
        SDL_Log("Set V[%X] to V[%X] OR V[%X]", X, X, Y);
      }
      chip8->V[X] = chip8->V[X] | chip8->V[Y];
      break;
    case 0x2:
      if (debug) {
        SDL_Log("Set V[%X] to V[%X] AND V[%X]", X, X, Y);
      }
      chip8->V[X] = chip8->V[X] & chip8->V[Y];
      break;
    case 0x3:
      if (debug) {
        SDL_Log("Set V[%X] to V[%X] XOR V[%X]", X, X, Y);
      }
      chip8->V[X] = chip8->V[X] ^ chip8->V[Y];
      break;
    case 0x4:
      if (debug) {
        SDL_Log("Set V[%X] to V[%X] + V[%X]", X, X, Y);
      }
      chip8->V[X] = chip8->V[Y];
      chip8->V[0xF] = (chip8->V[X] + chip8->V[Y]) > 255;
      break;
    case 0x5:
      if (debug) {
        SDL_Log("Set V[%X] to V[%X] - V[%X]", X, X, Y);
      }
      chip8->V[0xF] = chip8->V[X] >= chip8->V[Y];
      chip8->V[X] -= chip8->V[Y];
      break;
    case 0x6:
      if (debug) {
        SDL_Log(
            "Set V[%X] to V[%X], shift V[%X] one bit to the right, store bit "
            "shifted out in V[F]",
            X, Y, X);
      }
      if (ver == COSMAC) {
        chip8->V[X] = chip8->V[Y];
      }
      uint8_t bit = chip8->V[X] & 0b1;
      chip8->V[0xF] = bit;
      chip8->V[X] >>= 1;
      break;
    case 0x7:
      if (debug) {
        SDL_Log("Set V[%X] to V[%X] - V[%X]", X, Y, X);
      }
      chip8->V[0xF] = chip8->V[Y] >= chip8->V[X];
      chip8->V[X] = chip8->V[Y] - chip8->V[X];
      break;
    case 0xE:
      if (debug) {
        SDL_Log(
            "Set V[%X] to V[%X], shift V[%X] one bit to the left, store bit "
            "shifted out in V[F]",
            X, Y, X);
      }
      if (ver == COSMAC) {
        chip8->V[X] = chip8->V[Y];
      }
      uint8_t msb = (chip8->V[X] >> 7) & 0b1;  // Get MSB
      chip8->V[0xF] = msb;
      chip8->V[X] <<= 1;
      break;
    default:
      SDL_Log("Unknown instruction: %X", opcode);
      return false;
      break;
  }
  return true;
}

bool handle_f_prefixed_insns(const uint8_t NN, bool debug, const uint8_t X,
                             chip8_t* chip8, uint16_t opcode, chip8_ver_t ver) {
  switch (NN) {
    case 0x07:
      if (debug) {
        SDL_Log("Set V[%X] to delay timer (0x%04X)", X, chip8->delay_timer);
      }
      chip8->V[X] = chip8->delay_timer;
      break;
    case 0x15:
      if (debug) {
        SDL_Log("Set delay timer to V[%X] (0x%04X)", X, chip8->V[X]);
      }
      chip8->delay_timer = chip8->V[X];
      break;
    case 0x18:
      if (debug) {
        SDL_Log("Set sound timer to V[%X] (0x%04X)", X, chip8->V[X]);
      }
      chip8->sound_timer = chip8->V[X];
      break;
    case 0x1E:
      if (debug) {
        SDL_Log("Add 0x%04X to index register I", chip8->V[X]);
      }
      chip8->I += chip8->V[X];
      break;
    case 0x0A:
      if (debug) {
        SDL_Log("Block until key pressed");
      }
      uint8_t key_pressed = 255;
      for (int i = 0; i < 16; i++) {
        if (chip8->keypad[i]) {
          key_pressed = key_idx_to_hex[i];
          break;
        }
      }
      if (key_pressed == 255) {
        chip8->PC -= 2;  // Return to this instruction on next cycle
      } else {
        chip8->V[X] = key_pressed;
      }

      break;
    case 0x29:
      int idx = -1;
      for (int i = 0; i < 16; i++) {
        if (chip8->V[X] == key_idx_to_hex[i]) {
          idx = i;
          break;
        }
      }
      if (idx == -1) {
        SDL_Log("Could not find matching key in V[%X]", X);
        return false;
      }
      // End each char takes up 5 hex characters (5 positions in memory)
      int font_pos = FONT_ENTRYPOINT + (idx * 5);
      chip8->I = font_pos;

      if (debug) {
        SDL_Log("Set index register I to character %X", key_idx_to_hex[idx]);
      }
      break;
    case 0x33:
      int vx = chip8->V[X];
      if (debug) {
        SDL_Log("Set positions in memory to digits in %d", vx);
      }
      int divisors[3] = {100, 10, 1};

      for (int i = 0; i < 3; i++) {
        int digit = (vx / divisors[i]) % 10;
        chip8->ram[chip8->I + i] = digit;
      }
      break;
    case 0x55:
      if (debug) {
        SDL_Log("Store registers to memory");
      }
      for (int i = 0; i < X; i++) {
        chip8->ram[chip8->I + i] = chip8->V[i];
      }
      if (ver == COSMAC) {
        chip8->I += X + 1;
      }
      break;
    case 0x65:
      if (debug) {
        SDL_Log("Load register values from memory");
      }
      for (int i = 0; i < X; i++) {
        chip8->V[i] = chip8->ram[chip8->I + i];
      }
      if (ver == COSMAC) {
        chip8->I += X + 1;
      }
      break;
    default:
      SDL_Log("Unknown instruction: %x", opcode >> 12 & 0xF);
      return false;
  }

  return true;
}

/**
 * Executes one iteration of the chip8 fetch-decode-execute cycle.
 */
bool execute_cycle(chip8_t* chip8, chip8_ver_t ver, bool debug) {
  // Fetch insn from memory
  const uint16_t opcode =
      (chip8->ram[chip8->PC] << 8) | (chip8->ram[chip8->PC + 1]);
  chip8->PC += 2;  // Increment PC by 2 to get to next instruction

  const uint8_t X = (opcode >> 8) & 0xF;  // Second nibble
  const uint8_t Y = (opcode >> 4) & 0xF;  // Third nibble
  const uint8_t N = opcode & 0xF;         // Fourth nibble
  const uint8_t NN = opcode & 0xFF;       // Second byte
  const uint16_t NNN = opcode & 0xFFF;    // Second, third, and fourth nibbles

  switch (opcode >> 12 & 0xF) {
    case 0x0:
      if (!handle_routines(opcode, debug, chip8)) {
        return false;
      }
      break;
    case 0x1:
      if (debug) {
        SDL_Log("Jump to 0x%04X", NNN);
      }
      chip8->PC = NNN;
      break;
    case 0x2:
      if (debug) {
        SDL_Log("Push PC to stack, call subroutine at 0x%04X", NNN);
      }
      chip8->SP++;
      *chip8->SP = chip8->PC;  // Push PC to stack
      chip8->PC = NNN;
      break;
    case 0x3:
      if (chip8->V[X] == NN) {
        if (debug) {
          SDL_Log("Skip next instruction");
        }
        chip8->PC += 2;
      }
      break;
    case 0x4:
      if (chip8->V[X] != NN) {
        if (debug) {
          SDL_Log("Skip next instruction");
        }
        chip8->PC += 2;
      }
      break;
    case 0x5:
      if (chip8->V[X] == chip8->V[Y]) {
        if (debug) {
          SDL_Log("Skip next instruction");
        }
        chip8->PC += 2;
      }
      break;
    case 0x6:
      if (debug) {
        SDL_Log("Set register V%x to 0x%04X", X, NN);
      }
      chip8->V[X] = NN;
      break;
    case 0x7:
      if (debug) {
        SDL_Log("Add 0x%04X to register V%x", NN, X);
      }
      chip8->V[X] += NN;
      break;
    case 0x8:
      if (!handle_arithmetic(N, debug, X, Y, chip8, ver, opcode)) {
        return false;
      }
      break;
    case 0x9:
      if (chip8->V[X] != chip8->V[Y]) {
        if (debug) {
          SDL_Log("Skip next instruction");
        }
        chip8->PC += 2;
      }
      break;
    case 0xA:
      if (debug) {
        SDL_Log("Set index register I to 0x%04X", NNN);
      }
      chip8->I = NNN;
      break;
    case 0xB:
      uint16_t addr =
          ver == COSMAC ? NNN + chip8->V[0] : ((X << 8) | NN) + chip8->V[X];
      if (debug) {
        SDL_Log("Jump to 0x%04X", addr);
      }
      chip8->PC = addr;
      break;
    case 0xC:
      uint8_t random = (rand() % 256) & NN;
      chip8->V[X] = random;
      if (debug) {
        SDL_Log("Set V[%X] to 0x%04X", X, random);
      }
      break;
    case 0xD:
      if (debug) {
        SDL_Log("Update display");
      }
      update_display(chip8, X, Y, N);
      chip8->update_screen = true;
      break;
    case 0xE:
      switch (NN) {
        case 0x9E:
          if (chip8->keypad[chip8->V[X]]) {
            SDL_Log("Skip next instruction due to key pressed");
            chip8->PC += 2;
          }
          break;
        case 0xA1:
          if (!chip8->keypad[chip8->V[X]]) {
            SDL_Log("Skip next instruction due to key not pressed");
            chip8->PC += 2;
          }
          break;
      }
      break;
    case 0xF:
      if (!handle_f_prefixed_insns(NN, debug, X, chip8, opcode, ver)) {
        return false;
      }
      break;
    default:
      SDL_Log("Unknown instruction: %x", opcode >> 12 & 0xF);
      return false;
  }

  return true;
}
