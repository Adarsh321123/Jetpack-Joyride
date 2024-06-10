#ifndef __SCREEN_HELPER_H__
#define __SCREEN_HELPER_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "asset.h"
#include "asset_helper.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

typedef struct screen_state {
    list_t *text;
    list_t *backgrounds;
    list_t *manual_buttons;
} screen_state_t;

/**
 * Render the backgrounds, texts, and buttons for a tick.
 */
void render_items(screen_state_t screen_state);

/**
 * On mouse handler for the screen.
 */
void on_mouse(char key, void *screen_state, SDL_Event event);

/**
 * Initialize the screen state with the backgrounds, text, and buttons.
 */
screen_state_t screen_init(screen_state_t screen_state, background_info_t background_templates[],
                            text_info_t text_templates[], button_info_t button_templates[],
                            size_t num_backgrounds, size_t num_text, size_t num_buttons);

/**
 * Free the background, text, and buttons of the screen_state.
 */
void screen_free(screen_state_t screen_state);

#endif // #ifndef __SCREEN_HELPER_H__