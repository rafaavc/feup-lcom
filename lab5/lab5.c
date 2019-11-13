// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  vbe_mode_info_t vbe_info;

  vbe_get_mode_info(mode, &vbe_info);

  int r;

  struct minix_mem_range mr;
  memset(&mr, 0, sizeof(mr));	/* zero the structure */
  

  // Allow memory mapping 

  unsigned int phys_addr = vbe_info.PhysBasePtr;
  unsigned int size = vbe_info.XResolution*vbe_info.YResolution*(vbe_info.BitsPerPixel/8);
  
  mr.mr_base = (phys_bytes) phys_addr;	
  mr.mr_limit = phys_addr + size;  

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);


  // Map memory 
  /*

  void * video_mem;

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, size);

  if(video_mem == MAP_FAILED)
    panic("couldn't map video memory");
  
  */

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

  tickdelay(micros_to_ticks(delay*1000000));

  vg_exit();
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  /* To be completed */
  printf("%s(0x%03X, %u, %u, %u, %u, 0x%08x): under construction\n",
         __func__, mode, x, y, width, height, color);

  return 1;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  /* To be completed */
  printf("%s(0x%03x, %u, 0x%08x, %d): under construction\n", __func__,
         mode, no_rectangles, first, step);

  return 1;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  /* To be completed */
  printf("%s(%8p, %u, %u): under construction\n", __func__, xpm, x, y);

  return 1;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
