#include "math.h"
#include "sdl_wrapper.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef CONSTANTS_H
#define CONSTANTS_H

extern const vector_t MIN;
extern const vector_t MAX;
extern const vector_t CENTER;

extern const size_t INITIAL_ASSET_CAPACITY;

extern const size_t TEXT_SIZE;
extern const size_t TEXT_HEIGHT_SCALE;
extern const size_t NUM_TEXT_HOME;
extern const size_t NUM_TEXT_SETTINGS;
extern const size_t NUM_TEXT_GAME_OVER;
extern const size_t NUM_BACKGROUNDS;
extern const size_t NUM_BUTTONS_HOME;
extern const size_t NUM_BUTTONS_SETTINGS;
extern const size_t NUM_BUTTONS_GAME_OVER;

extern const size_t DEFAULT_WIDTH;
extern const size_t DEFAULT_HEIGHT;

extern const double INTERVAL;

#endif // CONSTANTS_H