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
#include "subject.h"
#include "observer.h"
#include "achievement.h"

// TODO: rockets laggy
// TODO: lasers appear randomly
// TODO: delay for saying removed active lasers

const double WEDGE_ANGLE = 3.6 * M_PI / 3;
const double INCREMENT_ANGLE = 0.1;
const double RADIUS = 40;
const double BULLET_RADIUS = 10;
const double ELASTICITY = 0;
const double G_CONSTANT = 500.0;

const vector_t START_POS = {500, 30};
const vector_t INVADER_BULLET_VEL = {0, -200};
const vector_t USER_VEL = {0, 200};
const vector_t BASE_OBJ_VEL = {30, 0};
const vector_t ZAPPER_VEL = {-100, 0};
const vector_t POWERUP_VEL = {-100, 0};
const vector_t ROCKET_VEL = {-300, 0};
const vector_t COIN_VEL = {-100, 0};
const double EXTRA_VEL_MULT = 10;
const double VEL_MULT_PROB = 0.2;

const double MASS = 5;
const double BULLET_MASS = 10;

const size_t SPAWN_TIME = 200; // number of frames in between new shapes

const double resting_speed = 300;
const double ACCEL = 100;

// USER: 
const double USER_OUTER_RADIUS = 15;
const double USER_INNER_RADIUS = 15;

// ZAPPER:
const double ZAPPER_WIDTH = 46;
const double ZAPPER_HEIGHT = 109;
const size_t OBSTACLE_HEIGHT = 30;
const vector_t OBS_WIDTHS = {30, 70};
const vector_t OBS_SPACING = {120, 350};

// LASERS: 
const double LASER_WIDTH_ACTIVE = 990;
const double LASER_HEIGHT_ACTIVE = 42.14;

// ROCKETS: 
const double ROCKET_WIDTH = 90;
const double ROCKET_HEIGHT = 45;
const double WARNING_WIDTH = 50;
const double WARNING_HEIGHT = 50;

// COINS:
const double COIN_OUTER_RADIUS = 10;
const double COIN_INNER_RADIUS = 10;
const size_t MIN_COIN_GRID_SIZE = 2;
const size_t MAX_COIN_GRID_SIZE = 4;
const size_t NUM_COINS = 20;
const size_t COIN_TEXT_SIZE = 50;
const size_t COIN_FONT_SIZE = 30;
const SDL_Rect COIN_BOX = (SDL_Rect){25, 75, 0, 0};

// POWERUPS:
const double POWERUP_WIDTH = 60;
const double POWERUP_HEIGHT = 60;
const size_t NUM_POWERUPS = 4;

// DISTANCE:
const size_t DISTANCE_TEXT_SIZE = 50;
const size_t DISTANCE_FONT_SIZE = 30;
const SDL_Rect DISTANCE_BOX = (SDL_Rect){25, 25, 0, 0};
const double UPDATE_INTERVAL = 0.5;

vector_t LASER1 = {.x = 500, .y = 80};
vector_t LASER2 = {.x = 500, .y = 101.25};
vector_t LASER3 = {.x = 500, .y = 143.75};
vector_t LASER4 = {.x = 500, .y = 186.25};
vector_t LASER5 = {.x = 500, .y = 228.75};
vector_t LASER6 = {.x = 500, .y = 271.25};
vector_t LASER7 = {.x = 500, .y = 313.75};
vector_t LASER8 = {.x = 500, .y = 356.25};
vector_t LASER9 = {.x = 500, .y = 398.75};
vector_t LASER10 = {.x = 500, .y = 420};


// TODO: First make a basic laser appear on the screen with some frequency
// Then, make the laser appear with a frequency based on a specific algorithm
// Then, fix the positions of the laser based on the closest location
// Then add, animation to the laser. 


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

const double ZAPPER_GENERATION_TIME = 5;
const double MIN_COIN_GENERATION_TIME = 3;
const double MAX_COIN_GENERATION_TIME = 6;
const double MIN_POWERUP_GENERATION_TIME = 3;
const double MAX_POWERUP_GENERATION_TIME = 6;
const double LASER_GENERATION_TIME = 12;
const double ROCKET_GENERATION_TIME = 10;

const double MIN_ZAPPER_GENERATION_TIME_EASY = 3;
const double MAX_ZAPPER_GENERATION_TIME_EASY = 5;
const double MIN_ZAPPER_GENERATION_TIME_MEDIUM = 2;
const double MAX_ZAPPER_GENERATION_TIME_MEDIUM = 3;
const double MIN_ZAPPER_GENERATION_TIME_HARD = 1;
const double MAX_ZAPPER_GENERATION_TIME_HARD = 2;

const char *USER_IMG_PATH = "assets/Barry.png";
const char *LOG_PATH = "assets/log.png";
const char *BACKGROUND_PATH = "assets/BackdropMain.png";
const char *ZAPPER_PATH = "assets/Zapper1.png";
const char *COIN_PATH = "assets/Coin.png";
const char *POWERUP_PATH = "assets/powerup.jpg";
const char *LASER_PATH_INACTIVE = "assets/laser_noneactive.png";
const char *LASER_PATH_ACTIVE = "assets/laser_active.png";
const char *ROCKET_WARNING_PATH = "assets/warning.png";
const char *ROCKET_PATH = "assets/missle.png";

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
  body_t *ceiling;
  body_t *ground;
  scene_t *scene;
  int16_t points;
};

