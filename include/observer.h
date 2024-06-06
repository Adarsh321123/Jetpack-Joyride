#ifndef __OBSERVER_H__
#define __OBSERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef enum {
    EVENT_COIN_COLLECTED,
    EVENT_DISTANCE_TRAVELED,
    EVENT_ZAPPERS_DODGED
} event_t;

// TODO: update all of these docstrings for observer, subject, achievement
// TODO: use this instead of putting in struct
/**
 * A observer handler.
 * When an event occurs, the observer is passed the event
 *
 * @param key a character indicating which key was pressed
 * @param state object passed in to retrieve and update idx
 * @param event event SDL_Event to retrieve position of mouse clicks
 */

// TODO: why need observer?
typedef struct observer observer_t;

typedef void (*on_notify_t)(observer_t *observer, event_t event);

// TODO: do we need this struct then?
// TODO: move back to c?
struct observer {
    on_notify_t on_notify;
};

/**
 * Initializes a observer struct with empty information.
 * 
 * @return an initialized `observer_t`
*/
observer_t *observer_init(on_notify_t on_notify);
void observer_free(observer_t *observer);

#endif  // #ifndef __OBSERVER_H__