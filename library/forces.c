#include "forces.h"
#include "collision.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <stdio.h>

const double MIN_DIST = 5;

typedef struct {
  double force_const;
  list_t *bodies;
} body_aux_t;

struct force_creator_info {
  force_creator_t force_creator;
  body_aux_t *aux;
  list_t *force_bodies;
};

body_aux_t *body_aux_init(double force_const, list_t *bodies) {
  body_aux_t *aux = malloc(sizeof(body_aux_t));
  assert(aux);

  aux->bodies = bodies;
  aux->force_const = force_const;

  return aux;
}

force_creator_info_t *force_creator_init(force_creator_t force_creator,
                                         void *aux, list_t *bodies) {
  force_creator_info_t *fc = malloc(sizeof(force_creator_info_t));
  assert(fc);
  fc->force_bodies = list_init(list_size(bodies), (free_func_t)list_free);
  for (size_t i = 0; i < list_size(bodies); i++) {
    list_add(fc->force_bodies, list_get(bodies, i));
  }
  free(bodies->elements);
  free(bodies);

  fc->force_creator = force_creator;
  fc->aux = (body_aux_t *)aux;

  return fc;
}

void body_aux_free(void *aux) {
  free(((body_aux_t *)aux)->bodies->elements);
  free(((body_aux_t *)aux)->bodies);
  free(aux);
}

void force_creator_free(void *fc) {
  free(((force_creator_info_t *)fc)->force_bodies->elements);
  free(((force_creator_info_t *)fc)->force_bodies);
  body_aux_free(((force_creator_info_t *)fc)->aux);
  free(fc);
}

list_t *get_bodies_force_creator(void *info) {
  force_creator_info_t *info_fc = (force_creator_info_t *)info;
  return info_fc->force_bodies;
}

void apply_force_creator(force_creator_info_t *info) {
  info->force_creator(info->aux);
}

void newtonian_gravity(void *info) {
  body_aux_t *aux = (body_aux_t *)info;
  vector_t displacement =
      vec_subtract(body_get_centroid(list_get(aux->bodies, 0)),
                   body_get_centroid(list_get(aux->bodies, 1)));
  vector_t unit_disp =
      vec_multiply(1 / sqrt(vec_dot(displacement, displacement)), displacement);

  double distance = sqrt(vec_dot(displacement, displacement));

  if (distance > MIN_DIST) {
    vector_t grav_force = vec_multiply(
        aux->force_const * body_get_mass(list_get(aux->bodies, 0)) *
            body_get_mass(list_get(aux->bodies, 1)) /
            vec_dot(displacement, displacement),
        unit_disp);

    body_add_force(list_get(aux->bodies, 1), grav_force);
    body_add_force(list_get(aux->bodies, 0), vec_multiply(-1, grav_force));
  }
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1,
                              body_t *body2) {
  list_t *bodies = list_init(2, NULL);
  list_t *aux_bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  list_add(aux_bodies, body1);
  list_add(aux_bodies, body2);
  body_aux_t *aux = body_aux_init(G, aux_bodies);
  scene_add_bodies_force_creator(scene, (force_creator_t)newtonian_gravity, aux,
                                 bodies);
}

void spring_force(void *info) {
  body_aux_t *aux = info;

  double k = aux->force_const;
  body_t *body1 = list_get(aux->bodies, 0);
  body_t *body2 = list_get(aux->bodies, 1);

  vector_t center_1 = body_get_centroid(body1);
  vector_t center_2 = body_get_centroid(body2);
  vector_t distance = vec_subtract(center_1, center_2);

  vector_t spring_force = {-k * distance.x, -k * distance.y};
  body_add_force(body1, spring_force);
  body_add_force(body2, vec_negate(spring_force));
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {
  list_t *bodies = list_init(2, NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  list_t *aux_bodies = list_init(2, NULL);
  list_add(aux_bodies, body1);
  list_add(aux_bodies, body2);
  body_aux_t *aux = body_aux_init(k, aux_bodies);
  scene_add_bodies_force_creator(scene, (force_creator_t)spring_force, aux,
                                 bodies);
}

void drag_force(void *info) {
  body_aux_t *aux = (body_aux_t *)info;
  vector_t cons_force = vec_multiply(
      -1 * aux->force_const, body_get_velocity(list_get(aux->bodies, 0)));

  body_add_force(list_get(aux->bodies, 0), cons_force);
}

void create_drag(scene_t *scene, double gamma, body_t *body) {
  list_t *bodies = list_init(1, NULL);
  list_t *aux_bodies = list_init(1, NULL);
  list_add(bodies, body);
  list_add(aux_bodies, body);
  body_aux_t *aux = body_aux_init(gamma, aux_bodies);
  scene_add_bodies_force_creator(scene, (force_creator_t)drag_force, aux,
                                 bodies);
}

void create_destructive_collision(scene_t *scene, body_t *body1,
                                  body_t *body2) {
  list_t *bodies = list_init(2, (free_func_t)NULL);
  list_add(bodies, body1);
  list_add(bodies, body2);
  list_t *aux_bodies = list_init(2, (free_func_t)NULL);
  list_add(aux_bodies, body1);
  list_add(aux_bodies, body2);
  body_aux_t *aux = body_aux_init(0, aux_bodies);
  scene_add_bodies_force_creator(scene, (force_creator_t)destructive_collision,
                                 aux, bodies);
}

void destructive_collision(void *info) {
  body_aux_t *aux = info;

  body_t *b1 = list_get(aux->bodies, 0);
  body_t *b2 = list_get(aux->bodies, 1);

  if (find_collision(b1, b2)) {
    body_remove(b1);
    body_remove(b2);
  }
}
