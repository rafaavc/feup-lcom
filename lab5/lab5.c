// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>
#include "video.h"
#include "keyboard.h"
#include "Macros.h"
#include "utils.h"
//#include "sprite.h"

// Any header files included below this line should have been created by you

extern void* video_mem;
extern uint8_t kbd_code, timer_counter;


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
  if (set_vbe_mode(mode) != 0) return 1;

  tickdelay(micros_to_ticks(delay*1000000));

  if (vg_exit() != 0) return 1;
  return 0;
}

void (kbc_ih)(){
  uint8_t status_data;
  bool error = false;

  if (util_sys_inb(STATUS_REG, &status_data) != 0) error = true;  // Reads the Status Reg

  if (status_data & (BIT(7) | BIT(6) | BIT(5))) error = true; // Checks parity error, Timeout error and Mouse data

  if (status_data & BIT(0)){ // Makes sure that Output buffer is full
    if (util_sys_inb(OUT_BUF, &kbd_code) != 0) error = true;
  }

  if (kbd_code == LARGEST_NUM)
    kbd_code = 0;
  if (error)
    kbd_code = 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  
  if (vg_init(mode) == NULL) return 1;

  draw_rectangle(x, y, height, width, color);

  int ipc_status;   // gets ipc_status
  int r;   // return value of driver receive
  message msg;
  uint8_t irq_set = BIT(0); // Keyboard's IRQ

  if (kbd_subscribe_int(& irq_set) != 0) return 1;  // Subscribes keyboard interruptions

  while (kbd_code != ESC_break)    //   Program exits when break code of escape key is read
  {
    kbd_code = 0;    //  Resets kbd_code

    if ( (r = driver_receive(ANY, &msg, &ipc_status) != 0))
    {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set)
          {
            (kbc_ih)();
          }
      }
    }
  }  // end of interrupt loop

  if (kbd_unsubscribe_int() != 0) return 1;   // Unsubscribing keyboard interruptions

  vg_exit();
 
  return 1;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {

  if (vg_init(mode) == NULL) return 1;

  uint16_t rectangle_width = get_xres()/no_rectangles, rectangle_height = get_yres()/no_rectangles;
  uint32_t color;

  uint8_t bits_per_pixel = get_bits_per_pixel(), red_mask = get_red_mask(), blue_mask = get_blue_mask(), green_mask = get_green_mask();


  
  for (unsigned int i = 0; i < no_rectangles; i++) {
    for (unsigned int j = 0; j < no_rectangles; j++) {

      if (mode == INDEXED_MODE) {
        color = (first + ((i) * no_rectangles + (j)) * step) % (1 << bits_per_pixel);
      } else {
        color = (first + (j + i) * step) % (1 << blue_mask);
        color += ((first >> blue_mask) + i * step) % (1 << (green_mask)) << blue_mask;
        color += ((first >> (blue_mask + green_mask)) + j * step) % (1 << (red_mask)) << (blue_mask + green_mask);
      }
      draw_rectangle(rectangle_width*j, rectangle_height*i, rectangle_height, rectangle_width, color);
    }
  }


  int ipc_status;   // gets ipc_status
  int r;   // return value of driver receive
  message msg;
  uint8_t irq_set = BIT(0); // Keyboard's IRQ

  if (kbd_subscribe_int(& irq_set) != 0) return 1;  // Subscribes keyboard interruptions

  while (kbd_code != ESC_break)    //   Program exits when break code of escape key is read
  {
    kbd_code = 0;    //  Resets kbd_code

    if ( (r = driver_receive(ANY, &msg, &ipc_status) != 0))
    {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set)
          {
            (kbc_ih)();
          }
      }
    }
  }  // end of interrupt loop

  if (kbd_unsubscribe_int() != 0) return 1;   // Unsubscribing keyboard interruptions

  vg_exit();

  return 1;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  
  if (set_vbe_mode(INDEXED_MODE) != 0) return 1;

  draw_pixmap(xpm, x, y);

  int ipc_status;   // gets ipc_status
  int r;   // return value of driver receive
  message msg;
  uint8_t irq_set = BIT(0); // Keyboard's IRQ

  if (kbd_subscribe_int(& irq_set) != 0) return 1;  // Subscribes keyboard interruptions

  while (kbd_code != ESC_break)    //   Program exits when break code of escape key is read
  {
    kbd_code = 0;    //  Resets kbd_code

    if ( (r = driver_receive(ANY, &msg, &ipc_status) != 0))
    {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set)
          {
            (kbc_ih)();
          }
      }
    }
  }  // end of interrupt loop

  if (kbd_unsubscribe_int() != 0) return 1;   // Unsubscribing keyboard interruptions

  vg_exit();

  return 1;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
    
  if (set_vbe_mode(INDEXED_MODE) != 0) return 1;
  //if (timer_set_frequency(0, fr_rate) != 0)
    //return 1;


  // Interrupt handling variables
  int ipc_status;   // gets ipc_status
  int r;   // return value of driver receive
  message msg;
  uint8_t irq_kbd = BIT(0); // Keyboard's IRQ
  uint8_t irq_timer0 = BIT(1); // Timer0's IRQ
  unsigned int frame_counter = 0;

  draw_pixmap(xpm, xi, yi);
  if (speed > 0){
    if (xi != xf) {
      if (xf-xi >= speed) {
        xi += speed;
      } else if (xi -xf >= speed){
        xi -= speed;
      } else {
        xi = xf;
      }
    }
    if (yi != yf) {
      if (yf-yi >= speed) {
        yi += speed;
      } else if (yi-yf >= speed) { 
        yi -= speed;
      } else {
        yi = yf;
      }
    }
  } else {
    if (xi != xf) {
      if (xf-xi >= 1) {
        xi += 1;
      } else if (xi -xf >= 1){
        xi -= 1;
      } else {
        xi = xf;
      }
    }
    if (yi != yf) {
      if (yf-yi >= 1) {
        yi += 1;
      } else if (yi-yf >= 1) { 
        yi -= 1;
      } else {
        yi = yf;
      }
    }
  }

  if (kbd_subscribe_int(& irq_kbd) != 0) return 1;  // Subscribes KBD interruptions
  if (timer_subscribe_int(& irq_timer0) != 0) return 1;  // Subscribes Timer0 interruptions


  while (kbd_code != ESC_break)  //  Exits program when break code of escape key is read
  {
    kbd_code = 0;
    if ( (r = driver_receive(ANY, &msg, &ipc_status) != 0))
    {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status))
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_kbd)   // KDB interrupt received
          {
            (kbc_ih)();
          }

          if (msg.m_notify.interrupts & irq_timer0)   // Timer0 interrupt received
          {
            timer_int_handler();
            if (speed > 0){
              if (timer_counter % (sys_hz()/fr_rate) == 0){
                draw_pixmap(xpm, xi, yi);
                if (xi != xf) {
                  if (xf-xi >= speed) {
                    xi += speed;
                  } else if (xi -xf >= speed){
                    xi -= speed;
                  } else {
                    xi = xf;
                  }
                }
                if (yi != yf) {
                  if (yf-yi >= speed) {
                    yi += speed;
                  } else if (yi-yf >= speed) { 
                    yi -= speed;
                  } else {
                    yi = yf;
                  }
                }
              }
            } else {
              if (timer_counter % (sys_hz()/fr_rate) == 0){
                frame_counter++;
              }
              if (frame_counter%(-speed) == 0){
                draw_pixmap(xpm, xi, yi);
                if (xi != xf) {
                  if (xf-xi >= 1) {
                    xi += 1;
                  } else if (xi -xf >= 1){
                    xi -= 1;
                  } else {
                    xi = xf;
                  }
                }
                if (yi != yf) {
                  if (yf-yi >= 1) {
                    yi += 1;
                  } else if (yi-yf >= 1) { 
                    yi -= 1;
                  } else {
                    yi = yf;
                  }
                }
              }
            }
          }

          break;

        default:
          break;
      }
    }
  }

  if (kbd_unsubscribe_int() != 0) return 1;  // unsubscribes KBD interrupts
  if (timer_unsubscribe_int() != 0) return 1;  // unsubscribes Timer0 interrupts
  printf("xi(%d) == xf(%d), yi(%d) == yf(%d)\n", xi, xf, yi, yf);
  vg_exit();

  return 0;
}

int(video_test_controller)() {
  /*
  vbe_mode_info_t vbe_info;
  vbe_get_mode_info(0x0, &vbe_info);

  struct vg_vbe_contr_info_t info;

*/
return 1;
}
