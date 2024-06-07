#include "polygon.h"
#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

const size_t INITIAL_SIZE = 10;
const double GRAVITY = 9.8;

typedef struct polygon {
  list_t *points;
  vector_t *velocity;
  double rotation_speed;
  rgb_color_t *color;
} polygon_t;

polygon_t *polygon_init(list_t *points, vector_t initial_velocity,
                        double rotation_speed, double red, double green,
                        double blue) {
  polygon_t *polygon = malloc(sizeof(polygon_t));
  assert(polygon != NULL);
  polygon->rotation_speed = rotation_speed;
  polygon->color = color_init(red, green, blue);
  polygon->points = points;
  polygon->velocity = malloc(sizeof(vector_t));
  assert(polygon->velocity != NULL);
  *(polygon->velocity) = initial_velocity;
  return polygon;
}

list_t *polygon_get_points(polygon_t *polygon) { return polygon->points; }

void polygon_move(polygon_t *polygon, double time_elapsed) {
  polygon_translate(polygon, vec_multiply(time_elapsed, *(polygon->velocity)));
  polygon_rotate(polygon, polygon->rotation_speed * time_elapsed,
                 polygon_centroid(polygon));
  (polygon->velocity)->y += time_elapsed * GRAVITY;
}

void polygon_set_velocity(polygon_t *polygon, vector_t vel) {
  polygon->velocity->x = vel.x;
  polygon->velocity->y = vel.y;
}

void polygon_free(polygon_t *polygon) {
  free(polygon->velocity);
  color_free(polygon->color);
  list_free(polygon->points);
  free(polygon);
}

double polygon_get_velocity_x(polygon_t *polygon) {
  return polygon->velocity->x;
}

double polygon_get_velocity_y(polygon_t *polygon) {
  return polygon->velocity->y;
}

double polygon_area(polygon_t *polygon) {
  list_t *points = polygon->points;
  size_t length = list_size(points);
  double area = 0;
  for (size_t i = 0; i < length - 1; i++) {
    vector_t *cur = list_get(points, i);
    vector_t *next = list_get(points, i + 1);
    area += (next->x + cur->x) * (next->y - cur->y);
  }

  vector_t *last = list_get(points, length - 1);
  vector_t *first = list_get(points, 0);
  area += (first->x + last->x) * (first->y - last->y);
  area = fabs(area);
  area /= 2;
  return area;
}

vector_t polygon_centroid(polygon_t *polygon) {
  double area = polygon_area(polygon);
  vector_t centroid;
  list_t *points = polygon->points;
  size_t length = list_size(points);

  double x = 0;
  for (size_t i = 0; i < length - 1; i++) {
    vector_t *cur = list_get(points, i);
    vector_t *next = list_get(points, i + 1);
    x += (cur->x + next->x) * (cur->x * next->y - next->x * cur->y);
  }

  vector_t *last = list_get(points, length - 1);
  vector_t *first = list_get(points, 0);
  x += (last->x + first->x) * (last->x * first->y - first->x * last->y);
  x /= 6 * area;
  centroid.x = x;

  double y = 0;
  for (size_t i = 0; i < length - 1; i++) {
    vector_t *cur = list_get(points, i);
    vector_t *next = list_get(points, i + 1);
    y += (cur->y + next->y) * (cur->x * next->y - next->x * cur->y);
  }

  last = list_get(points, length - 1);
  first = list_get(points, 0);
  y += (last->y + first->y) * (last->x * first->y - first->x * last->y);
  y /= 6 * area;
  centroid.y = y;

  return centroid;
}

void polygon_translate(polygon_t *polygon, vector_t translation) {
  list_t *points = polygon->points;
  size_t length = list_size(points);
  for (size_t i = 0; i < length; i++) {
    vector_t *cur = list_get(points, i);
    vector_t translated = vec_add(*cur, translation);
    *cur = translated;
  }
}

void polygon_rotate(polygon_t *polygon, double angle, vector_t point) {
  size_t length = list_size(polygon->points);
  // we can center each vector in polygon around point, rotate, and then
  // revert to the previous center
  for (size_t i = 0; i < length; i++) {
    vector_t *cur = list_get(polygon->points, i);
    vector_t subtracted = vec_subtract(*cur, point);
    vector_t rotated = vec_rotate(subtracted, angle);
    vector_t added = vec_add(rotated, point);
    *cur = added;
  }
}

rgb_color_t *polygon_get_color(polygon_t *polygon) { return polygon->color; }

void polygon_set_color(polygon_t *polygon, rgb_color_t *color) {
  polygon->color = color;
}

void polygon_set_center(polygon_t *polygon, vector_t centroid) {
  vector_t prev_centroid = polygon_centroid(polygon);
  vector_t translation = vec_subtract(centroid, prev_centroid);
  polygon_translate(polygon, translation);
}

vector_t polygon_get_center(polygon_t *polygon) {
  return polygon_centroid(polygon);
}

void polygon_set_rotation(polygon_t *polygon, double rot) {
  polygon->rotation_speed = rot;
}

double polygon_get_rotation(polygon_t *polygon) {
  return polygon->rotation_speed;
}