struct laser_state {
  // This is to use algorithm of random spawning
  double time_laser;
  // This is to track times when event happens
  double time_laser_spawn;
  double time_laser_activate;
  bool laser_inactive;
  bool laser_active;
  
  // // REMOVE LATER
  // asset_t *laser_inactive_asset;
  // asset_t *laser_active_asset;

  list_t *laser_inactive_assets;
  list_t *laser_active_assets;

  list_t *laser_centers;
  list_t *laser_spawn_positions;
};


struct coin_state {
  double time_until_coin;
  double coin_time;
  size_t coin_count;
};

struct powerup_state {
  double time_until_powerup;
  double powerup_time;
  double powerup_start_time;
  bool powerup_active;
  powerup_type_t powerup_type;
};

struct rocket_state {
  // This is to use algorithm of random spawning
  double time_rocket;
  // This is to track times when event happens
  double time_rocket_spawn;
  double time_rocket_activate;
  bool rocket_inactive;
  bool rocket_active;    
  
  asset_t *rocket_inactive_asset;
  // asset_t *rocket_active_asset;
  vector_t rocket_spawn_position;
};

struct game_play_state {
  double time;
  double last_update_time;
  double distance_traveled;
  double time_until_zapper;
  double min_zapper_generation_time;
  double max_zapper_generation_time;
  double zapper_time;
  TTF_Font *distance_font;
  TTF_Font *coins_collected_font;
  laser_state_t *laser;
  coin_state_t *coin;
  powerup_state_t *powerup;
  rocket_state_t *rocket;
  state_type_t curr_state;
  subject_t *subject;
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
      body_init_with_info(c, 10000, user_color, user_type, free);
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

body_t *make_laser(vector_t *center) {
  list_t *laser_shape = make_rectangle(*center, LASER_WIDTH_ACTIVE, LASER_HEIGHT_ACTIVE);
  body_t *laser = body_init_with_info(laser_shape, INFINITY, black,
                                      make_type_info(LASER), free);
  body_set_velocity(laser, VEC_ZERO);
  body_set_centroid(laser, *center);
  return laser;
}

body_t *make_laser_active(vector_t *center) {
  list_t *laser_shape = make_rectangle(*center, LASER_WIDTH_ACTIVE, LASER_HEIGHT_ACTIVE);
  body_t *laser = body_init_with_info(laser_shape, INFINITY, red,
                                      make_type_info(LASER_ACTIVE), free);
  body_set_velocity(laser, VEC_ZERO);
  body_set_centroid(laser, *center);
  return laser;
}


body_t *make_coin(double outer_radius, double inner_radius, vector_t center) {
  center.y += inner_radius;
  list_t *coin_shape = list_init(USER_NUM_POINTS, free);
  for (size_t i = 0; i < USER_NUM_POINTS; i++) {
    double angle = 2 * M_PI * i / USER_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + inner_radius * cos(angle),
                    center.y + outer_radius * sin(angle)};
    list_add(coin_shape, v);
  }
  body_t *coin =
      body_init_with_info(coin_shape, 1, blue, make_type_info(COIN), free);
  body_set_velocity(coin, COIN_VEL);
  body_set_centroid(coin, center);
  return coin;
}

body_t *make_powerup(vector_t center, double width, double height) {
  list_t *powerup_shape = make_rectangle(center, width, height);
  body_t *powerup = body_init_with_info(powerup_shape, INFINITY, black,
                                      make_type_info(POWERUP), free);
  body_set_velocity(powerup, POWERUP_VEL);
  body_set_centroid(powerup, center);
  return powerup;
}

body_t *make_rocket_warning(vector_t *center) {
  list_t *warning_shape = make_rectangle(*center, WARNING_WIDTH, WARNING_HEIGHT);
  body_t *warning = body_init_with_info(warning_shape, INFINITY, black,
                                      make_type_info(ROCKET_WARNING), free);
  body_set_velocity(warning, VEC_ZERO);
  body_set_centroid(warning, *center);
  return warning;
}

