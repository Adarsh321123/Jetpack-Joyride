#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "math.h"
#include "sdl_wrapper.h"
#include "vector.h"
#include "color.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// MISC:
extern const vector_t MIN;
extern const vector_t MAX;
extern const vector_t CENTER;
extern const double ZERO;
extern const size_t ONE;
extern const size_t INITIAL_ASSET_CAPACITY;
extern const size_t TEXT_SIZE;
extern const size_t FONT_SIZE;
extern const size_t OBSTACLE_HEIGHT;
extern const size_t SCALE;
extern const size_t INITIAL_CAPACITY;
extern const size_t INITIAL_LIST_CAPACITY;
extern const size_t LIST_START_INDEX;
extern const size_t INITIAL_NUM_BODIES;
extern const size_t INITIAL_BODY_CAPACITY;
extern const size_t INITIAL_FORCE_CREATOR_CAPACITY;
extern const double ROTATION_SPEED;
extern const double INITIAL_ANGLE;
extern const double ANGLE_90;
extern const double HALF;
extern const double MIN_DIST;
extern const double DEFAULT_FORCE_CONSTANT;
extern const double GRAVITY;
extern const double G_CONSTANT;
extern const char WINDOW_TITLE[];
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const double MS_PER_S;
extern const char *ERROR_MESSAGE_STATE;
extern const char *EASY_DIFFICULTY_TEXT;
extern const char *MEDIUM_DIFFICULTY_TEXT;
extern const char *HARD_DIFFICULTY_TEXT;
extern const char *UNKNOWN_DIFFICULTY_TEXT;
extern const char *ERROR_MESSAGE_ASSET;

// WALLS:
extern const double WALL_DIM;
extern const double CEILING_OFFSET;
extern const double GROUND_OFFSET;

// BACKGROUND:
extern const double BACKGROUND_SCROLL_SPEED;
extern const double BACKGROUND_OFFSET;

// COLORS:
extern const rgb_color_t BLACK;
extern const rgb_color_t RED;
extern const rgb_color_t GREEN;
extern const rgb_color_t BLUE;
extern const rgb_color_t YELLOW ;
extern const double COLOR_MAX;
extern const double WHITE_MIX;

// USER: 
extern const double USER_RADIUS;
extern const size_t USER_NUM_POINTS;
extern const vector_t USER_VEL;
extern const rgb_color_t user_color;
extern const double USER_MASS;
extern const double USER_X_OFFSET;
extern const double USER_Y_OFFSET;
extern const double USER_W_SCALE;
extern const double USER_H_SCALE;

// ZAPPER:
extern const double ZAPPER_WIDTH;
extern const double ZAPPER_HEIGHT;
extern const double MIN_ZAPPER_GENERATION_TIME_EASY;
extern const double MAX_ZAPPER_GENERATION_TIME_EASY;
extern const double MIN_ZAPPER_GENERATION_TIME_MEDIUM;
extern const double MAX_ZAPPER_GENERATION_TIME_MEDIUM ;
extern const double MIN_ZAPPER_GENERATION_TIME_HARD;
extern const double MAX_ZAPPER_GENERATION_TIME_HARD;
extern const vector_t ZAPPER_VEL;
extern const double ZAPPER_REMOVE_OFFSET;
extern const double ZAPPER_CEILING_OFFSET;
extern const double ZAPPER_GROUND_OFFSET;
extern const double ZAPPER_Y_POS_BUFFER;
extern const double ZAPPER_X_POS_BUFFER;

// LASERS: 
extern const double LASER_WIDTH_ACTIVE;
extern const double LASER_X;
extern const double LASER_HEIGHT_ACTIVE;
extern const double LASER_GENERATION_TIME_EASY;
extern const double LASER_GENERATION_TIME_MEDIUM;
extern const double LASER_GENERATION_TIME_HARD;
extern const double LASER_ACTIVATION_TIME;
extern const double LASER_ON_TIME;
extern const size_t MIN_NUM_LASERS;
extern const size_t MAX_NUM_LASERS;
extern const double NUM_LASERS;
extern const int16_t LASER_INITIAL_X;
extern const int16_t LASER_INITIAL_Y;
extern const double LASER_OFFSET_BOTTOM;
extern const double LASER_OFFSET_BETWEEN;
extern const double LASER_Y_OFFSET;
extern const double LASER_H_OFFSET;

