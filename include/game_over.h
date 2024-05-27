#include "math.h"
#include "sdl_wrapper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Stores any variable needed at every 'tick' of the game
 */
typedef struct game_over_state {
  double time;
  list_t *text;
  list_t *backgrounds;
  list_t *manual_buttons;
  list_t *button_assets;
  state_type_t curr_state;
} game_over_state_t;

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the game in a created state
 * variable
 * Returns the pointer to this state
 */
game_over_state_t *game_over_init();

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time
 * that has passed.
 *
 * @param game_over_state pointer to a state object
 * @return a boolean representing whether the game is over
 */
state_type_t game_over_main(game_over_state_t *game_over_state);

/**
 * Frees anything allocated in the game
 * Should free everything in state as well as state itself.
 *
 * @param game_over_state pointer to a game_over state object
 */
void game_over_free(game_over_state_t *game_over_state);
