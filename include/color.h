#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdbool.h>

typedef struct color {
  double r;
  double g;
  double b;
} rgb_color_t;

/**
 * Initialize a color object.
 *
 * @param red the double value for red
 * @param green the double value for green
 * @param blue the double value for blue
 * @return a pointer to a color_t object
 */
rgb_color_t *color_init(double red, double green, double blue);

/**
 * Randomly generate rgb values for color of polygon.
 *
 * @return a pointer to a color object with randomly generated rgb values
 */
rgb_color_t *color_get_random();

/**
 * Compare the rgb values of two color structs.
 *
 * @param c1 an rgb_color_t struct
 * @param c2 an rgb_color_t struct to compare with c1
 * @return a boolean representing whether the two structs are equal in content
 */
bool color_compare(rgb_color_t c1, rgb_color_t c2);

/**
 * Free memory allocated for the color object.
 *
 * @param color a color with r, g, b values
 */
void color_free(rgb_color_t *color);

#endif // #ifndef __COLOR_H__
