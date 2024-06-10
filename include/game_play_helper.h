#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "collision.h"
#include "forces.h"
#include "game_play.h"
#include "asset.h"
#include "constants.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

typedef enum { USER, CEILING, GROUND, ZAPPER, LASER, LASER_ACTIVE, 
ROCKET_WARNING, ROCKET, COIN, POWERUP} body_type_t;

struct state_temp {
  background_state_t *background_state;
  list_t *body_assets;
  body_t *user;
  body_t *ceiling;
  body_t *ground;
  scene_t *scene;
};

struct background_state {
  asset_t *bg1;
  asset_t *bg2;
  double scroll_speed;
  double bg_offset;
};

body_type_t *make_type_info(body_type_t type);

body_type_t get_type(body_t *body);

/** Make a rectangle-shaped body object.
 *
 * @param center a vector representing the center of the body.
 * @param width the width of the rectangle
 * @param height the height of the rectangle
 * @return pointer to the rectangle-shaped body
 */
list_t *make_rectangle(vector_t center, double width, double height);

/**
 * Adds walls as bodies to the scene.
 *
 * @param state the current state of the demo
 */
void add_walls(state_temp_t *state);

/**
 * Creates a generic rectangular body for an obstacle
 */
body_t *make_obstacle_rectangle(vector_t center, double width, double height, void *aux_info, double velocity);

/**
 * Creates a generic circular body for an obstacle
 */
body_t *make_obstacle_circle(double radius, vector_t center, void *aux_info, double mass, vector_t velocity);

/**
 * Initializes background
 */
background_state_t *background_init(const char *bg_path);

/**
 * Updates to the background so it is scrolling and wrapping
 */
void background_update(background_state_t *state, double dt);