#ifndef __GAME_PLAY_H__
#define __GAME_PLAY_H__

#include "math.h"
#include "sdl_wrapper.h"
#include "game_play_helper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct game_play_state game_play_state_t;

typedef struct zapper_state zapper_state_t;

typedef struct laser_state laser_state_t;

typedef struct rocket_state rocket_state_t;

typedef struct coin_state coin_state_t;

typedef struct powerup_state powerup_state_t;

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the game in a created state
 * variable
 * 
 * @param difficulty_level associated with the game
 * @return pointer to a game_play_state_t object
 */
game_play_state_t *game_play_init(difficulty_type_t difficulty_level);

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time
 * that has passed.
 *
 * @param game_play_state the current state of the game
 * @return the current state type
 */
state_type_t game_play_main(game_play_state_t *game_play_state);

/**
 * Frees anything allocated in the game
 * Should free everything in state as well as state itself.
 *
 * @param game_play_state pointer to a game_play state object
 */
void game_play_free(game_play_state_t *game_play_state);

#endif // #ifndef __GAME_PLAY_H__