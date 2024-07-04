#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include "color.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "state.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

// Values passed to a key handler when the given arrow key is pressed
typedef enum
{
  LEFT_ARROW = 1,
  UP_ARROW = 2,
  RIGHT_ARROW = 3,
  DOWN_ARROW = 4,
  SPACE_BAR = 5,
  MOUSE_RIGHT = 6,
  MOUSE_LEFT = 7,
} arrow_key_t;

/**
 * The possible types of key events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * typedef unsigned int KeyEventType;
 * #define KEY_PRESSED 0
 * #define KEY_RELEASED 1
 */
typedef enum
{
  KEY_PRESSED,
  KEY_RELEASED
} key_event_type_t;

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of key event (KEY_PRESSED or KEY_RELEASED)
 * @param held_time if a press event, the time the key has been held in seconds
 * @param state the current state
 */
typedef void (*key_handler_t)(char key, key_event_type_t type, double held_time,
                              void *state);

/**
 * A mouseclick handler.
 * When a click is registered or released, the handler is passed its char value.
 *
 * @param key a character indicating which key was pressed
 * @param state object passed in to retrieve and update idx
 * @param event event SDL_Event to retrieve position of mouse clicks
 */
typedef void (*mouse_handler_t)(char key, void *state, SDL_Event event);

/**
 * Initializes the SDL window and renderer.
 * Must be called once before any of the other SDL functions.
 *
 * @param min the x and y coordinates of the bottom left of the scene
 * @param max the x and y coordinates of the top right of the scene
 */
void sdl_init(vector_t min, vector_t max);

/**
 * Processes all SDL events and returns whether the window has been closed.
 * This function must be called in order to handle inputs.
 *
 * @param state object passed in for the key handler
 * @return true if the window was closed, false otherwise
 */
bool sdl_is_done(void *state);

/**
 * This function must be called in order to handle mouseclicks.
 * Identifies mouseclick and invokes `mouse_handler` to process
 * custom actions corresponding to different mouse_clicks.
 * 
 * @param state object passed in to retrieve and update idx
 */
void handle_mouse_events(void *state);

/**
 * Clears the screen. Should be called before drawing polygons in each frame.
 */
void sdl_clear(void);

/**
 * Destroys rendering and window.
 */
void sdl_destroy(void);

/**
 * Returns texture of image.
 *
 * @param IMG_PATH file path of image
 * @return the SDL texture
 */
SDL_Texture *get_texture(const char *IMG_PATH);

/**
 * Copys rendering onto target
 *
 * @param img image texture
 * @param texr defines area on rendering target
 */
void render_copy(SDL_Texture *img, SDL_Rect texr);

/**
 * Initalizes a custom font with a given size.
 *
 * @param FONT_PATH file path of font
 * @param size size of the font
 * @return the TTF_Font font object
 */
TTF_Font *init_font(const char *FONT_PATH, size_t size);

/**
 * Creates and returns a SDL_Rect for the bounding box
 *
 * @param x x-location
 * @param y y-location
 * @param w width
 * @param h height
 * @return a bounding box
 */
SDL_Rect make_texr(int x, int y, int w, int h);

/**
 * Find the bounding box of a body given as the input. Modifies based on
 * the type of the body
 *
 * @param body input body to find the bounding box of
 * @return bounding box of the body
 */
SDL_Rect find_bounding_box(body_t *body);

/**
 * Creates a surface and texture to render the given text.
 *
 * @param text string to output
 * @param fontin the font
 * @param rgb_color color of the text
 * @param texr box for position of text
 */
void render_text(const char *text, TTF_Font *fontin, rgb_color_t rgb_color,
                 SDL_Rect texr);

/**
 * Draws a polygon from the given list of vertices and a color.
 *
 * @param poly a struct representing the polygon
 * @param color the color used to fill in the polygon
 */
void sdl_draw_polygon(polygon_t *poly, rgb_color_t color);

/**
 * Displays the rendered frame on the SDL window.
 * Must be called after drawing the polygons in order to show them.
 */
void sdl_show(void);

/**
 * Draws all bodies in a scene.
 * This internally calls sdl_clear(), sdl_draw_polygon(), and sdl_show(),
 * so those functions should not be called directly.
 *
 * @param scene the scene to draw
 * @param aux an additional body to draw (can be NULL if no additional bodies)
 */
void sdl_render_scene(scene_t *scene, void *aux);

/**
 * Registers a function to be called every time a key is pressed.
 * Overwrites any existing handler.
 *
 * Example:
 * ```
 * void on_key(char key, key_event_type_t type, double held_time) {
 *     if (type == KEY_PRESSED) {
 *         switch (key) {
 *             case 'a':
 *                 printf("A pressed\n");
 *                 break;
 *             case UP_ARROW:
 *                 printf("UP pressed\n");
 *                 break;
 *         }
 *     }
 * }
 * int main(void) {
 *     sdl_on_key(on_key);
 *     while (!sdl_is_done());
 * }
 * ```
 *
 * @param handler the function to call with each key press
 */
void sdl_on_key(key_handler_t handler);

/**
 * Registers a function to be called every time a mouse click is registered.
 * Overwrites any existing handler.
 * 
 * @param handler_mouse the function to call with each mouse click
 */
void sdl_on_mouse(mouse_handler_t handler_mouse);

/**
 * Gets the amount of time that has passed since the last time
 * this function was called, in seconds.
 *
 * @return the number of seconds that have elapsed
 */
double time_since_last_tick(void);

#endif // #ifndef __SDL_WRAPPER_H__
