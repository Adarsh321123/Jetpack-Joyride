#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>

#include "settings.h"
#include "asset.h"
#include "asset_helper.h"
#include "constants.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"
#include "achievement.h"

/**
 * Handler for going back to home screen
 */
static void home(settings_state_t *settings_state){
  settings_state->curr_state = HOME;
}

/**
 * Handler for setting the difficulty of game to easy
 */
static void set_difficulty_easy(settings_state_t *settings_state){
  settings_state->difficulty_level = EASY;
}

/**
 * Handler for setting the difficulty of game to medium
 */
static void set_difficulty_medium(settings_state_t *settings_state){
  settings_state->difficulty_level = MEDIUM;
}

/**
 * Handler for setting the difficulty of game to hard
 */
static void set_difficulty_hard(settings_state_t *settings_state){
  settings_state->difficulty_level = HARD;
}

static background_info_t background_templates[] = {
     {.bg_path = "assets/BackdropMain.png",
     .bg_box = (SDL_Rect){0, 0, 1000, 500}}
     };

static text_info_t text_templates[] = {
     {.font_path = "assets/New Athletic M54.ttf",
     .text_box = (SDL_Rect){625, 50, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Settings"},
     {.font_path = "assets/New Athletic M54.ttf",
     .text_box = (SDL_Rect){50, 325, 150, 50},
     .text_color = (rgb_color_t){0, 0, 0},
     .text = "Current Difficulty Level : "}
     };

static text_info_t achievements_templates[] = {
      {.font_path = "assets/Roboto-Regular.ttf",
     .text_box = (SDL_Rect){525, 125, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Achievements"},
     {.font_path = "assets/Roboto-Regular.ttf",
     .text_box = (SDL_Rect){525, 175, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = ""},
     {.font_path = "assets/Roboto-Regular.ttf",
     .text_box = (SDL_Rect){525, 225, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = ""},
     {.font_path = "assets/Roboto-Regular.ttf",
     .text_box = (SDL_Rect){525, 275, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = ""}
};

static button_info_t button_templates[] = {
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){100, 25, 300, 100},
     .text_box = (SDL_Rect){200, 50, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Home",
     .handler = (void *)home},
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){50, 400, 300, 100},
     .text_box = (SDL_Rect){150, 425, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Easy",
     .handler = (void *)set_difficulty_easy},
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){350, 400, 300, 100},
     .text_box = (SDL_Rect){450, 425, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Medium",
     .handler = (void *)set_difficulty_medium},
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){650, 400, 300, 100},
     .text_box = (SDL_Rect){750, 425, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Hard",
     .handler = (void *)set_difficulty_hard}
     };

static void display_difficulty_level(settings_state_t *settings_state){
    char difficulty_text[DIFFICULTY_TEXT_SIZE];

    switch (settings_state->difficulty_level) {
        case EASY: {
            sprintf(difficulty_text, "%s", EASY_DIFFICULTY_TEXT);
            break;
        }
        case MEDIUM: {
            sprintf(difficulty_text, "%s", MEDIUM_DIFFICULTY_TEXT);
            break;
        }
        case HARD: {
            sprintf(difficulty_text, "%s", HARD_DIFFICULTY_TEXT);
            break;
        }
        default: {
            sprintf(difficulty_text, "%s", UNKNOWN_DIFFICULTY_TEXT);
            break;
        }
    }

    SDL_Rect bounding_box = DIFFICULTY_BOX;
    TTF_Font *font = settings_state->difficulty_font;
    TTF_SizeText(font, difficulty_text, &bounding_box.w, &bounding_box.h);
    render_text(difficulty_text, font, BLACK, bounding_box);
}

static void display_achievements(settings_state_t *settings_state) {
  size_t num_achievements = sizeof(achievements_templates) / sizeof(achievements_templates[0]);
  TTF_Font *font = settings_state->achievements_font;
  for (size_t i = 0; i < num_achievements; i++) {
    text_info_t info = achievements_templates[i];
    TTF_SizeText(font, info.text, &info.text_box.w, &info.text_box.h);
    render_text(info.text, font, info.text_color, info.text_box);
  }
}

settings_state_t *settings_init() {
  settings_state_t *settings_state = malloc(sizeof(settings_state_t));
  assert(settings_state);
  TTF_Init();
  sdl_on_mouse(on_mouse);
  sdl_init(MIN, MAX);
  asset_cache_init();
  size_t num_backgrounds = sizeof(background_templates) / sizeof(background_templates[0]);
  size_t num_text = sizeof(text_templates) / sizeof(text_templates[0]);
  size_t num_buttons = sizeof(button_templates) / sizeof(button_templates[0]);
  settings_state->screen_state = screen_init(settings_state->screen_state, background_templates,
                                          text_templates, button_templates,
                                          num_backgrounds, num_text, num_buttons);
  settings_state->time = 0;
  settings_state->curr_state = SETTINGS;
  settings_state->difficulty_level = EASY;
  settings_state->difficulty_font = init_font(FONT_PATH, DIFFICULTY_FONT_SIZE);
  settings_state->achievements_font = init_font(ACHIEVEMENTS_FONT_PATH, ACHIEVEMENTS_FONT_SIZE);
  list_t *results = read_achievements_settings();
  size_t num_results = list_size(results);
  for (size_t i = 0; i < num_results; i++) {
    achievements_templates[i + 1].text = list_get(results, i);  // + 1 since excluding achievements title
  }
  list_free(results);
  return settings_state;
}

state_type_t settings_main(settings_state_t *settings_state) {
  sdl_clear();
  settings_state->time += time_since_last_tick();
  render_items(settings_state->screen_state);
  display_difficulty_level(settings_state);
  display_achievements(settings_state);
  handle_mouse_events(settings_state);
  sdl_show();
  return settings_state->curr_state;
}

void settings_free(settings_state_t *settings_state) {
  screen_free(settings_state->screen_state);
  TTF_CloseFont(settings_state->difficulty_font);
  TTF_CloseFont(settings_state->achievements_font);
  asset_cache_destroy();
  free(settings_state);
  TTF_Quit();
}
