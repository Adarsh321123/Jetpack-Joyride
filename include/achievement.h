#ifndef __ACHIEVEMENT_H__
#define __ACHIEVEMENT_H__

#include "observer.h"
#include "list.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

typedef struct achievement achievement_t;
typedef struct achievements achievements_t;

// TODO: move these back to c
struct achievement {
    char *name;
    size_t progress;
    size_t target;
    bool unlocked;
};

struct achievements {
    observer_t observer;
    list_t *achievements_list;
};

/**
 * Reads the achievements file and returns a list of parsed achievements.
 * Use this for displaying achievements in the settings menu.
 */
list_t *read_achievements_settings();

/**
 * Event handler for the achievements.
 * Use aux to hold information for a specific achievement, like the total distance covered.
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