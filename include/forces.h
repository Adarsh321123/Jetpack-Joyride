#ifndef __FORCES_H__
#define __FORCES_H__

#include "scene.h"

typedef struct force_creator_info force_creator_info_t;

/**
 * Initialize the force creator struct with auxiliary values
 */
force_creator_info_t *force_creator_init(force_creator_t force_creator,
                                         void *aux, list_t *bodies);

/**
 * Free the force creator struct and its aux value
 */
void force_creator_free(void *fc);

/**
 * Returns a list of bodies associated to force_creator_info_t object
 * since list of bodies on which force creator is being applied is not directly
 * accessible by other files
 */
list_t *get_bodies_force_creator(void *info);

/**
 * Apply the force creator on the given auxiliary data for it.
 */
void apply_force_creator(force_creator_info_t *info);

/**
 * Adds a force creator to a scene that applies gravity between two bodies.
 * The force creator will be called each tick
 * to compute the Newtonian gravitational force between the bodies.
 * See
 * https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form.
 * The force should not be applied when the bodies are very close,
 * because its magnitude blows up as the distance between the bodies goes to 0.
 *
 * @param scene the scene containing the bodies
 * @param G the gravitational proportionality constant
 * @param body1 the first body
 * @param body2 the second body
 */
void create_newtonian_gravity(scene_t *scene, double G, body_t *body1,
                              body_t *body2);

/**
 * The force creator for gravitational forces between objects. Calculates
 * the magnitude of the force components and adds the force to each
 * associated body.
 * @param info auxiliary information about the force and associated bodies
 */
void newtonian_gravity(void *info);

/**
 * Adds a force creator to a scene that acts like a spring between two bodies.
 * The force creator will be called each tick
 * to compute the Hooke's-Law spring force between the bodies.
 * See https://en.wikipedia.org/wiki/Hooke%27s_law.
 *
 * @param scene the scene containing the bodies
 * @param k the Hooke's constant for the spring
 * @param body1 the first body
 * @param body2 the second body
 */
void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2);

/**
 * The force creator for spring forces between objects. Calculates
 * the magnitude of the force components and adds the force to each
 * associated body.
 * @param info auxiliary information about the force and associated bodies
 */
void spring_force(void *info);

/**
 * Adds a force creator to a scene that applies a drag force on a body.
 * The force creator will be called each tick
 * to compute the drag force on the body proportional to its velocity.
 * The force points opposite the body's velocity.
 *
 * @param scene the scene containing the bodies
 * @param gamma the proportionality constant between force and velocity
 *   (higher gamma means more drag)
 * @param body the body to slow down
 */
void create_drag(scene_t *scene, double gamma, body_t *body);

/**
 * The force creator for drag forces on an object. Calculates
 * the magnitude of the force components and adds the force to the
 * associated body.
 * @param info auxiliary information about the force and associated body
 */
void drag_force(void *info);

/**
 * Adds a force creator to a scene that destroys two bodies when they collide.
 * The bodies should be destroyed by calling body_remove().
 *
 * @param scene the scene containing the bodies
 * @param body1 the first body
 * @param body2 the second body
 */
void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2);

/**
 * The force creator for destructive collisions between objects. Checks
 * if two bodies collide and marks both bodies for removal if so.
 * @param info auxiliary information about the force and associated body
 */
void destructive_collision(void *info);

#endif // #ifndef __FORCES_H__
