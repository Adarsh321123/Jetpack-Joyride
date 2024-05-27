#include "math.h"
#include "sdl_wrapper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct state_temp state_temp_t;

typedef struct game_play_state game_play_state_t;

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the game in a created state
 * variable 
 * Returns the pointer to this state
 */
game_play_state_t *game_play_init();

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time
 * that has passed.
 *
 * @param game_play_state pointer to a state object
 * @return a boolean representing whether the game is play
 */
bool game_play_main(game_play_state_t *game_play_state);

/**
 * Frees anything allocated in the game
 * Should free everything in state as well as state itself.
 *
 * @param game_play_state pointer to a game_play state object
 */
void game_play_free(game_play_state_t *game_play_state);
