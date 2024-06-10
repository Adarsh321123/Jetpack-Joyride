#ifndef __LASER_H__
#define __LASER_H__

#include "game_play_helper.h"
#include "game_play.h"

void add_laser(game_play_state_t *game_play_state, double dt);

void remove_lasers_inactive(game_play_state_t *game_play_state);

void remove_lasers(game_play_state_t *game_play_state);

#endif // #ifndef __LASER_H__