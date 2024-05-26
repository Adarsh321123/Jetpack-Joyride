#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "list.h"
#include "scene.h"
#include "vector.h"
#include <stdbool.h>

/**
 * Determines whether two bodies intersect.
 *
 * @param body1 the first body
 * @param body2 the second body
 * @return whether the bodies are colliding
 */
bool find_collision(body_t *body1, body_t *body2);

#endif // #ifndef __COLLISION_H__