// ROCKETS: 
extern const double ROCKET_WIDTH;
extern const double ROCKET_HEIGHT;
extern const double WARNING_WIDTH;
extern const double WARNING_HEIGHT;
extern const double ROCKET_GENERATION_TIME_EASY;
extern const double ROCKET_GENERATION_TIME_MEDIUM;
extern const double ROCKET_GENERATION_TIME_HARD;
extern const vector_t ROCKET_VEL;
extern const double ROCKET_MASS;
extern const int16_t ROCKET_INITIAL_X;
extern const int16_t ROCKET_INITIAL_Y;
extern const double ROCKETS_REMOVE_OFFSET;
extern const double ROCKETS_Y_OFFSET;
extern const double ROCKETS_Y_POS_BUFFER;
extern const double ROCKETS_X_POS_BUFFER;
extern const double ROCKETS_ACTIVATION_TIME;

// COINS:
extern const size_t NUM_COINS;
extern const size_t COIN_TEXT_SIZE;
extern const size_t COIN_FONT_SIZE;
extern const size_t MIN_COIN_GRID_SIZE;
extern const size_t MAX_COIN_GRID_SIZE;
extern const double COIN_RADIUS;
extern const double MIN_COIN_GENERATION_TIME;
extern const double MAX_COIN_GENERATION_TIME;
extern const double COIN_GENERATION_TIME_POWERUP;
extern const SDL_Rect COIN_BOX;
extern const vector_t COIN_VEL;
extern const double COIN_MASS;
extern const int16_t COIN_OFFSET;
extern const double COIN_REMOVE_OFFSET;
extern const double COIN_CEILING_OFFSET;
extern const double COIN_GROUND_OFFSET;
extern const double COIN_X_POS_BUFFER;
extern const double MAX_COIN_DEVIATION_POWERUP;
extern const double MIN_COIN_DEVIATION_POWERUP;
extern const double COIN_SPACING_Y_SHIFT;
extern const double COIN_SPACING_X_SHIFT;
extern const double COIN_FORCE_CONSTANT;
extern const char *COINS_TEXT;

// POWERUPS:
extern const size_t NUM_POWERUPS;
extern const double POWERUP_SIZE;
extern const double POWERUP_DURATION;
extern const double MIN_POWERUP_GENERATION_TIME;
extern const double MAX_POWERUP_GENERATION_TIME;
extern const vector_t POWERUP_VEL;
extern const double POWERUP_MIN_DIST;
extern const int16_t POWERUP_OFFSET;
extern const double POWERUP_REMOVE_OFFSET;
extern const int16_t POWERUP_INITIAL_X;
extern const double POWERUP_CEILING_OFFSET;
extern const double POWERUP_GROUND_OFFSET;
extern const double POWERUP_Y_POS_BUFFER;
extern const double POWERUP_X_POS_BUFFER;
extern const double POWERUP_FORCE_CONSTANT;

// DISTANCE:
extern const size_t DISTANCE_TEXT_SIZE;
extern const size_t DISTANCE_FONT_SIZE;
extern const SDL_Rect DISTANCE_BOX;
extern const double UPDATE_INTERVAL;
extern const char *DISTANCE_TEXT;

// PATH:
extern const char *USER_IMG_PATH;
extern const char *BACKGROUND_PATH;
extern const char *ZAPPER_PATH;
extern const char *COIN_PATH;
extern const char *POWERUP_PATH;
extern const char *LASER_PATH_INACTIVE;
extern const char *LASER_PATH_ACTIVE;
extern const char *ROCKET_WARNING_PATH;
extern const char *ROCKET_PATH;
extern const char *SHIELD_PATH;
extern const char *MAGNETIC_PATH;
extern const char *MORE_COIN_PATH;
extern const char *GRAVITY_SWAP_PATH;
extern const char *FONT_PATH;
extern const char *ACHIEVEMENTS_FONT_PATH;

// TEXT:
extern const size_t DIFFICULTY_TEXT_SIZE;
extern const size_t DIFFICULTY_FONT_SIZE;
extern const size_t ACHIEVEMENTS_FONT_SIZE;
extern const SDL_Rect DIFFICULTY_BOX;

// ACHIEVEMENTS:
extern const size_t INITIAL_ACHIEVEMENTS;
extern const char *ACHIEVEMENTS_FILENAME;
extern const char *FIRST_ACHIEVEMENT;
extern const char *FIRST_ACHIEVEMENT_NAME;
extern const char *SECOND_ACHIEVEMENT;
extern const char *SECOND_ACHIEVEMENT_NAME;
extern const char *THIRD_ACHIEVEMENT;
extern const char *THIRD_ACHIEVEMENT_NAME;
extern const size_t INITIAL_NUM_OBSERVERS;
extern const size_t CHAR_TO_READ;
extern const char *PIPE;
extern const char *NEWLINE;
extern const char *SPACE;
extern const char *SLASH;
extern const char *TRUE_TEXT;
extern const char *FALSE_TEXT;

#endif // CONSTANTS_H
