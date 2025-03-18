#include "render.h"
#include "SDL2/SDL.h"
#include "init.h"

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} rgba_t;

// Clears the screen with black
void clear_screen(SDL_Renderer* renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
}

// Cleans up all sdl related processes
void cleanup_sdl(const sdl_t sdl) {
  SDL_DestroyWindow(sdl.window);
  SDL_DestroyRenderer(sdl.renderer);
  SDL_Quit();
}

rgba_t get_color(uint32_t color) {
  rgba_t rgba = {
      .r = (color >> 24) & 0xF,
      .g = (color >> 16) & 0xF,
      .b = (color >> 8) & 0xF,
      .a = (color >> 0) & 0xF,
  };
  return rgba;
}

// Updates the screen based on the current state of chip8
void update_screen(chip8_t chip8, config_t config, sdl_t* sdl) {
  // rgba_t pixel_color = get_color(config.pixel_color);

  for (int i = 0; i < SCREEN_W; i++) {
    for (int j = 0; j < SCREEN_H; j++) {
      bool pixel_on = chip8.display[j][i];
      if (pixel_on) {
        SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 255);
      } else {
        SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 0);
      }
      SDL_Rect pixel_rect = {
          .x = i * config.scale,
          .y = j * config.scale,
          .w = config.scale,
          .h = config.scale,
      };
      SDL_RenderFillRect(sdl->renderer, &pixel_rect);
    }
  }

  SDL_RenderPresent(sdl->renderer);
}