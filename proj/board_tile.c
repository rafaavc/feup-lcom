#include "board_tile.h"
#include "xpm_includes.h"
#include "video.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include <minix/driver.h>
#include <sys/mman.h>

#include <stddef.h>

extern unsigned grid_height;
extern unsigned grid_width;

bool get_dragging(Tile *t) { return (*t).dragging; }

Tile * create_tile() {
    Tile *t = (Tile *) malloc(sizeof(*t));
    (*t).state = 0;
    (*t).states[0] = get_block();
    (*t).states[1] = get_block1();
    (*t).dragging = false;
    return t;
}

void draw_tile(Tile *t, unsigned x, unsigned y, bool dragging) {
    if (!dragging) {
        draw_pixmap((*t).states[(*t).state], (uint16_t) x - (((BOARD_SIZE - 1) * grid_width) / 2), (uint16_t) y - (((BOARD_SIZE - 1) * grid_height) / 2), true, PREDEF_COLOR, "");
    } else {
        draw_pixmap((*t).states[(*t).state], (uint16_t) x, (uint16_t) y, false, PREDEF_COLOR, "");
    }
}

void start_dragging_tile(Tile *t) {
    (*t).dragging = true;
    (*t).state = 0;
}

void stop_dragging_tile(Tile *t, bool changed_position) {
    (*t).dragging = false;
    if (!changed_position) {
        (*t).state = 1;
    }
}

void toggle_need_to_be_moved(Tile *t) {
    if ((*t).state == 1) {
        (*t).state = 0;
    } else {
        (*t).state = 1;
    }
}

void create_tiles(Tile*t[], const unsigned int n) {
    for (unsigned i = 0; i < n; i++) {
        t[i] = create_tile();
    }
}

void create_board(int board[BOARD_SIZE][BOARD_SIZE]) {
    // n must not be an even number (5 rows of tiles, and an even number of offset to all sides)
    int tile = 0;
    unsigned offset = (BOARD_SIZE - 5) / 2;

    for (unsigned i = 0; i < BOARD_SIZE; i++) {
        //printf("\ni: %d\n  ", i);
        for (unsigned j = 0; j < BOARD_SIZE; j++) {
            //printf("j: %d, ", j);
            if (i == offset && j == offset + 2) {
                board[i][j] = tile;
            } else if (i == offset + 1) {
                if (j == offset + 1) {
                    board[i][j] = tile;
                } else if (j == offset + 3) {
                    board[i][j] = tile;
                } else {
                    board[i][j] = -1;
                }
            } else if (i == offset + 2) {
                if (j == offset) {
                    board[i][j] = tile;
                } else if (j == offset + 2) {
                    board[i][j] = tile;
                } else if (j == offset + 4) {
                    board[i][j] = tile;
                } else {
                    board[i][j] = -1;
                }

            } else if (i == offset + 3) {
                if (j == offset + 1) {
                    board[i][j] = tile;
                } else if (j == offset + 3) {
                    board[i][j] = tile;
                } else {
                    board[i][j] = -1;
                }
            } else if (i == offset + 4 && j == offset + 2) {
                board[i][j] = tile;
            } else {
                board[i][j] = -1;
            }

            if (board[i][j] != -1) {
                tile++;
            }
        }
    }
}


