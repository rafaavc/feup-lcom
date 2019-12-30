#include "player.h"
#include "video.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include <minix/driver.h>
#include <sys/mman.h>

#include <stddef.h>

extern unsigned grid_height, grid_width, move_count, error;
extern uint32_t color_palette[];
extern unsigned play_time, timer_counter_play;

Player * create_player(unsigned bi, unsigned bj, xpm_image_t animation_idle[12], unsigned starting_an) {
    Player *p = (Player*) malloc(sizeof(*p));
    (*p).board_i = (int) bi;
    (*p).board_j = (int) bj;
    (*p).counter = 0;
    (*p).animation_frame = starting_an;
    (*p).moving = false;
    (*p).name = "";
    for (int i = 0; i < 12; i++){
        (*p).animation_idle[i] = animation_idle[i];
    }
    (*p).next_x = (*p).next_y = 0;
    return p;
}

int p_get_j(Player * p) { return (*p).board_j; }

int p_get_i(Player * p) { return (*p).board_i; }

void p_set_name(Player * p, char * name) {
  (*p).name = name;
}

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

  if (abs((*p).next_x) > 3){
    x -= (*p).next_x;
    if ((*p).next_x > 0){
      (*p).next_x -= 3;
    }
    else {
      (*p).next_x += 3;
    }
  }
  if (abs((*p).next_y) > 2){
    y -= (*p).next_y;
    if ((*p).next_y > 0){
      (*p).next_y -= 2;
    }
    else {
      (*p).next_y += 2;
    }
  }

  draw_pixmap((*p).animation_idle[frame], x, y, true, PREDEF_COLOR, "");
  (*p).counter++;
}



bool move(Player *p, char direction, int board[BOARD_SIZE][BOARD_SIZE]) {
  //unsigned speed = 2;
  if (direction == 'w') {
    if ((*p).last_movement == 's'){
      error = 1;
      return false;
    }
    if (board[(*p).board_i - 1][(*p).board_j + 1] != -1) {
      (*p).next_x = 37;
      (*p).next_y = -23;
      player_move_w(p);
      (*p).last_movement = 'w';
      play_time = 0;
      timer_counter_play = 0;
    } else {
      return false;
    }
  }
  else if (direction == 'a'){
    if ((*p).last_movement == 'd'){
      error = 1;
      return false;
    }
    if (board[(*p).board_i - 1][(*p).board_j - 1] != -1) {
      (*p).next_x = -37;
      (*p).next_y = -23;
      player_move_a(p);
      (*p).last_movement = 'a';
      play_time = 0;
      timer_counter_play = 0;
    } else {
      return false;
    }
  }
  else if (direction == 's'){
    if ((*p).last_movement == 'w'){
      error = 1;
      return false;
    }
    if (board[(*p).board_i + 1][(*p).board_j - 1] != -1) {
      (*p).next_x = -37;
      (*p).next_y = 23;
      player_move_s(p);
      (*p).last_movement = 's';
      play_time = 0;
      timer_counter_play = 0;
    } else {
      return false;
    }
  }
  else if (direction == 'd'){
    if ((*p).last_movement == 'a'){
      error = 1;
      return false;
    }
    if (board[(*p).board_i + 1][(*p).board_j + 1] != -1) {
      (*p).next_x = 37;
      (*p).next_y = 23;
      player_move_d(p);
      (*p).last_movement = 'd';
      play_time = 0;
      timer_counter_play = 0;
    } else {
      return false;
    }
  }
  return true;
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

void p_set_last_movement(Player *p, char lmov){
  (*p).last_movement = lmov;
}

char p_get_last_movement(Player *p){
  return(*p).last_movement;
}
