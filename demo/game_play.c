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

struct zapper_state
{
  double time_until_zapper;
  double min_zapper_generation_time;
  double max_zapper_generation_time;
  double zapper_time;
};

struct laser_state
{
  double time_laser;
  double time_laser_spawn;
  double time_laser_activate;
  double laser_generation_time;
  bool laser_inactive;
  bool laser_active;
  list_t *laser_inactive_assets;
  list_t *laser_active_assets;
  list_t *laser_centers;
  list_t *laser_spawn_positions;
  laser_spawn_t laser_spawn_bool;
};

struct coin_state
{
  double time_until_coin;
  double coin_time;
  size_t coin_count;
  vector_t prev_center;
};

struct powerup_state
{
  double time_until_powerup;
  double powerup_time;
  double powerup_start_time;
  bool powerup_active;
  image_asset_t *powerup_img;
  powerup_type_t powerup_type;
};

struct rocket_state
{
  double time_rocket;
  double time_rocket_spawn;
  double time_rocket_activate;
  double rocket_generation_time;
  bool rocket_inactive;
  bool rocket_active;
  asset_t *rocket_inactive_asset;
  vector_t rocket_spawn_position;
};

struct game_play_state
{
  double time;
  double last_update_time;
  double distance_traveled;
  TTF_Font *distance_font;
  TTF_Font *coins_collected_font;
  zapper_state_t *zapper;
  laser_state_t *laser;
  coin_state_t *coin;
  powerup_state_t *powerup;
  rocket_state_t *rocket;
  state_type_t curr_state;
  subject_t *subject; // this is useful for achievements
  state_temp_t *state;
};

/**
 * Move user up if space bar is pressed and back down otherwise
 *
 * @param key the character of the key pressed
 * @param type event type connected to key
 * @param held_time double value representing the amount of time the key is held
 * down
 * @param state the current state of game
 */
void on_key(char key, key_event_type_t type, double held_time, 
            game_play_state_t *game_play_state)
{
  if (game_play_state->powerup->powerup_active && 
      game_play_state->powerup->powerup_type == GRAVITY_SWAP)
  {
    if (key == SPACE_BAR)
    {
      if (type == KEY_RELEASED)
      {
        vector_t user_vel = body_get_velocity(game_play_state->state->user);
        body_set_velocity(game_play_state->state->user, vec_negate(user_vel));
      }
    }
  }
  else
  {
    if (key == SPACE_BAR)
    {
      if (type == KEY_PRESSED)
      {
        body_set_velocity(game_play_state->state->user, USER_VEL);
      }
      else if (type == KEY_RELEASED)
      {
        body_set_velocity(game_play_state->state->user, vec_negate(USER_VEL));
      }
    }
  }
}

static void init_observers(game_play_state_t *game_play_state)
{
  game_play_state->subject = subject_init();
  achievements_t *achievements = achievements_init();
  observer_t *observer = achievements_get_observer(achievements);
  subject_add_observer(game_play_state->subject, observer);
}

static void create_objects(game_play_state_t *game_play_state) {
  game_play_state->zapper = malloc(sizeof(zapper_state_t));
  assert(game_play_state->zapper);
  game_play_state->laser = malloc(sizeof(laser_state_t));
  assert(game_play_state->laser);
  game_play_state->rocket = malloc(sizeof(rocket_state_t));
  assert(game_play_state->rocket);
  game_play_state->coin = malloc(sizeof(coin_state_t));
  assert(game_play_state->coin);
  game_play_state->powerup = malloc(sizeof(powerup_state_t));
  assert(game_play_state->powerup); 
}

static void set_rockets_values(game_play_state_t *game_play_state) {
  game_play_state->rocket->rocket_inactive = false;
  game_play_state->rocket->rocket_active = false;
  game_play_state->rocket->time_rocket = ZERO;
  game_play_state->rocket->time_rocket_spawn = ZERO;
  game_play_state->rocket->time_rocket_activate = ZERO;
  SDL_Rect bounding_box_rocket_warning = make_texr(ROCKET_INITIAL_X, 
                          ROCKET_INITIAL_Y, WARNING_WIDTH, WARNING_HEIGHT);
  game_play_state->rocket->rocket_inactive_asset = asset_make_image(
                                                  ROCKET_WARNING_PATH, 
                                                  bounding_box_rocket_warning);
  list_add(game_play_state->state->body_assets,
             game_play_state->rocket->rocket_inactive_asset);
}

