#include "math.h"
#include "sdl_wrapper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Stores the demo state
 * Use this to store any variable needed every 'tick' of your demo
 */
typedef struct state state_t;

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the demo in a created state
 * variable Returns the pointer to this state (This is the state emscripten_main
 * and emscripten_free work with)
 */
state_t *emscripten_init();

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time
 * that has passed.
 *
 * @param state pointer to a state object with info about demo
 * @return a boolean representing whether the game/demo is over
 */
bool emscripten_main(state_t *state);

/**
 * Frees anything allocated in the demo
 * Should free everything in state as well as state itself.
 *
 * @param state pointer to a state object with info about demo
 */
void emscripten_free(state_t *state);
