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
 * Handler for entering game play
 */
static void play(home_state_t *home_state);

static background_info_t background_templates[] = {
     {.bg_path = "assets/jetpack_joyride_home.jpg",
     .bg_box = (SDL_Rect){0, 0, 1000, 500}}
     };

static text_info_t text_templates[] = {
     {.font_path = "assets/New Athletic M54.ttf",
     .text_box = (SDL_Rect){625, 50, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Jetpack Joyride"}
     };

static button_info_t button_templates[] = {
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){350, 300, 300, 100},
     .text_box = (SDL_Rect){450, 325, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Play",
     .handler = (void *)play}
     };

static void play(home_state_t *home_state){
  home_state->curr_state = GAME_PLAY;
}

/**
 * Initializes and stores the background assets in the home_state.
 */
static void create_backgrounds(home_state_t *home_state) {
  for (size_t i = 0; i < NUM_BACKGROUNDS; i++) {
    background_info_t info = background_templates[i];
    asset_t *background = create_background_from_info(info);
    list_add(home_state->backgrounds, background);
  }
}

/**
 * Initializes and stores the text assets in the home_state.
 */
static void create_text(home_state_t *home_state) {
  for (size_t i = 0; i < NUM_TEXT; i++) {
    text_info_t info = text_templates[i];
    asset_t *text = create_text_from_info(info);
    list_add(home_state->text, text);
  }
}

/**
 * Initializes and stores the button assets in the home_state.
 */
static void create_buttons(home_state_t *home_state) {
  for (size_t i = 0; i < NUM_BUTTONS_HOME; i++) {
    button_info_t info = button_templates[i];
    asset_t *button = create_button_from_info(info);
    list_add(home_state->manual_buttons, button);
  }
}

static void on_mouse(char key, void *home_state, SDL_Event event) {
  if (key == MOUSE_RIGHT || key == MOUSE_LEFT) {
    asset_cache_handle_buttons(home_state, event.button.x, event.button.y);
  }
}

home_state_t *home_init() {
  home_state_t *home_state = malloc(sizeof(home_state_t));
  assert(home_state);
  sdl_on_mouse(on_mouse);
  sdl_init(MIN, MAX);
  TTF_Init();
  asset_cache_init();
  home_state->time = 0;
  // Note that `free_func` is NULL because `asset_cache` is reponsible for
  // freeing the button assets.
  home_state->backgrounds = list_init(NUM_BACKGROUNDS, NULL);
  create_backgrounds(home_state);

  home_state->text = list_init(NUM_TEXT, NULL);
  create_text(home_state);

  home_state->manual_buttons = list_init(NUM_BUTTONS_HOME, NULL);
  // We store the assets used for buttons to be freed at the end.
  home_state->button_assets = list_init(NUM_BUTTONS_HOME, (free_func_t)asset_destroy);
  create_buttons(home_state);

  home_state->curr_state = HOME;
  return home_state;
}

state_type_t home_main(home_state_t *home_state) {
  sdl_clear();
  home_state->time += time_since_last_tick();

  // render the backgrounds
  list_t *backgrounds = home_state->backgrounds;
  for (size_t i = 0; i < NUM_BACKGROUNDS; i++){
    asset_render(list_get(backgrounds, i));
  }

  // render the text
  list_t *text = home_state->text;
  for (size_t i = 0; i < NUM_TEXT; i++){
    asset_render(list_get(text, i));
  }

  // render the buttons
  list_t *buttons = home_state->manual_buttons;
  for (size_t i = 0; i < NUM_BUTTONS_HOME; i++) {
    asset_render(list_get(buttons, i));
  }

  handle_mouse_events(home_state);
  sdl_show();
  return home_state->curr_state;
}

void home_free(home_state_t *home_state) {
  TTF_Quit();
  list_free(home_state->backgrounds);
  list_free(home_state->text);
  list_free(home_state->manual_buttons);
  list_free(home_state->button_assets);
  asset_cache_destroy();
  free(home_state);
}
