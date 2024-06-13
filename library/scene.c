#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "forces.h"
#include "scene.h"
#include "constants.h"

struct scene
{
  size_t num_bodies;
  list_t *bodies;
  list_t *force_creators;
};

scene_t *scene_init(void)
{
  scene_t *scene = malloc(sizeof(scene_t));
  assert(scene != NULL);
  scene->num_bodies = INITIAL_NUM_BODIES;
  scene->bodies = list_init(INITIAL_BODY_CAPACITY, (free_func_t)body_free);
  assert(scene->bodies != NULL);
  scene->force_creators = list_init(INITIAL_FORCE_CREATOR_CAPACITY,
                                    (free_func_t)force_creator_free);
  assert(scene->force_creators != NULL);
  return scene;
}

void scene_free(scene_t *scene)
{
  list_free(scene->force_creators);
  list_free(scene->bodies);
  free(scene);
}

size_t scene_bodies(scene_t *scene) { return scene->num_bodies; }

body_t *scene_get_body(scene_t *scene, size_t index)
{
  return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body)
{
  list_add(scene->bodies, body);
  scene->num_bodies++;
}

void scene_remove_body(scene_t *scene, size_t index)
{
  assert(index >= 0);
  body_remove(list_get(scene->bodies, index));
}

void scene_tick(scene_t *scene, double dt)
{
  // initialize force creators (as done in last engine)
  size_t num_force_creators = list_size(scene->force_creators);
  for (size_t i = 0; i < num_force_creators; i++)
  {
    force_creator_info_t *info = list_get(scene->force_creators, i);
    apply_force_creator(info);
  }

  for (ssize_t i = 0; i < (ssize_t)scene->num_bodies; i++)
  {
    body_t *current_body = list_get(scene->bodies, i);
    bool check_body_removed = body_is_removed(current_body);

    if (check_body_removed)
    {
      ssize_t j = 0;
      while (j < (ssize_t)num_force_creators)
      {
        force_creator_info_t *fc_info = list_get(scene->force_creators, j);
        list_t *list_force_creators = get_bodies_force_creator(fc_info);
        size_t size_force_creator = list_size(list_force_creators);

        for (ssize_t k = 0; k < (ssize_t)size_force_creator; k++)
        {
          if (current_body == list_get(list_force_creators, k))
          {
            force_creator_info_t *removed_fc_info =
                list_remove(scene->force_creators, j);
            force_creator_free(removed_fc_info);
            num_force_creators--;
            j--;
            break;
          }
        }
        j++;
      }
      body_t *body_remove = list_remove(scene->bodies, i);
      assert(body_remove != NULL);
      scene->num_bodies--;
      i--;
    }
    else
    {
      body_tick(current_body, dt);
    }
  }
}

void scene_add_force_creator(scene_t *scene, force_creator_t force_creator,
                             void *aux)
{
  scene_add_bodies_force_creator(scene, force_creator, aux,
                                 list_init(0, (free_func_t)body_free));
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies)
{
  force_creator_info_t *fc_info = force_creator_init(forcer, aux, bodies);
  assert(fc_info != NULL);
  list_add(scene->force_creators, fc_info);
}
