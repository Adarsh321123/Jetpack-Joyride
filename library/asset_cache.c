#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset.h"
#include "constants.h"
#include "asset_cache.h"
#include "list.h"
#include "sdl_wrapper.h"

static list_t *ASSET_CACHE;

struct entry {
  asset_type_t type;
  const char *filepath;
  void *obj;
};

list_t *get_asset_cache() { return ASSET_CACHE; }

entry_t *get_entry(size_t i) { return list_get(ASSET_CACHE, i); }

void *get_entry_obj(entry_t *entry) { return entry->obj; }

static void asset_cache_free_entry(entry_t *entry) {
  switch (entry->type) {
  case ASSET_IMAGE: {
    SDL_DestroyTexture(entry->obj);
    break;
  }
  case ASSET_FONT: {
    TTF_CloseFont(entry->obj);
    break;
  }
  case ASSET_BUTTON: {
    asset_destroy(entry->obj);
    break;
  }
  default: {
    assert(false && "Asset type not image nor font nor button");
    break;
  }
  }
  free(entry);
}

void asset_cache_init() {
  ASSET_CACHE =
      list_init(INITIAL_CAPACITY, (free_func_t)asset_cache_free_entry);
}

void asset_cache_destroy() { list_free(ASSET_CACHE); }

void *asset_cache_find_obj(asset_type_t ty, const char *filepath) {
  size_t size = list_size(ASSET_CACHE);
  for (size_t i = 0; i < size; i++) {
    entry_t *entry = list_get(ASSET_CACHE, i);
    if (entry->filepath != NULL && filepath != NULL &&
        strcmp(entry->filepath, filepath) == 0) {
      assert(entry->type == ty);
      return entry->obj;
    }
  }
  return NULL;
}

void *asset_cache_obj_get_or_create(asset_type_t ty, const char *filepath) {
  void *asset = asset_cache_find_obj(ty, filepath);
  if (asset != NULL) {
    return asset;
  }
  entry_t *new_entry = malloc(sizeof(entry_t));
  assert(new_entry != NULL);
  new_entry->type = ty;
  new_entry->filepath = filepath;

  switch (ty) {
  case ASSET_IMAGE: {
    new_entry->obj = malloc(sizeof(SDL_Texture *));
    assert(new_entry->obj != NULL);
    new_entry->obj = get_texture(filepath);
    break;
  }
  case ASSET_FONT: {
    new_entry->obj = init_font(filepath, FONT_SIZE);
    break;
  }
  default: {
    assert(false && "Asset type not image nor font");
    break;
  }
  }
  list_add(ASSET_CACHE, new_entry);
  return new_entry->obj;
}

void asset_cache_register_button(asset_t *button) {
  assert(asset_get_type(button) == ASSET_BUTTON);
  entry_t *new_entry = malloc(sizeof(entry_t));
  assert(new_entry != NULL);
  new_entry->type = ASSET_BUTTON;
  new_entry->obj = button;
  new_entry->filepath = NULL;
  list_add(ASSET_CACHE, new_entry);
}

void asset_cache_handle_buttons(state_t *state, double x, double y) {
  size_t size = list_size(ASSET_CACHE);
  for (size_t i = 0; i < size; i++) {
    entry_t *entry = list_get(ASSET_CACHE, i);
    if (entry->type == ASSET_BUTTON) {
      asset_on_button_click(entry->obj, state, x, y);
    }
  }
}
