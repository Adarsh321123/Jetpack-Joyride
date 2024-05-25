#ifndef __ASSET_CACHE_H__
#define __ASSET_CACHE_H__

#include "asset.h"
#include <stddef.h>

/**
 * Initializes the empty, list-based global asset cache. The caller must then
 * destroy the cache with `asset_cache_destroy` when done.
 */
void asset_cache_init();

/**
 * Frees the global asset cache and its owned contents.
 */
void asset_cache_destroy();

/**
 * Gets the pointer to the object that is associated with the given filepath.
 * If the object exists, asserts that its type matches the given type.
 *
 * If the object doesn't exist, adds a new entry to the asset cache and returns
 * the pointer to the newly created object.
 *
 * Example:
 * ```
 * char *img_path = "assets/image.png";
 * SDL_Texture *obj = asset_cache_obj_get_or_create(ASSET_IMAGE, img_path);
 *
 * char *font_path = "assets/font.ttf";
 * TTF_Font *obj = asset_cache_obj_get_or_create(ASSET_FONT, font_path);
 * ```
 *
 * @param ty the type of the asset
 * @param filepath the filepath to the asset
 * @return the object that corresponds to the filepath, as a void*
 */
void *asset_cache_obj_get_or_create(asset_type_t ty, const char *filepath);

/**
 * Registers the button to the asset cache, effectively activating its button
 * handler. When this function is called, the asset_cache takes ownership of the
 * button so the caller does NOT have to free it.
 *
 * Asserts that the type of `button` is ASSET_BUTTON.
 *
 * @param button pointer to the button asset
 */
void asset_cache_register_button(asset_t *button);

/**
 * Runs `asset_on_button_click` on all the buttons stored in the asset cache.
 *
 * @param state the game state
 * @param x the x position of the mouse click
 * @param y the y position of the mouse click
 */
void asset_cache_handle_buttons(state_t *state, double x, double y);

#endif // #ifndef __ASSET_CACHE_H__
