#include "math.h"
#include "sdl_wrapper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

<<<<<<< HEAD
typedef struct state state_t;
=======
#ifndef STATE_H
#define STATE_H
>>>>>>> dhruv_clean

/**
 * Stores the demo state
 * Use this to store any variable needed every 'tick' of your demo
 */
typedef struct state state_t;

typedef enum {
    HOME,
    GAME_PLAY,
    GAME_OVER,
    EXIT
} state_type_t;

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the demo in a created state
 * variable 
 * Returns the pointer to this state (This is the state emscripten_main
 * and emscripten_free work with)
 */
<<<<<<< HEAD
game_play_state_t *emscripten_init();
=======
state_t *emscripten_init();
>>>>>>> dhruv_clean

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time
 * that has passed.
 *
 * @param state pointer to a state object with info about demo
 * @return a boolean representing whether the game/demo is over
 */
<<<<<<< HEAD
bool emscripten_main(game_play_state_t *game_play_state);
=======
bool emscripten_main(state_t *state);
>>>>>>> dhruv_clean

/**
 * Frees anything allocated in the demo
 * Should free everything in state as well as state itself.
 *
 * @param state pointer to a state object with info about demo
 */
<<<<<<< HEAD
void emscripten_free(game_play_state_t *game_play_state);
=======
void emscripten_free(state_t *state);


#endif // STATE_H
>>>>>>> dhruv_clean
