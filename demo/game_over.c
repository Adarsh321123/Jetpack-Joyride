#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_over.h"
#include "asset.h"
#include "constants.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

typedef struct background_info {
  const char *bg_path;
  SDL_Rect bg_box;
} background_info_t;

typedef struct text_info {
  const char *font_path;
  SDL_Rect text_box;
  rgb_color_t text_color;
  const char *text;
} text_info_t;

typedef struct button_info {
  const char *image_path;
  const char *font_path;
  SDL_Rect image_box;
  SDL_Rect text_box;
  rgb_color_t text_color;
  const char *text;
  button_handler_t handler;
} button_info_t;

/**
 * Handler for exiting the game
 */
static void exit_game(game_over_state_t *game_over_state);

/**
 * Handler for going to homescreen
 */
static void home(game_over_state_t *game_over_state);

static background_info_t background_templates[] = {
     {.bg_path = "assets/jetpack_joyride_game_over.jpg",
     .bg_box = (SDL_Rect){0, 0, 1000, 500}}
     };

static text_info_t text_templates[] = {
     {.font_path = "assets/New Athletic M54.ttf",
     .text_box = (SDL_Rect){625, 50, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Game Over..."}
     };

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

static void exit_game(game_over_state_t *game_over_state){
  game_over_state->curr_state = EXIT;
}

static void home(game_over_state_t *game_over_state){
  game_over_state->curr_state = HOME;
}

/**
 * Using `info`, initializes a background in the scene.
 *
 * @param info the background info struct used to initialize the background
 */
static asset_t *create_background_from_info(game_over_state_t *game_over_state, background_info_t info) {
  asset_t *background_asset = NULL;
  if (info.bg_path != NULL) {
    background_asset = asset_make_image(info.bg_path, info.bg_box);
  }
  return background_asset;
}

/**
 * Initializes and stores the background assets in the game_over_state.
 */
static void create_backgrounds(game_over_state_t *game_over_state) {
  for (size_t i = 0; i < NUM_BACKGROUNDS; i++) {
    background_info_t info = background_templates[i];
    asset_t *background = create_background_from_info(game_over_state, info);
    list_add(game_over_state->backgrounds, background);
  }
}

/**
 * Using `info`, initializes text in the scene.
 *
 * @param info the text info struct used to initialize the text
 */
static asset_t *create_text_from_info(game_over_state_t *game_over_state, text_info_t info) {
  asset_t *text_asset = NULL;
  if (info.font_path != NULL) {
    text_asset = asset_make_text(info.font_path, info.text_box, info.text,
                                 info.text_color);
  }
  return text_asset;
}

/**
 * Initializes and stores the text assets in the game_over_state.
 */
static void create_text(game_over_state_t *game_over_state) {
  for (size_t i = 0; i < NUM_TEXT; i++) {
    text_info_t info = text_templates[i];
    asset_t *text = create_text_from_info(game_over_state, info);
    list_add(game_over_state->text, text);
  }
}

/**
 * Using `info`, initializes a button in the scene.
 *
 * @param info the button info struct used to initialize the button
 */
static asset_t *create_button_from_info(game_over_state_t *game_over_state, button_info_t info) {
  asset_t *image_asset = NULL;
  if (info.image_path != NULL) {
    image_asset = asset_make_image(info.image_path, info.image_box);
  }
  asset_t *text_asset = NULL;
  if (info.font_path != NULL) {
    text_asset = asset_make_text(info.font_path, info.text_box, info.text,
                                 info.text_color);
  }
  asset_t *button_asset =
      asset_make_button(info.image_box, image_asset, text_asset, info.handler);
  asset_cache_register_button(button_asset);
  return button_asset;
}

/**
 * Initializes and stores the button assets in the game_over_state.
 */
static void create_buttons(game_over_state_t *game_over_state) {
  for (size_t i = 0; i < NUM_BUTTONS_GAME_OVER; i++) {
    button_info_t info = button_templates[i];
    asset_t *button = create_button_from_info(game_over_state, info);
    list_add(game_over_state->manual_buttons, button);
  }
}

static void on_mouse(char key, void *game_over_state, SDL_Event event) {
  if (key == MOUSE_RIGHT || key == MOUSE_LEFT) {
    asset_cache_handle_buttons(game_over_state, event.button.x, event.button.y);
  }
}

game_over_state_t *game_over_init() {
  game_over_state_t *game_over_state = malloc(sizeof(game_over_state_t));
  assert(game_over_state);
  sdl_on_mouse(on_mouse);
  sdl_init(MIN, MAX);
  TTF_Init();
  asset_cache_init();
  game_over_state->time = 0;
  // Note that `free_func` is NULL because `asset_cache` is reponsible for
  // freeing the button assets.
  game_over_state->backgrounds = list_init(NUM_BACKGROUNDS, NULL);
  create_backgrounds(game_over_state);

  game_over_state->text = list_init(NUM_TEXT, NULL);
  create_text(game_over_state);

  game_over_state->manual_buttons = list_init(NUM_BUTTONS_GAME_OVER, NULL);
  // We store the assets used for buttons to be freed at the end.
  game_over_state->button_assets = list_init(NUM_BUTTONS_GAME_OVER, (free_func_t)asset_destroy);
  create_buttons(game_over_state);

  game_over_state->curr_state = GAME_OVER;
  return game_over_state;
}

state_type_t game_over_main(game_over_state_t *game_over_state) {
  sdl_clear();
  game_over_state->time += time_since_last_tick();

  // render the backgrounds
  list_t *backgrounds = game_over_state->backgrounds;
  for (size_t i = 0; i < NUM_BACKGROUNDS; i++){
    asset_render(list_get(backgrounds, i));
  }

  // render the text
  list_t *text = game_over_state->text;
  for (size_t i = 0; i < NUM_TEXT; i++){
    asset_render(list_get(text, i));
  }

  // render the buttons
  list_t *buttons = game_over_state->manual_buttons;
  for (size_t i = 0; i < NUM_BUTTONS_GAME_OVER; i++) {
    asset_render(list_get(buttons, i));
  }

  handle_mouse_events(game_over_state);
  sdl_show();
  return game_over_state->curr_state;
}

void game_over_free(game_over_state_t *game_over_state) {
  TTF_Quit();
  list_free(game_over_state->backgrounds);
  list_free(game_over_state->text);
  list_free(game_over_state->manual_buttons);
  list_free(game_over_state->button_assets);
  asset_cache_destroy();
  free(game_over_state);
}
