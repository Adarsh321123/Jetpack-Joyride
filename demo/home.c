#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "home.h"
#include "asset.h"
#include "asset_helper.h"
#include "constants.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

/**
 * Handler for entering settings page
 */
static void settings(home_state_t *home_state)
{
  home_state->curr_state = SETTINGS;
}

/**
 * Handler for entering game play
 */
static void play(home_state_t *home_state)
{
  home_state->curr_state = GAME_PLAY;
}

static background_info_t background_templates[] = {
    {.bg_path = "assets/jetpack_joyride_home.jpg",
     .bg_box = (SDL_Rect){0, 0, 1000, 500}}};

static text_info_t text_templates[] = {
    {.font_path = "assets/New Athletic M54.ttf",
     .text_box = (SDL_Rect){625, 50, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Jetpack Joyride"}};

static button_info_t button_templates[] = {
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){150, 300, 300, 100},
     .text_box = (SDL_Rect){200, 325, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Settings",
     .handler = (void *)settings},
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){450, 300, 300, 100},
     .text_box = (SDL_Rect){550, 325, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Play",
     .handler = (void *)play}};

home_state_t *home_init()
{
  home_state_t *home_state = malloc(sizeof(home_state_t));
  assert(home_state);
  TTF_Init();
  sdl_on_mouse(on_mouse);
  sdl_init(MIN, MAX);
  asset_cache_init();
  size_t num_backgrounds = sizeof(background_templates) / sizeof(background_templates[0]);
  size_t num_text = sizeof(text_templates) / sizeof(text_templates[0]);
  size_t num_buttons = sizeof(button_templates) / sizeof(button_templates[0]);
  home_state->screen_state = screen_init(home_state->screen_state, background_templates,
                                         text_templates, button_templates,
                                         num_backgrounds, num_text, num_buttons);
  home_state->time = ZERO;
  home_state->curr_state = HOME;
  return home_state;
}
// TODO: if identical to game over can we furhter simplify
state_type_t home_main(home_state_t *home_state)
{
  sdl_clear();
  home_state->time += time_since_last_tick();
  render_items(home_state->screen_state);
  handle_mouse_events(home_state);
  sdl_show();
  return home_state->curr_state;
}

void home_free(home_state_t *home_state)
{
  screen_free(home_state->screen_state);
  asset_cache_destroy();
  free(home_state);
  TTF_Quit();
}
