#ifndef __POLYGON_H__
#define __POLYGON_H__

#include "color.h"
#include "list.h"
#include "vector.h"

typedef struct polygon polygon_t;

/**
 * Initialize a polygon object given a list of vertices.
 *
 * @param points the list of vertices that make up the polygon
 * @param initial_position a vector representing the initial center position of
 * the polygon
 * @param initial_velocity a vector representing the initial velocity of the
 * polygon
 * @param rotation_speed the rotation angle of the polygon per unit time
 * @param red double value between 0 and 1 representing the red of the polygon
 * @param green double value between 0 and 1 representing the green of the
 * polygon
 * @param blue double value between 0 and 1 representing the blue of the polygon
 * @return a polygon object pointer
 */
polygon_t *polygon_init(list_t *points, vector_t initial_velocity,
                        double rotation_speed, double red, double green,
                        double blue);

/**
 * Return the list of vectors representing the vertices of the polygon.
 *
 * @param polygon the list of vertices that make up the polygon
 * @return a list of vectors
 */
list_t *polygon_get_points(polygon_t *polygon);

/**
 * Translate and rotate the polygon then update velocity based on gravity.
 *
 * @param polygon the list of vertices that make up the polygon
 * @param time_elapsed time/# of frames elapsed since the last tick
 */
void polygon_move(polygon_t *polygon, double time_elapsed);

/**
 * Computes the area of a polygon.
 * See https://en.wikipedia.org/wiki/Shoelace_formula#Statement.
 *
 * @param polygon the list of vertices that make up the polygon,
 * listed in a counterclockwise direction. There is an edge between
 * each pair of consecutive vertices, plus one between the first and last.
 * @return the area of the polygon
 */
double polygon_area(polygon_t *polygon);

/**
 * Computes the center of mass of a polygon.
 * See https://en.wikipedia.org/wiki/Centroid#Of_a_polygon.
 *
 * @param polygon the list of vertices that make up the polygon,
 * listed in a counterclockwise direction. There is an edge between
 * each pair of consecutive vertices, plus one between the first and last.
 * @return the centroid of the polygon
 */
vector_t polygon_centroid(polygon_t *polygon);

/**
 * Translates all vertices in a polygon by a given vector.
 * Note: mutates the original polygon.
 *
 * @param polygon the list of vertices that make up the polygon
 * @param translation the vector to add to each vertex's position
 */
void polygon_translate(polygon_t *polygon, vector_t translation);

/**
 * Rotates vertices in a polygon by a given angle about a given point.
 * Note: mutates the original polygon.
 *
 * @param polygon the list of vertices that make up the polygon
 * @param angle the angle to rotate the polygon, in radians.
 * A positive angle means counterclockwise.
 * @param point the point to rotate around
 */
void polygon_rotate(polygon_t *polygon, double angle, vector_t point);

/**
 * Return the polygon's color.
 *
 * @param polygon the list of vertices that make up the polygon
 * @return the rgb_color_t struct representing the color
 */
rgb_color_t *polygon_get_color(polygon_t *polygon);

/**
 * Changes the color of the polygon.
 *
 * @param polygon a polygon_t struct
 * @param color a struct containing rgb values of the new color
 */
void polygon_set_color(polygon_t *polygon, rgb_color_t *color);

/**
 * Changes the centroid of the polygon.
 *
 * @param polygon a polygon_t struct
 * @param centroid a vector representing the new centroid
 */
void polygon_set_center(polygon_t *polygon, vector_t centroid);

/**
 * Returns the centroid of the polygon.
 *
 * @param polygon a polygon_t struct
 * @return a vector representing the centroid
 */
vector_t polygon_get_center(polygon_t *polygon);

/**
 * Sets the rotation angle of the polygon relative to the vertical.
 *
 * @param polygon a polygon_t struct
 * @param rot a double value of the angle in radians
 */
void polygon_set_rotation(polygon_t *polygon, double rot);

/**
 * Returns the rotation angle of the polygon relative to the vertical.
 *
 * @param polygon a polygon_t struct
 * @return a double value of the angle in radians
 */
double polygon_get_rotation(polygon_t *polygon);

/**
 * Set the x and y components of a polygon's velocity vector.
 *
 * @param polygon the list of vertices that make up the polygon
 * @param vel the new velocity
 */
void polygon_set_velocity(polygon_t *polygon, vector_t vel);

/**
 * Return x-component of polygon's velocity vector.
 *
 * @param polygon the list of vertices that make up the polygon
 * @return the x-velocity of a polygon
 */
double polygon_get_velocity_x(polygon_t *polygon);

/**
 * Return y-component of polygon's velocity vector.
 *
 * @param polygon the list of vertices that make up the polygon
 * @return the y-velocity of a polygon
 */
double polygon_get_velocity_y(polygon_t *polygon);

/**
 * Free memory allocated for object associated with a polygon.
 *
 * @param polygon the list of vertices that make up the polygon
 */
void polygon_free(polygon_t *polygon);

#endif // #ifndef __POLYGON_H__
