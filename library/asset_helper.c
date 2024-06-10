#include "asset_helper.h"

asset_t *create_background_from_info(background_info_t info)
{
  asset_t *background_asset = asset_make_image(info.bg_path, info.bg_box);
  return background_asset;
}

asset_t *create_text_from_info(text_info_t info)
{
  asset_t *text_asset = NULL;
  if (info.font_path != NULL)
  {
    text_asset = asset_make_text(info.font_path, info.text_box, info.text,
                                 info.text_color);
  }
  return text_asset;
}

asset_t *create_button_from_info(button_info_t info)
{
  asset_t *image_asset = NULL;
  if (info.image_path != NULL)
  {
    image_asset = asset_make_image(info.image_path, info.image_box);
  }
  asset_t *text_asset = NULL;
  if (info.font_path != NULL)
  {
    text_asset = asset_make_text(info.font_path, info.text_box, info.text,
                                 info.text_color);
  }
  asset_t *button_asset =
      asset_make_button(info.image_box, image_asset, text_asset, info.handler);
  asset_cache_register_button(button_asset);
  return button_asset;
}