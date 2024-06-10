#ifndef __GAME_PLAY_HELPER_H__
#define __GAME_PLAY_HELPER_H__

#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "collision.h"
#include "forces.h"
#include "asset.h"
#include "constants.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

typedef enum
{
  USER,
  CEILING,
  GROUND,
  ZAPPER,
  LASER,
  LASER_ACTIVE,
  ROCKET_WARNING,
  ROCKET,
  COIN,
  POWERUP
} body_type_t;

typedef struct background_state
{
  asset_t *bg1;
  asset_t *bg2;
  double scroll_speed;
  double bg_offset;
} background_state_t;

typedef struct state_temp
{
  background_state_t *background_state;
  list_t *body_assets;
  body_t *user;
  body_t *ceiling;
  body_t *ground;
  scene_t *scene;
} state_temp_t;

/**
 * Get type of the body.
*/
body_type_t get_type(body_t *body);

/**
 * Creates a generic rectangular body for an obstacle
 */
body_t *make_obstacle_rectangle(vector_t *center, double width, double height, body_type_t info, vector_t velocity);

/**
 * Creates a generic circular body for an obstacle
 */
body_t *make_obstacle_circle(double radius, vector_t *center, body_type_t info, double mass, vector_t velocity);

/**
 * Remove the bodies in the state of the game.
*/
void remove_moving_bodies(state_temp_t *state, body_type_t info);

/**
 * Updates to the background so it is scrolling and wrapping
 */
void background_update(background_state_t *state, double dt);

/**
 * Initialize as variables for state_temp_t.
*/
state_temp_t *state_temp_init();

#endif // #ifndef __GAME_PLAY_HELPER_H__