static void set_necessary_values(game_play_state_t *game_play_state,
                                state_temp_t *state) {
  game_play_state->laser->laser_inactive = false;
  game_play_state->laser->laser_active = false;
  game_play_state->state = state;
  game_play_state->time = ZERO;
  game_play_state->last_update_time = ZERO;
  game_play_state->distance_font = init_font(FONT_PATH, DISTANCE_FONT_SIZE);
  game_play_state->coins_collected_font = init_font(FONT_PATH, COIN_FONT_SIZE);
  game_play_state->distance_traveled = ZERO;
  game_play_state->zapper->time_until_zapper = ZERO;
  game_play_state->coin->time_until_coin = ZERO;
  game_play_state->zapper->zapper_time = ZERO;
  game_play_state->coin->coin_time = ZERO;
  game_play_state->laser->time_laser = ZERO;
  game_play_state->laser->time_laser_spawn = ZERO;
  game_play_state->laser->time_laser_activate = ZERO;
  game_play_state->coin->coin_count = ZERO;
  game_play_state->powerup->time_until_powerup = ZERO;
  game_play_state->powerup->powerup_time = ZERO;
  game_play_state->powerup->powerup_start_time = ZERO;
  game_play_state->powerup->powerup_active = false;
  game_play_state->laser->laser_spawn_bool = LASER_INACTIVE_STATE;
  game_play_state->powerup->powerup_type = SHIELD;
}

static void set_lasers_values(game_play_state_t *game_play_state) {
  SDL_Rect bounding_box_laser = make_texr(LASER_INITIAL_X, LASER_INITIAL_Y, 
                                      LASER_WIDTH_ACTIVE, LASER_HEIGHT_ACTIVE);
  game_play_state->laser->laser_active_assets = list_init(INITIAL_LIST_CAPACITY, 
                                                    NULL);
  game_play_state->laser->laser_inactive_assets = list_init(
                                                    INITIAL_LIST_CAPACITY,
                                                    NULL);
  for (size_t i = 0; i < INITIAL_LIST_CAPACITY; i++)
  {
    asset_t *laser_active = asset_make_image(LASER_PATH_ACTIVE,
                                              bounding_box_laser);
    list_add(game_play_state->laser->laser_active_assets,
            laser_active);
    asset_t *laser_inactive = asset_make_image(LASER_PATH_INACTIVE,
                                              bounding_box_laser);
    list_add(game_play_state->laser->laser_inactive_assets, laser_inactive);
    list_add(game_play_state->state->body_assets, laser_active);
    list_add(game_play_state->state->body_assets, laser_inactive);
  }
  game_play_state->laser->laser_centers = list_init(INITIAL_LIST_CAPACITY,
                                                    (free_func_t)vector_free);
  game_play_state->laser->laser_spawn_positions = list_init(
                                                      INITIAL_LIST_CAPACITY,
                                                      NULL);
  game_play_state->coin->prev_center = VEC_ZERO;
  for (size_t i = 0; i < INITIAL_LIST_CAPACITY; i++)
  {
    double y = (LASER_OFFSET_BOTTOM + (LASER_OFFSET_BETWEEN * i));
    vector_t *center = vector_init(LASER_X, y);
    list_add(game_play_state->laser->laser_centers, center);
  }
}

static void switch_generation_difficulty(game_play_state_t *game_play_state,
                                        difficulty_type_t difficulty_level) {
  switch (difficulty_level)
  {
    case EASY:
    {
      game_play_state->zapper->min_zapper_generation_time =
          MIN_ZAPPER_GENERATION_TIME_EASY;
      game_play_state->zapper->max_zapper_generation_time =
          MAX_ZAPPER_GENERATION_TIME_EASY;
      game_play_state->rocket->rocket_generation_time = ROCKET_GENERATION_TIME_EASY;
      game_play_state->laser->laser_generation_time = LASER_GENERATION_TIME_EASY;
      break;
    }
    case MEDIUM:
    {
      game_play_state->zapper->min_zapper_generation_time =
          MIN_ZAPPER_GENERATION_TIME_MEDIUM;
      game_play_state->zapper->max_zapper_generation_time =
          MAX_ZAPPER_GENERATION_TIME_MEDIUM;
      game_play_state->rocket->rocket_generation_time = ROCKET_GENERATION_TIME_MEDIUM;
      game_play_state->laser->laser_generation_time = LASER_GENERATION_TIME_MEDIUM;
      break;
    }
    case HARD:
    {
      game_play_state->zapper->min_zapper_generation_time =
          MIN_ZAPPER_GENERATION_TIME_HARD;
      game_play_state->zapper->max_zapper_generation_time =
          MAX_ZAPPER_GENERATION_TIME_HARD;
      game_play_state->rocket->rocket_generation_time = ROCKET_GENERATION_TIME_HARD;
      game_play_state->laser->laser_generation_time = LASER_GENERATION_TIME_HARD;
      break;
    }
    default:
    {
      break;
    }
  }

}

