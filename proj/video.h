#include <lcom/lcf.h>

void * (vg_init)(uint16_t mode);

void draw_pixel(uint16_t x, uint16_t y, uint32_t color);

void draw_vline(uint16_t x, uint16_t y, uint16_t height, uint32_t color);

void draw_hline(uint16_t x, uint16_t y, uint16_t width, uint32_t color);

void draw_rectangle(uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint32_t color);

void draw_pixmap(xpm_image_t xpm, uint16_t x, uint16_t y, bool centered, uint32_t color, char * s);

void draw_bg_buffer(xpm_image_t img, uint16_t x, uint16_t y, bool centered, uint32_t color1, char * relative_size);

unsigned get_letter_width_normal(char s);

unsigned get_string_width_normal(char *s, unsigned length);

void draw_string_centered(char* s, int ssize, uint16_t x, uint16_t y, uint16_t max_length_per_line, uint32_t color, char * relative_size);

void draw_string(char* s, int ssize, uint16_t x, uint16_t y, uint16_t max_lenght_per_line, uint32_t color, char * size_);

void increment_with_speed(uint16_t *xi, uint16_t *yi, uint16_t xf, uint16_t yf, uint16_t speed);

void increment_with_one(uint16_t *xi, uint16_t *yi, uint16_t xf, uint16_t yf);

int get_mode_info(uint16_t mode, vbe_mode_info_t *vmi_p);

uint16_t get_xres();

uint16_t get_yres();

uint8_t get_bits_per_pixel();

uint8_t get_red_mask();

uint8_t get_green_mask();

uint8_t get_blue_mask();

void* get_video_mem();

void* get_double_buffer();

void* get_background_buffer();
