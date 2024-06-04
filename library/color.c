#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "color.h"

const double COLOR_MAX = 255; // max value of each rgb value
const double WHITE_MIX = 1;

rgb_color_t *color_init(double red, double green, double blue) {
  rgb_color_t *color = malloc(sizeof(rgb_color_t));
  assert(color);

  color->r = red;
  color->g = green;
  color->b = blue;

  return color;
}

rgb_color_t *color_get_random() {
  double r = (double)rand();
  double g = (double)rand();
  double b = (double)rand();

  r = fmod(r, COLOR_MAX) / COLOR_MAX;
  g = fmod(g, COLOR_MAX) / COLOR_MAX;
  b = fmod(b, COLOR_MAX) / COLOR_MAX;

  r = (r + WHITE_MIX) / 2;
  g = (g + WHITE_MIX) / 2;
  b = (b + WHITE_MIX) / 2;

  return color_init(r, g, b);
}

bool color_compare(rgb_color_t c1, rgb_color_t c2) {
  return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

void color_free(rgb_color_t *color) { free(color); }
