#include "player.h"
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

Player * create_player(unsigned x, unsigned y, xpm_image_t animation_idle[12], unsigned starting_an){
    Player *p = (Player*) malloc(sizeof(*p));
    (*p).x = (int) x;
    (*p).y = (int) y;
    (*p).counter = 0;
    (*p).animation_frame = starting_an;
    (*p).xvel = 0;
    (*p).yvel = 0;
    for (int i = 0; i < 12; i++){
        (*p).animation_idle[i] = animation_idle[i];
    }
    return p;
}

int p_get_xpos(Player * p) { return (*p).x; }

int p_get_ypos(Player * p) { return (*p).y; }

int p_set_xvel(Player * p) { return (*p).xvel; }

int p_set_yvel(Player * p) { return (*p).yvel; }

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

  if ((*p).counter % 2 == 0) {  
    (*p).x += (*p).xvel;
    (*p).y += (*p).yvel;
  }
  draw_pixmap((*p).animation_idle[frame], (*p).x - 21, (*p).y - 36, false, PREDEF_COLOR, "");
  (*p).counter++;
}

void move(Player *p, char direction) {
  unsigned speed = 2;
  if (direction == 'w') {
    (*p).xvel = speed + 1;
    (*p).yvel = -speed;
  }
  else if (direction == 'a'){
    (*p).xvel = -speed - 1;
    (*p).yvel = -speed;
  }
  else if (direction == 's'){
    (*p).xvel = -speed - 1;
    (*p).yvel = speed;
  }
  else if (direction == 'd'){
    (*p).xvel = speed + 1;
    (*p).yvel = speed;
  }
}
