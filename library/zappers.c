#include "zappers.h"
#include "laser.h"

void add_zapper(game_play_state_t *game_play_state, double dt) {
  game_play_state->zapper->zapper_time += dt;
  bool laser_state = !game_play_state->laser->laser_active;
  if (laser_state && game_play_state->zapper->zapper_time >= game_play_state->zapper->time_until_zapper) {
    game_play_state->zapper->zapper_time = ZERO;
    game_play_state->zapper->time_until_zapper = fmod(rand(), 
    game_play_state->zapper->max_zapper_generation_time - 
    game_play_state->zapper->min_zapper_generation_time) + 
    game_play_state->zapper->min_zapper_generation_time;
    double y_pos = fmod(rand(), (MAX.y - ZAPPER_CEILING_OFFSET) - (MIN.y + ZAPPER_GROUND_OFFSET)) + ZAPPER_Y_POS_BUFFER;
    double x_pos = MAX.x + ZAPPER_X_POS_BUFFER;
    vector_t center = {.x = x_pos, .y = y_pos};
    // body_t *zapper = make_zapper(center, ZAPPER_WIDTH, ZAPPER_HEIGHT);
    body_t *zapper = make_obstacle_rectangle(&center, ZAPPER_WIDTH, ZAPPER_HEIGHT, ZAPPER, ZAPPER_VEL);
    scene_add_body(game_play_state->state->scene, zapper);
    asset_t *img = asset_make_image_with_body(ZAPPER_PATH, zapper);
    list_add(game_play_state->state->body_assets, img);
    if (!game_play_state->powerup->powerup_active || 
        game_play_state->powerup->powerup_type != SHIELD) {
          create_collision(game_play_state->state->scene, zapper, game_play_state->state->user, 
            game_over, game_play_state, NULL, DEFAULT_FORCE_CONSTANT);
        }
  }
}