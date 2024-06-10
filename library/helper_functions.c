#include "helper_functions.h"

body_type_t *make_type_info(body_type_t type)
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

list_t *make_rectangle(vector_t center, double width, double height)
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

body_t *make_obstacle_rectangle(vector_t center, double width, double height, void *aux_info, double velocity)
{
  list_t *rectangle_shape = make_rectangle(center, width, height);
  body_t *rectangle = body_init_with_info(rectangle_shape, INFINITY, BLACK,
                                          make_type_info(aux_info), free);
  body_set_velocity(rectangle, velocity);
  body_set_centroid(rectangle, center);
  return rectangle;
}

body_t *make_obstacle_circle(double radius, vector_t center, void *aux_info, double mass, vector_t velocity)
{
  center.y += radius;
  list_t *circle_shape = list_init(USER_NUM_POINTS, free);
  for (size_t i = 0; i < USER_NUM_POINTS; i++)
  {
    double angle = 2 * M_PI * i / USER_NUM_POINTS;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + radius * cos(angle),
                    center.y + radius * sin(angle)};
    list_add(circle_shape, v);
  }
  body_t *coin =
      body_init_with_info(circle_shape, mass, BLUE, make_type_info(aux_info), free);
  body_set_velocity(coin, velocity);
  body_set_centroid(coin, center);
  return coin;
}

void add_walls(state_temp_t *state)
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