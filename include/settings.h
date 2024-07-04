#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "math.h"
#include "sdl_wrapper.h"
#include "screen_helper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Stores the game settings state
 */
typedef struct settings_state
{
  screen_state_t screen_state;
  double time;
  state_type_t curr_state;
  difficulty_type_t difficulty_level;
  TTF_Font *difficulty_font;
  TTF_Font *achievements_font;
} settings_state_t;

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the game in a created state
 * variable
 * 
 * @return pointer to a settings_state_t object
 */
settings_state_t *settings_init();

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time
 * that has passed.
 *
 * @param settings_state pointer to a state object
 * @return the current state type of the game
 */
state_type_t settings_main(settings_state_t *settings_state);

/**
 * Frees anything allocated in the game
 * Should free everything in state as well as state itself.
 *
 * @param settings_state pointer to a settings state object
 */
void settings_free(settings_state_t *settings_state);

#endif // #ifndef __SETTINGS_H__