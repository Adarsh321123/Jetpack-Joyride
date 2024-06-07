#ifndef __SUBJECT_H__
#define __SUBJECT_H__

#include "list.h"
#include "observer.h"

typedef struct subject subject_t;

/**
 * Initializes a subject struct with a list of observers.
 * 
 * @return an initialized `subject_t`
*/
subject_t *subject_init();

/**
 * Add an observer to the given subject
 * 
 * @param subject subject to add the observer to
 * @param observer observer to add to the subject
*/
void subject_add_observer(subject_t *subject, observer_t *observer);

/**
 * Retrieves the number of observers observing this subject
 * 
 * @return a `size_t` with the number of observers observing this subject
*/
size_t subject_num_observers(subject_t *subject);

/**
 * Remove an observer from the given subject
 * 
 * @param subject subject to remove the observer from
 * @param observer observer to remove from the subject
*/
void subject_remove_observer(subject_t *subject, observer_t *observer);

/**
 * Notifies each of the subject's observers that an event happened.
 * 
 * @param subject subject that is being observerd
 * @param event event that occurred that is sent to the observers
*/
void subject_notify(subject_t *subject, event_t event, void *aux);

void subject_free(subject_t *subject);

#endif // #ifndef __SUBJECT_H__