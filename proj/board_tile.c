#include "board_tile.h"
#include "xpm_includes.h"
#include "video.h"
#include "Macros.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include <minix/driver.h>
#include <sys/mman.h>

#include <stddef.h>

extern unsigned grid_height;
extern unsigned grid_width;

int get_xpos(Tile *t) { return (*t).x; }

int get_ypos(Tile *t) { return (*t).y; }

Tile * create_tile(int x, int y) {
    Tile *t = (Tile *) malloc(sizeof(*t));
    (*t).x = x;
    (*t).y = y;
    return t;
}

void draw_tile(Tile *t) {
    draw_pixmap(get_block(), (uint16_t) (*t).x, (uint16_t) (*t).y, true, PREDEF_COLOR);
}

void dragging_tile(Tile *t, int xvariance, int yvariance) {
    (*t).x += xvariance;
    (*t).y -= yvariance;
}

void create_board(Tile*t[], const unsigned int n) {
    t[0] = create_tile(0, -grid_height);
    t[1] = create_tile(-grid_width, 0);
    t[2] = create_tile(-2*grid_width, grid_height);
    t[3] = create_tile(grid_width, 0);
    t[4] = create_tile(0, grid_height);
    t[5] = create_tile(-grid_width, 2*grid_height);
    t[6] = create_tile(2*grid_width, grid_height);
    t[7] = create_tile(grid_width, 2*grid_height);
    t[8] = create_tile(0, 3*grid_height);
}
