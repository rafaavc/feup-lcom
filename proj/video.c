#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>
#include "string.h"
#include "Macros.h"
#include "video.h"
#include "xpm_includes.h"


static void* video_mem;
static void* double_buffer;
static void* background_buffer;
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
    double_buffer = (void *) malloc(size);
    background_buffer = (void *) malloc(size);
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
    uint8_t *memory = double_buffer;
    uint8_t bytes_per_pixel = (bits_per_pixel+7) / 8;

    unsigned int pos = bytes_per_pixel * (xres * y + x);
    memory += pos;
    for (unsigned int i = 0; i < bytes_per_pixel; i++) {
        *memory = color >> 8 * i;
        memory++;
    }
}

void draw_pixel_bg_buffer(uint16_t x, uint16_t y, uint32_t color){
    if (x >= xres || y >= yres)
        return;
    uint8_t *memory = background_buffer;
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

void draw_vline(uint16_t x, uint16_t y, uint16_t height, uint32_t color) {
    for (unsigned int i = 0; i < height; i++) {
        draw_pixel(x, y + i, color);
    }
}

void draw_rectangle(uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint32_t color) {
    for (unsigned int i = 0; i < height; i++) {
        draw_hline(x, y+i, width, color);
    }
}

void draw_pixmap(xpm_image_t img, uint16_t x, uint16_t y, bool centered, uint32_t color1, char * relative_size) {
    /*if (abs(x) > xres/2 || abs(y) > yres/2) { 
      printf("Error: Invalid screen position for pixmap.\n");
      return;
    }*/
    unsigned module_size = 1;
    if (strcmp(relative_size, "small") == 0) {
      module_size = 2;
    } else if (strcmp(relative_size, "smaller") == 0) {
      module_size = 3;
    }

    
    uint32_t color;
    bool custom_color = false;
    if (color1 != 0xFF0000) {
      custom_color = true;
    }

    uint8_t bytes_per_pixel = (bits_per_pixel+7)/8;
    uint8_t *map = img.bytes;
    
    if (centered) {
      x = get_xres()/2 + x -(img.width/2);
      y = get_yres()/2 + y -(img.height/2);
    }

    //draw_rectangle(0, 0, yres, xres, 0x0);
    for (unsigned i = 0; i < img.height; i++) {
      if (i % module_size == 0) {
        for (unsigned j = 0; j < img.width; j++) {
          if (j % module_size == 0) {
            color = 0;
            for (unsigned byte = 0; byte < bytes_per_pixel; byte++) {
              color += map[img.width*i + j + byte] << byte * 8;
            }
            if (color != 0xFF0000) {
              if (custom_color) {
                color = color1;
              }
              draw_pixel(x+(j/module_size), y+(i/module_size), color);
            }            
          }
          map += bytes_per_pixel-1;
        }
      } else {
        map += (bytes_per_pixel-1) * img.width;
      }
    }
}

void draw_bg_buffer(xpm_image_t img, uint16_t x, uint16_t y, bool centered, uint32_t color1, char * relative_size) {
    /*if (abs(x) > xres/2 || abs(y) > yres/2) { 
      printf("Error: Invalid screen position for pixmap.\n");
      return;
    }*/
    unsigned module_size = 1;
    if (strcmp(relative_size, "small") == 0) {
      module_size = 2;
    } else if (strcmp(relative_size, "smaller") == 0) {
      module_size = 3;
    }

    
    uint32_t color;
    bool custom_color = false;
    if (color1 != 0xFF0000) {
      custom_color = true;
    }

    uint8_t bytes_per_pixel = (bits_per_pixel+7)/8;
    uint8_t *map = img.bytes;
    
    if (centered) {
      x = get_xres()/2 + x -(img.width/2);
      y = get_yres()/2 + y -(img.height/2);
    }

    //draw_rectangle(0, 0, yres, xres, 0x0);
    for (unsigned i = 0; i < img.height; i++) {
      if (i % module_size == 0) {
        for (unsigned j = 0; j < img.width; j++) {
          if (j % module_size == 0) {
            color = 0;
            for (unsigned byte = 0; byte < bytes_per_pixel; byte++) {
              color += map[img.width*i + j + byte] << byte * 8;
            }
            if (color != 0xFF0000) {
              if (custom_color) {
                color = color1;
              }
              draw_pixel_bg_buffer(x+(j/module_size), y+(i/module_size), color);
            }            
          }
          map += bytes_per_pixel-1;
        }
      } else {
        map += (bytes_per_pixel-1) * img.width;
      }
    }
}

unsigned get_letter_width_normal(char s) {
  unsigned letter_width;
  if (s == 'i') {
    letter_width = 19;
  } else if (s == 'f') {
    letter_width = 30;
  } else if (s == 'g') {
    letter_width = 38;
  } else if (s == 'l') {
    letter_width = 20;
  } else if (s == 'm') {
    letter_width = 48;
  } else if (s == 't') {
    letter_width = 30;
  } else if (s == 'v') {
    letter_width = 32;
  } else if (s == 'w') {
    letter_width = 48;
  } else if (s == 'B') {
    letter_width = 38;
  } else if (s == 'D') {
    letter_width = 40;
  } else if (s == 'G') {
    letter_width = 38;
  } else if (s == 'H') {
    letter_width = 38;
  } else if (s == 'K') {
    letter_width = 38;
  } else if (s == 'M') {
    letter_width = 45;
  } else if (s == 'O') {
    letter_width = 41;
  } else if (s == 'Q') {
    letter_width = 46;
  } else if (s == 'R') {
    letter_width = 38;
  } else if (s == 'P') {
    letter_width = 40;
  } else if (s == 'U') {
    letter_width = 41;
  } else if (s == 'V') {
    letter_width = 39;
  } else if (s == 'W') {
    letter_width = 53;
  } else if (s == 'Y') {
    letter_width = 38;
  } else if (s == 'Z') {
    letter_width = 40;
  } else if (s == ' ') {
    letter_width = 30;
  } else {
    letter_width = 35;
  }

  letter_width -= 5;
  return letter_width;
}


unsigned get_string_width_normal(char *s, unsigned length) {
  unsigned res = 0;
  for (unsigned i = 0; i < length; i++) {
    res += get_letter_width_normal(s[i]);
  }
  return res;
}

void draw_string_centered(char* s, int ssize, uint16_t x, uint16_t y, uint16_t max_length_per_line, uint32_t color, char * relative_size) {
  /*
    This function draws the string centered with the x specified in the arguments.
  */
  unsigned module_size = 1;
  if (strcmp(relative_size, "small") == 0) {
    module_size = 2;
  } else if (strcmp(relative_size, "smaller") == 0) {
    module_size = 3;
  }
  x = x - (get_string_width_normal(s, ssize)/(module_size*2));
  draw_string(s, ssize, x, y, max_length_per_line, color, relative_size);
}


void draw_string(char* s, int ssize, uint16_t x, uint16_t y, uint16_t max_length_per_line, uint32_t color, char * relative_size){
  uint16_t xtmp = x;
  max_length_per_line += x;
  unsigned letter_width;
  unsigned module_size = 1;
  if (strcmp(relative_size, "small") == 0) {
    module_size = 2;
  } else if (strcmp(relative_size, "smaller") == 0) {
    module_size = 3;
  }
  for(int i = 0; i < ssize; i++){ 
    letter_width = get_letter_width_normal(s[i]);
    letter_width = letter_width/module_size;

    if (s[i] == ' ' && i < ssize){
      xtmp += letter_width;
    } else if (i < ssize) {
      if (xtmp < max_length_per_line - 35){
        draw_pixmap(get_letter(s[i]), xtmp, y, false, color, relative_size);
        xtmp += letter_width;
      } else {
        y += 72/module_size;
        xtmp = x;
        draw_pixmap(get_letter(s[i]), xtmp, y, false, color, relative_size);
        xtmp += letter_width;
      }
    }
  }
}

void* get_double_buffer() {
  return double_buffer;
}
void* get_background_buffer() {
  return background_buffer;
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
