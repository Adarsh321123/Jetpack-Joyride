#ifndef __GAME_OVER_H__
#define __GAME_OVER_H__

#include "math.h"
#include "sdl_wrapper.h"
#include "screen_helper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Stores any variable needed at every 'tick' of the game
 */
typedef struct game_over_state
{
  screen_state_t screen_state;
  double time;
  state_type_t curr_state;
} game_over_state_t;

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the game in a created state
 * variable.
 * 
 * @return pointer to a game_over_state_t object
 */
game_over_state_t *game_over_init();

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time
 * that has passed.
 *
 * @param game_over_state the current state of the game
 * @return the current state type of the game
 */
state_type_t game_over_main(game_over_state_t *game_over_state);

/**
 * Frees anything allocated in the game
 * Should free everything in state as well as state itself.
 *
 * @param game_over_state the current state of the game
 */
void game_over_free(game_over_state_t *game_over_state);

#endif // #ifndef __GAME_OVER_H__