game_play_state_t *game_play_init(difficulty_type_t difficulty_level)
{
  game_play_state_t *game_play_state = malloc(sizeof(game_play_state_t));
  assert(game_play_state != NULL);
  asset_cache_init();
  sdl_init(MIN, MAX);
  srand(time(NULL));
  sdl_on_key((key_handler_t)on_key);
  state_temp_t *state = state_temp_init();
  game_play_state->state = state;
  game_play_state->curr_state = GAME_PLAY;
  create_objects(game_play_state);
  set_rockets_values(game_play_state);
  set_necessary_values(game_play_state, state);
  set_lasers_values(game_play_state);
  switch_generation_difficulty(game_play_state, difficulty_level);
  init_observers(game_play_state);
  return game_play_state;
}

void game_over(body_t *body1, body_t *body2, vector_t axis, void *aux1,
               void *aux2, double force_const)
{
  game_play_state_t *game_play_state = (game_play_state_t *)aux1;
  game_play_state->curr_state = GAME_OVER;
}

void collect_coin(body_t *body1, body_t *body2, vector_t axis, void *aux1,
                  void *aux2, double force_const)
{
  asset_t *asset = (asset_t *)aux1;
  body_remove(body1);
  asset_update_bounding_box_x(asset, COIN_OFFSET);

  game_play_state_t *game_play_state = (game_play_state_t *)aux2;
  game_play_state->coin->coin_count++;
  subject_notify(game_play_state->subject, EVENT_COIN_COLLECTED, NULL);
}

void make_individual_powerup(game_play_state_t *game_play_state,
                            const char *path)
{
  image_asset_t *img = asset_image_make_image_with_body(path,
                                                  game_play_state->state->user);
  game_play_state->powerup->powerup_img = img;
  list_add(game_play_state->state->body_assets, &img->base);
}

void activate_powerup(body_t *body1, body_t *body2, vector_t axis, void *aux1,
                      void *aux2, double force_const)
{
  asset_t *asset = (asset_t *)aux1;
  body_remove(body1);
  asset_update_bounding_box_x(asset, POWERUP_OFFSET);
  size_t powerup_type = (size_t)rand() % NUM_POWERUPS;
  game_play_state_t *game_play_state = (game_play_state_t *)aux2;
  if (!game_play_state->powerup->powerup_active)
  {
    game_play_state->powerup->powerup_active = true;
    game_play_state->powerup->powerup_start_time = game_play_state->time;
    game_play_state->powerup->powerup_type = powerup_type;
  }
  switch (game_play_state->powerup->powerup_type)
  {
    case SHIELD:
    {
      make_individual_powerup(game_play_state, SHIELD_PATH);
      break;
    }
    case MAGNETIC:
    {
      make_individual_powerup(game_play_state, MAGNETIC_PATH);
      break;
    }
    case MORE_COIN:
    {
      make_individual_powerup(game_play_state, MORE_COIN_PATH);
      if (game_play_state->powerup->powerup_active &&
          game_play_state->powerup->powerup_type == MORE_COIN)
      {
        game_play_state->coin->time_until_coin = COIN_GENERATION_TIME_POWERUP;
      }
      break;
    }
    case GRAVITY_SWAP:
    {
      make_individual_powerup(game_play_state, GRAVITY_SWAP_PATH);
    }
    default:
    {
      break;
    }
  }
}

void remove_zappers(game_play_state_t *game_play_state)
{
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < num_bodies; i++)
  {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == ZAPPER && body_get_centroid(body).x +
                                                  ZAPPER_REMOVE_OFFSET < MIN.x)
    {
      scene_remove_body(game_play_state->state->scene, i);
    }
  }
}

