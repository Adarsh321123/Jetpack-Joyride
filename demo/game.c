#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "home.h"
#include "game_play.h"
#include "game_over.h"
#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

struct state {
  state_type_t curr_state;
  home_state_t *home_state;
  game_play_state_t *game_play_state;
  game_over_state_t *game_over_state;
};

void run_home(state_t *state) {
  home_state_t *home_state = state->home_state;
  if (!home_state) {
    home_state = home_init();
  }
  state_type_t next_state = home_main(home_state);
  if (sdl_is_done((void *)home_state)) { 
    home_free(home_state);
  }
  else if (next_state != HOME) {
    home_free(home_state);
    state->curr_state = next_state;
  }
}

void run_game_play(state_t *state) {
  game_play_state_t *game_play_state = state->game_play_state;
  if (!game_play_state) {
    game_play_state = game_play_init();
  }
  bool game_over = game_play_main(game_play_state);
  if (sdl_is_done((void *)game_play_state)) { 
    game_play_free(game_play_state);
  }
  else if (game_over) {
    game_play_free(game_play_state);
    state->curr_state = GAME_OVER;
  }
}

void run_game_over(state_t *state) {
  game_over_state_t *game_over_state = state->game_over_state;
  if (!game_over_state) {
    game_over_state = game_over_init();
  }
  state_type_t next_state = game_over_main(game_over_state);
  if (sdl_is_done((void *)game_over_state)) { 
    game_over_free(game_over_state);
  }
  else if (next_state != GAME_OVER) {
    game_over_free(game_over_state);
    state->curr_state = next_state;
  }
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  state->curr_state = HOME;
  state->home_state = NULL;
  state->game_play_state = NULL;
  state->game_over_state = NULL;
  return state;
}

bool emscripten_main(state_t *state) {
  switch (state->curr_state) {
      case HOME: {
          run_home(state);
          break;
      }
      case GAME_PLAY: {
          run_game_play(state);
          break;
      }
      case GAME_OVER: {
          run_game_over(state);
          break;
      }
      case EXIT: {
          return true;
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
