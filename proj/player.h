#include <lcom/lcf.h>
#include "Macros.h"

typedef struct {
    unsigned counter, animation_frame;
    int board_i, board_j;
    bool moving;
    xpm_image_t animation_idle[12];
    char * name;
    int next_x, next_y; //How many pixels it's gonna move in x and y
} Player;

Player * create_player(unsigned x, unsigned y, xpm_image_t animation_idle[12], unsigned starting_an);

int p_get_j(Player * p);

int p_get_i(Player * p);

int p_set_xvel(Player * p);

int p_set_yvel(Player * p);

unsigned next_animation_frame(Player *p);

void draw_player(Player *p);

void move(Player *p, char direction, int board[BOARD_SIZE][BOARD_SIZE]);

void player_move_w(Player * p);

void player_move_a(Player * p);

void player_move_s(Player * p);

void player_move_d(Player * p);
