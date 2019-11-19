#include <stdio.h>

void *( vg_init)(uint16_t mode);

int set_vbe_mode(uint16_t mode);

void draw_pixel(uint16_t x, uint16_t y, uint32_t color);

void draw_hline(uint16_t x, uint16_t y, uint16_t width, uint32_t color);

void draw_rectangle(uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint32_t color);

uint16_t get_xres();

uint16_t get_yres();

uint8_t get_bits_per_pixel();
