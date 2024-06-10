#include "screen_helper.h"
#include "constants.h"

/**
 * Initializes and stores the background assets in the screen_state.
 */
static void create_backgrounds(screen_state_t screen_state, background_info_t background_templates[], size_t num_backgrounds) {
  for (size_t i = 0; i < num_backgrounds; i++) {
    background_info_t info = background_templates[i];
    asset_t *background = create_background_from_info(info);
    list_add(screen_state.backgrounds, background);
  }
}

/**
 * Initializes and stores the text assets in the screen_state.
 */
static void create_text(screen_state_t screen_state, text_info_t text_templates[], size_t num_text) {
  for (size_t i = 0; i < num_text; i++) {
    text_info_t info = text_templates[i];
    asset_t *text = create_text_from_info(info);
    list_add(screen_state.text, text);
  }
}

/**
 * Initializes and stores the button assets in the screen_state.
 */
static void create_buttons(screen_state_t screen_state, button_info_t button_templates[], size_t num_buttons) {
  for (size_t i = 0; i < num_buttons; i++) {
    button_info_t info = button_templates[i];
    asset_t *button = create_button_from_info(info);
    list_add(screen_state.manual_buttons, button);
  }
}

void render_items(screen_state_t screen_state) {
    list_t *backgrounds = screen_state.backgrounds;
    size_t num_backgrounds = list_size(backgrounds);
    for (size_t i = 0; i < num_backgrounds; i++) {
        asset_render(list_get(backgrounds, i));
    }

    list_t *text = screen_state.text;
    size_t num_text = list_size(text);
    for (size_t i = 0; i < num_text; i++) {
        asset_render(list_get(text, i));
    }

    list_t *buttons = screen_state.manual_buttons;
    size_t num_buttons = list_size(buttons);
    for (size_t i = 0; i < num_buttons; i++) {
        asset_render(list_get(buttons, i));
    }
}

void on_mouse(char key, void *screen_state, SDL_Event event) {
  if (key == MOUSE_RIGHT || key == MOUSE_LEFT) {
    asset_cache_handle_buttons(screen_state, event.button.x, event.button.y);
  }
}

screen_state_t screen_init(screen_state_t screen_state, background_info_t background_templates[],
                            text_info_t text_templates[], button_info_t button_templates[],
                            size_t num_backgrounds, size_t num_text, size_t num_buttons) {
  screen_state.backgrounds = list_init(INITIAL_ASSET_CAPACITY, (free_func_t)asset_destroy);
  create_backgrounds(screen_state, background_templates, num_backgrounds);
  screen_state.text = list_init(INITIAL_ASSET_CAPACITY, (free_func_t)asset_destroy);
  create_text(screen_state, text_templates, num_text);
  screen_state.manual_buttons = list_init(INITIAL_ASSET_CAPACITY, NULL);
  create_buttons(screen_state, button_templates, num_buttons);
  return screen_state;
}

void screen_free(screen_state_t screen_state) {
    list_free(screen_state.backgrounds);
    list_free(screen_state.text);
    list_free(screen_state.manual_buttons);
}