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
const size_t NUM_BUTTONS = 4;

const size_t DEFAULT_WIDTH = 0;
const size_t DEFAULT_HEIGHT = 0;

const double INTERVAL = 1;

struct state {
  list_t *memes;
  double time;
  size_t meme;
  list_t *manual_buttons;
  asset_t *play_button;
  asset_t *pause_button;
  list_t *button_assets;
  bool paused;
};

typedef struct meme {
  size_t num_captions;
  char **captions;
  vector_t *locations;
  const char *font_path;
  const char *bg_path;
  vector_t bg_loc;
  vector_t bg_size;
} meme_t;

typedef struct button_info {
  const char *image_path;
  const char *font_path;
  SDL_Rect image_box;
  SDL_Rect text_box;
  rgb_color_t text_color;
  const char *text;
  button_handler_t handler;
} button_info_t;

void next_meme(state_t *state);

void previous_meme(state_t *state);

void toggle_play(state_t *state);

meme_t meme_templates[] = {
    {.num_captions = 3,
     .captions = (char *[]){"MEMORY LEAKS", "ASAN", "CS3 STUDENTS"},
     .locations = (vector_t[]){(vector_t){270, 360}, (vector_t){545, 250},
                               (vector_t){585, 330}},
     .font_path = "assets/impact.ttf",
     .bg_path = "assets/asan_meme.png",
     .bg_loc = (vector_t){170, 0},
     .bg_size = (vector_t){660, 440}},
    {
        .font_path = "assets/Cascadia.ttf",
        .bg_path = "assets/code_quality_meme.png",
        .num_captions = 6,
        .bg_loc = (vector_t){250, 0},
        .bg_size = (vector_t){500, 500},
        .captions =
            (char *[]){"if flag == true {", "    return true;", "} else {",
                       "    return false;", "}", "return flag;"},
        .locations = (vector_t[]){(vector_t){530, 30}, (vector_t){530, 70},
                                  (vector_t){530, 110}, (vector_t){530, 150},
                                  (vector_t){530, 190}, (vector_t){530, 350}},
    },
    {
        .font_path = "assets/impact.ttf",
        .bg_path = "assets/malloc_meme.png",
        .num_captions = 2,
        .bg_loc = (vector_t){250, 0},
        .bg_size = (vector_t){500, 500},
        .captions =
            (char *[]){"FOR ALL MALLOC I SEE", "A FREE THERE SHOULD BE"},
        .locations = (vector_t[]){(vector_t){300, 50}, (vector_t){300, 450}},
    }};

button_info_t button_templates[] = {
    {.image_path = "assets/black_circle.png",
     .font_path = "assets/Roboto-Regular.ttf",
     .image_box = (SDL_Rect){0, 400, 100, 100},
     .text_box = (SDL_Rect){25, 425, 50, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Back",
     .handler = (void *)previous_meme},
    {.image_path = "assets/black_circle.png",
     .font_path = "assets/Roboto-Regular.ttf",
     .image_box = (SDL_Rect){900, 400, 100, 100},
     .text_box = (SDL_Rect){925, 425, 50, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Next",
     .handler = (void *)next_meme},
    {.image_path = "assets/play_button.png",
     .image_box = (SDL_Rect){0, 200, 100, 100},
     .handler = (void *)toggle_play},
    {.image_path = "assets/pause_button.png",
     .image_box = (SDL_Rect){0, 200, 100, 100},
     .handler = (void *)toggle_play}};

/**
 * Returns a 2-D array of assets that are grouped by the meme they belong to. In
 * other words, the `i`th index of the list should be a list of assets
 * corresponding to the `i`th meme.
 *
 * @param state the state of the game
 * @return a 2-D list of assets to be rendered
 */
list_t *generate_memes(state_t *state) {
  list_t *memes = list_init(INITIAL_MEME_CAPACITY, (free_func_t)list_free);

  for (size_t i = 0; i < NUM_MEMES; i++) {
    list_t *assets =
        list_init(INITIAL_ASSET_CAPACITY, (free_func_t)asset_destroy);
    meme_t *curr_meme = &meme_templates[i];

    SDL_Rect bounding_box =
        make_texr(curr_meme->bg_loc.x, curr_meme->bg_loc.y,
                  curr_meme->bg_size.x, curr_meme->bg_size.y);
    asset_t *image = asset_make_image(curr_meme->bg_path, bounding_box);
    list_add(assets, image);

    for (size_t j = 0; j < curr_meme->num_captions; j++) {
      SDL_Rect texr =
          make_texr(curr_meme->locations[j].x, curr_meme->locations[j].y,
                    DEFAULT_WIDTH, DEFAULT_HEIGHT);
      asset_t *text =
          asset_make_text(curr_meme->font_path, texr, curr_meme->captions[j],
                          *color_init(BLUE.r, BLUE.g, BLUE.b));
      list_add(assets, text);
    }
    list_add(memes, assets);
  }
  return memes;
}

void next_meme(state_t *state) {
  state->meme += 1;
  state->meme = state->meme % list_size(state->memes);
  state->paused = true;
}

void previous_meme(state_t *state) {
  // We can't just decrement by 1 in case state->meme is 0. Why won't this work?
  if (state->meme == 0) {
    state->meme = list_size(state->memes) - 1;
  } else {
    state->meme -= 1;
  }
  state->paused = true;
}

/**
 * Plays/pauses the memes depending on the curret setting.
 */
void toggle_play(state_t *state) {
  state->paused = !state->paused;
  state->time = 0;
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
    if (info.font_path == NULL) {
      if (strcmp(info.image_path, "assets/play_button.png") == 0) {
        state->play_button = button;
      } else {
        state->pause_button = button;
      }
    } else {
      list_add(state->manual_buttons, button);
    }
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
  state->memes = generate_memes(state);
  state->meme = 0;
  state->paused = true;
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

  // render the memes
  list_t *memes = list_get(state->memes, state->meme);
  for (size_t i = 0; i < list_size(memes); i++) {
    asset_render(list_get(memes, i));
  }

  // render the "next" and "back" buttons
  list_t *buttons = state->manual_buttons;
  for (size_t i = 0; i < list_size(buttons); i++) {
    asset_render(list_get(buttons, i));
  }

  // render the play/pause buttons
  if (state->paused) {
    asset_render(state->play_button);
  } else {
    asset_render(state->pause_button);
  }

  // rotate between the memes if playing
  if (!state->paused && state->time > INTERVAL) {
    state->time -= INTERVAL;
    state->meme = (state->meme + 1) % list_size(state->memes);
  }

  handle_mouse_events(state);
  sdl_show();
  return false;
}

void emscripten_free(state_t *state) {
  TTF_Quit();
  list_free(state->memes);
  list_free(state->manual_buttons);
  list_free(state->button_assets);
  asset_cache_destroy();
  free(state);
}
