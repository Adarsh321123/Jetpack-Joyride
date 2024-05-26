#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

const rgb_color_t BLUE = (rgb_color_t){0, 0, 255};

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

const size_t NUM_MEMES = 3;
const size_t INITIAL_MEME_CAPACITY = 10;
const size_t INITIAL_ASSET_CAPACITY = 10;

const size_t TEXT_SIZE = 18;
const size_t TEXT_HEIGHT_SCALE = 2;
const size_t NUM_BUTTONS = 2;

const size_t DEFAULT_WIDTH = 0;
const size_t DEFAULT_HEIGHT = 0;

const double INTERVAL = 1;

struct state {
  double time;
  list_t *manual_buttons;
  list_t *button_assets;
};

typedef struct button_info {
  const char *image_path;
  const char *font_path;
  SDL_Rect image_box;
  SDL_Rect text_box;
  rgb_color_t text_color;
  const char *text;
  button_handler_t handler;
} button_info_t;

void play(state_t *state);

button_info_t button_templates[] = {
    {.image_path = "assets/home_button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){0, 400, 100, 100},
     .text_box = (SDL_Rect){25, 425, 50, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Play",
     .handler = (void *)play},
    {.image_path = "assets/home_button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){700, 300, 300, 100},
     .text_box = (SDL_Rect){725, 325, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Play",
     .handler = (void *)play}};


void play(state_t *state){
  return;
}

/**
 * Using `info`, initializes a button in the scene.
 *
 * @param info the button info struct used to initialize the button
 */
asset_t *create_button_from_info(state_t *state, button_info_t info) {
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
 * Initializes and stores the button assets in the state.
 */
void create_buttons(state_t *state) {
  for (size_t i = 0; i < NUM_BUTTONS; i++) {
    button_info_t info = button_templates[i];
    asset_t *button = create_button_from_info(state, info);
    list_add(state->manual_buttons, button);
  }
}

static void on_mouse(char key, void *state, SDL_Event event) {
  if (key == MOUSE_RIGHT || key == MOUSE_LEFT) {
    asset_cache_handle_buttons(state, event.button.x, event.button.y);
  }
}

state_t *emscripten_init() {
  state_t *state = malloc(sizeof(state_t));
  assert(state);
  sdl_on_mouse(on_mouse);
  sdl_init(MIN, MAX);
  TTF_Init();
  asset_cache_init();
  state->time = 0;
  // Note that `free_func` is NULL because `asset_cache` is reponsible for
  // freeing the button assets.
  state->manual_buttons = list_init(NUM_BUTTONS, NULL);
  // We store the assets used for buttons to be freed at the end.
  state->button_assets = list_init(NUM_BUTTONS, (free_func_t)asset_destroy);
  create_buttons(state);
  return state;
}

bool emscripten_main(state_t *state) {
  sdl_clear();
  state->time += time_since_last_tick();

  // render the "next" and "back" buttons
  list_t *buttons = state->manual_buttons;
  for (size_t i = 0; i < list_size(buttons); i++) {
    asset_render(list_get(buttons, i));
  }

  handle_mouse_events(state);
  sdl_show();
  return false;
}

void emscripten_free(state_t *state) {
  TTF_Quit();
  list_free(state->manual_buttons);
  list_free(state->button_assets);
  asset_cache_destroy();
  free(state);
}
