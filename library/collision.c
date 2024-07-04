#include "collision.h"
#include "body.h"
#include "constants.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

/**
 * Returns a list of vectors representing the edges of a shape.
 *
 * @param shape the list of vectors representing the vertices of a shape
 * @return a list of vectors representing the edges of the shape
 */
static list_t *get_edges(list_t *shape)
{
  list_t *edges = list_init(list_size(shape), free);

  for (size_t i = 0; i < list_size(shape); i++)
  {
    vector_t *vec = malloc(sizeof(vector_t));
    assert(vec);
    *vec =
        vec_subtract(*(vector_t *)list_get(shape, i % list_size(shape)),
                     *(vector_t *)list_get(shape, (i + 1) % list_size(shape)));
    list_add(edges, vec);
  }

  return edges;
}

/**
 * Returns a vector containing the maximum and minimum length projections given
 * a unit axis and shape.
 *
 * @param shape the list of vectors representing the vertices of a shape
 * @param unit_axis the unit axis to project eeach vertex on
 * @return a vector in the form (max, min) where `max` is the maximum projection
 * length and `min` is the minimum projection length.
 */
static vector_t get_max_min_projections(list_t *shape, vector_t unit_axis)
{
  double max = -__DBL_MAX__;
  double min = __DBL_MAX__;
  size_t shape_size = list_size(shape);
  for (size_t i = 0; i < shape_size; i++)
  {
    vector_t vertex = *(vector_t *)list_get(shape, i);
    double dot_prod = vec_dot(vertex, unit_axis);
    if (dot_prod < min)
    {
      min = dot_prod;
    }
    if (dot_prod > max)
    {
      max = dot_prod;
    }
  }
  return (vector_t){max, min};
}

/**
 * Determines whether two convex polygons intersect.
 * The polygons are given as lists of vertices in counterclockwise order.
 * There is an edge between each pair of consecutive vertices,
 * and one between the first vertex and the last vertex.
 *
 * @param shape1 the first shape
 * @param shape2 the second shape
 * @param min_overlap the minimum overlap between the two shapes
 * @return whether the shapes are colliding
 */
static collision_info_t compare_collision(list_t *shape1, list_t *shape2,
                                          double *min_overlap)
{
  collision_info_t collision_info;
  list_t *edges1 = get_edges(shape1);
  assert(edges1 != NULL);

  for (size_t i = 0; i < list_size(edges1); i++)
  {
    vector_t curr_vector = *(vector_t *)list_get(edges1, i);
    vector_t perpendicular = vec_rotate(curr_vector, ANGLE_90);
    double len_vec = vec_get_length(perpendicular);
    vector_t unit_perpendicular = vec_multiply(1 / len_vec, perpendicular);
    vector_t get_shape1 = get_max_min_projections(shape1, unit_perpendicular);
    vector_t get_shape2 = get_max_min_projections(shape2, unit_perpendicular);
    if ((get_shape1.x < get_shape2.y) || (get_shape2.x < get_shape1.y))
    {
      collision_info.collided = false;
      list_free(edges1);
      return collision_info;
    }
    else
    {
      double overlap =
          fmin(get_shape1.x, get_shape2.x) - fmax(get_shape1.y, get_shape2.y);
      if (overlap < *min_overlap)
      {
        *min_overlap = overlap;
        collision_info.axis = unit_perpendicular;
      }
    }
  }

  collision_info.collided = true;
  list_free(edges1);
  return collision_info;
}

collision_info_t find_collision(body_t *body1, body_t *body2)
{
  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);

  double c1_overlap = __DBL_MAX__;
  double c2_overlap = __DBL_MAX__;

  collision_info_t collision1 = compare_collision(shape1, shape2, &c1_overlap);
  collision_info_t collision2 = compare_collision(shape2, shape1, &c2_overlap);

  list_free(shape1);
  list_free(shape2);

  if (!collision1.collided)
  {
    return collision1;
  }

  if (!collision2.collided)
  {
    return collision2;
  }

  if (c1_overlap < c2_overlap)
  {
    return collision1;
  }
  return collision2;
}
