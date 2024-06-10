#include "helper_functions.h"

body_t *make_rectangle(vector_t center, double width, double height, void *aux_info) {
  list_t *rectangle_shape = make_rectangle(center, width, height);
  body_t *rectangle = body_init_with_info(rectangle_shape, INFINITY, BLACK,
                                      make_type_info(aux_info), free);
  body_set_velocity(rectangle, ZAPPER_VEL);
  body_set_centroid(rectangle, center);
  return rectangle;
}

body_t *make_circle(double radius, vector_t center, void *aux_info, double mass, vector_t velocity) {
  center.y += radius;
  list_t *circle_shape = list_init(USER_NUM_POINTS, free);
  for (size_t i = 0; i < USER_NUM_POINTS; i++) {
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