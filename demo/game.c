#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "home_state.h"
#include "game_play_state.h"
#include "game_over_state.h"
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
  home_state_t *home_state = state->home_state;

  // if (!home_state) {
  //   home_state = home_init();
  // }
  // bool game_over = home_main(home_state);
  // if (sdl_is_done((void *)home_state)) { 
  //   home_free(home_state);
  // }
  // else if (game_over) {
  //   SDL_Quit();
  // }
}

void run_game_play(state_t *state) {
  // If needed, generate a pointer to our initial state
  game_play_state_t *game_play_state = state->game_play_state;

  // if (!game_play_state) {
  //   game_play_state = game_play_init();
  // }
  // bool game_over = game_play_main(game_play_state);
  // if (sdl_is_done((void *)game_play_state)) { 
  //   game_play_free(game_play_state);
  // }
  // else if (game_over) {
  //   SDL_Quit();
  // }
}

void run_game_over(state_t *state) {
  // If needed, generate a pointer to our initial state
  game_over_state_t *game_over_state = state->game_over_state;

  // if (!game_over_state) {
  //   game_over_state = game_over_init();
  // }
  // bool game_over = game_over_main(game_over_state);
  // if (sdl_is_done((void *)game_over_state)) { 
  //   game_over_free(game_over_state);
  // }
  // else if (game_over) {
  //   SDL_Quit();
  // }
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
          run_game_over(state);
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
