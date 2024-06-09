#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "home.h"
#include "settings.h"
#include "game_play.h"
#include "game_over.h"
#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

struct state {
  state_type_t curr_state;
  difficulty_type_t difficulty_level;
  home_state_t *home_state;
  settings_state_t *settings_state;
  game_play_state_t *game_play_state;
  game_over_state_t *game_over_state;
};

void run_home(state_t *state) {
  home_state_t *home_state = state->home_state;
  if (!home_state) {
    home_state = home_init();
    state->home_state = home_state;
  }
  state_type_t next_state = home_main(home_state);
  if (sdl_is_done((void *)home_state)) {
    state->home_state = NULL;
    home_free(home_state);
  }
  else if (next_state != HOME) {
    state->home_state = NULL;
    state->curr_state = next_state;
    home_free(home_state);
  }
}

void run_settings(state_t *state) {
  settings_state_t *settings_state = state->settings_state;
  if (!settings_state) {
    settings_state = settings_init();
    state->settings_state = settings_state;
    settings_state->difficulty_level = state->difficulty_level;
  }
  state_type_t next_state = settings_main(settings_state);
  state->difficulty_level = settings_state->difficulty_level;
  if (sdl_is_done((void *)settings_state)) {
    state->settings_state = NULL;
    settings_free(settings_state);
  }
  else if (next_state != SETTINGS) {
    state->settings_state = NULL;
    state->curr_state = next_state;
    settings_free(settings_state);
  }
}

void run_game_play(state_t *state) {
  game_play_state_t *game_play_state = state->game_play_state;
  if (!game_play_state) {
    game_play_state = game_play_init(state->difficulty_level);
    state->game_play_state = game_play_state;
  }
  state_type_t next_state = game_play_main(game_play_state);
  if (sdl_is_done((void *)game_play_state)) {
    state->game_play_state = NULL;
    game_play_free(game_play_state);
  }
  else if (next_state != GAME_PLAY) {
    state->game_play_state = NULL;
    state->curr_state = next_state;
    game_play_free(game_play_state);
  }
}

void run_game_over(state_t *state) {
  game_over_state_t *game_over_state = state->game_over_state;
  if (!game_over_state) {
    game_over_state = game_over_init();
    state->game_over_state = game_over_state;
  }
  state_type_t next_state = game_over_main(game_over_state);
  if (sdl_is_done((void *)game_over_state)) {
    state->game_over_state = NULL;
    game_over_free(game_over_state);
  }
  else if (next_state != GAME_OVER) {
    state->game_over_state = NULL;
    state->curr_state = next_state;
    game_over_free(game_over_state);
  }
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  state->curr_state = HOME;
  state->difficulty_level = EASY;
  state->home_state = NULL;
  state->settings_state = NULL;
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
      case SETTINGS: {
          run_settings(state);
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
  if (state->home_state != NULL) {
    home_free(state->home_state);
  }
  if (state->settings_state != NULL) {
    settings_free(state->settings_state);
  }
  if (state->game_play_state != NULL) {
    game_play_free(state->game_play_state);
  }
  if (state->game_over_state != NULL) {
    game_over_free(state->game_over_state);
  }
  free(state);
}
