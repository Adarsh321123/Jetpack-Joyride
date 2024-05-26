#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_play.h"
#include "asset.h"
#include "constants.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

game_play_state_t *game_play_init() {
  game_play_state_t *game_play_state = malloc(sizeof(game_play_state_t));
  assert(game_play_state);
  sdl_init(MIN, MAX);
  TTF_Init();
  asset_cache_init();
  game_play_state->time = 0;
  return game_play_state;
}

bool game_play_main(game_play_state_t *game_play_state) {
  sdl_clear();
  game_play_state->time += time_since_last_tick();
  sdl_show();
  return false;
}

void game_play_free(game_play_state_t *game_play_state) {
  TTF_Quit();
  asset_cache_destroy();
  free(game_play_state);
}