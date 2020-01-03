#include "Macros.h"
#include "mouse_trigger.h"
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include <minix/driver.h>
#include <sys/mman.h>

#include <stddef.h>

extern int mouse_xvariance, mouse_yvariance;
extern int p1_mouse_xvariance, p1_mouse_yvariance; 

MouseTrigger * create_mouse_trigger(unsigned x, unsigned y, unsigned width, unsigned height, enum Event event) {
    MouseTrigger *t = (MouseTrigger *) malloc(sizeof(*t));
    (*t).x = x;
    (*t).y = y;
    (*t).width = width;
    (*t).height = height;
    (*t).event = event;
    (*t).mouse_over = false;
    (*t).obj = NULL;
    return t;
}

unsigned mt_get_xpos(MouseTrigger *t) { return (*t).x; }

unsigned mt_get_ypos(MouseTrigger *t) { return (*t).y; }

unsigned mt_get_endxpos(MouseTrigger *t) { return (*t).x + (*t).width; }

unsigned mt_get_endypos(MouseTrigger *t) { return (*t).y + (*t).height; }

enum Event mt_get_event(MouseTrigger *t) { return (*t).event; }

bool check_mouse_overlap(MouseTrigger *t) {
    return (unsigned) mouse_xvariance > mt_get_xpos(t) && (unsigned) mouse_yvariance > mt_get_ypos(t) && (unsigned) mouse_xvariance < mt_get_endxpos(t) && (unsigned) mouse_yvariance < mt_get_endypos(t);
}

bool p1_check_mouse_overlap(MouseTrigger *t) {
    return (unsigned) p1_mouse_xvariance > mt_get_xpos(t) && (unsigned) p1_mouse_yvariance > mt_get_ypos(t) && (unsigned) p1_mouse_xvariance < mt_get_endxpos(t) && (unsigned) p1_mouse_yvariance < mt_get_endypos(t);
}

bool mt_get_mouse_over(MouseTrigger *t) {
    bool mo = (*t).mouse_over;
    if ((unsigned) mouse_xvariance < mt_get_xpos(t) || (unsigned) mouse_xvariance > mt_get_endxpos(t) || (unsigned) mouse_yvariance < mt_get_ypos(t) || (unsigned) mouse_yvariance > mt_get_endypos(t))
        (*t).mouse_over = false;
    return mo;
}

void mt_set_mouse_over(MouseTrigger *t) {
    (*t).mouse_over = true;
}

void mt_set_obj(MouseTrigger *t, void * o) {
    (*t).obj = o;
}

void * mt_get_obj(MouseTrigger *t) {
    return (*t).obj;
}



