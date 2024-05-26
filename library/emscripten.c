#include "math.h"
#include "sdl_wrapper.h"
#include "state.h"
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

state_t *state;

void loop() {
  // If needed, generate a pointer to our initial state
  if (!state) {
    state = emscripten_init();
  }

  bool game_over = emscripten_main(state);

  if (sdl_is_done((void *)state)) { // Once our demo exits...
    emscripten_free(state);         // Free any state variables we've been using
#ifdef __EMSCRIPTEN__ // Clean up emscripten environment (if we're using it)
    emscripten_cancel_main_loop();
    emscripten_force_exit(0);
#else
    exit(0);
#endif
    return;
  } else if (game_over) {
    SDL_Quit();
  }
}

int main() {
#ifdef __EMSCRIPTEN__
  // Set loop as the function emscripten calls to request a new frame
  emscripten_set_main_loop_arg(loop, NULL, 0, 1);
#else
  while (1) {
    loop();
  }
#endif
}
