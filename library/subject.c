#include "subject.h"

const size_t INITIAL_NUM_OBSERVERS = 10;

struct subject {
  list_t *observers;
};

subject_t *subject_init() {
    subject_t *subject = malloc(sizeof(subject_t));
    assert(subject != NULL);
    subject->observers = list_init(INITIAL_NUM_OBSERVERS, (free_func_t) free);
    fprintf(stderr, "Subject initialized at %p\n", (void*)subject);
    return subject;
}

void subject_add_observer(subject_t *subject, observer_t *observer) {
    list_add(subject->observers, observer);
    fprintf(stderr, "Adding observer at %p to subject at %p\n", (void*)observer, (void*)subject);
}

size_t subject_num_observers(subject_t *subject) {
    return list_size(subject->observers);
}

void subject_remove_observer(subject_t *subject, observer_t *observer) {
  size_t num_observers = subject_num_observers(subject);
  for (size_t i = 0; i < num_observers; i++) {
    observer_t *cur_observer = list_get(subject->observers, i);
    if (observer == cur_observer) {
        list_remove(subject->observers, i);
        return;
    }
  }
}

void subject_notify(subject_t *subject, event_t event) {
    size_t num_observers = subject_num_observers(subject);
    fprintf(stderr, "Notifying %zu observers for event %d\n", num_observers, event);
    for (size_t i = 0; i < num_observers; i++) {
        observer_t *cur_observer = list_get(subject->observers, i);
        fprintf(stderr, "Observer %zu at %p, notify function %p\n", i, (void*)cur_observer, (void*)cur_observer->on_notify);
        // TODO: remove
        if (cur_observer == NULL) {
            fprintf(stderr, "Observer %zu is null\n", i);
            continue;
        }
        if (cur_observer->on_notify == NULL) {
            fprintf(stderr, "Observer %zu notify function is null\n", i);
            continue;
        }
        cur_observer->on_notify(cur_observer, event);
        fprintf(stderr, "Notified observer %zu at %p, notify function %p\n", i, (void*)cur_observer, (void*)cur_observer->on_notify);
    }
    fprintf(stderr, "Notified %zu observers\n", num_observers);
}

void subject_free(subject_t *subject) {
    fprintf(stderr, "Freeing subject at %p\n", (void*)subject);
    if (subject->observers != 0 && subject->observers != NULL) {
        size_t num_observers = list_size(subject->observers);
        for (size_t i = 0; i < num_observers; i++) {
            observer_t *observer = list_get(subject->observers, i);
            if (observer != NULL && observer->freer != NULL) {
                observer->freer(observer);
            }
        }
    }
    free(subject);
}