void remove_coins(game_play_state_t *game_play_state)
{
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < num_bodies; i++)
  {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == COIN && body_get_centroid(body).x + COIN_REMOVE_OFFSET < MIN.x)
    {
      scene_remove_body(game_play_state->state->scene, i);
    }
  }
}

void remove_powerups(game_play_state_t *game_play_state)
{
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < num_bodies; i++)
  {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == POWERUP && body_get_centroid(body).x + POWERUP_REMOVE_OFFSET < MIN.x)
    {
      scene_remove_body(game_play_state->state->scene, i);
    }
  }
}

void remove_individual_powerup(game_play_state_t *game_play_state)
{
  game_play_state->powerup->powerup_img->body = NULL;
  asset_update_bounding_box_x(&game_play_state->powerup->powerup_img->base, POWERUP_INITIAL_X);
}

void remove_lasers_inactive(game_play_state_t *game_play_state)
{
  // size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < scene_bodies(game_play_state->state->scene); i++)
  {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == LASER)
    {
      // asset_update_bounding_box_x(game_play_state->laser_inactive_asset, 2 * 1000);
      scene_remove_body(game_play_state->state->scene, i);
    }
  }
  for (size_t i = 0; i < INITIAL_LIST_CAPACITY; i++)
  {
    asset_update_bounding_box_x(list_get(game_play_state->laser->laser_inactive_assets, i), LASER_INITIAL_X);
  }
}

void remove_lasers(game_play_state_t *game_play_state)
{
  if (game_play_state->time - game_play_state->laser->time_laser_activate >= LASER_ON_TIME)
  {
    game_play_state->laser->laser_active = false;
    game_play_state->laser->laser_spawn_bool = LASER_INACTIVE_STATE;
    game_play_state->laser->time_laser_activate = INFINITY;
    // size_t num_bodies = scene_bodies(game_play_state->state->scene);
    for (size_t i = 0; i < scene_bodies(game_play_state->state->scene); i++)
    {
      body_t *body = scene_get_body(game_play_state->state->scene, i);
      if (get_type(body) == LASER_ACTIVE)
      {
        // asset_update_bounding_box_x(game_play_state->laser_active_asset, 2 * 1000);
        scene_remove_body(game_play_state->state->scene, i);
        subject_notify(game_play_state->subject, EVENT_LASERS_AVOIDED, NULL);
      }
    }

    for (size_t i = 0; i < INITIAL_LIST_CAPACITY; i++)
    {
      asset_update_bounding_box_x(list_get(game_play_state->laser->laser_active_assets, i), LASER_INITIAL_X);
    }
  }
}

void remove_warnings(game_play_state_t *game_play_state)
{
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < num_bodies; i++)
  {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == ROCKET_WARNING)
    {
      asset_update_bounding_box_x(game_play_state->rocket->rocket_inactive_asset, ROCKET_INITIAL_X);
      scene_remove_body(game_play_state->state->scene, i);
    }
  }
}

void remove_rockets(game_play_state_t *game_play_state)
{
  game_play_state->rocket->time_rocket_activate = game_play_state->time;
  size_t num_bodies = scene_bodies(game_play_state->state->scene);
  for (size_t i = 0; i < num_bodies; i++)
  {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == ROCKET && body_get_centroid(body).x + ROCKETS_REMOVE_OFFSET < MIN.x)
    {
      // asset_update_bounding_box_x(game_play_state->rocket->rocket_active_asset, 2 * 1000);
      scene_remove_body(game_play_state->state->scene, i);
    }
  }
}

