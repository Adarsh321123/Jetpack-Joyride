#include "vector.h"
#include <math.h>

const vector_t VEC_ZERO = (vector_t){0.0, 0.0};

vector_t *vector_init(double x, double y) {
  vector_t *vector = malloc(sizeof(vector_t));
  assert(vector != NULL);
  vector->x = x;
  vector->y = y;
  return vector;
}

void vector_free(vector_t *vector) {
  free(vector);
}

vector_t vec_add(vector_t v1, vector_t v2) {
  vector_t v3;
  v3.x = v1.x + v2.x;
  v3.y = v1.y + v2.y;
  return v3;
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
  vector_t v3;
  v3.x = v1.x - v2.x;
  v3.y = v1.y - v2.y;
  return v3;
}

vector_t vec_negate(vector_t v) {
  vector_t v3;
  v3.x = -1 * v.x;
  v3.y = -1 * v.y;
  return v3;
}

vector_t vec_multiply(double scalar, vector_t v) {
  vector_t v3;
  v3.x = scalar * v.x;
  v3.y = scalar * v.y;
  return v3;
}

double vec_dot(vector_t v1, vector_t v2) { return v1.x * v2.x + v1.y * v2.y; }

double vec_cross(vector_t v1, vector_t v2) { return v1.x * v2.y - v1.y * v2.x; }

vector_t vec_rotate(vector_t v, double angle) {
  vector_t v3;
  v3.x = v.x * cos(angle) - v.y * sin(angle);
  v3.y = v.x * sin(angle) + v.y * cos(angle);
  return v3;
}

double vec_get_length(vector_t v) { return sqrt(pow(v.x, 2) + pow(v.y, 2)); }