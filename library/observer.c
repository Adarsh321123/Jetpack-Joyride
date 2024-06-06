#include "observer.h"
#include <stddef.h>

observer_t *observer_init(on_notify_t on_notify) {
    observer_t *observer = malloc(sizeof(observer_t));
    assert(observer != NULL);
    observer->on_notify = on_notify;
    fprintf(stderr, "Observer initialized at %p with notify function %p\n", (void*)observer, (void*)on_notify);
    return observer;
}

void observer_free(observer_t *observer) {
    fprintf(stderr, "Freeing observer at %p with notify function %p\n", (void*)observer, (void*)observer->on_notify);
    free(observer);
}

// TODO: free observer, subject, achievement