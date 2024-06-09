#ifndef __ACHIEVEMENT_H__
#define __ACHIEVEMENT_H__

#include "observer.h"
#include "list.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

typedef struct achievement achievement_t;
typedef struct achievements achievements_t;

// TODO: add docstrings
// TODO: ensure all h match with
observer_t *achievements_get_observer(achievements_t *achievements);
size_t achievements_size(achievements_t *achievements);
void init_achievements_file();
list_t *read_achievements_settings();
void read_achievements(achievements_t *achievements);
void write_achievements(achievements_t *achievements);
void sync_to_persistent_storage();
void sync_from_persistent_storage_and_read(achievements_t *achievements);
void sync_from_persistent_storage_and_write(achievements_t *achievements);
void mount_persistent_fs();
void achievements_on_notify(observer_t *observer, event_t event, void *aux);
achievements_t *achievements_init();
void achievements_free(void *observer);

#endif // #ifndef __ACHIEVEMENT_H__