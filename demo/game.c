#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

typedef enum {
    HOME,
    GAME_PLAY,
    GAME_OVER
} game_state_t;

struct state {
  game_state_t game_state;
};

void run_home(state_t *state) {
  // If needed, generate a pointer to our initial state
  if (!state) {
    state = emscripten_home_init();
  }
  bool game_over = emscripten_home_main(state);
  if (sdl_is_done((void *)state)) { 
    emscripten_home_free(state);
  }
  else if (game_over) {
    SDL_Quit();
  }
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state));
  state->game_state = HOME;
  return state;
}

bool emscripten_main(state_t *state) {
  switch (state->game_state) {
      case HOME: {
          run_home();
          break;
      }
      case GAME_PLAY: {
          run_game_play();
          break;
      }
      case GAME_OVER: {
          run_game_over();
          break;
      }
      default: {
          printf("Unknown state!\n");
          exit(1);
      }
  }
  return false;
}

void emscripten_free(state_t *state) {
  free(state);
}