body_t *make_rocket(vector_t *center) {
  list_t *rocket_shape = make_rectangle(*center, ROCKET_WIDTH, ROCKET_HEIGHT);
  body_t *rocket = body_init_with_info(rocket_shape, INFINITY, red,
                                      make_type_info(ROCKET), free);
  body_set_velocity(rocket, ROCKET_VEL);
  body_set_centroid(rocket, *center);
  return rocket;
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
  state->ceiling = ceiling;
  scene_add_body(state->scene, ground);
  state->ground = ground;
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

void init_observers(game_play_state_t *game_play_state) {  
  game_play_state->subject = subject_init();
  achievements_t *achievements = achievements_init();
  fprintf(stderr, "Observer created notify function is %p\n", (void*)achievements->observer.on_notify);
  subject_add_observer(game_play_state->subject, &(achievements->observer));
  fprintf(stderr, "End of init_observers in game_play.c\n");
}

game_play_state_t *game_play_init(difficulty_type_t difficulty_level) {
  game_play_state_t *game_play_state = malloc(sizeof(game_play_state_t));
  assert(game_play_state != NULL);

  asset_cache_init();
  sdl_init(MIN, MAX);
  srand(time(NULL));
  // state is defined here
  state_temp_t *state = malloc(sizeof(state_temp_t));
  
  assert(state != NULL);
  sdl_on_key((key_handler_t)on_key);

  state->scene = scene_init();
  state->body_assets = list_init(1, (free_func_t)asset_destroy);

  // SET everything for the user
  state->user = make_user(USER_OUTER_RADIUS, USER_INNER_RADIUS, VEC_ZERO);
  vector_t start_pos = {MAX.x / 2, MIN.y + USER_OUTER_RADIUS + 50};
  body_set_centroid(state->user, start_pos);
  scene_add_body(state->scene, state->user);
  asset_t *img = asset_make_image_with_body(USER_IMG_PATH, state->user);
  list_add(state->body_assets, img);



  // fprintf(stderr, "scene_bodies: %zu\n", scene_bodies(state->scene));
  // Walls and Backgrounds:
  state->background_state = background_init(BACKGROUND_PATH);
  game_play_state->state = state;
  add_walls(game_play_state->state);
  game_play_state->curr_state = GAME_PLAY;

  game_play_state->laser = malloc(sizeof(laser_state_t));
  game_play_state->rocket = malloc(sizeof(rocket_state_t));
  game_play_state->coin = malloc(sizeof(coin_state_t));
  game_play_state->powerup = malloc(sizeof(powerup_state_t));

  // ROCKET
  game_play_state->rocket->rocket_inactive = false;
  game_play_state->rocket->rocket_active = false;
  game_play_state->rocket->time_rocket = 0;
  game_play_state->rocket->time_rocket_spawn = 0;
  game_play_state->rocket->time_rocket_activate = 0;
  // SDL_Rect bounding_box_rocket = make_texr(-2000, 0, ROCKET_WIDTH, ROCKET_HEIGHT);
  SDL_Rect bounding_box_rocket_warning = make_texr(-2000, 0, WARNING_WIDTH, WARNING_HEIGHT);

  // game_play_state->rocket->rocket_active_asset = asset_make_image(ROCKET_PATH, bounding_box_rocket);
  game_play_state->rocket->rocket_inactive_asset = asset_make_image(ROCKET_WARNING_PATH, bounding_box_rocket_warning);

  // LASER
  game_play_state->laser->laser_inactive = false;
  game_play_state->laser->laser_active = false;
  game_play_state->state = state;
  game_play_state->time = 0;
  game_play_state->last_update_time = 0;
  game_play_state->distance_font = init_font(FONT_PATH, DISTANCE_FONT_SIZE);
  game_play_state->coins_collected_font = init_font(FONT_PATH, COIN_FONT_SIZE);
  game_play_state->distance_traveled = 0;
  game_play_state->time_until_zapper = 0;
  game_play_state->coin->time_until_coin = 0;
  game_play_state->zapper_time = 0;
  game_play_state->coin->coin_time = 0;
  game_play_state->laser->time_laser = 0;
  game_play_state->laser->time_laser_spawn = 0;
  game_play_state->laser->time_laser_activate = 0;
  game_play_state->coin->coin_count = 0;
  game_play_state->powerup->time_until_powerup = 0;
  game_play_state->powerup->powerup_time = 0;
  game_play_state->powerup->powerup_start_time = 0;
  game_play_state->powerup->powerup_active = false;
  game_play_state->powerup->powerup_type = OBSTACLE;

  SDL_Rect bounding_box_laser = make_texr(-2000, 0, LASER_WIDTH_ACTIVE, LASER_HEIGHT_ACTIVE);
  // asset_t *active_laser = asset_make_image(LASER_PATH_ACTIVE, bounding_box);
  // asset_t *inactive_laser = asset_make_image(LASER_PATH_INACTIVE, bounding_box);

  game_play_state->laser->laser_active_assets = list_init(10, NULL);
  game_play_state->laser->laser_inactive_assets = list_init(10, NULL);

  for (size_t i = 0; i < 10; i++) {
    asset_t *laser_active = asset_make_image(LASER_PATH_ACTIVE, bounding_box_laser);
    list_add(game_play_state->laser->laser_active_assets, laser_active);
    asset_t *laser_inactive = asset_make_image(LASER_PATH_INACTIVE, bounding_box_laser);
    list_add(game_play_state->laser->laser_inactive_assets, laser_inactive);  
    list_add(game_play_state->state->body_assets, laser_active);
    list_add(game_play_state->state->body_assets, laser_inactive);
  }

  game_play_state->laser->laser_centers = list_init(10, NULL);
  game_play_state->laser->laser_spawn_positions = list_init(10, (free_func_t) free);


  // list_add(game_play_state->laser->laser_centers, &LASER1);
  // for (size_t i = 0; i < 8; i++){
  //   vector_t center = {.x = 500, .y = 101.25 + 42.5 * i};
  //   list_add(game_play_state->laser->laser_centers, &center);
  // }
  // list_add(game_play_state->laser->laser_centers, &LASER10);  

  list_add(game_play_state->laser->laser_centers, &LASER1);
  list_add(game_play_state->laser->laser_centers, &LASER2);
  list_add(game_play_state->laser->laser_centers, &LASER3);
  list_add(game_play_state->laser->laser_centers, &LASER4);
  list_add(game_play_state->laser->laser_centers, &LASER5);
  list_add(game_play_state->laser->laser_centers, &LASER6);
  list_add(game_play_state->laser->laser_centers, &LASER7);
  list_add(game_play_state->laser->laser_centers, &LASER8);
  list_add(game_play_state->laser->laser_centers, &LASER9);
  list_add(game_play_state->laser->laser_centers, &LASER10);
  
  if (difficulty_level == EASY) {
    fprintf(stdout, "easy\n");
  }
  if (difficulty_level == MEDIUM) {
    fprintf(stdout, "medium\n");
  }
  if (difficulty_level == HARD) {
    fprintf(stdout, "hard\n");
  }
  switch (difficulty_level) {
      case EASY: {
          game_play_state->min_zapper_generation_time = 
                                              MIN_ZAPPER_GENERATION_TIME_EASY;
          game_play_state->max_zapper_generation_time = 
                                              MAX_ZAPPER_GENERATION_TIME_EASY;
          break;
      }
      case MEDIUM: {
          game_play_state->min_zapper_generation_time = 
                                              MIN_ZAPPER_GENERATION_TIME_MEDIUM;
          game_play_state->max_zapper_generation_time = 
                                              MAX_ZAPPER_GENERATION_TIME_MEDIUM;
          break;
      }
      case HARD: {
          game_play_state->min_zapper_generation_time = 
                                              MIN_ZAPPER_GENERATION_TIME_HARD;
          game_play_state->max_zapper_generation_time = 
                                              MAX_ZAPPER_GENERATION_TIME_HARD;
          break;
      }
      default: {
          break;
      }
  }

  init_observers(game_play_state);

  return game_play_state;
}

void game_over(body_t *body1, body_t *body2, vector_t axis, void *aux1,
                        void *aux2, double force_const) {
  fprintf(stderr, "game over!\n");
  game_play_state_t *game_play_state = (game_play_state_t *) aux1;
  game_play_state->curr_state = GAME_OVER;
}

void collect_coin(body_t *body1, body_t *body2, vector_t axis, void *aux1,
                        void *aux2, double force_const) {
  fprintf(stderr, "collected coin!\n");
  asset_t *asset = (asset_t *) aux1;
  body_remove(body1);
  asset_update_bounding_box_x(asset, -1000);

  game_play_state_t *game_play_state = (game_play_state_t *) aux2;
  game_play_state->coin->coin_count++;
  subject_notify(game_play_state->subject, EVENT_COIN_COLLECTED, NULL);
}

void disable_obstacles_powerup(body_t *body1, body_t *body2, vector_t axis, void *aux1,
                        void *aux2, double force_const) {
  fprintf(stderr, "disable obstacles powerup!\n");
  asset_t *asset = (asset_t *) aux1;
  body_remove(body1);
  asset_update_bounding_box_x(asset, -1000);

  game_play_state_t *game_play_state = (game_play_state_t *) aux2;
}

void increase_coin_freq_powerup(body_t *body1, body_t *body2, vector_t axis, void *aux1,
                        void *aux2, double force_const) {
  fprintf(stderr, "increase coin generation powerup!\n");
  asset_t *asset = (asset_t *) aux1;
  body_remove(body1);
  asset_update_bounding_box_x(asset, -1000);

  game_play_state_t *game_play_state = (game_play_state_t *) aux2;
}

void magnetic_coin_powerup(body_t *body1, body_t *body2, vector_t axis, void *aux1,
                        void *aux2, double force_const) {
  fprintf(stderr, "magnetic coin powerup!\n");
  asset_t *asset = (asset_t *) aux1;
  body_remove(body1);
  asset_update_bounding_box_x(asset, -1000);

  game_play_state_t *game_play_state = (game_play_state_t *) aux2;
  if (!game_play_state->powerup->powerup_active) {
    game_play_state->powerup->powerup_active = true;
    game_play_state->powerup->powerup_start_time = game_play_state->time;
    game_play_state->powerup->powerup_type = MAGNETIC;
  }
}

void gravity_swap_powerup(body_t *body1, body_t *body2, vector_t axis, void *aux1,
                        void *aux2, double force_const) {
  fprintf(stderr, "gravity swap powerup!\n");
  asset_t *asset = (asset_t *) aux1;
  body_remove(body1);
  asset_update_bounding_box_x(asset, -1000);

  game_play_state_t *game_play_state = (game_play_state_t *) aux2;
}

void remove_zappers(game_play_state_t *game_play_state) {
  // TODO: remove from body assets once deleted so the image is not re-rendered each time
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == ZAPPER && body_get_centroid(body).x + 50 < MIN.x) {
      scene_remove_body(game_play_state->state->scene, i);
      fprintf(stderr, "removed zapper!\n");
    }
  }
}

