#include "player.h"
#include "video.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include <minix/driver.h>
#include <sys/mman.h>

#include <stddef.h>

extern unsigned grid_height, grid_width, move_count;

Player * create_player(unsigned bi, unsigned bj, xpm_image_t animation_idle[12], unsigned starting_an) {
    Player *p = (Player*) malloc(sizeof(*p));
    (*p).board_i = (int) bi;
    (*p).board_j = (int) bj;
    (*p).counter = 0;
    (*p).animation_frame = starting_an;
    (*p).moving = false;
    (*p).name = "bi";
    for (int i = 0; i < 12; i++){
        (*p).animation_idle[i] = animation_idle[i];
    }
    return p;
}

int p_get_j(Player * p) { return (*p).board_j; }

int p_get_i(Player * p) { return (*p).board_i; }

/*int p_set_xvel(Player * p) { return (*p).xvel; }

int p_set_yvel(Player * p) { return (*p).yvel; }*/

unsigned next_animation_frame(Player *p) {
  (*p).animation_frame++;
  if ((*p).animation_frame > 11){
    (*p).animation_frame = 0;
  }
  return (*p).animation_frame;
}

void draw_player(Player *p) {
  unsigned frame = (*p).animation_frame;

  if ((*p).counter % 4 == 0) // Changes frame at every 4 frames
    frame = next_animation_frame(p);
  
  unsigned i, j;
  i = (*p).board_i - 2;
  j = (*p).board_j;

  unsigned y = i*grid_height - ((((BOARD_SIZE - 1) * grid_height)/2));
  unsigned x = j*grid_width - ((((BOARD_SIZE - 1) * grid_width)/2));

  draw_pixmap((*p).animation_idle[frame], x, y, true, PREDEF_COLOR, "");
  (*p).counter++;
}

void move(Player *p, char direction, int board[BOARD_SIZE][BOARD_SIZE]) {
  //unsigned speed = 2;
  if (direction == 'w') {
    if (board[(*p).board_i - 1][(*p).board_j + 1] != -1) {
      player_move_w(p);
    }
  }
  else if (direction == 'a'){
    if (board[(*p).board_i - 1][(*p).board_j - 1] != -1) {
      player_move_a(p);
    }
  }
  else if (direction == 's'){
    if (board[(*p).board_i + 1][(*p).board_j - 1] != -1) {
      player_move_s(p);
    }
  }
  else if (direction == 'd'){
    if (board[(*p).board_i + 1][(*p).board_j + 1] != -1) {
      player_move_d(p);
    }
  }
}

void player_move_w(Player * p) {
  (*p).board_i -= 1;
  (*p).board_j += 1;
  move_count++;
}

void player_move_a(Player * p) {
  (*p).board_i -= 1;
  (*p).board_j -= 1;
  move_count++;
}

void player_move_s(Player * p) {
  (*p).board_i += 1;
  (*p).board_j -= 1;
  move_count++;
}

void player_move_d(Player * p) {
  (*p).board_i += 1;
  (*p).board_j += 1;
  move_count++;
}
