#include "constants.h"

// MISC:
const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};
const double ZERO = 0;
const size_t ONE = 1;
const size_t INITIAL_ASSET_CAPACITY = 10;
const size_t TEXT_SIZE = 50;
const size_t FONT_SIZE = 50;
const size_t OBSTACLE_HEIGHT = 30;
const size_t SCALE = 2;
const size_t INITIAL_CAPACITY = 5;
const size_t INITIAL_LIST_CAPACITY = 10;  // TODO: use for all lists
const size_t LIST_START_INDEX = 0;
const size_t INITIAL_NUM_BODIES = 0;
const size_t INITIAL_BODY_CAPACITY = 10;
const size_t INITIAL_FORCE_CREATOR_CAPACITY = 5;
const double ROTATION_SPEED = 0.0;
const double INITIAL_ANGLE = 0.0;
const double ANGLE_90 = M_PI * 0.5;
const double HALF = 0.5;
const double MIN_DIST = 5;
const double DEFAULT_FORCE_CONSTANT = 0.0;
const double GRAVITY = 9.8;
const double G_CONSTANT = 2500.0;
const char WINDOW_TITLE[] = "CS 3";
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 500;
const double MS_PER_S = 1e3;
const char *ERROR_MESSAGE_STATE = "Invalid state. Exiting...";
const char *EASY_DIFFICULTY_TEXT = "Easy";
const char *MEDIUM_DIFFICULTY_TEXT = "Medium";
const char *HARD_DIFFICULTY_TEXT = "Hard";
const char *UNKNOWN_DIFFICULTY_TEXT = "Unknown";
const char *ERROR_MESSAGE_ASSET = "Invalid asset. Exiting...";

// WALLS:
const double WALL_DIM = 1;
const double CEILING_OFFSET = 50;
const double GROUND_OFFSET = 50;

// BACKGROUND:
const double BACKGROUND_SCROLL_SPEED = 100;
const double BACKGROUND_OFFSET = 0;

// COLORS:
const rgb_color_t BLACK = (rgb_color_t){0, 0, 0};
const rgb_color_t RED = (rgb_color_t){255, 0, 0};
const rgb_color_t GREEN = (rgb_color_t){0, 255, 0};
const rgb_color_t BLUE = (rgb_color_t){0, 0, 255};
const rgb_color_t YELLOW = (rgb_color_t){255, 255, 0};
const double COLOR_MAX = 255;
const double WHITE_MIX = 1;

// USER: 
const double USER_RADIUS = 15;
const size_t USER_NUM_POINTS = 20;
const vector_t USER_VEL = {0, 200};
const rgb_color_t user_color = (rgb_color_t){0.1, 0.9, 0.2};
const double USER_MASS = 10000;
const double USER_X_OFFSET = 15;
const double USER_Y_OFFSET = 10;
const double USER_W_SCALE = 2;
const double USER_H_SCALE = 2;

// ZAPPER:
const double ZAPPER_WIDTH = 46;
const double ZAPPER_HEIGHT = 109;
const double MIN_ZAPPER_GENERATION_TIME_EASY = 3;
const double MAX_ZAPPER_GENERATION_TIME_EASY = 5;
const double MIN_ZAPPER_GENERATION_TIME_MEDIUM = 2;
const double MAX_ZAPPER_GENERATION_TIME_MEDIUM = 3;
const double MIN_ZAPPER_GENERATION_TIME_HARD = 1;
const double MAX_ZAPPER_GENERATION_TIME_HARD = 2;
const vector_t ZAPPER_VEL = {-100, 0};
const double ZAPPER_REMOVE_OFFSET = 50;
const double ZAPPER_CEILING_OFFSET = 100;
const double ZAPPER_GROUND_OFFSET = 100;
const double ZAPPER_Y_POS_BUFFER = 100;
const double ZAPPER_X_POS_BUFFER = 15;

// LASERS: 
const double LASER_WIDTH_ACTIVE = 990;
const double LASER_X = 500;
const double LASER_HEIGHT_ACTIVE = 42.14;
const double LASER_GENERATION_TIME_EASY = 20;
const double LASER_GENERATION_TIME_MEDIUM = 12;
const double LASER_GENERATION_TIME_HARD = 8;
const double LASER_ACTIVATION_TIME = 3;
const double LASER_ON_TIME = 3;
const size_t MIN_NUM_LASERS = 1;
const size_t MAX_NUM_LASERS = 4;
const double NUM_LASERS = 10.0;
const int16_t LASER_INITIAL_X = -2000;
const int16_t LASER_INITIAL_Y = 0;
const double LASER_OFFSET_BOTTOM = 60;
const double LASER_OFFSET_BETWEEN = 42.5;
const double LASER_Y_OFFSET = 28;
const double LASER_H_OFFSET = 56;

// ROCKETS: 
const double ROCKET_WIDTH = 90;
const double ROCKET_HEIGHT = 45;
const double WARNING_WIDTH = 50;
const double WARNING_HEIGHT = 50;
const double ROCKET_GENERATION_TIME_EASY = 12;
const double ROCKET_GENERATION_TIME_MEDIUM = 8;
const double ROCKET_GENERATION_TIME_HARD = 5;
const vector_t ROCKET_VEL = {-300, 0};
const double ROCKET_MASS = 1;
const int16_t ROCKET_INITIAL_X = -2000;
const int16_t ROCKET_INITIAL_Y = 0;
const double ROCKETS_REMOVE_OFFSET = 50;
const double ROCKETS_Y_OFFSET = 100;
const double ROCKETS_Y_POS_BUFFER = 50;
const double ROCKETS_X_POS_BUFFER = 50;
const double ROCKETS_ACTIVATION_TIME = 3;