void add_zapper(game_play_state_t *game_play_state, double dt)
{
  game_play_state->zapper->zapper_time += dt;
  bool laser_state = !game_play_state->laser->laser_active;
  if (laser_state && game_play_state->zapper->zapper_time >= game_play_state->zapper->time_until_zapper)
  {
    game_play_state->zapper->zapper_time = ZERO;
    game_play_state->zapper->time_until_zapper = fmod(rand(),
                                                      game_play_state->zapper->max_zapper_generation_time -
                                                          game_play_state->zapper->min_zapper_generation_time) +
                                                 game_play_state->zapper->min_zapper_generation_time;
    double y_pos = fmod(rand(), (MAX.y - ZAPPER_CEILING_OFFSET) - (MIN.y + ZAPPER_GROUND_OFFSET)) + ZAPPER_Y_POS_BUFFER;
    double x_pos = MAX.x + ZAPPER_X_POS_BUFFER;
    vector_t center = {.x = x_pos, .y = y_pos};
    // body_t *zapper = make_zapper(center, ZAPPER_WIDTH, ZAPPER_HEIGHT);
    body_t *zapper = make_obstacle_rectangle(&center, ZAPPER_WIDTH, ZAPPER_HEIGHT, ZAPPER, ZAPPER_VEL);
    scene_add_body(game_play_state->state->scene, zapper);
    asset_t *img = asset_make_image_with_body(ZAPPER_PATH, zapper);
    list_add(game_play_state->state->body_assets, img);
    if (!game_play_state->powerup->powerup_active ||
        game_play_state->powerup->powerup_type != SHIELD)
    {
      create_collision(game_play_state->state->scene, zapper, game_play_state->state->user,
                       game_over, game_play_state, NULL, DEFAULT_FORCE_CONSTANT);
    }
  }
}

void magnetic_powerup(game_play_state_t *game_play_state)
{
  if (game_play_state->powerup->powerup_active &&
      game_play_state->powerup->powerup_type == MAGNETIC)
  {
    size_t num_bodies = scene_bodies(game_play_state->state->scene);
    for (size_t i = 0; i < num_bodies; i++)
    {
      body_t *coin = scene_get_body(game_play_state->state->scene, i);
      if (get_type(coin) == COIN)
      {
        double euc_dist = euclidean_distance(game_play_state->state->user, coin);
        if (euc_dist <= POWERUP_MIN_DIST)
        {
          create_newtonian_gravity(game_play_state->state->scene, G_CONSTANT,
                                   game_play_state->state->user, coin);
        }
      }
    }
  }
}

void add_coins(game_play_state_t *game_play_state, double dt)
{
  magnetic_powerup(game_play_state);
  game_play_state->coin->coin_time += dt;
  bool laser_state = !game_play_state->laser->laser_active;
  if (laser_state && game_play_state->coin->coin_time >= game_play_state->coin->time_until_coin)
  {
    game_play_state->coin->coin_time = ZERO;
    game_play_state->coin->time_until_coin = fmod(rand(),
                                                  MAX_COIN_GENERATION_TIME - MIN_COIN_GENERATION_TIME) +
                                             MIN_COIN_GENERATION_TIME;
    double y_pos = fmod(rand(), (MAX.y - COIN_CEILING_OFFSET) - (MIN.y + COIN_GROUND_OFFSET));
    double x_pos = MAX.x + COIN_X_POS_BUFFER;
    vector_t center = {.x = x_pos, .y = y_pos};
    if (game_play_state->powerup->powerup_active &&
        game_play_state->powerup->powerup_type == MORE_COIN)
    {
      game_play_state->coin->time_until_coin = COIN_GENERATION_TIME_POWERUP;
      double new_deviation = fmod(rand(), MAX_COIN_DEVIATION_POWERUP) - MIN_COIN_DEVIATION_POWERUP;
      y_pos = game_play_state->coin->prev_center.y + new_deviation;
    }
    game_play_state->coin->prev_center = center;
    size_t COIN_GRID_WIDTH = (rand() % (MAX_COIN_GRID_SIZE - MIN_COIN_GRID_SIZE + ONE)) + MIN_COIN_GRID_SIZE;
    size_t COIN_GRID_HEIGHT = (rand() % (MAX_COIN_GRID_SIZE - MIN_COIN_GRID_SIZE + ONE)) + MIN_COIN_GRID_SIZE;
    for (size_t i = 0; i < COIN_GRID_WIDTH; i++)
    {
      for (size_t j = 0; j < COIN_GRID_HEIGHT; j++)
      {
        vector_t center = {.x = x_pos + (i * COIN_SPACING_X_SHIFT), .y = y_pos + (j * COIN_SPACING_Y_SHIFT)};
        // body_t *coin = make_coin(COIN_RADIUS, center);
        body_t *coin = make_obstacle_circle(COIN_RADIUS, &center, COIN, COIN_MASS, COIN_VEL);
        scene_add_body(game_play_state->state->scene, coin);
        asset_t *img = asset_make_image_with_body(COIN_PATH, coin);
        list_add(game_play_state->state->body_assets, img);
        create_collision(game_play_state->state->scene, coin, game_play_state->state->user,
                         collect_coin, img, game_play_state, COIN_FORCE_CONSTANT);
      }
    }
  }
}

