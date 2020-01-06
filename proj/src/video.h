#include <lcom/lcf.h>

/** @defgroup Video Video
 * @{
 * Contains functions for graphics
 */

/**
 * @brief initiates minix's video mode
 * 
 * @param mode mode to initialize
 */
void * (vg_init)(uint16_t mode);

/**
 * @brief draws a pixel
 * 
 * @param x x position of the pixel
 * @param y y position of the pixel
 * @param color color of the pixel
 */
void draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/**
 * @brief draw a vertical line
 * 
 * @param x x position of the first pixel
 * @param y y position of the first pixel
 * @param height height of the line
 * @param color color of the line
 */
void draw_vline(uint16_t x, uint16_t y, uint16_t height, uint32_t color);

/**
 * @brief draw a vertical line
 * 
 * @param x x position of the first pixel
 * @param y y position of the first pixel
 * @param width width of the line
 * @param color color of the line
 */
void draw_hline(uint16_t x, uint16_t y, uint16_t width, uint32_t color);

/**
 * @brief draw a rectangle
 * 
 * @param x x position of the first pixel
 * @param y y position of the first pixel
 * @param height height of the rectangle
 * @param width width of the rectangle
 * @param color color of the rectangle
 */
void draw_rectangle(uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint32_t color);

/**
 * @brief draws a xpm
 * 
 * @param xpm xpm to draw on the screen
 * @param x x position of the initial pixel
 * @param y y position of the initial pixel
 * @param centered if the xpm is centered or not
 * @param color color of the pixel if different from PREDEF_COLOR, will draw the xpm with every pixel of color 'color'
 * @param size size of the xpm. Can be "" (normal), "small" or "smaller"
 */
void draw_pixmap(xpm_image_t xpm, uint16_t x, uint16_t y, bool centered, uint32_t color, char * size);

/**
 * @brief gets background buffer ready to copy
 * 
 * @param img background xpm
 * @param x x postion of the initial pixel
 * @param y y position of the initial pixel
 * @param centered if the xpm is centered or not
 * @param color if different from PREDEF_COLOR background is of color
 * @param relative_size size of the xpm. Can be "" (normal), "small" or "smaller"
 */
void draw_bg_buffer(xpm_image_t img, uint16_t x, uint16_t y, bool centered, uint32_t color, char * relative_size);

/**
 * @brief get a character width
 * 
 * @param s character to get width from
 * 
 * @return s width
 */
unsigned get_letter_width_normal(char s);

/**
 * @brief get a string width
 * 
 * @param s string to get width from
 * @param length length of the string
 * 
 * @return s width
 */
unsigned get_string_width_normal(char *s, unsigned length);

/**
 * @brief draws a string centered in the screen
 * 
 * @param s string to draw
 * @param ssize string s size
 * @param x initial x position
 * @param y initial y position
 * @param max_length_per_line max lenght per line
 * @param color color of the string
 * @param relative_size size of the string. Can be "" (normal), "small" or "smaller"
 */
void draw_string_centered(char* s, int ssize, uint16_t x, uint16_t y, uint16_t max_length_per_line, uint32_t color, char * relative_size);

/**
 * @brief draws a string in the screen
 * 
 * @param s string to draw
 * @param ssize string s size
 * @param x initial x position
 * @param y initial y position
 * @param max_lenght_per_line max lenght per line
 * @param color color of the string
 * @param relative_size size of the string. Can be "" (normal), "small" or "smaller"
 */
void draw_string(char* s, int ssize, uint16_t x, uint16_t y, uint16_t max_lenght_per_line, uint32_t color, char * relative_size);

/**
 * @brief Returns information on the input VBE mode, including screen dimensions, color depth and VRAM physical address.
 * 
 * @param mode	mode whose information should be returned
 * @param vmi_p	address of vbe_mode_info_t structure to be initialized 
 * 
 * @return 0 on success and non-zero otherwise
 */
int get_mode_info(uint16_t mode, vbe_mode_info_t *vmi_p);

/**
 * @brief get screen width
 * 
 * @return screen width in pixels
 */
uint16_t get_xres();

/**
 * @brief get screen height
 * 
 * @return screen height in pixels
 */
uint16_t get_yres();

/**
 * @brief get bits per pixel
 * 
 * @return bits per pixel
 */
uint8_t get_bits_per_pixel();

/**
 * @brief get method for the red mask used in the mode
 * 
 * @return red mask
 */
uint8_t get_red_mask();

/**
 * @brief get method for the green mask used in the mode
 * 
 * @return green mask
 */
uint8_t get_green_mask();

/**
 * @brief get method for the blue mask used in the mode
 * 
 * @return blue mask
 */
uint8_t get_blue_mask();

/**
 * @brief get method for video_mem
 * 
 * @return video_mem 
 */
void* get_video_mem();

/**
 * @brief get method for the double buffer
 * 
 * @return double_buffer
 */
void* get_double_buffer();

/**
 * @brief get method for background buffer
 * 
 * @return background buffer
 */
void* get_background_buffer();

/** @}*/
