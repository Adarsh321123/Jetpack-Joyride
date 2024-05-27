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

const vector_t USER_CENTER = {500, 30};
const vector_t INVADER_BULLET_VEL = {0, -200};
const vector_t USER_BULLET_VEL = {0, 200};
const vector_t SHIP_VELOCITY = {500, 0};
const vector_t INVADER_VELOCITY = {100, 0};

const double INVADER_MASS = 5;
const double SHIP_MASS = 5;
const double BULLET_MASS = 10;

const size_t SPAWN_TIME = 200; // number of frames in between new shapes
const double TIME_BETWEEN_INVADER_BULLETS = 2.0;
const double TIME_BETWEEN_USER_BULLETS = 0.4;

const double resting_speed = 300;
const double ACCEL = 100;

const double OUTER_RADIUS = 25;
const double INNER_RADIUS = 60;

const size_t SHIP_NUM_POINTS = 20;

const rgb_color_t invader_color = (rgb_color_t){0.2, 0.2, 0.3};
const rgb_color_t user_color = (rgb_color_t){0.1, 0.9, 0.2};

// constants to create invaders
const size_t NUM_ROWS = 3;
const size_t Y_START = 390;
const size_t Y_SPACE = 10;
const size_t INVADERS_PER_ROW = 8;
const size_t X_START = 100;
const size_t X_SPACE = 15;

const size_t OFFSET = 3;
const size_t CIRC_NPOINTS = 4;

typedef struct state_temp {
  scene_t *scene;
  body_t *ship;
  size_t invader_count;
  double time_since_invader_bullet;
  double time_since_user_bullet;
} state_temp_t;

typedef struct game_play_state {
  double time;
  state_t *state;
} game_play_state_t;






/** Make a circle-shaped body object.
 *
 * @param center a vector representing the center of the body.
 * @param radius the radius of the circle
 * @param mass the mass of the body
 * @param color the color of the circle
 * @return pointer to the circle-shaped body
 */
body_t *make_bullet(vector_t center, double radius, double mass,
                    rgb_color_t color, void *info) {
  list_t *c = list_init(CIRC_NPOINTS, free);
  for (size_t i = 0; i < CIRC_NPOINTS; i++) {
    double angle = 2 * M_PI * i / CIRC_NPOINTS;
    vector_t *v = malloc(sizeof(*v));
    assert(v != NULL);
    *v = (vector_t){center.x + radius * cos(angle),
                    center.y + radius * sin(angle)};
    list_add(c, v);
  }
  return body_init_with_info(c, mass, color, info, NULL);
}

/** Return a list of points representing the invader shape.
 *
 * @param center a vector representing the center of the invader
 * @return list of vectors representing points of invader object.
 */
list_t *make_invader(vector_t center, double wedge) {
  list_t *points = list_init(10, free);
  vector_t *vec = malloc(sizeof(vector_t));
  assert(vec);

  double x;
  double y;
  for (double i = (wedge / 2); i <= (2 * M_PI) - (wedge / 2);
       i += INCREMENT_ANGLE) {
    vector_t *new_vec = malloc(sizeof(vector_t));
    assert(new_vec);

    x = RADIUS * cos(i) + center.x;
    y = RADIUS * sin(i) + center.y;

    *new_vec = (vector_t){x, y};
    list_add(points, new_vec);
  }

  vector_t *new_vec = malloc(sizeof(vector_t));
  assert(new_vec);
  *new_vec = center;
  list_add(points, new_vec);

  return points;
}

list_t *make_ship(double outer_radius, double inner_radius) {
  vector_t center = VEC_ZERO;
  center.y += inner_radius;
  list_t *c = list_init(SHIP_NUM_POINTS, free);
  for (size_t i = 0; i < SHIP_NUM_POINTS; i++) {
    double angle = 2 * M_PI * i / SHIP_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    assert(v != NULL);
    *v = (vector_t){center.x + inner_radius * cos(angle),
                    center.y + outer_radius * sin(angle)};
    list_add(c, v);
  }
  return c;
}