void add_powerup(game_play_state_t *game_play_state, double dt)
{
  game_play_state->powerup->powerup_time += dt;
  bool laser_state = !game_play_state->laser->laser_active;
  if (laser_state && game_play_state->powerup->powerup_time >= game_play_state->powerup->time_until_powerup)
  {
    game_play_state->powerup->powerup_time = ZERO;
    game_play_state->powerup->time_until_powerup = fmod(rand(),
                                                        MAX_POWERUP_GENERATION_TIME - MIN_POWERUP_GENERATION_TIME) +
                                                   MIN_POWERUP_GENERATION_TIME;
    double y_pos = fmod(rand(), (MAX.y - POWERUP_CEILING_OFFSET) - (MIN.y + POWERUP_GROUND_OFFSET)) + POWERUP_Y_POS_BUFFER;
    double x_pos = MAX.x + POWERUP_X_POS_BUFFER;
    vector_t center = {.x = x_pos, .y = y_pos};
    //  body_t *powerup = make_powerup(center, POWERUP_SIZE, POWERUP_SIZE);
    body_t *powerup = make_obstacle_rectangle(&center, POWERUP_SIZE, POWERUP_SIZE, POWERUP, POWERUP_VEL);
    scene_add_body(game_play_state->state->scene, powerup);
    asset_t *img = asset_make_image_with_body(POWERUP_PATH, powerup);
    list_add(game_play_state->state->body_assets, img);
    create_collision(game_play_state->state->scene, powerup,
                     game_play_state->state->user, activate_powerup, img,
                     game_play_state, POWERUP_FORCE_CONSTANT);
  }
  if (game_play_state->powerup->powerup_active && game_play_state->time >=
                                                      game_play_state->powerup->powerup_start_time + POWERUP_DURATION)
  {
    game_play_state->powerup->powerup_active = false;
    remove_individual_powerup(game_play_state);
  }
}

void add_rocket(game_play_state_t *game_play_state, double dt)
{
  game_play_state->rocket->time_rocket += dt;
  bool laser_state = !game_play_state->laser->laser_active;
  if (laser_state && !game_play_state->rocket->rocket_inactive &&
      game_play_state->rocket->time_rocket >= game_play_state->rocket->rocket_generation_time)
  {
    game_play_state->rocket->time_rocket = ZERO;
    game_play_state->rocket->time_rocket_spawn = game_play_state->time;
    game_play_state->rocket->rocket_active = false;
    double y_pos = fmod(rand(), (MAX.y - MIN.y) - ROCKETS_Y_OFFSET) + ROCKETS_Y_POS_BUFFER;
    double x_pos = MAX.x - ROCKETS_X_POS_BUFFER;
    vector_t center = {.x = x_pos, .y = y_pos};
    game_play_state->rocket->rocket_spawn_position = center;
    // body_t *warning = make_rocket_warning(&center);
    body_t *warning = make_obstacle_rectangle(&center, WARNING_WIDTH, WARNING_HEIGHT, ROCKET_WARNING, VEC_ZERO);
    scene_add_body(game_play_state->state->scene, warning);
    asset_update_bounding_box(game_play_state->rocket->rocket_inactive_asset, warning);
    game_play_state->rocket->rocket_inactive = true;
  }

  else if (game_play_state->rocket->rocket_inactive && game_play_state->time - game_play_state->rocket->time_rocket_spawn >= ROCKETS_ACTIVATION_TIME)
  {
    game_play_state->rocket->time_rocket_activate = game_play_state->time;
    game_play_state->rocket->time_rocket_spawn = INFINITY;

    remove_warnings(game_play_state);
    game_play_state->rocket->rocket_spawn_position.x += ROCKETS_X_POS_BUFFER;
    // body_t *rocket = make_rocket(&game_play_state->rocket->rocket_spawn_position);
    body_t *rocket = make_obstacle_rectangle(&game_play_state->rocket->rocket_spawn_position, ROCKET_WIDTH, ROCKET_HEIGHT, ROCKET, ROCKET_VEL);
    scene_add_body(game_play_state->state->scene, rocket);

    // asset_update_bounding_box(game_play_state->rocket->rocket_active_asset, rocket);
    // asset_t *img = game_play_state->rocket->rocket_active_asset;
    asset_t *img = asset_make_image_with_body(ROCKET_PATH, rocket);

    list_add(game_play_state->state->body_assets, img);
    if (!game_play_state->powerup->powerup_active ||
        game_play_state->powerup->powerup_type != SHIELD)
    {
      create_collision(game_play_state->state->scene, rocket, game_play_state->state->user, game_over,
                       game_play_state, NULL, DEFAULT_FORCE_CONSTANT);
    }
    game_play_state->rocket->rocket_inactive = false;
    game_play_state->rocket->rocket_active = true;
  }
}

