#ifndef __HOME_H__
#define __HOME_H__

#include "math.h"
#include "sdl_wrapper.h"
#include "screen_helper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Stores the game home state
 */
typedef struct home_state
{
  screen_state_t screen_state;
  double time;
  state_type_t curr_state;
} home_state_t;

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the game in a created state
 * variable
 * 
 * @return pointer to a home_state_t object
 */
home_state_t *home_init();

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time
 * that has passed.
 *
 * @param home_state pointer to a state object
 * @return the current state type of the game
 */
state_type_t home_main(home_state_t *home_state);

/**
 * Frees anything allocated in the game
 * Should free everything in state as well as state itself.
 *
 * @param home_state pointer to a home state object
 */
void home_free(home_state_t *home_state);

#endif // #ifndef __HOME_H__