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

// TODO: need filename?
struct achievements {
    observer_t observer;
    list_t *achievements_list;
};

// TODO: add docstrings
size_t achievements_size(achievements_t *achievements);
void init_achievements_file(achievements_t *achievements);
void read_achievements(achievements_t *achievements);
void write_achievements(achievements_t *achievements);
void sync_to_persistent_storage();
void sync_from_persistent_storage_and_read(achievements_t *achievements);
void sync_from_persistent_storage_and_write(achievements_t *achievements);
void mount_persistent_fs();
void achievements_on_notify(observer_t *observer, event_t event);
achievements_t *achievements_init();
void achievements_free(achievements_t *achievements);

#endif // #ifndef __ACHIEVEMENT_H__