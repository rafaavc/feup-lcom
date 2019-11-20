#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>
#include "sprite.h"

Sprite *create_sprite(uint8_t* map, uint16_t x, uint16_t y, uint16_t xspeed, uint16_t yspeed) {
  //allocate space for the "object"
  Sprite *sp = (Sprite *) malloc(sizeof(Sprite));
  if (sp == NULL)
    return NULL;
  // read the sprite pixmap
  sp->x = x;
  sp->y = y;
  sp->xspeed = xspeed;
  sp->yspeed = yspeed;
  sp->map = (char*) map;
  if (sp->map == NULL) {
    free(sp);
    return NULL;
  }
  return sp;
}
