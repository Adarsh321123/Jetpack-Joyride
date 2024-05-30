#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset.h"
#include "constants.h"
#include "asset_cache.h"
#include "color.h"
#include "sdl_wrapper.h"

struct text_asset {
  asset_t base;
  TTF_Font *font;
  const char *text;
  rgb_color_t color;
};

struct image_asset {
  asset_t base;
  SDL_Texture *texture;
  body_t *body;
};

struct button_asset {
  asset_t base;
  image_asset_t *image_asset;
  text_asset_t *text_asset;
  button_handler_t handler;
  bool is_rendered;
};

/**
 * Allocates memory for an asset with the given parameters.
 *
 * @param ty the type of the asset
 * @param bounding_box the bounding box containing the location and dimensions
 * of the asset when it is rendered
 * @return a pointer to the newly allocated asset
 */
static asset_t *asset_init(asset_type_t ty, SDL_Rect bounding_box) {
  asset_t *new;
  switch (ty) {
  case ASSET_IMAGE: {
    new = malloc(sizeof(image_asset_t));
    break;
  }
  case ASSET_FONT: {
    new = malloc(sizeof(text_asset_t));
    break;
  }
  case ASSET_BUTTON: {
    new = malloc(sizeof(button_asset_t));
    break;
  }
  default: {
    assert(false && "Unknown asset type");
  }
  }
  assert(new);
  new->type = ty;
  new->bounding_box = bounding_box;
  return new;
}

asset_type_t asset_get_type(asset_t *asset) { return asset->type; }

body_t *image_asset_get_body(image_asset_t *image_asset) { return image_asset->body; }

/**
 * Encapsulates the duplicate code used by both the functions
 *
 * @param filepath file path to the image
 * @param bounding_box the bounding box containing the location and dimensions
 * of the asset when it is rendered
 * @param body body to render it on
 * @return a pointer to the newly allocated image_asset_t
 */
static image_asset_t *asset_encapsulate_image(const char *filepath,
                                              SDL_Rect bounding_box,
                                              body_t *body) {
  image_asset_t *image_asset = malloc(sizeof(image_asset_t));
  assert(image_asset != NULL);
  image_asset->texture = asset_cache_obj_get_or_create(ASSET_IMAGE, filepath);
  image_asset->base = *asset_init(ASSET_IMAGE, bounding_box);
  image_asset->body = body;
  return image_asset;
}

asset_t *asset_make_image(const char *filepath, SDL_Rect bounding_box) {
  image_asset_t *image_asset =
      asset_encapsulate_image(filepath, bounding_box, NULL);
  return (asset_t *)image_asset;
}

asset_t *asset_make_image_with_body(const char *filepath, body_t *body) {
  SDL_Rect bounding_box = make_texr(0, 0, 0, 0);
  image_asset_t *image_asset =
      asset_encapsulate_image(filepath, bounding_box, body);
  return (asset_t *)image_asset;
}

asset_t *asset_make_text(const char *filepath, SDL_Rect bounding_box,
                         const char *text, rgb_color_t color) {
  text_asset_t *text_asset = malloc(sizeof(text_asset_t));
  assert(text_asset != NULL);
  text_asset->font = asset_cache_obj_get_or_create(ASSET_FONT, filepath);
  TTF_SizeText(text_asset->font, text, &bounding_box.w, &bounding_box.h);
  text_asset->base = *asset_init(ASSET_FONT, bounding_box);
  text_asset->text = text;
  text_asset->color = color;
  return (asset_t *)text_asset;
}

asset_t *asset_make_button(SDL_Rect bounding_box, asset_t *image_asset,
                           asset_t *text_asset, button_handler_t handler) {
  button_asset_t *button_asset = malloc(sizeof(button_asset_t));
  assert(button_asset != NULL);
  assert(image_asset == NULL || image_asset->type == ASSET_IMAGE);
  assert(text_asset == NULL || text_asset->type == ASSET_FONT);
  button_asset->base = *asset_init(ASSET_BUTTON, bounding_box);
  button_asset->handler = handler;
  button_asset->image_asset = (image_asset_t *)image_asset;
  button_asset->text_asset = (text_asset_t *)text_asset;
  button_asset->is_rendered = false;
  return (asset_t *)button_asset;
}

/**
 * Checks whether the mouse click is within the bounding
 * box of the given button.
 */
static bool in_bounding_box(asset_t *button, double x, double y) {
  // x increases to the right and y increases to the bottom
  int left = button->bounding_box.x;
  int right = left + button->bounding_box.w;
  int top = button->bounding_box.y;
  int bottom = top + button->bounding_box.h;
  return (x >= left && x <= right && y <= bottom && y >= top);
}

void asset_on_button_click(asset_t *button, state_t *state, double x,
                           double y) {
  button_asset_t *button_asset = (button_asset_t *)button;
  if (!button_asset->is_rendered) {
    return;
  }
  button_asset->is_rendered = false;
  if (in_bounding_box(button, x, y)) {
    button_asset->handler(state);
  }
}

void asset_render(asset_t *asset) {
  switch (asset->type) {
  case ASSET_IMAGE: {
    image_asset_t *image_asset = (image_asset_t *)asset;
    if (image_asset->body != NULL) {
      image_asset->base.bounding_box = find_bounding_box(image_asset->body);
    }
    render_copy(image_asset->texture, image_asset->base.bounding_box);
    break;
  }
  case ASSET_FONT: {
    text_asset_t *text_asset = (text_asset_t *)asset;
    render_text(text_asset->text, text_asset->font, text_asset->color,
                text_asset->base.bounding_box);
    break;
  }
  case ASSET_BUTTON: {
    button_asset_t *button_asset = (button_asset_t *)asset;
    if (button_asset->image_asset != NULL) {
      asset_render((asset_t *)button_asset->image_asset);
    }
    if (button_asset->text_asset != NULL) {
      asset_render((asset_t *)button_asset->text_asset);
    }
    button_asset->is_rendered = true;
    break;
  }
  default: {
    assert(false && "Asset type not image nor font");
    break;
  }
  }
}

void asset_remove_image(body_t *body, list_t *body_assets, size_t num_assets) {
  for (size_t i = 0; i < num_assets; i++) {
    image_asset_t *cur_img = (image_asset_t *)list_get(body_assets, i);
    if ((image_asset_get_body(cur_img)) == body) {
      list_t *asset_cache = get_asset_cache();
      size_t asset_cache_size = list_size(asset_cache);
      for (size_t j = 0; j < asset_cache_size; j++) {
        entry_t *cur_entry = get_entry(j);
        void *obj = get_entry_obj(cur_entry);
        if (cur_img == obj) {
          list_remove(asset_cache, i);
          asset_destroy((asset_t *)cur_img);
          return;
        }
      }
    }
  }
}

void asset_destroy(asset_t *asset) { free(asset); }
