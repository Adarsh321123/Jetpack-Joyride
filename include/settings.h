#include "math.h"
#include "sdl_wrapper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Stores the game settings state
 * Use this to store any variable needed every 'tick' of your game
 */
typedef struct settings_state {
  double time;
  list_t *text;
  list_t *backgrounds;
  list_t *manual_buttons;
  list_t *button_assets;
  state_type_t curr_state;
  difficulty_type_t difficulty_level;
} settings_state_t;

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the game in a created state
 * variable 
 * Returns the pointer to this state
 */
settings_state_t *settings_init();

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time
 * that has passed.
 *
 * @param settings_state pointer to a state object
 * @return a boolean representing whether the game is over
 */
state_type_t settings_main(settings_state_t *settings_state);

/**
 * Frees anything allocated in the game
 * Should free everything in state as well as state itself.
 *
 * @param settings_state pointer to a settings state object
 */
void settings_free(settings_state_t *settings_state);
