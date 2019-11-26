#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <lcom/xpm.h>
#include <stdio.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include "xpms/bg.xpm"
#include "xpms/block.xpm"
#include "xpms/cursor_simple.xpm"


static xpm_image_t background;
static xpm_image_t block;
static xpm_image_t mouse_simple;



void load_pixmaps() {
    xpm_load(background_xpm, XPM_8_8_8, &background);
    xpm_load(block_xpm, XPM_8_8_8, &block);
    xpm_load(cursor_simple_xpm, XPM_8_8_8, &mouse_simple);
}

xpm_image_t get_background() { return background; }

xpm_image_t get_block() { return block; }

xpm_image_t get_mouse_simple() { return mouse_simple; }