void remove_coins(game_play_state_t *game_play_state) {
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == COIN && body_get_centroid(body).x + 50 < MIN.x) {
      scene_remove_body(game_play_state->state->scene, i);
      fprintf(stderr, "removed coin!\n");
    }
  }
}

void remove_powerups(game_play_state_t *game_play_state) {
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == POWERUP && body_get_centroid(body).x + 50 < MIN.x) {
      scene_remove_body(game_play_state->state->scene, i);
      fprintf(stderr, "removed powerup!\n");
    }
  }
}


void remove_lasers_inactive(game_play_state_t *game_play_state) {
  // size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < scene_bodies(game_play_state->state->scene); i++) {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == LASER) {
      // asset_update_bounding_box_x(game_play_state->laser_inactive_asset, 2 * 1000);
      scene_remove_body(game_play_state->state->scene, i);
      fprintf(stderr, "removed inactive laser!\n");
    }
  }

  for (size_t i = 0; i < 10; i++) {
    asset_update_bounding_box_x(list_get(game_play_state->laser->laser_inactive_assets, i), 2 * 1000);
  }
}

void remove_lasers(game_play_state_t *game_play_state) {
  if (game_play_state->time - game_play_state->laser->time_laser_activate >= 3) {
    game_play_state->laser->laser_active = false;                        
    game_play_state->laser->time_laser_activate = game_play_state->time;
    // size_t num_bodies = scene_bodies(game_play_state->state->scene);
    for (size_t i = 0; i < scene_bodies(game_play_state->state->scene); i++) {
      body_t *body = scene_get_body(game_play_state->state->scene, i);
      if (get_type(body) == LASER_ACTIVE) {
        // asset_update_bounding_box_x(game_play_state->laser_active_asset, 2 * 1000);
        scene_remove_body(game_play_state->state->scene, i);
        fprintf(stderr, "removed laser!\n");
        subject_notify(game_play_state->subject, EVENT_LASERS_AVOIDED, NULL);
      }
    }

    for (size_t i = 0; i < 10; i++) {
      asset_update_bounding_box_x(list_get(game_play_state->laser->laser_active_assets, i), 2 * 1000);
    }
  }
}


