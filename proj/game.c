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
#include "game.h"
#include "menus.h"
#include "mouse_trigger.h"

extern uint8_t kbd_code, timer_counter;
extern int mouse_xvariance, mouse_yvariance;

extern uint8_t mouse_code;
extern uint8_t bytes_read[];
extern int hook_id_mouse;

unsigned grid_height = 23;
unsigned grid_width = 37;

bool on = true;
bool added_mouse_events_main_menu = false, added_mouse_events_pause = false;

MouseTrigger * mouse_triggers_main_menu[3];
MouseTrigger * mouse_triggers_pause[3];

enum Event current_event = NO_EVENT;

                    //      text    text_over   other    (Will make it easier to change all colors to dark/light theme)
uint32_t color_palette[] = {WHITE, DIRTY_WHITE, BLACK};

/*void create_game() {
    
}
*/

void execute_event(enum State *s) {
  switch (current_event) {
    case NO_EVENT:
      break;
    case START_GAME:
      *s = GAME;
      current_event = NO_EVENT;
      break;
    case QUIT_GAME:
      on = false;
      break;
    case OPEN_TUTORIAL:
      break;
    default:
      break;
  }
}

void draw_main_menu() {
  draw_pixmap(get_background(), 0, 0, true, PREDEF_COLOR, "");


/*

    NEED TO MAKE FUNCTION TO DRAW STRINGS WITH EVENT LISTENING AND HOVER CAPABILITIES

*/

  if (added_mouse_events_main_menu && mt_get_mouse_over(mouse_triggers_main_menu[0])) {
    draw_string_centered("Start Game", 10, get_xres()/2, 200, 800, color_palette[1], "");
  } else {
    draw_string_centered("Start Game", 10, get_xres()/2, 200, 800, color_palette[0], "");
    if (!added_mouse_events_main_menu) {
      mouse_triggers_main_menu[0] = create_mouse_trigger((get_xres()/2) - (get_string_width_normal("Start Game", 10)/2), 200, get_string_width_normal("Start Game", 10), N_LETTER_H, START_GAME);
    }
  }


  if (added_mouse_events_main_menu && mt_get_mouse_over(mouse_triggers_main_menu[1])) {
    draw_string_centered("Tutorial", 8, get_xres()/2, 275, 800, color_palette[1], "");
  } else {
    draw_string_centered("Tutorial", 8, get_xres()/2, 275, 800, color_palette[0], "");
    if (!added_mouse_events_main_menu) {
      mouse_triggers_main_menu[1] = create_mouse_trigger((get_xres()/2) - (get_string_width_normal("Tutorial", 8)/2), 275, get_string_width_normal("Tutorial", 8), N_LETTER_H, OPEN_TUTORIAL);
    }
  }

  if (added_mouse_events_main_menu && mt_get_mouse_over(mouse_triggers_main_menu[2])) {
    draw_string_centered("Quit", 4, get_xres()/2, 350, 800, color_palette[1], "");
  } else {
    draw_string_centered("Quit", 4, get_xres()/2, 350, 800, color_palette[0], "");
    if (!added_mouse_events_main_menu) {
      mouse_triggers_main_menu[2] = create_mouse_trigger((get_xres()/2) - (get_string_width_normal("Quit", 4)/2), 350, get_string_width_normal("Quit", 4), N_LETTER_H, QUIT_GAME);
      added_mouse_events_main_menu = true;
    }
  }


  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_pause_menu() {
  draw_pixmap(get_background(), 0, 0, true, PREDEF_COLOR, "");
  draw_string_centered("PAUSED", 6, get_xres()/2, 150, 800, color_palette[0], "");


  if (added_mouse_events_pause && mt_get_mouse_over(mouse_triggers_pause[0])) {
    draw_string_centered("Resume Game", 11, get_xres()/2, 280, 800, color_palette[1], "small");
  } else {
    draw_string_centered("Resume Game", 11, get_xres()/2, 280, 800, color_palette[0], "small");
    if (!added_mouse_events_pause) {
      mouse_triggers_pause[0] = create_mouse_trigger((get_xres()/2) - (get_string_width_normal("Resume Game", 11)/(2*2)), 280, get_string_width_normal("Start Game", 11)/2, S_LETTER_H, START_GAME);
    }
  }

  if (added_mouse_events_pause && mt_get_mouse_over(mouse_triggers_pause[1])) {
    draw_string_centered("Quit", 4, get_xres()/2, 330, 800, color_palette[1], "small");
  } else {
    draw_string_centered("Quit", 4, get_xres()/2, 330, 800, color_palette[0], "small");
    if (!added_mouse_events_pause) {
      mouse_triggers_pause[1] = create_mouse_trigger((get_xres()/2) - (get_string_width_normal("Quit", 4)/(2*2)), 330, get_string_width_normal("Quit", 4)/2, S_LETTER_H, QUIT_GAME);
      added_mouse_events_pause = true;
    }
  }
  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void handle_keyboard_events(enum State *s) {
  switch (*s) {
    case MAIN_MENU: case PAUSE:
      if (kbd_code == ESC_break) {
        on = false;
      }
      break;
    case GAME:
      if (kbd_code == ESC_break) {
        *s = PAUSE;
      }
      break;
    default:
      break;
  }
}



void handle_mouse_events(enum State *s, struct packet *mouse_data) {
  switch (*s) {
    case MAIN_MENU:
      for (unsigned i = 0; i < 3; i++) {
        if ((unsigned) mouse_xvariance > mt_get_xpos(mouse_triggers_main_menu[i]) && (unsigned) mouse_yvariance > mt_get_ypos(mouse_triggers_main_menu[i]) && (unsigned) mouse_xvariance < mt_get_endxpos(mouse_triggers_main_menu[i]) && (unsigned) mouse_yvariance < mt_get_endypos(mouse_triggers_main_menu[i])) {
          mt_set_mouse_over(mouse_triggers_main_menu[i]);
          break;
        }
      }
      if (mouse_data->lb) {
        for (unsigned i = 0; i < 3; i++) {
          if ((unsigned) mouse_xvariance > mt_get_xpos(mouse_triggers_main_menu[i]) && (unsigned) mouse_yvariance > mt_get_ypos(mouse_triggers_main_menu[i]) && (unsigned) mouse_xvariance < mt_get_endxpos(mouse_triggers_main_menu[i]) && (unsigned) mouse_yvariance < mt_get_endypos(mouse_triggers_main_menu[i])) {
            current_event = mt_get_event(mouse_triggers_main_menu[i]);
            break;
          }
        }
      }
      break;
    case PAUSE:
      for (unsigned i = 0; i < 2; i++) {
        if ((unsigned) mouse_xvariance > mt_get_xpos(mouse_triggers_pause[i]) && (unsigned) mouse_yvariance > mt_get_ypos(mouse_triggers_pause[i]) && (unsigned) mouse_xvariance < mt_get_endxpos(mouse_triggers_pause[i]) && (unsigned) mouse_yvariance < mt_get_endypos(mouse_triggers_pause[i])) {
          mt_set_mouse_over(mouse_triggers_pause[i]);
          break;
        }
      }
      if (mouse_data->lb) {
        for (unsigned i = 0; i < 2; i++) {
          if ((unsigned) mouse_xvariance > mt_get_xpos(mouse_triggers_pause[i]) && (unsigned) mouse_yvariance > mt_get_ypos(mouse_triggers_pause[i]) && (unsigned) mouse_xvariance < mt_get_endxpos(mouse_triggers_pause[i]) && (unsigned) mouse_yvariance < mt_get_endypos(mouse_triggers_pause[i])) {
            current_event = mt_get_event(mouse_triggers_pause[i]);
            break;
          }
        }
      }
      break;
    default:
      break;
  }
}

void update_game() {

}

void draw_game(Tile * tiles[], const unsigned tile_no) {
  draw_pixmap(get_background(), 0, 0, true, PREDEF_COLOR, "");
  for (unsigned int i = 0; i < tile_no; i++) {
    draw_tile(tiles[i]);
  }
  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  draw_pixmap(get_letter('1'), 100, 100, false, color_palette[0], "");
  draw_string("Random try", 10, 50, 200, 200, color_palette[0], "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

int game() {
  int ipc_status;   // gets ipc_status
  int r;   // return value of driver receive
  message msg;

  uint8_t irq_kbd = BIT(0), irq_timer0 = BIT(1), irq_mouse = BIT(2); // IRQ's of keyboard, timer and mouse
  unsigned int frame_counter = 0;

  uint8_t fr_rate = 60;

  unsigned int byte_counter = 0;
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

  while (on)    //   Program exits when break code of escape key is read
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
            handle_keyboard_events(&s);
          }
          if (msg.m_notify.interrupts & irq_timer0) {   // Timer0 interrupt received
            timer_int_handler();
            if (timer_counter % (sys_hz()/fr_rate) == 0){
              frame_counter++;
              switch(s) {
                case MAIN_MENU:
                  draw_main_menu();
                  break;
                case PAUSE:
                  draw_pause_menu();
                  break;
                case GAME:
                  update_game();
                  draw_game(tiles, tile_no);
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
              handle_mouse_events(&s, &mouse_data);
              execute_event(&s);
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

