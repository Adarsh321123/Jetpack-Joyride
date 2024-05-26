#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_play_state.h"
#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

struct game_play_state {
  
};

game_play_state_t *game_play_init() {
  return NULL;
}

bool game_play_main(game_play_state_t *game_play_state) {
  return false;
}

void game_play_free(game_play_state_t *game_play_state) {
  free(state);
}