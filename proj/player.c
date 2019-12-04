#include "player.h"
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

Player * create_player(unsigned x, unsigned y, xpm_image_t * animation_idle[12]){
    Player *p = (Player*) malloc(sizeof(*p));
    (*p).x = x;
    (*p).y = y;
    (*p).animation_frame = 0;
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
  if ((*p).animation_frame > 12){
    (*p).animation_frame = 0;
  }
  return (*p).animation_frame;
}

void draw_player(Player *p){
  unsigned frame = next_animation_frame(p);
  (*p).x += (*p).xvel;
  (*p).y += (*p).yvel;
  draw_pixmap(*(*p).animation_idle[frame],(*p).x,(*p).y,false,PREDEF_COLOR,"");
}

