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
} state_type_t;

struct state {
  state_type_t state_type;
  home_state_t *home_state;
  game_play_state_t *game_play_state;
  game_over_state_t *game_over_state;
};

void run_home(state_t *state) {
  // If needed, generate a pointer to our initial state
  if (!state) {
    state = home_init();
  }
  bool game_over = home_main(state);
  if (sdl_is_done((void *)state)) { 
    home_free(state);
  }
  else if (game_over) {
    SDL_Quit();
  }
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state));
  state->state_type = HOME;
  state->home_state = NULL;
  state->game_play_state = NULL;
  state->game_over_state = NULL;
  return state;
}

bool emscripten_main(state_t *state) {
  switch (state->state_type) {
      case HOME: {
          run_home(state);
          break;
      }
      case GAME_PLAY: {
          run_game_play(state);
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
