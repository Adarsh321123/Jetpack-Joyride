#ifndef __STATE_H__
#define __STATE_H__

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

// TODO :move
typedef enum
{
    HOME,
    SETTINGS,
    GAME_PLAY,
    GAME_OVER,
    EXIT
} state_type_t;

typedef enum
{
    EASY,
    MEDIUM,
    HARD
} difficulty_type_t;

typedef enum
{
    SHIELD,
    MAGNETIC,
    MORE_COIN,
    GRAVITY_SWAP
} powerup_type_t;

typedef enum
{
    LASER_INACTIVE_STATE,
    LASER_ACTIVE_STATE
} laser_spawn_t;

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the demo in a created state
 * variable
 * Returns the pointer to this state (This is the state emscripten_main
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

#endif // #ifndef __STATE_H__
