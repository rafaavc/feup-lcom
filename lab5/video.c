#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>

#include "video.h"


static void* video_mem;
static uint8_t bits_per_pixel;
static uint16_t xres, yres;
static uint8_t red_screen_mask, blue_screen_mask, green_screen_mask;



void * (vg_init)(uint16_t mode){
    int r;
    vbe_mode_info_t vbe_info;

    if (get_mode_info(mode, &vbe_info) != 0) return NULL;

    struct minix_mem_range mr;
    memset(&mr, 0, sizeof(mr));	 //zero the structure 
    bits_per_pixel = vbe_info.BitsPerPixel;
    xres = vbe_info.XResolution;
    yres = vbe_info.YResolution;
    red_screen_mask = vbe_info.RedMaskSize;
    blue_screen_mask = vbe_info.BlueMaskSize;
    green_screen_mask = vbe_info.GreenMaskSize;

    // Allow memory mapping 

    unsigned int phys_addr = vbe_info.PhysBasePtr;
    unsigned int size = vbe_info.XResolution*vbe_info.YResolution*((vbe_info.BitsPerPixel+7)/8);

    mr.mr_base = (phys_bytes) phys_addr;	
    mr.mr_limit = phys_addr + size;  

    if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
        return NULL;
    }

    // Map memory
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, size);

    if(video_mem == MAP_FAILED){
        panic("couldn't map video memory");
        return NULL;
    }

    struct reg86 reg86_;
    memset(&reg86_, 0, sizeof(reg86_));	 //zero the structure 

    reg86_.intno = 0x10;  // Call via INT 10h
    reg86_.ah = 0x4F;     // distinguishing the VBE function from standard VGA BIOS functions
    reg86_.al = 0x02;     // VBE function
    reg86_.bx = mode|(BIT(14));
    if(sys_int86(&reg86_) != OK ) {
        printf("vg_exit(): sys_int86() failed \n");
        return NULL;
    }
    return video_mem;
}

void draw_pixel(uint16_t x, uint16_t y, uint32_t color){
    if (x >= xres || y >= yres)
        return;
    uint8_t *memory = video_mem;
    uint8_t bytes_per_pixel = (bits_per_pixel+7) / 8;

    unsigned int pos = bytes_per_pixel * (xres * y + x);
    memory += pos;
    for (unsigned int i = 0; i < bytes_per_pixel; i++) {
        *memory = color >> 8 * i;
        memory++;
    }
}
    
void draw_hline(uint16_t x, uint16_t y, uint16_t width, uint32_t color) {
    for (unsigned int i = 0; i < width; i++) {
        draw_pixel(x+i, y, color);
    }
}

void draw_rectangle(uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint32_t color) {
    for (unsigned int i = 0; i < height; i++) {
        draw_hline(x, y+i, width, color);
    }
}

void draw_pixmap(xpm_map_t xpm, uint16_t x, uint16_t y) {
    //uint8_t *mem = get_video_mem();
    xpm_image_t img;

    uint8_t *map = xpm_load(xpm, XPM_INDEXED, &img);
    draw_rectangle(0, 0, yres, xres, 0x0);
    for (unsigned i = 0; i < img.height; i++) {
        for (unsigned j = 0; j < img.width; j++) {
            draw_pixel(x+j, y+i, map[img.width*i + j]);
        }
    }
}

void increment_with_speed(uint16_t *xi, uint16_t *yi, uint16_t xf, uint16_t yf, uint16_t speed) {
    if (*xi != xf) {
      if (xf-*xi >= speed) {
        *xi += speed;
      } else if (*xi -xf >= speed){
        *xi -= speed;
      } else {
        *xi = xf;
      }
    }
    if (*yi != yf) {
      if (yf-*yi >= speed) {
        *yi += speed;
      } else if (*yi-yf >= speed) { 
        *yi -= speed;
      } else {
        *yi = yf;
      }
    }
}

void increment_with_one(uint16_t *xi, uint16_t *yi, uint16_t xf, uint16_t yf) {
    if (*xi != xf) {
      if (xf-*xi >= 1) {
        *xi += 1;
      } else if (*xi -xf >= 1){
        *xi -= 1;
      } else {
        *xi = xf;
      }
    }
    if (*yi != yf) {
      if (yf-*yi >= 1) {
        *yi += 1;
      } else if (*yi-yf >= 1) { 
        *yi -= 1;
      } else {
        *yi = yf;
      }
    }
}

int get_mode_info(uint16_t mode, vbe_mode_info_t *vmi_p) {
    phys_bytes buf;

    struct reg86 reg86_;
    memset(&reg86_, 0, sizeof(reg86_));

    mmap_t mem;
    lm_alloc(sizeof(vbe_mode_info_t), &mem);

    buf = mem.phys;

    /*
    Visa Bios Extension Documentation

    Input: AX = 4F01h Return VBE mode information
    CX = Mode number
    ES:DI = Pointer to ModeInfoBlock structure
    */

    reg86_.ax = 0x4F01;
    reg86_.es = PB2BASE(buf);
    reg86_.di = PB2OFF(buf);
    reg86_.cx = mode;
    reg86_.intno = 0x10;


    /*
    The mmap_t includes both:
        The physical address, for use by VBE
        The virtual address, for use in Minix 3

    PB2BASE Is a macro for computing the base of a segment, a
    16-bit value, given a 32-bit linear address;
    PB2OFF Is a macro for computing the offset with respect to the
    base of a segment, a 16-bit value, given a 32-bit linear address;
    
    */


    if (sys_int86(&reg86_) != OK) {
        return 1;
    }

    *vmi_p = *(vbe_mode_info_t *) (mem.virt);

    lm_free(&mem);
    
    return 0;
}

uint16_t get_xres() {
    return xres;
}

uint16_t get_yres() {
    return yres;
}
uint8_t get_bits_per_pixel() {
    return bits_per_pixel;
}
uint8_t get_red_mask() {
    return red_screen_mask;
}
uint8_t get_blue_mask() {
    return blue_screen_mask;
}
uint8_t get_green_mask() {
    return green_screen_mask;
}
void* get_video_mem() {
    return video_mem;
}
