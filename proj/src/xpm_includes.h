#include <lcom/lcf.h>

/** @defgroup XPM XPM
 * @{
 * Contains functions for handling xpms
 */

/**
 * @brief loads all the pixmaps needed for the program
 */
void load_pixmaps();

/**
 * @brief get method for logo xpm
 */
xpm_image_t get_logo();

/**
 * @brief get method for the day background xpm
 */
xpm_image_t get_background();

/**
 * @brief get method for the wasd pad xpm
 */
xpm_image_t get_wasd();

/**
 * @brief get method for a game's block
 */
xpm_image_t get_block();

/**
 * @brief get method for a game's block that needs to be moved
 */
xpm_image_t get_block1();

/**
 * @brief get method for the mouse
 */
xpm_image_t get_mouse_simple();

/**
 * @brief get method for the secondary mouse (Serial port opponent's mouse)
 */
xpm_image_t get_mouse_secondary();

/**
 * @brief get method for letters xpm_image_t
 */
xpm_image_t get_letter(char c);

/**
 * @brief get method for the red ball animation
 */
xpm_image_t * get_red_ball_animation();

/**
 * @brief get method for the blue ball animation
 */
xpm_image_t * get_blue_ball_animation();

/** @}*/
