#include <assert.h>
#include <state.h>
#include <stdio.h>
#include <stdlib.h>
#include <emscripten.h>  // TODO: this is also in emscripten.c, is that fine?

#include "home.h"
#include "asset.h"
#include "asset_helper.h"
#include "constants.h"
#include "asset_cache.h"
#include "sdl_wrapper.h"

static bool mounted = false;

/**
 * Handler for entering settings page
 */
static void settings(home_state_t *home_state);

/**
 * Handler for entering game play
 */
static void play(home_state_t *home_state);

static background_info_t background_templates[] = {
     {.bg_path = "assets/jetpack_joyride_home.jpg",
     .bg_box = (SDL_Rect){0, 0, 1000, 500}}
     };

static text_info_t text_templates[] = {
     {.font_path = "assets/New Athletic M54.ttf",
     .text_box = (SDL_Rect){625, 50, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Jetpack Joyride"}
     };

static button_info_t button_templates[] = {
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){150, 300, 300, 100},
     .text_box = (SDL_Rect){200, 325, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Settings",
     .handler = (void *)settings},
    {.image_path = "assets/button.png",
     .font_path = "assets/New Athletic M54.ttf",
     .image_box = (SDL_Rect){450, 300, 300, 100},
     .text_box = (SDL_Rect){550, 325, 150, 50},
     .text_color = (rgb_color_t){255, 255, 255},
     .text = "Play",
     .handler = (void *)play}
     };

static void settings(home_state_t *home_state){
  home_state->curr_state = SETTINGS;
}

static void play(home_state_t *home_state){
  home_state->curr_state = GAME_PLAY;
}

/**
 * Initializes and stores the background assets in the home_state.
 */
static void create_backgrounds(home_state_t *home_state) {
  for (size_t i = 0; i < NUM_BACKGROUNDS; i++) {
    background_info_t info = background_templates[i];
    asset_t *background = create_background_from_info(info);
    list_add(home_state->backgrounds, background);
  }
}

/**
 * Initializes and stores the text assets in the home_state.
 */
static void create_text(home_state_t *home_state) {
  for (size_t i = 0; i < NUM_TEXT_HOME; i++) {
    text_info_t info = text_templates[i];
    asset_t *text = create_text_from_info(info);
    list_add(home_state->text, text);
  }
}

/**
 * Initializes and stores the button assets in the home_state.
 */
static void create_buttons(home_state_t *home_state) {
  for (size_t i = 0; i < NUM_BUTTONS_HOME; i++) {
    button_info_t info = button_templates[i];
    asset_t *button = create_button_from_info(info);
    list_add(home_state->manual_buttons, button);
  }
}

static void on_mouse(char key, void *home_state, SDL_Event event) {
  if (key == MOUSE_RIGHT || key == MOUSE_LEFT) {
    asset_cache_handle_buttons(home_state, event.button.x, event.button.y);
  }
}

void init_achievements_file(const char *achievements_filename) {
  // TODO: repetition with writing to file mehtod
  // TODO: if first time ever playing will need to init the storage with usable stuff, test by clearing cache?
  // TODO: is it ok to change the makefile like I did? unintended consequences?
  // TODO: remove the local file
  FILE *achievements_file = fopen(achievements_filename, "w");
  assert(achievements_file != NULL);
  fprintf(achievements_file, "Player Name: %s\n", "Adarsh");
  fprintf(achievements_file, "Player Name: %s\n", "Dhruv");
  fprintf(achievements_file, "Player Name: %s\n", "Rayhan");
  int close_result = fclose(achievements_file);  // using int from Adam's example
  assert(close_result == 0);
  fprintf(stderr, "Initialized new achievements file\n");
}

void read_achievements(const char *achievements_filename) {
  FILE *achievements_file = fopen(achievements_filename, "r");
  if (achievements_file == NULL) {
    fprintf(stderr, "Achievements file not found. Creating a new one.\n");
    init_achievements_file(achievements_filename);
    achievements_file = fopen(achievements_filename, "r");
    assert(achievements_file != NULL);
  }
  fprintf(stderr, "File opened for reading\n");
  size_t char_read = 100;
  char *temp_string = malloc(sizeof(char) * (char_read + 1));
  while(fgets(temp_string, char_read, achievements_file)) {
    fprintf(stderr, "%s", temp_string);
  }
  int close_result = fclose(achievements_file);
  assert(close_result == 0);
  free(temp_string);
}

void write_achievements(const char *achievements_filename) {
  FILE *achievements_file = fopen(achievements_filename, "w");
  assert(achievements_file != NULL);
  fprintf(stderr, "File opened for writing\n");
  fprintf(achievements_file, "Player Name: %s\n", "Zach");
  fprintf(stderr, "File written to\n");

  int close_result = fclose(achievements_file);
  assert(close_result == 0);
}

void sync_to_persistent_storage() {
  EM_ASM(
    FS.syncfs(function (err) {
      assert(!err);
      console.log("Filesystem synchronized to persistent storage.");
    });
  );
}
// TODO: remove console logs
void sync_from_persistent_storage_and_write() {
  EM_ASM(
    FS.syncfs(true, function (err) {
      assert(!err);
      console.log("Filesystem synchronized from persistent storage.");
      ccall('read_achievements', 'void', ['string'], ['/persistent/achievements.txt']);
      ccall('write_achievements', 'void', ['string'], ['/persistent/achievements.txt']);
      ccall('sync_to_persistent_storage', 'void', []);
    });
  );
}

void mount_persistent_fs() {
  if (!mounted) {
    EM_ASM(
      if (!FS.analyzePath('/persistent').exists) {
        FS.mkdir('/persistent');
      }
      FS.mount(IDBFS, {}, '/persistent');
    );
    mounted = true;
  }
}

// TODO: clean up later such as constants for strings
// TODO: can we use these macros?
home_state_t *home_init() {
  home_state_t *home_state = malloc(sizeof(home_state_t));
  assert(home_state);
  sdl_on_mouse(on_mouse);
  sdl_init(MIN, MAX);
  TTF_Init();
  asset_cache_init();
  home_state->time = 0;
  // Note that `free_func` is NULL because `asset_cache` is reponsible for
  // freeing the button assets.
  home_state->backgrounds = list_init(NUM_BACKGROUNDS, NULL);
  create_backgrounds(home_state);

  home_state->text = list_init(NUM_TEXT_HOME, NULL);
  create_text(home_state);

  home_state->manual_buttons = list_init(NUM_BUTTONS_HOME, NULL);
  // We store the assets used for buttons to be freed at the end.
  home_state->button_assets = list_init(NUM_BUTTONS_HOME, (free_func_t)asset_destroy);
  create_buttons(home_state);

  home_state->curr_state = HOME;

  // if (SDL_Init(SDL_INIT_AUDIO) < 0) {
  //   fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
  //   return NULL;
  // }

  // if (Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1) {
  //   fprintf(stderr, "Mix_OpenAudio: %s\n", Mix_GetError());
  //   SDL_Quit();
  //   return NULL;
  // }

  // char *path = "assets/avengers.mp3";
  // FILE *file = fopen(path, "r");
  // if (!file) {
  //   fprintf(stderr, "Error: Unable to find '%s'\n", path);
  // } else {
  //   fclose(file);
  //   // Mix_Music *music = Mix_LoadMUS(path);
  //   fprintf(stderr, "Loaded 1\n");
  // }

  // Mix_Music *music = Mix_LoadMUS(path);
  // fprintf(stderr, "Loaded 2\n");
  // if (music == NULL) {
  //   fprintf(stderr, "Mix_LoadMUS(\"%s\"): %s\n", path, Mix_GetError());
  //   Mix_CloseAudio();
  //   SDL_Quit();
  //   return NULL;
  // }

  // if ( Mix_PlayMusic( music, -1) == -1 ) {
  //   return NULL;
  // }

  // SDL_Window* window = SDL_CreateWindow("SDL Music Player",
  //                                       SDL_WINDOWPOS_CENTERED,
  //                                       SDL_WINDOWPOS_CENTERED,
  //                                       640, 480,
  //                                       SDL_WINDOW_SHOWN);
  // if (!window) {
  //   fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
  //   Mix_FreeMusic(music);
  //   Mix_CloseAudio();
  //   SDL_Quit();
  //   return NULL;
  // }

  // SDL_bool done = SDL_FALSE;
  // while (!done) {
  //   SDL_Event event;
  //   while (SDL_PollEvent(&event)) {
  //     if (event.type == SDL_QUIT || 
  //       (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
  //       done = SDL_TRUE;
  //     }
  //   }
  // }

  // SDL_DestroyWindow(window);
  // Mix_FreeMusic(music);
  // Mix_CloseAudio();
  // SDL_Quit();

  // return 0;

  // if (SDL_Init(SDL_INIT_AUDIO) < 0) {
  //   fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
  //   return NULL;
  // }

  // if (Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1) {
  //   fprintf(stderr, "Mix_OpenAudio: %s\n", Mix_GetError());
  //   SDL_Quit();
  //   return NULL;
  // }

  // char *path = "assets/BabyElephantWalk60.wav";
  // FILE *file = fopen(path, "r");
  // if (!file) {
  //   fprintf(stderr, "Error: Unable to find '%s'\n", path);
  // } else {
  //   fclose(file);
  //   // Mix_Music *music = Mix_LoadMUS(path);
  //   fprintf(stderr, "Loaded 1\n");
  // }

  // Mix_Chunk *wave = Mix_LoadWAV(path);
  // fprintf(stderr, "Loaded 2\n");
  // if (wave == NULL) {
  //   fprintf(stderr, "Mix_LoadWAV(\"%s\"): %s\n", path, Mix_GetError());
  //   Mix_CloseAudio();
  //   SDL_Quit();
  //   return NULL;
  // }

  // int channel = Mix_PlayChannel(-1, wave, 0);
  // fprintf(stderr, "here 3\n");
  // if ( channel == -1 ) {
  //   fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
  //   Mix_FreeChunk(wave);
  //   Mix_CloseAudio();
  //   SDL_Quit();
  //   return NULL;
  // }

  // fprintf(stderr, "here 4\n");
  // while (Mix_Playing(channel) != 0) {
  //   fprintf(stderr, "here\n");
  //   SDL_Delay(100);
  // }

  // fprintf(stderr, "here 6\n");
  // Mix_FreeChunk(wave);
  // Mix_CloseAudio();
  // SDL_Quit();

  // return 0;

  mount_persistent_fs();
  // sync the filesystem from IndexedDB to the in-memory filesystem
  // then, write and sync back to persistent storage
  sync_from_persistent_storage_and_write();

  return home_state;
}

state_type_t home_main(home_state_t *home_state) {
  sdl_clear();
  home_state->time += time_since_last_tick();

  // render the backgrounds
  list_t *backgrounds = home_state->backgrounds;
  for (size_t i = 0; i < NUM_BACKGROUNDS; i++){
    asset_render(list_get(backgrounds, i));
  }

  // render the text
  list_t *text = home_state->text;
  for (size_t i = 0; i < NUM_TEXT_HOME; i++){
    asset_render(list_get(text, i));
  }

  // render the buttons
  list_t *buttons = home_state->manual_buttons;
  for (size_t i = 0; i < NUM_BUTTONS_HOME; i++) {
    asset_render(list_get(buttons, i));
  }

  handle_mouse_events(home_state);
  sdl_show();
  return home_state->curr_state;
}

void home_free(home_state_t *home_state) {
  TTF_Quit();
  list_free(home_state->backgrounds);
  list_free(home_state->text);
  list_free(home_state->manual_buttons);
  list_free(home_state->button_assets);
  asset_cache_destroy();
  free(home_state);
}
