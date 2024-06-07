#include "constants.h"

// TODO: add more here
const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
const vector_t CENTER = {500, 250};

const size_t INITIAL_ASSET_CAPACITY = 10;

// TODO: ulimit -n 4096 does not work anymore
const size_t TEXT_SIZE = 50;
const size_t FONT_SIZE = 50;
const size_t TEXT_HEIGHT_SCALE = 2;
const size_t NUM_TEXT_HOME = 1;
const size_t NUM_TEXT_SETTINGS = 3;  // TODO: why is this a constant instead of getting the size of text list
const size_t NUM_TEXT_GAME_OVER = 1;
const size_t NUM_BACKGROUNDS = 1;
const size_t NUM_BUTTONS_HOME = 2;
const size_t NUM_BUTTONS_SETTINGS = 4;
const size_t NUM_BUTTONS_GAME_OVER = 2;

const size_t DEFAULT_WIDTH = 0;
const size_t DEFAULT_HEIGHT = 0;

const rgb_color_t black = (rgb_color_t){0, 0, 0};
const rgb_color_t white = (rgb_color_t){255, 255, 255};
const rgb_color_t red = (rgb_color_t){255, 0, 0};
const rgb_color_t green = (rgb_color_t){0, 255, 0};
const rgb_color_t blue = (rgb_color_t){0, 0, 255};
const rgb_color_t yellow = (rgb_color_t){255, 255, 0};

const char *FONT_PATH = "assets/New Athletic M54.ttf";

const double INTERVAL = 1;
