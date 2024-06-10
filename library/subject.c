#include "subject.h"
#include "constants.h"

struct subject {
  list_t *observers;
};

subject_t *subject_init() {
    subject_t *subject = malloc(sizeof(subject_t));
    assert(subject != NULL);
    subject->observers = list_init(INITIAL_NUM_OBSERVERS, NULL);
    return subject;
}

void subject_add_observer(subject_t *subject, observer_t *observer) {
    list_add(subject->observers, observer);
}

size_t subject_num_observers(subject_t *subject) {
    return list_size(subject->observers);
}

void subject_notify(subject_t *subject, event_t event, void *aux) {
    size_t num_observers = subject_num_observers(subject);
    for (size_t i = 0; i < num_observers; i++) {
        observer_t *cur_observer = list_get(subject->observers, i);
        if (cur_observer == NULL) {
            continue;
        }
        if (cur_observer->on_notify == NULL) {
            continue;
        }
        cur_observer->on_notify(cur_observer, event, aux);
    }
}

void subject_free(subject_t *subject) {
    if (subject->observers != 0 && subject->observers != NULL) {
        size_t num_observers = list_size(subject->observers);
        for (size_t i = 0; i < num_observers; i++) {
            observer_t *observer = list_get(subject->observers, i);
            if (observer != NULL && observer->freer != NULL) {
                observer->freer(observer);
            }
        }
        list_free(subject->observers);
    }
    free(subject);
}