// COINS:
const size_t NUM_COINS = 20;
const size_t COIN_TEXT_SIZE = 50;
const size_t COIN_FONT_SIZE = 30;
const size_t MIN_COIN_GRID_SIZE = 2;
const size_t MAX_COIN_GRID_SIZE = 4;
const double COIN_RADIUS = 10;
const double MIN_COIN_GENERATION_TIME = 3;
const double MAX_COIN_GENERATION_TIME = 6;
const double COIN_GENERATION_TIME_POWERUP = 1;
const SDL_Rect COIN_BOX = (SDL_Rect){25, 75, 0, 0};
const vector_t COIN_VEL = {-100, 0};
const double COIN_MASS = 1;
const int16_t COIN_OFFSET = -1000;
const double COIN_REMOVE_OFFSET = 50;
const double COIN_CEILING_OFFSET = 100;
const double COIN_GROUND_OFFSET = 100;
const double COIN_X_POS_BUFFER = 15;
const double MAX_COIN_DEVIATION_POWERUP = 200;
const double MIN_COIN_DEVIATION_POWERUP = 100;
const double COIN_SPACING_Y_SHIFT = 40;
const double COIN_SPACING_X_SHIFT = 40;
const double COIN_FORCE_CONSTANT = 0;
const char *COINS_TEXT = "COINS";

// POWERUPS:
const size_t NUM_POWERUPS = 4;
const double POWERUP_SIZE = 60;
const double POWERUP_DURATION = 10;
const double MIN_POWERUP_GENERATION_TIME = 16;
const double MAX_POWERUP_GENERATION_TIME = 12;
const vector_t POWERUP_VEL = {-100, 0};
const double POWERUP_MIN_DIST = 100.0;
const int16_t POWERUP_OFFSET = -1000;
const double POWERUP_REMOVE_OFFSET = 50;
const int16_t POWERUP_INITIAL_X = -2000;
const double POWERUP_CEILING_OFFSET = 100;
const double POWERUP_GROUND_OFFSET = 100;
const double POWERUP_Y_POS_BUFFER = 100;
const double POWERUP_X_POS_BUFFER = 15;
const double POWERUP_FORCE_CONSTANT = 0;

// DISTANCE:
const size_t DISTANCE_TEXT_SIZE = 50;
const size_t DISTANCE_FONT_SIZE = 30;
const SDL_Rect DISTANCE_BOX = (SDL_Rect){25, 25, 0, 0};
const double UPDATE_INTERVAL = 0.5;
const char *DISTANCE_TEXT = "M";

// PATH:
const char *USER_IMG_PATH = "assets/barry.png";
const char *BACKGROUND_PATH = "assets/BackdropMain.png";
const char *ZAPPER_PATH = "assets/Zapper1.png";
const char *COIN_PATH = "assets/Coin.png";
const char *POWERUP_PATH = "assets/powerup.png";
const char *LASER_PATH_INACTIVE = "assets/laser_noneactive.png";
const char *LASER_PATH_ACTIVE = "assets/laser_active.png";
const char *ROCKET_WARNING_PATH = "assets/warning.png";
const char *ROCKET_PATH = "assets/missle.png";
const char *SHIELD_PATH = "assets/force_shield.png";
const char *MAGNETIC_PATH = "assets/magnet.png";
const char *MORE_COIN_PATH = "assets/money.png";
const char *GRAVITY_SWAP_PATH = "assets/gravity_swap.png";
const char *FONT_PATH = "assets/New Athletic M54.ttf";
const char *ACHIEVEMENTS_FONT_PATH = "assets/Roboto-Regular.ttf";

// TEXT:
const size_t DIFFICULTY_TEXT_SIZE = 50;
const size_t DIFFICULTY_FONT_SIZE = 50;
const size_t ACHIEVEMENTS_FONT_SIZE = 25;
const SDL_Rect DIFFICULTY_BOX = (SDL_Rect){600, 325, 0, 0};

// ACHIEVEMENTS:
const size_t INITIAL_ACHIEVEMENTS = 3;
const char *ACHIEVEMENTS_FILENAME = "/achievements.txt";
const char *FIRST_ACHIEVEMENT = "Collect 50 Coins|0|50|false";
const char *FIRST_ACHIEVEMENT_NAME = "Collect 50 Coins";
const char *SECOND_ACHIEVEMENT = "Travel 1000 Meters In A Game|0|1000|false";
const char *SECOND_ACHIEVEMENT_NAME = "Travel 1000 Meters In A Game";
const char *THIRD_ACHIEVEMENT = "Avoid 5 Lasers|0|5|false";
const char *THIRD_ACHIEVEMENT_NAME = "Avoid 5 Lasers";
const size_t INITIAL_NUM_OBSERVERS = 10;
const size_t CHAR_TO_READ = 256;
const char *PIPE = "|";
const char *NEWLINE = "\n";
const char *SPACE = " ";
const char *SLASH = "/";
const char *TRUE_TEXT = "true";
const char *FALSE_TEXT = "false";