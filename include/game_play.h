
#include "math.h"
#include "sdl_wrapper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { USER, CEILING, GROUND, ZAPPER, LASER, LASER_ACTIVE, 
ROCKET_WARNING, ROCKET, COIN, POWERUP} body_type_t;

typedef struct background_state background_state_t;

typedef struct state_temp state_temp_t;

typedef struct game_play_state game_play_state_t;

typedef struct zapper_state zapper_state_t;

typedef struct laser_state laser_state_t;

typedef struct rocket_state rocket_state_t;

typedef struct coin_state coin_state_t;

typedef struct powerup_state powerup_state_t;

/**
 * Gets the type of the given body.
 * @param body the body we want to get the type of
 * @return the `body_type_t` of the body
*/
body_type_t get_type(body_t *body);

/**
 * Initializes sdl as well as the variables needed
 * Creates and stores all necessary variables for the game in a created state
 * variable 
 * Returns the pointer to this state
 * @param difficulty_level associated with the game
 */
game_play_state_t *game_play_init(difficulty_type_t difficulty_level);

/**
 * Called on each tick of the program
 * Updates the state variables and display as necessary, depending on the time
 * that has passed.
 *
 * @param game_play_state pointer to a state object
 * @return the current state that the game is in
 */
state_type_t game_play_main(game_play_state_t *game_play_state);

/**
 * Frees anything allocated in the game
 * Should free everything in state as well as state itself.
 *
 * @param game_play_state pointer to a game_play state object
 */
void game_play_free(game_play_state_t *game_play_state);
