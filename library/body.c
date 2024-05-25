#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "body.h"

const double ROTATION_SPEED = 0.0;
const size_t INITIAL_LIST_CAPACITY = 10;
const double INITIAL_ANGLE = 0.0;
const double HALF = 0.5;

/**
 * A rigid body constrained to the plane.
 * Implemented as a polygon with uniform density.
 */
struct body {
  polygon_t *poly;

  double mass;
  double angle;
  vector_t centroid;

  vector_t force;
  vector_t impulse;
  bool removed;

  void *info;
  free_func_t info_freer;
};

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer) {
  body_t *body = malloc(sizeof(body_t));
  assert(body != NULL);
  body->poly =
      polygon_init(shape, VEC_ZERO, ROTATION_SPEED, color.r, color.g, color.b);
  assert(body->poly != NULL);
  body->mass = mass;
  body->angle = INITIAL_ANGLE;
  body->centroid = polygon_centroid(body->poly);
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
  body->removed = false;
  body->info = info;
  body->info_freer = info_freer;
  return body;
}

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  return body_init_with_info(shape, mass, color, NULL, NULL);
}

void body_free(body_t *body) {
  polygon_free(body->poly);
  if (body->info_freer != NULL) {
    body->info_freer(body->info);
  }
  free(body);
}

list_t *body_get_shape(body_t *body) {
  list_t *points = polygon_get_points(body->poly);
  list_t *copy = list_init(INITIAL_LIST_CAPACITY, free);
  size_t size = list_size(points);
  assert(size >= 0);
  for (size_t i = 0; i < size; i++) {
    vector_t *list_v = malloc(sizeof(*list_v));
    assert(list_v);
    vector_t *curr = list_get(points, i);
    list_v->x = curr->x;
    list_v->y = curr->y;
    list_add(copy, list_v);
  }
  return copy;
}

vector_t body_get_centroid(body_t *body) { return body->centroid; }

vector_t body_get_velocity(body_t *body) {
  double x_velocity = polygon_get_velocity_x(body->poly);
  double y_velocity = polygon_get_velocity_y(body->poly);
  return (vector_t){.x = x_velocity, .y = y_velocity};
}

rgb_color_t *body_get_color(body_t *body) {
  return polygon_get_color(body->poly);
}

void *body_get_info(body_t *body) { return body->info; }

polygon_t *body_get_polygon(body_t *body) { return body->poly; }

void body_set_color(body_t *body, rgb_color_t *col) {
  polygon_set_color(body->poly, col);
}

void body_set_centroid(body_t *body, vector_t x) {
  polygon_set_center(body->poly, x);
  body->centroid = x;
}

void body_set_velocity(body_t *body, vector_t v) {
  polygon_set_velocity(body->poly, v);
}

double body_get_rotation(body_t *body) { return body->angle; }

void body_set_rotation(body_t *body, double angle) {
  polygon_rotate(body->poly, angle - body->angle, body->centroid);
  body->angle = angle;
}

void body_tick(body_t *body, double dt) {
  // by impulse momentum theorem, 1/mass * impulse gives delta v
  double inverse_mass = 1 / body->mass;
  vector_t dv = VEC_ZERO;
  dv = vec_multiply(inverse_mass, body->impulse);
  vector_t vel = body_get_velocity(body);
  vector_t new_vel = vec_add(vel, dv);

  // by newton's second law, F/m = a
  vector_t a = VEC_ZERO;
  inverse_mass = 1 / body->mass;
  a = vec_multiply(inverse_mass, body->force);
  a = vec_multiply(dt, a);
  new_vel = vec_add(new_vel, a);

  // move body and reset force and impulse for damping to work
  vector_t avg_vel = vec_add(vel, new_vel);
  avg_vel = vec_multiply(HALF, avg_vel);
  vector_t translation = vec_multiply(dt, avg_vel);
  polygon_translate(body->poly, translation);
  body_set_velocity(body, new_vel);
  body->centroid = vec_add(body->centroid, translation);
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
}

double body_get_mass(body_t *body) { return body->mass; }

void body_add_force(body_t *body, vector_t force) {
  body->force = vec_add(body->force, force);
}

void body_add_impulse(body_t *body, vector_t impulse) {
  body->impulse = vec_add(body->impulse, impulse);
}

void body_reset(body_t *body) {
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
}

void body_remove(body_t *body) { body->removed = true; }

bool body_is_removed(body_t *body) { return body->removed; }
