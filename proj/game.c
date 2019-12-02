#include <stdio.h>
#include <lcom/lcf.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "video.h"
#include "keyboard.h"
#include "timer.h"
#include "Macros.h"
#include "utils.h"
#include "xpm_includes.h"
#include "mouse.h"
#include "board_tile.h"
#include "game.h"
#include "menus.h"

extern uint8_t kbd_code, timer_counter;
extern int mouse_xvariance, mouse_yvariance;

extern uint8_t mouse_code;
extern uint8_t bytes_read[];
extern int hook_id_mouse;

unsigned grid_height = 23;
unsigned grid_width = 37;

void draw_game() {
    printf("draw_game\n");
}

/*void create_game() {
    
}
*/
int game() {
  int ipc_status;   // gets ipc_status
  int r;   // return value of driver receive
  message msg;

  uint8_t irq_kbd = BIT(0), irq_timer0 = BIT(1), irq_mouse = BIT(2); // IRQ's of keyboard, timer and mouse
  unsigned int frame_counter = 0;

  uint8_t fr_rate = 60;

  unsigned int byte_counter = 0;
  bool menu = true;
  struct packet mouse_data;

  load_pixmaps();

  if ((mouse_subscribe_int)(& irq_mouse) != 0) return 1;  // Subscribes mouse interruptions

  sys_irqdisable(&hook_id_mouse);

  //  ler command byte e ver se mouse está a 1 & enable stream mode
  send_command_to_mouse(ENABLE_DATA_REPORTING);

  sys_irqenable(&hook_id_mouse);

  if (kbd_subscribe_int(& irq_kbd) != 0) return 1;  // Subscribes keyboard interruptions
  if (timer_subscribe_int(& irq_timer0) != 0) return 1;  // Subscribes Timer0 interruptions

  if (vg_init(0x115) == NULL) return 1;


  const unsigned int tile_no = 9;
  Tile * tiles[9];

  create_board(tiles, tile_no);

  enum State s; 
  s = MAIN_MENU;

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
          if (msg.m_notify.interrupts & irq_kbd)
          {
            kbc_ih();
          }
          if (msg.m_notify.interrupts & irq_timer0) {   // Timer0 interrupt received
            timer_int_handler();
            if (timer_counter % (sys_hz()/fr_rate) == 0){
              frame_counter++;
              switch(s) {
                case MAIN_MENU:
                  draw_pixmap(get_background(), 0, 0, true, PREDEF_COLOR);
                  draw_string("Start Game", 10, 250, 200, 800, WHITE);
                  draw_string("Tutorial", 8, 300, 275, 800, WHITE);
                  draw_string("Quit", 4, 350, 350, 800, WHITE);
                  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR);
                  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
                  break;
                case PAUSE:
                  break;
                case GAME:
                  draw_pixmap(get_background(), 0, 0, true, PREDEF_COLOR);
                  for (unsigned int i = 0; i < tile_no; i++) {
                    draw_tile(tiles[i]);
                  }
                  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR);
                  draw_pixmap(get_letter('1'), 100, 100, false, PREDEF_COLOR);
                  draw_string("Random try", 10, 50, 200, 200, PREDEF_COLOR);
                  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
                  break;
                default:
                  break;
              }
                        
            }
          }
          if (msg.m_notify.interrupts & irq_mouse)
          {
            mouse_ih();

            if ((mouse_code & BIT(3)) && byte_counter == 0){       //If BIT(3) != 1 certainly not first byte
              mouse_data.bytes[0] = mouse_code;
              byte_counter++;
            }
            else if (byte_counter == 1){
              mouse_data.bytes[1] = mouse_code;
              byte_counter++;
            }
            else if (byte_counter == 2){
              mouse_data.bytes[2] = mouse_code;
              parse_packet(&mouse_data);
              byte_counter = 0;
            }
            if (menu && mouse_data.lb){
              if (mouse_xvariance > 350 && mouse_xvariance < 550 && mouse_yvariance > 200 && mouse_yvariance < 250){
                s = GAME;
                menu = false;
              } else if (mouse_xvariance > 300 && mouse_xvariance < 500 && mouse_yvariance > 275 && mouse_yvariance < 325){
                continue;
                //s = Tutorial;
              } else if (mouse_xvariance > 350 && mouse_xvariance < 450 && mouse_yvariance > 350 && mouse_yvariance < 400){
                kbd_code = ESC_break;
              }
            }
          }
      }
    }
  }  // end of interrupt loop
  if ((mouse_unsubscribe_int)() != 0) return 1;   // Unsubscribing mouse interruptions
  if (kbd_unsubscribe_int() != 0) return 1;   // Unsubscribing keyboard interruptions
  if (timer_unsubscribe_int() != 0) return 1;  // unsubscribes Timer0 interrupts

  vg_exit();

  return 0;
}

