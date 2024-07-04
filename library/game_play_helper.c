#include "game_play_helper.h"

/**
 * Make type of the body.
*/
static body_type_t *make_type_info(body_type_t type)
{
  body_type_t *info = malloc(sizeof(body_type_t));
  assert(info != NULL);
  *info = type;
  return info;
}

body_type_t get_type(body_t *body)
{
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
static list_t *make_rectangle(vector_t center, double width, double height)
{
  list_t *points = list_init(INITIAL_LIST_CAPACITY, free);
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
static void add_walls(state_temp_t *state)
{
  list_t *ceiling_shape =
      make_rectangle((vector_t){MAX.x / 2, MAX.y - CEILING_OFFSET}, MAX.x, WALL_DIM);
  body_t *ceiling = body_init_with_info(ceiling_shape, INFINITY, BLACK,
                                        make_type_info(CEILING), free);
  list_t *ground_shape =
      make_rectangle((vector_t){MAX.x / 2, MIN.y + GROUND_OFFSET}, MAX.x, WALL_DIM);
  body_t *ground = body_init_with_info(ground_shape, INFINITY, BLACK,
                                       make_type_info(GROUND), free);
  scene_add_body(state->scene, ceiling);
  state->ceiling = ceiling;
  scene_add_body(state->scene, ground);
  state->ground = ground;
}

body_t *make_obstacle_rectangle(vector_t *center, double width, double height,
                                body_type_t info, vector_t velocity)
{
  list_t *rectangle_shape = make_rectangle(*center, width, height);
  body_t *rectangle = body_init_with_info(rectangle_shape, INFINITY, BLACK,
                                          make_type_info(info), free);
  body_set_velocity(rectangle, velocity);
  body_set_centroid(rectangle, *center);
  return rectangle;
}

body_t *make_obstacle_circle(double radius, vector_t *center, body_type_t info,
                              double mass, vector_t velocity)
{
  center->y += radius;
  list_t *circle_shape = list_init(USER_NUM_POINTS, free);
  for (size_t i = 0; i < USER_NUM_POINTS; i++)
  {
    double angle = 2 * M_PI * i / USER_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center->x + radius * cos(angle),
                    center->y + radius * sin(angle)};
    list_add(circle_shape, v);
  }
  body_t *coin =
      body_init_with_info(circle_shape, mass, BLUE, make_type_info(info), free);
  body_set_velocity(coin, velocity);
  body_set_centroid(coin, *center);
  return coin;
}

void remove_moving_bodies(state_temp_t *state, body_type_t info)
{
  size_t num_bodies = scene_bodies(state->scene);
  for (size_t i = 0; i < num_bodies; i++)
  {
    body_t *body = scene_get_body(state->scene, i);
    if (get_type(body) == info && body_get_centroid(body).x +
                                                      REMOVE_OFFSET < MIN.x)
    {
      scene_remove_body(state->scene, i);
    }
  }
}

/**
 * Initializes background
 */
static background_state_t *background_init(const char *bg_path)
{
  background_state_t *state = malloc(sizeof(background_state_t));
  assert(state != NULL);
  state->scroll_speed = BACKGROUND_SCROLL_SPEED;
  state->bg_offset = BACKGROUND_OFFSET;
  SDL_Rect bg_bounds = make_texr(MIN.x, MIN.y, MAX.x, MAX.y);
  state->bg1 = asset_make_image(bg_path, bg_bounds);
  state->bg2 = asset_make_image(bg_path, bg_bounds);
  return state;
}

void background_update(background_state_t *state, double dt)
{
  state->bg_offset -= state->scroll_speed * dt;
  if (state->bg_offset <= -WINDOW_WIDTH)
  {
    state->bg_offset += WINDOW_WIDTH;
  }
  state->bg1->bounding_box.x = state->bg_offset;
  state->bg2->bounding_box.x = state->bg_offset + WINDOW_WIDTH;
}

state_temp_t *state_temp_init()
{
  state_temp_t *state = malloc(sizeof(state_temp_t));
  assert(state != NULL);
  state->scene = scene_init();
  state->body_assets = list_init(INITIAL_LIST_CAPACITY, (free_func_t)asset_destroy);
  vector_t zero = VEC_ZERO;
  state->user = make_obstacle_circle(USER_RADIUS, &zero, USER, USER_MASS, VEC_ZERO);
  vector_t start_pos = {MAX.x / 2, MIN.y + USER_RADIUS + GROUND_OFFSET};
  body_set_centroid(state->user, start_pos);
  scene_add_body(state->scene, state->user);
  asset_t *img = asset_make_image_with_body(USER_IMG_PATH, state->user);
  list_add(state->body_assets, img);
  add_walls(state);
  state->background_state = background_init(BACKGROUND_PATH);
  return state;
}
