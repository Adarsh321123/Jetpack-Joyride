#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_over.h"
#include "asset.h"
#include "asset_helper.h"
#include "constants.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

/**
 * Handler for exiting the game
 */
static void exit_game(game_over_state_t *game_over_state)
{
  game_over_state->curr_state = EXIT;
}

/**
 * Handler for going to homescreen
 */
static void home(game_over_state_t *game_over_state)
{
  game_over_state->curr_state = HOME;
}

static background_info_t background_templates[] = {
    {.bg_path = "assets/jetpack_joyride_game_over.jpg",
     .bg_box = (SDL_Rect){0, 0, 1000, 500}}};

static text_info_t text_templates[] = {
    {.font_path = "assets/New Athletic M54.ttf",
     .text_box = (SDL_Rect){625, 50, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Game Over..."}};

static button_info_t button_templates[] = {
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){200, 300, 300, 100},
     .text_box = (SDL_Rect){300, 325, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Exit",
     .handler = (void *)exit_game},
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){500, 300, 300, 100},
     .text_box = (SDL_Rect){525, 325, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Play Again",
     .handler = (void *)home}};

game_over_state_t *game_over_init()
{
  game_over_state_t *game_over_state = malloc(sizeof(game_over_state_t));
  assert(game_over_state);
  TTF_Init();
  sdl_on_mouse(on_mouse);
  sdl_init(MIN, MAX);
  asset_cache_init();
  size_t num_backgrounds = sizeof(background_templates) / 
                          sizeof(background_templates[0]);
  size_t num_text = sizeof(text_templates) / sizeof(text_templates[0]);
  size_t num_buttons = sizeof(button_templates) / sizeof(button_templates[0]);
  game_over_state->screen_state = screen_init(game_over_state->screen_state,
                                              background_templates,
                                              text_templates, button_templates,
                                              num_backgrounds, num_text, num_buttons);
  game_over_state->time = 0;
  game_over_state->curr_state = GAME_OVER;
  return game_over_state;
}

state_type_t game_over_main(game_over_state_t *game_over_state)
{
  sdl_clear();
  game_over_state->time += time_since_last_tick();
  render_items(game_over_state->screen_state);
  handle_mouse_events(game_over_state);
  sdl_show();
  return game_over_state->curr_state;
}

void game_over_free(game_over_state_t *game_over_state)
{
  screen_free(game_over_state->screen_state);
  asset_cache_destroy();
  free(game_over_state);
  TTF_Quit();
}