void add_laser(game_play_state_t *game_play_state, double dt)
{
  game_play_state->laser->time_laser += dt;
  switch (game_play_state->laser->laser_spawn_bool)
  {
    case LASER_INACTIVE_STATE:
    {
      if (game_play_state->laser->time_laser >= game_play_state->laser->laser_generation_time)
      {
        game_play_state->laser->laser_active = true;
        game_play_state->laser->time_laser = ZERO;
        size_t size_of_spawn_list = list_size(game_play_state->laser->laser_spawn_positions);
        for (size_t i = 0; i < size_of_spawn_list; i++)
        {
          list_remove(game_play_state->laser->laser_spawn_positions, LIST_START_INDEX);
        }
        game_play_state->laser->time_laser_spawn = game_play_state->time;
        size_t num_laser_initialize = (size_t)round(fmod(rand(), MAX_NUM_LASERS)) + MIN_NUM_LASERS;
        size_t start_laser_position = (size_t)round(fmod(rand(), NUM_LASERS - num_laser_initialize));

        for (size_t i = start_laser_position; i < start_laser_position + num_laser_initialize; i++)
        {
          vector_t *center = list_get(game_play_state->laser->laser_centers, i);
          list_add(game_play_state->laser->laser_spawn_positions, center);
          // body_t *laser = make_laser(center);
          body_t *laser = make_obstacle_rectangle(center, LASER_WIDTH_ACTIVE, LASER_HEIGHT_ACTIVE, LASER, VEC_ZERO);
          scene_add_body(game_play_state->state->scene, laser);
          asset_update_bounding_box(list_get(game_play_state->laser->laser_inactive_assets, i), laser);
        }
        game_play_state->laser->laser_spawn_bool = LASER_ACTIVE_STATE;
      }
      break;
    }

    case LASER_ACTIVE_STATE:
    {
      if (game_play_state->time - game_play_state->laser->time_laser_spawn >= LASER_ACTIVATION_TIME)
      {
        game_play_state->laser->time_laser_activate = game_play_state->time;
        game_play_state->laser->time_laser_spawn = INFINITY;

        remove_lasers_inactive(game_play_state);
        size_t size_of_spawn_list = list_size(game_play_state->laser->laser_spawn_positions);

        for (size_t i = 0; i < size_of_spawn_list; i++)
        {
          // body_t *laser = make_laser_active(list_get(game_play_state->laser->laser_spawn_positions, i));
          body_t *laser = make_obstacle_rectangle(list_get(game_play_state->laser->laser_spawn_positions, i), LASER_WIDTH_ACTIVE, LASER_HEIGHT_ACTIVE, LASER_ACTIVE, VEC_ZERO);
          scene_add_body(game_play_state->state->scene, laser);
          asset_update_bounding_box(list_get(game_play_state->laser->laser_active_assets, i), laser);
          if (!game_play_state->powerup->powerup_active ||
              game_play_state->powerup->powerup_type != SHIELD)
          {
            create_collision(game_play_state->state->scene, laser, game_play_state->state->user, game_over,
                            game_play_state, NULL, DEFAULT_FORCE_CONSTANT);
          }
        }
        for (size_t i = 0; i < size_of_spawn_list; i++)
        {
          list_remove(game_play_state->laser->laser_spawn_positions, LIST_START_INDEX);
        }
      }
      break;
    }
    default:
    {
      break;
    }
  }
}

/**
 * Renders the distance traveled on the screen
 * Updates the achievements accordingly
 */
