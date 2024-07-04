#ifndef __ACHIEVEMENT_H__
#define __ACHIEVEMENT_H__

#include "observer.h"
#include "list.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

typedef struct achievement achievement_t;
typedef struct achievements achievements_t;

/**
 * Return the observer of the achievement.
*/
observer_t *achievements_get_observer(achievements_t *achievements);

/**
 * Read the file of the achievement adn return for settings.
*/
list_t *read_achievements_settings();

/**
 * Event handler for the achievements.
 * Use aux to hold information for a specific achievement, like total distance covered.
 */
void achievements_on_notify(observer_t *observer, event_t event, void *aux);

/**
 * Initialize achievements struct, its superclass observer, and its list of achievements.
 */
achievements_t *achievements_init();

/**
 * Free the achievements.
 */
void achievements_free(void *observer);

#endif // #ifndef __ACHIEVEMENT_H__