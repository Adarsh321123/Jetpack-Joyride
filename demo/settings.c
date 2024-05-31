#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "settings.h"
#include "asset.h"
#include "asset_helper.h"
#include "constants.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

/**
 * Handler for going back to home screen
 */
static void home(settings_state_t *settings_state);

/**
 * Handler for setting the difficulty of game to easy
 */
static void set_difficulty_easy(settings_state_t *settings_state);

/**
 * Handler for setting the difficulty of game to medium
 */
static void set_difficulty_medium(settings_state_t *settings_state);

/**
 * Handler for setting the difficulty of game to hard
 */
static void set_difficulty_hard(settings_state_t *settings_state);

static background_info_t background_templates[] = {
     {.bg_path = "assets/jetpack_joyride_home.jpg",
     .bg_box = (SDL_Rect){0, 0, 1000, 500}}
     };

static text_info_t text_templates[] = {
     {.font_path = "assets/New Athletic M54.ttf",
     .text_box = (SDL_Rect){625, 50, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Settings"},
     {.font_path = "assets/New Athletic M54.ttf",
     .text_box = (SDL_Rect){100, 325, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Current Difficulty Level: "}
     };

static button_info_t button_templates[] = {
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){100, 25, 300, 100},
     .text_box = (SDL_Rect){200, 50, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Home",
     .handler = (void *)home},
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){50, 400, 300, 100},
     .text_box = (SDL_Rect){150, 425, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Easy",
     .handler = (void *)set_difficulty_easy},
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){350, 400, 300, 100},
     .text_box = (SDL_Rect){450, 425, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Medium",
     .handler = (void *)set_difficulty_medium},
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){650, 400, 300, 100},
     .text_box = (SDL_Rect){750, 425, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Hard",
     .handler = (void *)set_difficulty_hard}
     };

static void home(settings_state_t *settings_state){
  settings_state->curr_state = HOME;
}

static void set_difficulty_easy(settings_state_t *settings_state){
  settings_state->difficulty_level = EASY;
}

static void set_difficulty_medium(settings_state_t *settings_state){
  settings_state->difficulty_level = MEDIUM;
}

static void set_difficulty_hard(settings_state_t *settings_state){
  settings_state->difficulty_level = HARD;
}

/**
 * Initializes and stores the background assets in the settings_state.
 */
static void create_backgrounds(settings_state_t *settings_state) {
  for (size_t i = 0; i < NUM_BACKGROUNDS; i++) {
    background_info_t info = background_templates[i];
    asset_t *background = create_background_from_info(info);
    list_add(settings_state->backgrounds, background);
  }
}

/**
 * Initializes and stores the text assets in the settings_state.
 */
static void create_text(settings_state_t *settings_state) {
  for (size_t i = 0; i < NUM_TEXT_SETTINGS; i++) {
    text_info_t info = text_templates[i];
    asset_t *text = create_text_from_info(info);
    list_add(settings_state->text, text);
  }
}

/**
 * Initializes and stores the button assets in the settings_state.
 */
static void create_buttons(settings_state_t *settings_state) {
  for (size_t i = 0; i < NUM_BUTTONS_SETTINGS; i++) {
    button_info_t info = button_templates[i];
    asset_t *button = create_button_from_info(info);
    list_add(settings_state->manual_buttons, button);
  }
}

static void on_mouse(char key, void *settings_state, SDL_Event event) {
  if (key == MOUSE_RIGHT || key == MOUSE_LEFT) {
    asset_cache_handle_buttons(settings_state, event.button.x, event.button.y);
  }
}

settings_state_t *settings_init() {
  settings_state_t *settings_state = malloc(sizeof(settings_state_t));
  assert(settings_state);
  sdl_on_mouse(on_mouse);
  sdl_init(MIN, MAX);
  TTF_Init();
  asset_cache_init();
  settings_state->time = 0;
  // Note that `free_func` is NULL because `asset_cache` is reponsible for
  // freeing the button assets.
  settings_state->backgrounds = list_init(NUM_BACKGROUNDS, NULL);
  create_backgrounds(settings_state);

  settings_state->text = list_init(NUM_TEXT_SETTINGS, NULL);
  create_text(settings_state);

  settings_state->manual_buttons = list_init(NUM_BUTTONS_SETTINGS, NULL);
  // We store the assets used for buttons to be freed at the end.
  settings_state->button_assets = list_init(NUM_BUTTONS_SETTINGS, (free_func_t)asset_destroy);
  create_buttons(settings_state);

  settings_state->curr_state = SETTINGS;
  settings_state->difficulty_level = EASY;
  return settings_state;
}

state_type_t settings_main(settings_state_t *settings_state) {
  sdl_clear();
  settings_state->time += time_since_last_tick();

  // render the backgrounds
  list_t *backgrounds = settings_state->backgrounds;
  for (size_t i = 0; i < NUM_BACKGROUNDS; i++){
    asset_render(list_get(backgrounds, i));
  }

  // render the text
  list_t *text = settings_state->text;
  for (size_t i = 0; i < NUM_TEXT_SETTINGS; i++){
    asset_render(list_get(text, i));
  }

  // render the buttons
  list_t *buttons = settings_state->manual_buttons;
  for (size_t i = 0; i < NUM_BUTTONS_SETTINGS; i++) {
    asset_render(list_get(buttons, i));
  }

  handle_mouse_events(settings_state);
  sdl_show();
  return settings_state->curr_state;
}

void settings_free(settings_state_t *settings_state) {
  TTF_Quit();
  list_free(settings_state->backgrounds);
  list_free(settings_state->text);
  list_free(settings_state->manual_buttons);
  list_free(settings_state->button_assets);
  asset_cache_destroy();
  free(settings_state);
}