/**
 * Wrap object around other side of screen display if it reaches any edge of the
 * display.
 *
 * @param body the body object representing an invader
 * @param amount amount to offset in the y-direction
 */
void wrap_edges(body_t *body, double offset) {
  vector_t velocity = body_get_velocity(body);
  vector_t centroid = body_get_centroid(body);
  if (centroid.x + RADIUS > MAX.x && velocity.x > 0) {
    body_set_centroid(body, (vector_t){centroid.x, centroid.y - offset});
    body_set_velocity(body, vec_negate(velocity));
  } else if (centroid.x - RADIUS < MIN.x && velocity.x < 0) {
    body_set_centroid(body, (vector_t){centroid.x, centroid.y - offset});
    body_set_velocity(body, vec_negate(velocity));
  }
}

/**
 * Wrap object around other side of screen display if it reaches any edge of the
 * display.
 *
 * @param body the body object representing the user
 */
void user_wrap_edges(body_t *body) {
  vector_t centroid = body_get_centroid(body);
  if (centroid.x - RADIUS > MAX.x) {
    body_set_centroid(body, (vector_t){MIN.x, centroid.y});
  } else if (centroid.x + RADIUS < MIN.x) {
    body_set_centroid(body, (vector_t){MAX.x, centroid.y});
  }
}

// Calculate new locations after wrap-around for invaders
void wrap_invaders(state_t *state) {
  for (size_t i = 0; i < state->invader_count; i++) {
    body_t *invader = scene_get_body(state->scene, i);
    wrap_edges(invader, (RADIUS + 10) * OFFSET);
  }
}

/**
 * Check conditions to see if game is over. Game is over if there are
 * no more invaders (win), the invaders reach the bottom of the screen (loss)
 * or the user is hit by an invader bullet (loss).
 *
 * @param state a pointer to a state object representing the current demo state
 */
bool game_over(state_t *state) {
  if (state->invader_count == 0) {
    return true;
  }
  for (size_t i = 0; i < state->invader_count; i++) {
    body_t *invader = scene_get_body(state->scene, i);
    assert(strcmp(body_get_info(invader), "invader") == 0);
    vector_t center = body_get_centroid(invader);
    if (center.y < MIN.y) {
      return true;
    }
  }
  bool body_removed = body_is_removed(state->ship);
  if (body_removed) {
    return true;
  }
  return false;
}

/** Shoots a bullet from user ship at invaders.
 *
 * @param state a pointer to a state object representing the current demo state
 */
void user_shoot_bullet(state_t *state) {
  body_t *bullet = make_bullet(body_get_centroid(state->ship), BULLET_RADIUS,
                               BULLET_MASS, user_color, "user_bullet");
  body_set_velocity(bullet, USER_BULLET_VEL);
  scene_add_body(state->scene, bullet);
  for (size_t i = 0; i < state->invader_count; i++) {
    create_destructive_collision(state->scene, bullet,
                                 scene_get_body(state->scene, i));
  }
}

/**
 * Move ship on display screen or shoots bullet based based on key pressed.
 *
 * @param key the character of the key pressed
 * @param type event type connected to key
 * @param held_time double value representing the amount of time the key is held
 * down
 * @param state the current state of game
 */
void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  if (type == KEY_PRESSED) {
    switch (key) {
    case LEFT_ARROW: {
      body_set_velocity(state->ship, vec_negate(SHIP_VELOCITY));
      break;
    }
    case RIGHT_ARROW: {
      body_set_velocity(state->ship, SHIP_VELOCITY);
      break;
    }
    case SPACE_BAR: {
      if (state->time_since_user_bullet > TIME_BETWEEN_USER_BULLETS) {
        user_shoot_bullet(state);
        state->time_since_user_bullet -= TIME_BETWEEN_USER_BULLETS;
      }
      break;
    }
    default: {
      return;
    }
    }
  } else {
    body_set_velocity(state->ship, VEC_ZERO);
  }
}

