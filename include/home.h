#include "math.h"
#include "sdl_wrapper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Stores the game home state
 * Use this to store any variable needed every 'tick' of your game
 */
typedef struct home_state {
    int calm;
} home_state_t;

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the game in a created state
 * variable 
 * Returns the pointer to this state
 */
home_state_t *home_init();

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time
 * that has passed.
 *
 * @param home_state pointer to a state object
 * @return a boolean representing whether the game is over
 */
bool home_main(home_state_t *home_state);

/**
 * Frees anything allocated in the game
 * Should free everything in state as well as state itself.
 *
 * @param home_state pointer to a home state object
 */
void home_free(home_state_t *home_state);