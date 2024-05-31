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
const double ELASTICITY = 0;

const vector_t START_POS = {500, 30};
const vector_t INVADER_BULLET_VEL = {0, -200};
const vector_t USER_VEL = {0, 200};
const vector_t BASE_OBJ_VEL = {30, 0};
const vector_t ZAPPER_VEL = {-100, 0};
const double EXTRA_VEL_MULT = 10;
const double VEL_MULT_PROB = 0.2;

const double MASS = 5;
const double BULLET_MASS = 10;

const size_t SPAWN_TIME = 200; // number of frames in between new shapes

const double resting_speed = 300;
const double ACCEL = 100;

const double OUTER_RADIUS = 15;
const double INNER_RADIUS = 15;
const double ZAPPER_WIDTH = 46;
const double ZAPPER_HEIGHT = 109;
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
rgb_color_t black = (rgb_color_t){0, 0, 0};

const double ZAPPER_GENERATION_TIME = 5;

const char *USER_IMG_PATH = "assets/Barry.png";
const char *LOG_PATH = "assets/log.png";
const char *BACKGROUND_PATH = "assets/BackdropMain.png";
const char *ZAPPER_PATH = "assets/Zapper1.png";

struct background_state {
  asset_t *bg1;
  asset_t *bg2;
  double scroll_speed;
  double bg_offset;
};

struct state_temp {
  background_state_t *background_state;
  list_t *body_assets;
  body_t *user;
  scene_t *scene;
  int16_t points;
};

struct game_play_state {
  double time;
  state_type_t curr_state;
  state_temp_t *state;
};

body_type_t *make_type_info(body_type_t type) {
  body_type_t *info = malloc(sizeof(body_type_t));
  assert(info != NULL);
  *info = type;
  return info;
}

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
  body_type_t *user_type = make_type_info(USER);
  body_t *user =
      body_init_with_info(c, 1, user_color, user_type, free);
  return user;
}

body_type_t get_type(body_t *body) {
  void *body_info = body_get_info(body);
  assert(body_info != NULL);
  return *(body_type_t *)body_info;
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

body_t *make_zapper(vector_t center, double width, double height) {
  list_t *zapper_shape = make_rectangle(center, width, height);
  body_t *zapper = body_init_with_info(zapper_shape, INFINITY, black,
                                      make_type_info(ZAPPER), free);
  body_set_velocity(zapper, ZAPPER_VEL);
  body_set_centroid(zapper, center);
  return zapper;
}

/**
 * Adds walls as bodies to the scene.
 *
 * @param state the current state of the demo
 */
void add_walls(state_temp_t *state) {
  list_t *ceiling_shape =
      make_rectangle((vector_t){MAX.x / 2, MAX.y - 50}, MAX.x, WALL_DIM);
  body_t *ceiling = body_init_with_info(ceiling_shape, INFINITY, black,
                                        make_type_info(CEILING), free);
  list_t *ground_shape =
      make_rectangle((vector_t){MAX.x / 2, MIN.y + 50}, MAX.x, WALL_DIM);
  body_t *ground = body_init_with_info(ground_shape, INFINITY, black,
                                       make_type_info(GROUND), free);
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
  // fprintf(stderr, "before getting user\n");
  // fprintf(stderr, "scene_bodies inside: %zu\n", scene_bodies(game_play_state->state->scene));
  // fprintf(stderr, "after getting user\n");
  if (key == SPACE_BAR) {
    if (type == KEY_PRESSED) {
      body_set_velocity(game_play_state->state->user, USER_VEL);
      // fprintf(stderr, "space bar hit\n");
    } else if (type == KEY_RELEASED) {
      body_set_velocity(game_play_state->state->user, vec_negate(USER_VEL));
      // fprintf(stderr, "no space bar hit\n");
    }
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
  if (state->bg_offset <= - WINDOW_WIDTH) {
    state->bg_offset += WINDOW_WIDTH;
  }
  state->bg1->bounding_box.x = state->bg_offset;
  state->bg2->bounding_box.x = state->bg_offset + WINDOW_WIDTH;
}

/**
 * Adds collision handler force creators between appropriate bodies.
 *
 * @param state the current state of the demo
 */
void add_force_creators(game_play_state_t *game_play_state) {
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  // fprintf(stderr, "num bodies fcs: %zu\n", num_bodies);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    // TODO: either this or the centorid check but not both
    if (get_type(body) == CEILING || get_type(body) == GROUND) {
      create_physics_collision(game_play_state->state->scene, game_play_state->state->user, body, ELASTICITY);
    }
    // TODO: lag if stuck on ceiling nad then let go
  }
}

game_play_state_t *game_play_init() {
  game_play_state_t *game_play_state = malloc(sizeof(game_play_state_t));
  assert(game_play_state != NULL);

  asset_cache_init();
  sdl_init(MIN, MAX);
  srand(time(NULL));
  state_temp_t *state = malloc(sizeof(state_temp_t));
  assert(state != NULL);

  state->scene = scene_init();
  state->body_assets = list_init(1, (free_func_t)asset_destroy);
  state->user = make_user(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
  vector_t start_pos = {MAX.x / 2, MIN.y + OUTER_RADIUS + 50};
  body_set_centroid(state->user, start_pos);
  scene_add_body(state->scene, state->user);
  asset_t *img = asset_make_image_with_body(USER_IMG_PATH, state->user);
  list_add(state->body_assets, img);
  // fprintf(stderr, "scene_bodies: %zu\n", scene_bodies(state->scene));
  sdl_on_key((key_handler_t)on_key);
  state->background_state = background_init(BACKGROUND_PATH);
  game_play_state->state = state;
  add_walls(game_play_state->state);
  add_force_creators(game_play_state);
  game_play_state->curr_state = GAME_PLAY;

  game_play_state->state = state;
  game_play_state->time = 0;
  return game_play_state;
}

void game_over(body_t *body1, body_t *body2, vector_t axis, void *aux,
                        double force_const) {
  fprintf(stderr, "game over!\n");
  game_play_state_t *game_play_state = (game_play_state_t *) aux;
  game_play_state->curr_state = GAME_OVER;
}

void add_zapper(game_play_state_t *game_play_state, double dt) {
  game_play_state->time += dt;
  if (game_play_state->time >= ZAPPER_GENERATION_TIME) {
    fprintf(stderr, "added zapper!\n");
    game_play_state->time = 0;
    double y_pos = fmod(rand(), (MAX.y - 50) - (MIN.y + 50));
    double x_pos = MAX.x + 15;
    vector_t center = {.x = x_pos, .y = y_pos};
    body_t *zapper = make_zapper(center, ZAPPER_WIDTH, ZAPPER_HEIGHT);
    scene_add_body(game_play_state->state->scene, zapper);

    asset_t *img = asset_make_image_with_body(ZAPPER_PATH, zapper);
    list_add(game_play_state->state->body_assets, img);
    // fprintf(stderr, "before collision\n");
    create_collision(game_play_state->state->scene, zapper, game_play_state->state->user, game_over, game_play_state,
                       0);
    // fprintf(stderr, "after collision\n");
  }
}

void remove_zappers(game_play_state_t *game_play_state) {
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == ZAPPER && body_get_centroid(body).x + 50 < MIN.x) {
      scene_remove_body(game_play_state->state->scene, i);
      fprintf(stderr, "removed zapper!\n");
    }
  }
}

