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

const char *FROGGER_PATH = "assets/frogger.png";
const char *LOG_PATH = "assets/log.png";
const char *BACKGROUND_PATH = "assets/frogger-background.png";

typedef struct state_temp {
  list_t *body_assets;
  asset_t *frog;
  scene_t *scene;
  int16_t points;
} state_temp_t;

typedef struct game_play_state {
  double time;
  state_temp_t *state;
} game_play_state_t;



// body_t *make_obstacle(size_t w, size_t h, vector_t center) {
//   list_t *c = list_init(4, free);
//   vector_t *v1 = malloc(sizeof(vector_t));
//   *v1 = (vector_t){0, 0};
//   list_add(c, v1);

//   vector_t *v2 = malloc(sizeof(vector_t));
//   *v2 = (vector_t){w, 0};
//   list_add(c, v2);

//   vector_t *v3 = malloc(sizeof(vector_t));
//   *v3 = (vector_t){w, h};
//   list_add(c, v3);

//   vector_t *v4 = malloc(sizeof(vector_t));
//   *v4 = (vector_t){0, h};
//   list_add(c, v4);
//   body_t *obstacle = body_init(c, 1, obs_color);
//   body_set_centroid(obstacle, center);
//   return obstacle;
// }

// body_t *make_frog(double outer_radius, double inner_radius, vector_t center) {
//   center.y += inner_radius;
//   list_t *c = list_init(SHIP_NUM_POINTS, free);
//   for (size_t i = 0; i < SHIP_NUM_POINTS; i++) {
//     double angle = 2 * M_PI * i / SHIP_NUM_POINTS;
//     vector_t *v = malloc(sizeof(*v));
//     *v = (vector_t){center.x + inner_radius * cos(angle),
//                     center.y + outer_radius * sin(angle)};
//     list_add(c, v);
//   }
//   body_t *froggy = body_init(c, 1, frog_color);
//   return froggy;
// }

// void wrap_edges(body_t *body) {
//   vector_t centroid = body_get_centroid(body);
//   if (centroid.x > MAX.x) {
//     body_set_centroid(body, (vector_t){MIN.x, centroid.y});
//   } else if (centroid.x < MIN.x) {
//     body_set_centroid(body, (vector_t){MAX.x, centroid.y});
//   } else if (centroid.y > MAX.y) {
//     body_set_centroid(body, (vector_t){centroid.x, MIN.y});
//   } else if (centroid.y < MIN.y) {
//     body_set_centroid(body, (vector_t){centroid.x, MAX.y});
//   }
// }

// void reset_user(body_t *body) { body_set_centroid(body, RESET_POS); }

// void reset_user_handler(body_t *body1, body_t *body2, vector_t axis, void *aux,
//                         double force_const) {
//   reset_user(body1);
// }

// void player_wrap_edges(state_temp_t *state) {
//   body_t *player = scene_get_body(state->scene, 0);
//   vector_t centroid = body_get_centroid(player);
//   if (centroid.y > MAX.y - V_STEP) {
//     state->points += 1;
//     reset_user(player);
//     fprintf(stdout, "You have %d points!\n", state->points);
//   }
// }

// void on_key(char key, key_event_type_t type, double held_time, state_temp_t *state) {
//   body_t *froggy = scene_get_body(state->scene, 0);
//   vector_t translation = (vector_t){0, 0};
//   if (type == KEY_PRESSED && type != KEY_RELEASED) {
//     switch (key) {
//     case LEFT_ARROW:
//       translation.x = -H_STEP;
//       break;
//     case RIGHT_ARROW:
//       translation.x = H_STEP;
//       break;
//     case UP_ARROW:
//       translation.y = V_STEP;
//       break;
//     case DOWN_ARROW:
//       if (body_get_centroid(froggy).y > START_POS.y) {
//         translation.y = -V_STEP;
//       }
//       break;
//     }
//     vector_t new_centroid = vec_add(body_get_centroid(froggy), translation);
//     body_set_centroid(froggy, new_centroid);
//   }
// }

// double rand_double(double low, double high) {
//   return (high - low) * rand() / RAND_MAX + low;
// }



game_play_state_t *game_play_init() {
  game_play_state_t *game_play_state = malloc(sizeof(game_play_state_t));
  assert(game_play_state);

  asset_cache_init();
  sdl_init(MIN, MAX);
  state_temp_t *state = malloc(sizeof(state_temp_t));
  // state->points = 0;
  // srand(time(NULL));
  state->scene = scene_init();
  state->body_assets = list_init(2, (free_func_t)asset_destroy);

  // body_t *froggy = make_frog(OUTER_RADIUS, INNER_RADIUS, VEC_ZERO);
  // body_set_centroid(froggy, RESET_POS);

  // scene_add_body(state->scene, froggy);

  SDL_Rect bounding_background = make_texr(MIN.x, MIN.y, MAX.x, MAX.y);
  asset_t *background = asset_make_image(BACKGROUND_PATH, bounding_background);
  list_add(state->body_assets, background);
  // asset_t *img = asset_make_image_with_body(FROGGER_PATH, froggy);
  // list_add(state->body_assets, img);

  // for (size_t r = 3; r < ROWS + 3; r++) {
  //   double cx = 0;
  //   double cy = r * V_STEP + body_get_centroid(froggy).y;
  //   double multiplier = 0;
  //   if (r % 2 == 0) {
  //     multiplier = 1;
  //   } else {
  //     multiplier = -1;
  //   }
  //   if ((double)rand() / RAND_MAX < VEL_MULT_PROB) {
  //     multiplier *= EXTRA_VEL_MULT;
  //   }
  //   while (cx < MAX.x) {
  //     double w = rand_double(OBS_WIDTHS.x, OBS_WIDTHS.y);
  //     body_t *obstacle = make_obstacle(w, OBSTACLE_HEIGHT, (vector_t){cx, cy});
  //     cx += w + rand_double(OBS_SPACING.x, OBS_SPACING.y);

  //     body_set_velocity(obstacle, vec_multiply(multiplier, BASE_OBJ_VEL));
  //     scene_add_body(state->scene, obstacle);

  //     create_collision(state->scene, froggy, obstacle, reset_user_handler, NULL,
  //                      0);

  //     asset_t *log = asset_make_image_with_body(LOG_PATH, obstacle);
  //     list_add(state->body_assets, log);
  //   }
  // }
  sdl_on_key((key_handler_t)on_key);
  game_play_state->state = state;
  game_play_state->time = 0;
  return game_play_state;
}

bool game_play_main(game_play_state_t *game_play_state) {
  //sdl_clear();

  double dt = time_since_last_tick();
  state_temp_t *state = game_play_state->state;
  // player_wrap_edges(state);
  // for (int i = 1; i < scene_bodies(state->scene); i++) {
  //   wrap_edges(scene_get_body(state->scene, i));
  // }
  sdl_clear();
  for (size_t i = 0; i < list_size(state->body_assets); i++) {
    asset_render(list_get(state->body_assets, i));
  }
  sdl_show();

  scene_tick(state->scene, dt);
  return false;
}

void game_play_free(game_play_state_t *game_play_state) {
  state_temp_t *state = game_play_state->state;
  list_free(state->body_assets);
  scene_free(state->scene);
  asset_cache_destroy();
  free(state);
  //TTF_Quit();
  //asset_cache_destroy();
  free(game_play_state);
}
