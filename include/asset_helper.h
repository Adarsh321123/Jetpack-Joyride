#ifndef __ASSET_HELPER_H__
#define __ASSET_HELPER_H__

#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "asset.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

typedef struct background_info
{
  const char *bg_path;
  SDL_Rect bg_box;
} background_info_t;

typedef struct text_info
{
  const char *font_path;
  SDL_Rect text_box;
  rgb_color_t text_color;
  const char *text;
} text_info_t;

typedef struct button_info
{
  const char *image_path;
  const char *font_path;
  SDL_Rect image_box;
  SDL_Rect text_box;
  rgb_color_t text_color;
  const char *text;
  button_handler_t handler;
} button_info_t;

/**
 * Using `info`, initializes a background in the scene.

 * @param info the background info struct used to initialize the background
 * @return a pointer to the newly created background asset
 */
asset_t *create_background_from_info(background_info_t info);

/**
 * Using `info`, initializes text in the scene.
 *
 * @param info the text info struct used to initialize the text
 * @return a pointer to the newly created text asset
 */
asset_t *create_text_from_info(text_info_t info);

/**
 * Using `info`, initializes a button in the scene.
 *
 * @param info the button info struct used to initialize the button
 * @return a pointer to the newly created button asset
 */
asset_t *create_button_from_info(button_info_t info);

#endif // #ifndef __ASSET_HELPER_H__