static void render_distance(game_play_state_t *game_play_state)
{
  char distance_text[DISTANCE_TEXT_SIZE];
  size_t distance = (size_t)floor(game_play_state->distance_traveled);
  sprintf(distance_text, "%zu %s", distance, DISTANCE_TEXT); // convert the time to a string
  SDL_Rect bounding_box = DISTANCE_BOX;
  TTF_Font *font = game_play_state->distance_font;
  TTF_SizeText(font, distance_text, &bounding_box.w, &bounding_box.h);
  render_text(distance_text, font, GREEN, bounding_box);
  if (game_play_state->time - game_play_state->last_update_time > UPDATE_INTERVAL)
  {
    game_play_state->last_update_time = game_play_state->time;
    subject_notify(game_play_state->subject, EVENT_DISTANCE_TRAVELED, distance_text);
  }
}

/**
 * Renders the coins collected on the screen
 * Updates the achievements accordingly
 */
static void render_coins_collected(game_play_state_t *game_play_state)
{
  char coin_text[COIN_TEXT_SIZE];
  size_t coins = game_play_state->coin->coin_count;
  sprintf(coin_text, "%zu %s", coins, COINS_TEXT); // convert the time to a string
  SDL_Rect bounding_box = COIN_BOX;
  TTF_Font *font = game_play_state->coins_collected_font;
  TTF_SizeText(font, coin_text, &bounding_box.w, &bounding_box.h);
  render_text(coin_text, font, YELLOW, bounding_box);
}

/**
 * Updates the user position to prevent going above the ceiling or below the ground
 */
static void update_user_pos(game_play_state_t *game_play_state)
{
  vector_t user_centroid = body_get_centroid(game_play_state->state->user);
  vector_t user_vel = body_get_velocity(game_play_state->state->user);
  vector_t ground_centroid = body_get_centroid(game_play_state->state->ground);
  vector_t ceiling_centroid = body_get_centroid(game_play_state->state->ceiling);
  if (user_vel.y < ZERO && user_centroid.y - ground_centroid.y < USER_RADIUS)
  {
    vector_t new_centroid = {.x = ground_centroid.x, .y = ground_centroid.y + USER_RADIUS};
    body_set_centroid(game_play_state->state->user, new_centroid);
  }
  else if (user_vel.y > ZERO && ceiling_centroid.y - user_centroid.y < USER_RADIUS)
  {
    vector_t new_centroid = {.x = ceiling_centroid.x, .y = ceiling_centroid.y - USER_RADIUS};
    body_set_centroid(game_play_state->state->user, new_centroid);
  }
}

state_type_t game_play_main(game_play_state_t *game_play_state)
{
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
  for (size_t i = 0; i < num_assets; i++)
  {
    asset_render(list_get(state->body_assets, i));
  }

  render_distance(game_play_state);
  render_coins_collected(game_play_state);
  update_user_pos(game_play_state);

  remove_zappers(game_play_state);
  remove_lasers(game_play_state);
  remove_rockets(game_play_state);
  remove_coins(game_play_state);
  remove_powerups(game_play_state);

  sdl_show();
  scene_tick(state->scene, dt);
  return game_play_state->curr_state;
}

/**
 * Frees everything in the general state
 */
static void state_free(game_play_state_t *game_play_state)
{
  state_temp_t *state = game_play_state->state;
  asset_destroy(state->background_state->bg1);
  asset_destroy(state->background_state->bg2);
  list_free(state->body_assets);
  free(state->background_state);
  scene_free(state->scene);
  free(state);
}

/**
 * Frees everything in the laser state
 */
static void laser_free(game_play_state_t *game_play_state)
{
  laser_state_t *state = game_play_state->laser;
  list_free(state->laser_centers);
  list_free(state->laser_spawn_positions);
  list_free(state->laser_active_assets);
  list_free(state->laser_inactive_assets);
  free(state);
}

void game_play_free(game_play_state_t *game_play_state)
{
  laser_free(game_play_state);
  free(game_play_state->rocket);
  free(game_play_state->zapper);
  free(game_play_state->coin);
  subject_free(game_play_state->subject);
  asset_cache_destroy();
  TTF_CloseFont(game_play_state->distance_font);
  TTF_CloseFont(game_play_state->coins_collected_font);
  state_free(game_play_state);
  free(game_play_state->powerup);
  free(game_play_state);
}