void remove_warnings(game_play_state_t *game_play_state) {
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == ROCKET_WARNING) {
      asset_update_bounding_box_x(game_play_state->rocket->rocket_inactive_asset, 2 * 1000);
      scene_remove_body(game_play_state->state->scene, i);
      fprintf(stderr, "removed warning!\n");
    }
  }
}

void remove_rockets(game_play_state_t *game_play_state) {
  game_play_state->rocket->time_rocket_activate = game_play_state->time;
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == ROCKET && body_get_centroid(body).x + 50 < MIN.x) {
      // asset_update_bounding_box_x(game_play_state->rocket->rocket_active_asset, 2 * 1000);
      scene_remove_body(game_play_state->state->scene, i);
      fprintf(stderr, "removed rocket!\n");
    }
  }
}



void add_zapper(game_play_state_t *game_play_state, double dt) {
  game_play_state->zapper_time += dt;
  bool laser_state = !game_play_state->laser->laser_active && !game_play_state->laser->laser_inactive;
  if (laser_state && game_play_state->zapper_time >= game_play_state->time_until_zapper) {
    fprintf(stderr, "added zapper!\n");
    game_play_state->zapper_time = 0;
    game_play_state->time_until_zapper = fmod(rand(), 
    game_play_state->max_zapper_generation_time - 
    game_play_state->min_zapper_generation_time) + 
    game_play_state->min_zapper_generation_time;
    double y_pos = fmod(rand(), (MAX.y - 50) - (MIN.y + 50));
    double x_pos = MAX.x + 15;
    vector_t center = {.x = x_pos, .y = y_pos};
    body_t *zapper = make_zapper(center, ZAPPER_WIDTH, ZAPPER_HEIGHT);
    scene_add_body(game_play_state->state->scene, zapper);
    asset_t *img = asset_make_image_with_body(ZAPPER_PATH, zapper);
    list_add(game_play_state->state->body_assets, img);
    // fprintf(stderr, "before collision\n");
    create_collision(game_play_state->state->scene, zapper, game_play_state->state->user, 
    game_over, game_play_state, NULL, 0);
    // fprintf(stderr, "after collision\n");
  }
}


