#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "collision.h"
#include "forces.h"
#include "game_play.h"
#include "asset.h"
#include "constants.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

const double WEDGE_ANGLE = 3.6 * M_PI / 3;
const double INCREMENT_ANGLE = 0.1;
const double RADIUS = 40;
const double BULLET_RADIUS = 10;

const vector_t START_POS = {500, 30};
const vector_t RESET_POS = {500, 45};
const vector_t INVADER_BULLET_VEL = {0, -200};
const vector_t BASE_OBJ_VEL = {30, 0};
const double EXTRA_VEL_MULT = 10;
const double VEL_MULT_PROB = 0.2;

const double MASS = 5;
const double BULLET_MASS = 10;

const size_t SPAWN_TIME = 200; // number of frames in between new shapes

const double resting_speed = 300;
const double ACCEL = 100;

const double OUTER_RADIUS = 15;
const double INNER_RADIUS = 15;
const size_t OBSTACLE_HEIGHT = 30;
const vector_t OBS_WIDTHS = {30, 70};
const vector_t OBS_SPACING = {120, 350};

const size_t SHIP_NUM_POINTS = 20;

const rgb_color_t obs_color = (rgb_color_t){0.2, 0.2, 0.3};
const rgb_color_t frog_color = (rgb_color_t){0.1, 0.9, 0.2};

// constants to create invaders
const int16_t H_STEP = 20;
const int16_t V_STEP = 40;
const size_t OBS_PER_ROW = 5;
const size_t ROWS = 8;

const size_t OFFSET = 3;
const size_t CIRC_NPOINTS = 4;
const size_t BODY_ASSETS = 2;
const size_t USER_NUM_POINTS = 20;
const rgb_color_t user_color = (rgb_color_t){0.1, 0.9, 0.2};

const char *USER_IMG_PATH = "assets/Barry.png";
const char *LOG_PATH = "assets/log.png";
const char *BACKGROUND_PATH = "assets/BackdropMain.png";

struct background_state {
  asset_t *bg1;
  asset_t *bg2;
  double scroll_speed;
  double bg_offset;
};

struct state_temp {
  background_state_t *background_state;
  list_t *body_assets;
  asset_t *user;
  scene_t *scene;
  int16_t points;
};

struct game_play_state {
  double time;
  state_temp_t *state;
};

body_t *make_user(double outer_radius, double inner_radius, vector_t center) {
  center.y += inner_radius;
  list_t *c = list_init(USER_NUM_POINTS, free);
  for (size_t i = 0; i < USER_NUM_POINTS; i++) {
    double angle = 2 * M_PI * i / USER_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + inner_radius * cos(angle),
                    center.y + outer_radius * sin(angle)};
    list_add(c, v);
  }
  body_t *user = body_init(c, 1, user_color);
  return user;
}

// TODO: add space bar stuff

static background_state_t *background_init(const char *bg_path) {
  background_state_t *state = malloc(sizeof(background_state_t));
  assert(state != NULL);

  state->scroll_speed = 100.0;
  state->bg_offset = 0;
  SDL_Rect bg_bounds = make_texr(MIN.x, MIN.y, MAX.x, MAX.y);
  state->bg1 = asset_make_image(bg_path, bg_bounds);
  state->bg2 = asset_make_image(bg_path, bg_bounds);

  return state;
}

static void background_update(background_state_t *state, double dt) {
  state->bg_offset -= state->scroll_speed * dt;
  double WINDOW_WIDTH = MAX.x - MIN.x;
  // TODO: maybe make cleaner
  if (state->bg_offset <= - WINDOW_WIDTH) {
    state->bg_offset += WINDOW_WIDTH;
  }
  state->bg1->bounding_box.x = state->bg_offset;
  state->bg2->bounding_box.x = state->bg_offset + WINDOW_WIDTH;
}

game_play_state_t *game_play_init() {
  game_play_state_t *game_play_state = malloc(sizeof(game_play_state_t));
  assert(game_play_state);

  asset_cache_init();
  sdl_init(MIN, MAX);
  state_temp_t *state = malloc(sizeof(state_temp_t));

  state->scene = scene_init();
  state->body_assets = list_init(1, (free_func_t)asset_destroy);
  // TODO: add body assets for the zappers and stuff
  body_t *user = make_user(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
  body_set_centroid(user, RESET_POS);
  scene_add_body(state->scene, user);
  asset_t *img = asset_make_image_with_body(USER_IMG_PATH, user);
  list_add(state->body_assets, img);
  
  state->background_state = background_init(BACKGROUND_PATH);

  game_play_state->state = state;
  game_play_state->time = 0;
  return game_play_state;
}

bool game_play_main(game_play_state_t *game_play_state) {

  double dt = time_since_last_tick();
  state_temp_t *state = game_play_state->state;
  sdl_clear();

  background_update(state->background_state, dt);
  asset_render(state->background_state->bg1);
  asset_render(state->background_state->bg2);

  for (size_t i = 0; i < list_size(state->body_assets); i++) {
    asset_render(list_get(state->body_assets, i));
  }

  // for (size_t i = 0; i < list_size(state->background); i++) {
  //   asset_update_bounding_box(list_get(state->background, i), 1000*dt);
  //   asset_render(list_get(state->background, i));
  // }
  sdl_show();

  scene_tick(state->scene, dt);
  return false;
}

void game_play_free(game_play_state_t *game_play_state) {
  state_temp_t *state = game_play_state->state;
  asset_destroy(state->background_state->bg1);
  asset_destroy(state->background_state->bg1);
  free(state->background_state);
  list_free(state->body_assets);
  scene_free(state->scene);
  asset_cache_destroy();
  free(state);
  //TTF_Quit();
  //asset_cache_destroy();
  free(game_play_state);
}
