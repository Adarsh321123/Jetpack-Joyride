#include "laser.h"
#include "zappers.h"


void add_laser(game_play_state_t *game_play_state, double dt) {
  game_play_state->laser->time_laser += dt;
  switch(game_play_state->laser->laser_spawn_bool) {
    case LASER_INACTIVE_STATE: {      
      if (game_play_state->laser->time_laser >= game_play_state->laser->laser_generation_time) {
        game_play_state->laser->laser_active = true;
        game_play_state->laser->time_laser = ZERO;
        size_t size_of_spawn_list = list_size(game_play_state->laser->laser_spawn_positions);
        for (size_t i = 0; i < size_of_spawn_list; i++) { 
          list_remove(game_play_state->laser->laser_spawn_positions, LIST_START_INDEX);
        }
        game_play_state->laser->time_laser_spawn = game_play_state->time;
        size_t num_laser_initialize = (size_t) round(fmod(rand(), MAX_NUM_LASERS)) + MIN_NUM_LASERS;
        size_t start_laser_position = (size_t) round(fmod(rand(), NUM_LASERS - num_laser_initialize)); 

        for (size_t i = start_laser_position; i < start_laser_position + num_laser_initialize; i++) {
          vector_t *center = list_get(game_play_state->laser->laser_centers, i);
          list_add(game_play_state->laser->laser_spawn_positions, center);
          // body_t *laser = make_laser(center);
          body_t *laser = make_obstacle_rectangle(center, LASER_WIDTH_ACTIVE, LASER_HEIGHT_ACTIVE, LASER, VEC_ZERO);
          scene_add_body(game_play_state->state->scene, laser);
          asset_update_bounding_box(list_get(game_play_state->laser->laser_inactive_assets, i), laser);
        }
        game_play_state->laser->laser_spawn_bool = LASER_ACTIVE_STATE;        
      }
      break;
    }

    case LASER_ACTIVE_STATE: {
      if (game_play_state->time - game_play_state->laser->time_laser_spawn >= LASER_ACTIVATION_TIME) {
        game_play_state->laser->time_laser_activate = game_play_state->time;
        game_play_state->laser->time_laser_spawn = INFINITY;   

        remove_lasers_inactive(game_play_state);
        size_t size_of_spawn_list = list_size(game_play_state->laser->laser_spawn_positions);

        for (size_t i = 0; i < size_of_spawn_list; i++) {
          //body_t *laser = make_laser_active(list_get(game_play_state->laser->laser_spawn_positions, i));
          body_t *laser = make_obstacle_rectangle(list_get(game_play_state->laser->laser_spawn_positions, i), LASER_WIDTH_ACTIVE, LASER_HEIGHT_ACTIVE, LASER_ACTIVE, VEC_ZERO);          
          scene_add_body(game_play_state->state->scene, laser);
          asset_update_bounding_box(list_get(game_play_state->laser->laser_active_assets, i), laser);
          if (!game_play_state->powerup->powerup_active || 
              game_play_state->powerup->powerup_type != SHIELD) {              
            create_collision(game_play_state->state->scene, laser, game_play_state->state->user, game_over, 
            game_play_state, NULL, DEFAULT_FORCE_CONSTANT);
          }  
        }
        for (size_t i = 0; i < size_of_spawn_list; i++) { 
          list_remove(game_play_state->laser->laser_spawn_positions, LIST_START_INDEX);
        }
      }
      break;
    }
    default: {
      break;
    }
  }
}  


void remove_lasers_inactive(game_play_state_t *game_play_state) {
  for (size_t i = 0; i < scene_bodies(game_play_state->state->scene); i++) {
    body_t *body = scene_get_body(game_play_state->state->scene, i);
    if (get_type(body) == LASER) {
      scene_remove_body(game_play_state->state->scene, i);
    }
  }
  for (size_t i = 0; i < INITIAL_LIST_CAPACITY; i++) {
    asset_update_bounding_box_x(list_get(game_play_state->laser->laser_inactive_assets, i), LASER_INITIAL_X);
  }
}

void remove_lasers(game_play_state_t *game_play_state) {
  if (game_play_state->time - game_play_state->laser->time_laser_activate >= LASER_ON_TIME) {
    game_play_state->laser->laser_active = false;
    game_play_state->laser->laser_spawn_bool = LASER_INACTIVE_STATE;                        
    game_play_state->laser->time_laser_activate = INFINITY;
    for (size_t i = 0; i < scene_bodies(game_play_state->state->scene); i++) {
      body_t *body = scene_get_body(game_play_state->state->scene, i);
      if (get_type(body) == LASER_ACTIVE) {
        scene_remove_body(game_play_state->state->scene, i);
        subject_notify(game_play_state->subject, EVENT_LASERS_AVOIDED, NULL);
      }
    }

    for (size_t i = 0; i < INITIAL_LIST_CAPACITY; i++) {
      asset_update_bounding_box_x(list_get(game_play_state->laser->laser_active_assets, i), LASER_INITIAL_X);
    }
  }
}