void add_coins(game_play_state_t *game_play_state, double dt) {
  game_play_state->coin->coin_time += dt;
  bool laser_state = !game_play_state->laser->laser_active && !game_play_state->laser->laser_inactive;
  if (laser_state && game_play_state->coin->coin_time >= game_play_state->coin->time_until_coin) {
    fprintf(stderr, "added coins!\n");
    game_play_state->coin->coin_time = 0;
    game_play_state->coin->time_until_coin = fmod(rand(), 
    MAX_COIN_GENERATION_TIME - MIN_COIN_GENERATION_TIME) + 
    MIN_COIN_GENERATION_TIME;
    double y_pos = fmod(rand(), (MAX.y - 50) - (MIN.y + 50));
    double x_pos = MAX.x + 15;
    double y_shift = 40;
    double x_shift = 40;
    size_t COIN_GRID_WIDTH = (rand() % (MAX_COIN_GRID_SIZE - MIN_COIN_GRID_SIZE + 1)) + MIN_COIN_GRID_SIZE;
    size_t COIN_GRID_HEIGHT = (rand() % (MAX_COIN_GRID_SIZE - MIN_COIN_GRID_SIZE + 1)) + MIN_COIN_GRID_SIZE;
    for (size_t i = 0; i < COIN_GRID_WIDTH; i++) {
      for (size_t j = 0; j < COIN_GRID_HEIGHT; j++) {
        vector_t center = {.x = x_pos + (i * x_shift), .y = y_pos + (j * y_shift)};
        body_t *coin = make_coin(COIN_OUTER_RADIUS, COIN_INNER_RADIUS, center);
        scene_add_body(game_play_state->state->scene, coin);
        body_t *user = scene_get_body(game_play_state->state->scene, 0);
        create_newtonian_gravity(game_play_state->state->scene, G_CONSTANT, 
        user, coin);
        asset_t *img = asset_make_image_with_body(COIN_PATH, coin);
        list_add(game_play_state->state->body_assets, img);
        // fprintf(stderr, "before collision\n");
        create_collision(game_play_state->state->scene, coin, game_play_state->state->user, 
        collect_coin, img, game_play_state, 0);
      }
    }
    // vector_t center = {.x = x_pos, .y = y_pos};
    // body_t *coin = make_coin(COIN_OUTER_RADIUS, COIN_INNER_RADIUS, center);
    // scene_add_body(game_play_state->state->scene, coin);
    // asset_t *img = asset_make_image_with_body(COIN_PATH, coin);
    // list_add(game_play_state->state->body_assets, img);
    // // fprintf(stderr, "before collision\n");
    // create_collision(game_play_state->state->scene, coin, game_play_state->state->user, 
    // collect_coin, img, 0);
    // // fprintf(stderr, "after collision\n");
  }
}

void add_powerup(game_play_state_t *game_play_state, double dt) {
  game_play_state->powerup->powerup_time += dt;
  bool laser_state = !game_play_state->laser->laser_active && !game_play_state->laser->laser_inactive;
  if (laser_state && game_play_state->powerup->powerup_time >= game_play_state->powerup->time_until_powerup) {
    fprintf(stderr, "added powerup!\n");
    game_play_state->powerup->powerup_time = 0;
    game_play_state->powerup->time_until_powerup = fmod(rand(), 
    MAX_POWERUP_GENERATION_TIME - MIN_POWERUP_GENERATION_TIME) + 
    MIN_POWERUP_GENERATION_TIME;
    double y_pos = fmod(rand(), (MAX.y - 50) - (MIN.y + 50));
    double x_pos = MAX.x + 15;
    vector_t center = {.x = x_pos, .y = y_pos};
    body_t *powerup = make_powerup(center, POWERUP_WIDTH, POWERUP_HEIGHT);
    scene_add_body(game_play_state->state->scene, powerup);
    asset_t *img = asset_make_image_with_body(POWERUP_PATH, powerup);
    list_add(game_play_state->state->body_assets, img);
    // fprintf(stderr, "before collision\n");
    size_t powerup_type = (size_t) fmod(rand(), NUM_POWERUPS);
    // fprintf(stderr, "lebron;%zu\n", powerup_type);
    switch (powerup_type) {
          case OBSTACLE: {
              // fprintf(stderr, "lebron\n");
              create_collision(game_play_state->state->scene, powerup, 
              game_play_state->state->user, magnetic_coin_powerup, img, 
              game_play_state, 0);
              break;
          }
          case MORE_COIN: {
              // fprintf(stderr, "lebron1\n");
              create_collision(game_play_state->state->scene, powerup, 
              game_play_state->state->user, magnetic_coin_powerup, img, 
              game_play_state, 0);
              break;
          }
          case MAGNETIC: {
              // fprintf(stderr, "lebron2\n");
              create_collision(game_play_state->state->scene, powerup, 
              game_play_state->state->user, magnetic_coin_powerup, img, 
              game_play_state, 0);
              break;
          }
          case GRAVITY: {
              // fprintf(stderr, "lebron3\n");
              create_collision(game_play_state->state->scene, powerup, 
              game_play_state->state->user, magnetic_coin_powerup, img, 
              game_play_state, 0);
              break;
          }
          default: {
              fprintf(stderr, "lebron4\n");
              break;
          }
      }
    // create_collision(game_play_state->state->scene, powerup, game_play_state->state->user, 
    // magnetic_coin_powerup, img, game_play_state, 0);
    // fprintf(stderr, "after collision\n");
  }
}