state_type_t game_play_main(game_play_state_t *game_play_state) {

  double dt = time_since_last_tick();
  state_temp_t *state = game_play_state->state;
  add_zapper(game_play_state, dt);
  sdl_clear();

  background_update(state->background_state, dt);
  sdl_render_scene(state->scene, NULL);
  // asset_render(state->background_state->bg1);
  // asset_render(state->background_state->bg2);
  // TODO: got stuck below the ceiling

  size_t num_assets = list_size(state->body_assets);
  for (size_t i = 0; i < num_assets; i++) {
    asset_render(list_get(state->body_assets, i));
  }

  // TODO: change user image to be jetpack and circular so that it works
  // TODO: store ground and ceiling in state
  // TODO: slow without asan sometimes, dont make images if not needed
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    vector_t user_centroid = body_get_centroid(game_play_state->state->user);
    vector_t user_vel = body_get_velocity(game_play_state->state->user);
    vector_t body_centroid = body_get_centroid(body);
    double displacement = OUTER_RADIUS;
    if (get_type(body) == GROUND) {
      if (user_vel.y < 0 && user_centroid.y - body_centroid.y < displacement) {
        vector_t new_centroid = {.x = body_centroid.x, .y = body_centroid.y + displacement};
        body_set_centroid(game_play_state->state->user, new_centroid);
      }
    } else if (get_type(body) == CEILING) {
      if (user_vel.y > 0 && body_centroid.y - user_centroid.y < displacement) {
        vector_t new_centroid = {.x = body_centroid.x, .y = body_centroid.y - displacement};
        body_set_centroid(game_play_state->state->user, new_centroid);
      }
    }
  }

  remove_zappers(game_play_state);

  // for (size_t i = 0; i < list_size(state->background); i++) {
  //   asset_update_bounding_box(list_get(state->background, i), 1000*dt);
  //   asset_render(list_get(state->background, i));
  // }
  sdl_show();

  scene_tick(state->scene, dt);
  // fprintf(stderr, "y of the user %f\n", body_get_centroid(game_play_state->state->user).y);
  return game_play_state->curr_state;
}

void game_play_free(game_play_state_t *game_play_state) {
  state_temp_t *state = game_play_state->state;
  asset_destroy(state->background_state->bg1);
  asset_destroy(state->background_state->bg2);
  free(state->background_state);
  free(state->user);
  size_t num_assets = list_size(state->body_assets);
  for (size_t i = 0; i < num_assets; i++) {
    asset_destroy(list_get(state->body_assets, i));
  }
  // TODO: add int main and link and compile to find memory leaks
  list_free(state->body_assets);
  // TODO: why is this failing
  // scene_free(state->scene);
  asset_cache_destroy();
  free(state);
  free(game_play_state);
}