// initialize the invaders at start of game
void invader_init(state_t *state) {
  for (int i = 0; i < NUM_ROWS; i++) {
    double y = Y_START + i * (RADIUS + Y_SPACE);
    for (int j = 0; j < INVADERS_PER_ROW; j++) {
      double x = X_START + j * (RADIUS * 2 + X_SPACE);
      list_t *invader_points = make_invader((vector_t){x, y}, WEDGE_ANGLE);
      body_t *invader = body_init_with_info(invader_points, INVADER_MASS,
                                            invader_color, "invader", NULL);
      body_set_velocity(invader, INVADER_VELOCITY);
      body_set_rotation(invader, 3 * M_PI / 2);

      scene_add_body(state->scene, invader);
      state->invader_count++;
    }
  }
}

/** Chooses a random invader out of the remaining invaders
 * in the demo to shoot a bullet at the user.
 *
 * @param state a pointer to a state object representing the current demo state
 */
void invader_shoot_bullet(state_t *state) {
  if (state->invader_count == 0) {
    return;
  }
  size_t index = rand() % (state->invader_count);
  body_t *invader = scene_get_body(state->scene, index);
  vector_t bullet_position = body_get_centroid(invader);
  body_t *bullet = make_bullet(bullet_position, BULLET_RADIUS, BULLET_MASS,
                               invader_color, "invader_bullet");
  body_set_velocity(bullet, INVADER_BULLET_VEL);
  scene_add_body(state->scene, bullet);
  create_destructive_collision(state->scene, bullet, state->ship);
}

/**
 * Initialize the window and invaders
 * Add invaders to the scene.
 */
void init_invaders(state_t *state) {
  sdl_init(MIN, MAX);
  sdl_on_key((key_handler_t)on_key);
  state->scene = scene_init();
  state->invader_count = 0;
  state->time_since_invader_bullet = 0.0;
  state->time_since_user_bullet = 0.0;
  invader_init(state);
}

/**
 * Initialize the ship
 */
void init_ship(state_t *state) {
  list_t *ship_points = make_ship(OUTER_RADIUS, INNER_RADIUS);
  state->ship = body_init(ship_points, SHIP_MASS, user_color);
  body_set_centroid(state->ship, USER_CENTER);
}

/**
 * Updates the invader count after scene/body ticks
 */
void update_invader_count(state_t *state) {
  state->invader_count = 0;
  size_t num_bodies = scene_bodies(state->scene);
  for (size_t i = 0; i < num_bodies; i++) {
    body_t *body = scene_get_body(state->scene, i);
    if (strcmp(body_get_info(body), "invader") == 0) {
      state->invader_count++;
    }
  }
}












game_play_state_t *emscripten_init() {
  game_play_state_t *game_play_state = malloc(sizeof(game_play_state_t));
  assert(game_play_state);

  state_t *state = malloc(sizeof(state_temp_t));
  assert(state != NULL);
  init_invaders(state);
  init_ship(state);
  srand(time(NULL));

  //sdl_init(MIN, MAX);
  //TTF_Init();
  //asset_cache_init();
  game_play_state->state = state;
  game_play_state->time = 0;
  return game_play_state;
}

bool emscripten_main(game_play_state_t *game_play_state) {
  sdl_clear();

  double dt = time_since_last_tick();
  state_t *state = game_play_state->state;
  state->time_since_invader_bullet += dt;
  state->time_since_user_bullet += dt;
  if (state->time_since_invader_bullet > TIME_BETWEEN_INVADER_BULLETS) {
    invader_shoot_bullet(state);
    state->time_since_invader_bullet -= TIME_BETWEEN_INVADER_BULLETS;
  }
  scene_tick(state->scene, dt);
  body_tick(state->ship, dt);
  user_wrap_edges(state->ship);
  wrap_invaders(state);
  sdl_render_scene(state->scene, state->ship);
  sdl_is_done(state);
  update_invader_count(state);
  bool is_over = game_over(state);
  game_play_state->time += dt;
  sdl_show();
  return is_over;
}

void emscripten_free(game_play_state_t *game_play_state) {
  state_t *state = game_play_state->state;
  scene_free(state->scene);
  body_free(state->ship);
  free(state);
  //TTF_Quit();
  //asset_cache_destroy();
  free(game_play_state);
}