void add_rocket(game_play_state_t *game_play_state, double dt) {
  game_play_state->rocket->time_rocket += dt;
  bool laser_state = !game_play_state->laser->laser_active && !game_play_state->laser->laser_inactive;
  if (laser_state && !game_play_state->rocket->rocket_inactive && game_play_state->rocket->time_rocket >= ROCKET_GENERATION_TIME) {
    fprintf(stderr, "added rocket warning!\n");
    game_play_state->rocket->time_rocket = 0;

    game_play_state->rocket->time_rocket_spawn = game_play_state->time;
    game_play_state->rocket->rocket_active = false;                        
    double y_pos = fmod(rand(), (MAX.y - MIN.y) - 100) + 50;
    double x_pos = MAX.x - 50;
    vector_t center = {.x = x_pos, .y = y_pos};
    game_play_state->rocket->rocket_spawn_position = center;
    body_t *warning = make_rocket_warning(&center);
    scene_add_body(game_play_state->state->scene, warning);

    asset_update_bounding_box(game_play_state->rocket->rocket_inactive_asset, warning);
    asset_t *img = game_play_state->rocket->rocket_inactive_asset;

    list_add(game_play_state->state->body_assets, img);
    game_play_state->rocket->rocket_inactive = true;
    // fprintf(stderr, "before collision\n");
    // We dont want collision in non-active state
    // fprintf(stderr, "after collision\n");
  }

  else if (game_play_state->rocket->rocket_inactive && game_play_state->time - game_play_state->rocket->time_rocket_spawn >= 3) {
    game_play_state->rocket->time_rocket_activate = game_play_state->time;
    game_play_state->rocket->time_rocket_spawn = INFINITY;
    
    remove_warnings(game_play_state);
    fprintf(stderr, "added active rocket!\n");
    game_play_state->rocket->rocket_spawn_position.x += 50;
    body_t *rocket = make_rocket(&game_play_state->rocket->rocket_spawn_position);
    scene_add_body(game_play_state->state->scene, rocket);
 
    // asset_update_bounding_box(game_play_state->rocket->rocket_active_asset, rocket);
    // asset_t *img = game_play_state->rocket->rocket_active_asset;
    asset_t *img = asset_make_image_with_body(ROCKET_PATH, rocket);

    list_add(game_play_state->state->body_assets, img);
    body_t *user = scene_get_body(game_play_state->state->scene, 0);
    assert(user);
    create_collision(game_play_state->state->scene, rocket, user, game_over, 
    game_play_state, NULL, 0);
    game_play_state->rocket->rocket_inactive = false;                        
    game_play_state->rocket->rocket_active = true;                        
  }
}






void add_laser(game_play_state_t *game_play_state, double dt) {
  game_play_state->laser->time_laser += dt;

  if (!game_play_state->laser->laser_inactive && game_play_state->laser->time_laser >= LASER_GENERATION_TIME) {
    game_play_state->laser->time_laser = 0;

    for (size_t i = 0; i < list_size(game_play_state->laser->laser_spawn_positions); i++) { 
      list_remove(game_play_state->laser->laser_spawn_positions, i);
    }

    game_play_state->laser->time_laser_spawn = game_play_state->time;

    size_t num_laser_initialize = (size_t) round(fmod(rand(), 5));
    size_t start_laser_position = (size_t) round(fmod(rand(), 10.0 - num_laser_initialize)); 

    for (size_t i = start_laser_position; i < start_laser_position + num_laser_initialize; i++) {
      fprintf(stderr, "added inactive laser!\n");
      vector_t *center = list_get(game_play_state->laser->laser_centers, i);
      list_add(game_play_state->laser->laser_spawn_positions, center);
      body_t *laser = make_laser(center);
      scene_add_body(game_play_state->state->scene, laser);
      asset_update_bounding_box(list_get(game_play_state->laser->laser_inactive_assets, i), laser);
    }

    game_play_state->laser->laser_inactive = true;
  }

  if (game_play_state->laser->laser_inactive && game_play_state->time - game_play_state->laser->time_laser_spawn >= 3) {
    game_play_state->laser->time_laser_activate = game_play_state->time;
    game_play_state->laser->time_laser_spawn = game_play_state->time;
    
    remove_lasers_inactive(game_play_state);

    for (size_t i = 0; i < list_size(game_play_state->laser->laser_spawn_positions); i++) {
      fprintf(stderr, "added active laser!\n");
      body_t *laser = make_laser_active(list_get(game_play_state->laser->laser_spawn_positions, i));
      scene_add_body(game_play_state->state->scene, laser);
      asset_update_bounding_box(list_get(game_play_state->laser->laser_active_assets, i), laser);
      body_t *user = scene_get_body(game_play_state->state->scene, 0);
      assert(user);    
      create_collision(game_play_state->state->scene, laser, user, game_over, 
      game_play_state, NULL, 0);
    }
    for (size_t i = 0; i < list_size(game_play_state->laser->laser_spawn_positions); i++) { 
      list_remove(game_play_state->laser->laser_spawn_positions, i);
    }

    game_play_state->laser->laser_inactive = false;                        
    game_play_state->laser->laser_active = true;                        
  }
}

