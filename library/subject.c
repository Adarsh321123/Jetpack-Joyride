#include "subject.h"

const size_t INITIAL_NUM_OBSERVERS = 10;

struct subject {
  list_t *observers;
};

subject_t *subject_init() {
    subject_t *subject = malloc(sizeof(subject_t));
    assert(subject != NULL);
    subject->observers = list_init(INITIAL_NUM_OBSERVERS, (free_func_t) list_free);
    fprintf(stderr, "Subject initialized\n");
    return subject;
}

void subject_add_observer(subject_t *subject, observer_t *observer) {
    list_add(subject->observers, observer);
    fprintf(stderr, "Added observer to subject\n");
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
    fprintf(stderr, "Notify: Number of observers: %zu\n", num_observers);
    for (size_t i = 0; i < num_observers; i++) {
        observer_t *cur_observer = list_get(subject->observers, i);
        fprintf(stderr, "retrieved observer %zu\n", i);
        // TODO: remove
        if (cur_observer == NULL) {
            fprintf(stderr, "Observer %zu is null\n", i);
            continue;
        }
        if (cur_observer->on_notify == NULL) {
            fprintf(stderr, "Observer %zu notify function is null\n", i);
            continue;
        }
        fprintf(stderr, "Calling notify function for observer %zu\n", i);
        cur_observer->on_notify(cur_observer, event);
        fprintf(stderr, "notified observer %zu\n", i);
    }
    fprintf(stderr, "Notified %zu observers\n", num_observers);
}

void subject_free(subject_t *subject) {
    list_free(subject->observers);
    free(subject);
}