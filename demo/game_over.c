#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_over.h"
#include "asset.h"
#include "constants.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

// const vector_t MIN = {0, 0};
// const vector_t MAX = {1000, 500};
// const vector_t CENTER = {500, 250};

// const size_t INITIAL_ASSET_CAPACITY = 10;

// const size_t TEXT_SIZE = 50;
// const size_t TEXT_HEIGHT_SCALE = 2;
// const size_t NUM_BACKGROUNDS = 1;
// const size_t NUM_BUTTONS = 1;

// const size_t DEFAULT_WIDTH = 0;
// const size_t DEFAULT_HEIGHT = 0;

// const double INTERVAL = 1;

typedef struct background_info {
  const char *bg_path;
  vector_t bg_loc;
  vector_t bg_size;
} background_info_t;

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
 * Handler for entering game play
 */
static void home(game_over_state_t *game_over_state);

static background_info_t background_templates[] = {
     {.bg_path = "assets/jetpack_joyride_game_over.jpg",
     .bg_loc = (vector_t){0, 0},
     .bg_size = (vector_t){1000, 500}}
     };

static button_info_t button_templates[] = {
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){350, 300, 300, 100},
     .text_box = (SDL_Rect){450, 325, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Play Again",
     .handler = (void *)home}
     };

static void home(game_over_state_t *game_over_state){
  game_over_state->curr_state = HOME;
}

/**
 * Using `info`, initializes a background in the scene.
 *
 * @param info the background info struct used to initialize the background
 */
static asset_t *create_background_from_info(game_over_state_t *game_over_state, background_info_t info) {
  SDL_Rect bounding_box =
        make_texr(info.bg_loc.x, info.bg_loc.y,
                  info.bg_size.x, info.bg_size.y);
  asset_t *background_asset = asset_make_image(info.bg_path, bounding_box);
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
  for (size_t i = 0; i < NUM_BUTTONS; i++) {
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

  game_over_state->manual_buttons = list_init(NUM_BUTTONS, NULL);
  // We store the assets used for buttons to be freed at the end.
  game_over_state->button_assets = list_init(NUM_BUTTONS, (free_func_t)asset_destroy);
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

  // render the "play" button
  list_t *buttons = game_over_state->manual_buttons;
  for (size_t i = 0; i < NUM_BUTTONS; i++) {
    asset_render(list_get(buttons, i));
  }

  handle_mouse_events(game_over_state);
  sdl_show();
  return game_over_state->curr_state;
}

void game_over_free(game_over_state_t *game_over_state) {
  TTF_Quit();
  list_free(game_over_state->backgrounds);
  list_free(game_over_state->manual_buttons);
  list_free(game_over_state->button_assets);
  asset_cache_destroy();
  free(game_over_state);
}