void render_distance(game_play_state_t *game_play_state) {
  char distance_text[DISTANCE_TEXT_SIZE];
  size_t distance = (size_t)floor(game_play_state->distance_traveled);
  sprintf(distance_text, "%zu M", distance); // convert the time to a string
  SDL_Rect bounding_box = DISTANCE_BOX;
  // TTF_Font *font = init_font(FONT_PATH, DISTANCE_FONT_SIZE);
  TTF_Font *font = game_play_state->distance_font;
  TTF_SizeText(font, distance_text, &bounding_box.w, &bounding_box.h);
  render_text(distance_text, font, green, bounding_box);
  if (game_play_state->time - game_play_state->last_update_time > UPDATE_INTERVAL) {
    game_play_state->last_update_time = game_play_state->time;
    subject_notify(game_play_state->subject, EVENT_DISTANCE_TRAVELED, distance_text);
  }
}

void render_coins_collected(game_play_state_t *game_play_state) {
  char coin_text[COIN_TEXT_SIZE];
  size_t coins = game_play_state->coin->coin_count;
  sprintf(coin_text, "%zu COINS", coins); // convert the time to a string
  SDL_Rect bounding_box = COIN_BOX;
  // TTF_Font *font = init_font(FONT_PATH, COIN_FONT_SIZE);
  TTF_Font *font = game_play_state->coins_collected_font;
  TTF_SizeText(font, coin_text, &bounding_box.w, &bounding_box.h);
  render_text(coin_text, font, yellow, bounding_box);
}


state_type_t game_play_main(game_play_state_t *game_play_state) {

  double dt = time_since_last_tick();
  game_play_state->time += dt;
  
  state_temp_t *state = game_play_state->state;

  double scroll_speed = state->background_state->scroll_speed;
  game_play_state->distance_traveled += (dt * scroll_speed);

  add_zapper(game_play_state, dt);
  add_laser(game_play_state, dt);
  add_rocket(game_play_state, dt);
  add_coins(game_play_state, dt);
  add_powerup(game_play_state, dt);
  sdl_clear();

  background_update(state->background_state, dt);
  // sdl_render_scene(state->scene, NULL);
  asset_render(state->background_state->bg1);
  asset_render(state->background_state->bg2);

  size_t num_assets = list_size(state->body_assets);
  for (size_t i = 0; i < num_assets; i++) {
    asset_render(list_get(state->body_assets, i));
  }

  render_distance(game_play_state);
  render_coins_collected(game_play_state);

  vector_t user_centroid = body_get_centroid(game_play_state->state->user);
  vector_t user_vel = body_get_velocity(game_play_state->state->user);
  vector_t ground_centroid = body_get_centroid(game_play_state->state->ground);
  vector_t ceiling_centroid = body_get_centroid(game_play_state->state->ceiling);
  if (user_vel.y < 0 && user_centroid.y - ground_centroid.y < USER_OUTER_RADIUS) {
    vector_t new_centroid = {.x = ground_centroid.x, .y = ground_centroid.y + USER_OUTER_RADIUS};
    body_set_centroid(game_play_state->state->user, new_centroid);
  } else if (user_vel.y > 0 && ceiling_centroid.y - user_centroid.y < USER_OUTER_RADIUS) {
    vector_t new_centroid = {.x = ceiling_centroid.x, .y = ceiling_centroid.y - USER_OUTER_RADIUS};
    body_set_centroid(game_play_state->state->user, new_centroid);
  }

  remove_zappers(game_play_state);
  remove_lasers(game_play_state);
  remove_rockets(game_play_state);
  remove_coins(game_play_state);
  remove_powerups(game_play_state);

  sdl_show();

  scene_tick(state->scene, dt);
  // fprintf(stderr, "y of the user %f\n", body_get_centroid(game_play_state->state->user).y);
  return game_play_state->curr_state;
}

void game_play_free(game_play_state_t *game_play_state) {
  state_temp_t *state = game_play_state->state;
  // TODO: if time, change to jetpack sprite + have bullets show up
  //asset_destroy(state->background_state->bg1);
  //asset_destroy(state->background_state->bg2);
  body_free(state->user);
  body_free(state->ceiling);
  body_free(state->ground);
  //asset_destroy(game_play_state->laser_active_asset);
  //asset_destroy(game_play_state->laser_inactive_asset);

  // TODO: add int main and link and compile to find memory leaks
  list_free(state->body_assets);
  list_free(game_play_state->laser->laser_centers);
  free(game_play_state->laser);
  // list_free(game_play_state->laser_spawn_positions);
  free(state->background_state);  
  subject_free(game_play_state->subject);
  // TODO: why is this failing
  // scene_free(state->scene);
  asset_cache_destroy();
  free(state);
  free(game_play_state);
}
