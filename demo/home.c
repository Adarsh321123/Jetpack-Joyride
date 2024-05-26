#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "home_state.h"
#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

struct home_state {
  
};

home_state_t *home_init() {
  return NULL;
}

bool home_main(home_state_t *home_state) {
  return false;
}

void home_free(home_state_t *home_state) {
  free(state);
}