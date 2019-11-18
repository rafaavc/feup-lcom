#include <stdio.h>

int allow_and_map_memory(uint16_t mode, bool map_memory);

int set_vbe_mode(uint16_t mode);

void draw_pixel(uint16_t x, uint16_t y, uint32_t color);

void draw_hline(uint16_t x, uint16_t y, uint16_t width, uint32_t color);
