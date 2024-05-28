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
const vector_t USER_VEL = {0, 200};
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
const double WALL_DIM = 1;
rgb_color_t white = (rgb_color_t){1, 1, 1};

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

typedef enum { WALL, GROUND } body_type_t;

body_type_t *make_type_info(body_type_t type) {
  body_type_t *info = malloc(sizeof(body_type_t));
  assert(info != NULL);
  *info = type;
  return info;
}

/** Make a rectangle-shaped body object.
 *
 * @param center a vector representing the center of the body.
 * @param width the width of the rectangle
 * @param height the height of the rectangle
 * @return pointer to the rectangle-shaped body
 */
list_t *make_rectangle(vector_t center, double width, double height) {
  list_t *points = list_init(4, free);
  vector_t *p1 = malloc(sizeof(vector_t));
  assert(p1 != NULL);
  *p1 = (vector_t){center.x - width / 2, center.y - height / 2};

  vector_t *p2 = malloc(sizeof(vector_t));
  assert(p2 != NULL);
  *p2 = (vector_t){center.x + width / 2, center.y - height / 2};

  vector_t *p3 = malloc(sizeof(vector_t));
  assert(p3 != NULL);
  *p3 = (vector_t){center.x + width / 2, center.y + height / 2};

  vector_t *p4 = malloc(sizeof(vector_t));
  assert(p4 != NULL);
  *p4 = (vector_t){center.x - width / 2, center.y + height / 2};

  list_add(points, p1);
  list_add(points, p2);
  list_add(points, p3);
  list_add(points, p4);

  return points;
}

/**
 * Adds walls as bodies to the scene.
 *
 * @param state the current state of the demo
 */
void add_walls(state_temp_t *state) {
  // TODO: remove asserts
  list_t *wall1_shape =
      make_rectangle((vector_t){MAX.x, MAX.y / 2}, WALL_DIM, MAX.y);
  assert(wall1_shape != NULL);
  body_t *wall1 = body_init_with_info(wall1_shape, INFINITY, white,
                                      make_type_info(WALL), free);
  assert(wall1 != NULL);                                   
  list_t *wall2_shape =
      make_rectangle((vector_t){0, MAX.y / 2}, WALL_DIM, MAX.y);
  assert(wall2_shape != NULL);
  body_t *wall2 = body_init_with_info(wall2_shape, INFINITY, white,
                                      make_type_info(WALL), free);
  assert(wall2 != NULL);
  list_t *ceiling_shape =
      make_rectangle((vector_t){MAX.x / 2, MAX.y}, MAX.x, WALL_DIM);
  assert(ceiling_shape != NULL);
  body_t *ceiling = body_init_with_info(ceiling_shape, INFINITY, white,
                                        make_type_info(WALL), free);
  assert(ceiling != NULL);
  list_t *ground_shape =
      make_rectangle((vector_t){MAX.x / 2, 0}, MAX.x, WALL_DIM);
  assert(ground_shape != NULL);
  body_t *ground = body_init_with_info(ground_shape, INFINITY, white,
                                       make_type_info(GROUND), free);
  assert(ground != NULL);
  scene_add_body(state->scene, wall1);
  scene_add_body(state->scene, wall2);
  scene_add_body(state->scene, ceiling);
  scene_add_body(state->scene, ground);
}

/**
 * Move user up if space bar is pressed and back down otherwise
 *
 * @param key the character of the key pressed
 * @param type event type connected to key
 * @param held_time double value representing the amount of time the key is held
 * down
 * @param state the current state of game
 */
void on_key(char key, key_event_type_t type, double held_time, game_play_state_t *game_play_state) {
  // TODO: no change if add top or bottom of screen
  fprintf(stderr, "before getting user\n");
  fprintf(stderr, "scene_bodies inside: %zu\n", scene_bodies(game_play_state->state->scene));
  body_t *user = scene_get_body(game_play_state->state->scene, 0);
  fprintf(stderr, "after getting user\n");
  if (type == KEY_PRESSED) {
    if (key == SPACE_BAR) {
      body_set_velocity(user, USER_VEL);
      fprintf(stderr, "space bar hit\n");
    }
  } else {
    body_set_velocity(user, vec_negate(USER_VEL));
    fprintf(stderr, "no space bar hit\n");
  }
}

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
  assert(game_play_state != NULL);

  asset_cache_init();
  sdl_init(MIN, MAX);
  state_temp_t *state = malloc(sizeof(state_temp_t));
  assert(state != NULL);

  state->scene = scene_init();
  state->body_assets = list_init(1, (free_func_t)asset_destroy);
  // TODO: add body assets for the zappers and stuff
  body_t *user = make_user(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
  body_set_centroid(user, RESET_POS);
  scene_add_body(state->scene, user);
  asset_t *img = asset_make_image_with_body(USER_IMG_PATH, user);
  list_add(state->body_assets, img);
  fprintf(stderr, "scene_bodies: %zu\n", scene_bodies(state->scene));
  sdl_on_key((key_handler_t)on_key);
  state->background_state = background_init(BACKGROUND_PATH);
  game_play_state->state = state;
  add_walls(game_play_state->state);

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
  body_t *user = scene_get_body(game_play_state->state->scene, 0);
  fprintf(stderr, "y of the user %d\n", body_get_centroid(user).y;);
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
