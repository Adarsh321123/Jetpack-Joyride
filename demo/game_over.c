#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_over.h"
#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

struct game_over_state {
  
};

game_over_state_t *game_over_init() {
  return NULL;
}

bool game_over_main(game_over_state_t *game_over_state) {
  return false;
}

void game_over_free(game_over_state_t *game_over_state) {
  free(state);
}