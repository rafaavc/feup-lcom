#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>

#include "video.h"


static void* video_mem;
static uint8_t bits_per_pixel;
static uint8_t xres, yres;


int allow_and_map_memory(uint16_t mode, bool map_memory){
    int r;
    vbe_mode_info_t vbe_info;

    if (vbe_get_mode_info(mode, &vbe_info) != 0) return 1;

    struct minix_mem_range mr;
    memset(&mr, 0, sizeof(mr));	/* zero the structure */

    bits_per_pixel = vbe_info.BitsPerPixel;
    xres = vbe_info.XResolution;
    yres = vbe_info.YResolution;

    // Allow memory mapping 

    unsigned int phys_addr = vbe_info.PhysBasePtr;
    unsigned int size = vbe_info.XResolution*vbe_info.YResolution*(vbe_info.BitsPerPixel/8);

    mr.mr_base = (phys_bytes) phys_addr;	
    mr.mr_limit = phys_addr + size;  

    if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
        return 1;
    }

    // Map memory 
    if (map_memory){

        video_mem = vm_map_phys(SELF, (void *)mr.mr_base, size);

        if(video_mem == MAP_FAILED){
            panic("couldn't map video memory");
            return 1;
        }
    }
    return 0;
}


int set_vbe_mode(uint16_t mode){
    if (allow_and_map_memory(mode, true) != 0) return 1;
    

    struct reg86 reg86_;
    memset(&reg86_, 0, sizeof(reg86_));	/* zero the structure */

    reg86_.intno = 0x10;  // Call via INT 10h
    reg86_.ah = 0x4F;     // distinguishing the VBE function from standard VGA BIOS functions
    reg86_.al = 0x02;     // VBE function
    reg86_.bx = mode|(BIT(14));

    if(sys_int86(&reg86_) != OK ) {
        printf("vg_exit(): sys_int86() failed \n");
        return 1;
    }
    return 0;
}

void draw_pixel(uint16_t x, uint16_t y, uint32_t color){
    if (x >= xres || y >= yres)
        return;
    uint8_t *memory = video_mem;
    uint8_t bytes_per_pixel = (bits_per_pixel + 7) / 8;

    int pos = bytes_per_pixel * xres * y + bytes_per_pixel * x;

    memory += pos;
    uint8_t color_tmp;
    for (unsigned int i = 0; i < bytes_per_pixel; i++) {
        color_tmp = color >> 8 * i;
        *memory = color_tmp;
        memory++;
    }
}

void draw_hline(uint16_t x, uint16_t y, uint16_t width, uint32_t color) {
    for (unsigned int i = 0; i < width; i++) {
        draw_pixel(x+i, y, color);
    }
}
