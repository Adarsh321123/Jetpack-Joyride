#ifndef __OBSERVER_H__
#define __OBSERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef enum
{
  EVENT_COIN_COLLECTED,
  EVENT_DISTANCE_TRAVELED,
  EVENT_LASERS_AVOIDED
} event_t;

/**
 * A observer handler.
 * When an event occurs, the observer is passed the event
 */
typedef struct observer observer_t;

/**
 * Function pointer to call when the observer is notified
*/
typedef void (*on_notify_t)(observer_t *observer, event_t event, void *aux);

/**
 * The freer to use to free the observer.
*/
typedef void (*freer_t)(void *observer);

struct observer
{
  on_notify_t on_notify;
  freer_t freer;
};

#endif // #ifndef __OBSERVER_H__