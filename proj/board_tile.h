
#include <lcom/lcf.h>
#include "Macros.h"

typedef struct {
    unsigned state;
    xpm_image_t states[3];
    bool dragging;
} Tile;



Tile * create_tile();

void draw_tile(Tile *t, unsigned x, unsigned y, bool dragging);

void toggle_need_to_be_moved(Tile *t);

void start_dragging_tile(Tile *t);

void stop_dragging_tile(Tile *t, bool changed_position);

void create_tiles(Tile*t[], const unsigned int n);

bool get_dragging(Tile *t);

void create_board(int board[BOARD_SIZE][BOARD_SIZE]);
