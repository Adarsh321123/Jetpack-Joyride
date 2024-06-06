#include "observer.h"
#include <stddef.h>

observer_t *observer_init(on_notify_t on_notify) {
    observer_t *observer = malloc(sizeof(observer_t));
    assert(observer != NULL);
    observer->on_notify = on_notify;
    fprintf(stderr, "Observer initialized with notify function %p\n", on_notify);
    return observer;
}

void observer_free(observer_t *observer) {
    free(observer);
}

// TODO: free observer, subject, achievement