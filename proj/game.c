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

extern uint8_t kbd_code, timer_counter, mouse_code, bytes_read[];
extern int mouse_xvariance, mouse_yvariance, hook_id_mouse;

unsigned grid_height = 23, grid_width = 37;

bool on = true, added_mouse_events_main_menu = false, added_mouse_events_pause = false, added_mouse_events_tutorial = false;

const unsigned triggers_mm_no = 3, triggers_p_no = 3, triggers_t_no = 1;

MouseTrigger * mouse_triggers_main_menu[3];
MouseTrigger * mouse_triggers_pause[3];
MouseTrigger * mouse_triggers_tutorial[1];

enum Event current_event = NO_EVENT;

                    //      text    text_over   other    (Will make it easier to change all colors to dark/light theme)
uint32_t color_palette[] = {WHITE, DIRTY_WHITE, BLACK};

/*void create_game() {
    
}
*/

void draw_text_button(bool *added_mouse_events, MouseTrigger * mouse_trigger[], bool needs_to_set_added_mouse_events, enum Event event, char* s, int ssize, uint16_t x, uint16_t y, uint16_t max_length_per_line, uint32_t color, uint32_t color_over, char * relative_size) {
  unsigned module_size = 1, height = N_LETTER_H;
  if (strcmp(relative_size, "small") == 0) {
    module_size = 2;
    height = S_LETTER_H;
  } else if (strcmp(relative_size, "smaller") == 0) {
    module_size = 3;
    height = SR_LETTER_H;
  }

  if (color == PREDEF_COLOR)
    color = color_palette[0];
  if (color_over == PREDEF_COLOR)
    color_over = color_palette[1];

  if (*added_mouse_events && mt_get_mouse_over(*mouse_trigger)) {
    draw_string_centered(s, ssize, x, y, max_length_per_line, color_over, relative_size);
  } else {
    draw_string_centered(s, ssize, x, y, max_length_per_line, color, relative_size);
    if (!(*added_mouse_events)) {
      *mouse_trigger = create_mouse_trigger(x - (get_string_width_normal(s, ssize)/(module_size*2)), y, get_string_width_normal(s, ssize), height, event);
      if (needs_to_set_added_mouse_events) {
        *added_mouse_events = true;
      }
    }
  }
}

void execute_event(enum State *s, Tile * tiles[], unsigned tile_no) {
  switch (current_event) {
    case NO_EVENT:
      break;
    case START_GAME:
      *s = GAME;
      current_event = NO_EVENT;
      break;
    case QUIT_GAME:
      free_allocated_memory(tiles, tile_no);
      on = false;
      break;
    case OPEN_TUTORIAL:
      *s = TUTORIAL;
      break;
    case END_GAME:
      clear_game();
      *s = MAIN_MENU;
      break;
    case OPEN_MAIN_MENU:
      *s = MAIN_MENU;
      break;
    case PAUSE_GAME:
      *s = PAUSE;
      break;
    default:
      break;
  }
}

void draw_main_menu() {
  draw_pixmap(get_background(), 0, 0, true, PREDEF_COLOR, "");

  draw_text_button(&added_mouse_events_main_menu, &mouse_triggers_main_menu[0], false, START_GAME, "Start Game", 10, get_xres()/2, 200, 800, PREDEF_COLOR, PREDEF_COLOR, "");

  draw_text_button(&added_mouse_events_main_menu, &mouse_triggers_main_menu[1], false, OPEN_TUTORIAL, "Tutorial", 8, get_xres()/2, 275, 800, PREDEF_COLOR, PREDEF_COLOR, "");

  draw_text_button(&added_mouse_events_main_menu, &mouse_triggers_main_menu[2], true, QUIT_GAME, "Quit", 4, get_xres()/2, 350, 800, PREDEF_COLOR, PREDEF_COLOR, "");

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_tutorial() {
  draw_pixmap(get_background(), 0, 0, true, PREDEF_COLOR, "");

  draw_string_centered("TUTORIAL", 8, get_xres()/2, 30, 800, color_palette[0], "");

  draw_text_button(&added_mouse_events_tutorial, &mouse_triggers_tutorial[0], true, OPEN_MAIN_MENU, "Return", 6, get_xres()/2, get_yres()-70, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_pause_menu() {
  draw_pixmap(get_background(), 0, 0, true, PREDEF_COLOR, "");
  draw_string_centered("PAUSED", 6, get_xres()/2, 150, 800, color_palette[0], "");

  draw_text_button(&added_mouse_events_pause, &mouse_triggers_pause[0], false, START_GAME, "Resume Game", 11, get_xres()/2, 280, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  draw_text_button(&added_mouse_events_pause, &mouse_triggers_pause[1], false, END_GAME, "End Game", 8, get_xres()/2, 320, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  draw_text_button(&added_mouse_events_pause, &mouse_triggers_pause[2], true, QUIT_GAME, "Quit", 4, get_xres()/2, 360, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void handle_keyboard_events(enum State *s) {
  switch (*s) {
    case MAIN_MENU:
      if (kbd_code == ESC_break) {
        current_event = QUIT_GAME;
      }
      break;
    case TUTORIAL:
      if (kbd_code == ESC_break) {
        current_event = OPEN_MAIN_MENU;
      }
    case PAUSE:
      if (kbd_code == ESC_break) {
        current_event = END_GAME;
      }
      break;
    case GAME:
      if (kbd_code == ESC_break) {
        current_event = PAUSE_GAME;
      }
      break;
    default:
      break;
  }
}

void handle_mouse_events(enum State *s, struct packet *mouse_data) {
  switch (*s) {
    case MAIN_MENU:
      for (unsigned i = 0; i < triggers_mm_no; i++) {
        if (check_mouse_overlap(mouse_triggers_main_menu[i])) {
          mt_set_mouse_over(mouse_triggers_main_menu[i]);
          break;
        }
      }
      if (mouse_data->lb) {
        for (unsigned i = 0; i < triggers_mm_no; i++) {
          if (check_mouse_overlap(mouse_triggers_main_menu[i])) {
            current_event = mt_get_event(mouse_triggers_main_menu[i]);
            break;
          }
        }
      }
      break;
    case TUTORIAL:
      for (unsigned i = 0; i < triggers_t_no; i++) {
        if (check_mouse_overlap(mouse_triggers_tutorial[i])) {
          mt_set_mouse_over(mouse_triggers_tutorial[i]);
          break;
        }
      }
      if (mouse_data->lb) {
        for (unsigned i = 0; i < triggers_t_no; i++) {
          if (check_mouse_overlap(mouse_triggers_tutorial[i])) {
            current_event = mt_get_event(mouse_triggers_tutorial[i]);
            break;
          }
        }
      }
      break;
    case PAUSE:
      for (unsigned i = 0; i < triggers_p_no; i++) {
        if (check_mouse_overlap(mouse_triggers_pause[i])) {
          mt_set_mouse_over(mouse_triggers_pause[i]);
          break;
        }
      }
      if (mouse_data->lb) {
        for (unsigned i = 0; i < triggers_p_no; i++) {
          if (check_mouse_overlap(mouse_triggers_pause[i])) {
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

void free_allocated_memory(Tile * tiles[], unsigned tile_no) {
  for (unsigned i = 0; i < triggers_mm_no; i++) {
    free(mouse_triggers_main_menu[i]);
  }
  for (unsigned i = 0; i < triggers_p_no; i++) {
    free(mouse_triggers_pause[i]);
  }
  for (unsigned i = 0; i < triggers_t_no; i++) {
    free(mouse_triggers_tutorial[i]);
  }
  for (unsigned i = 0; i < tile_no; i++) {
    free(tiles[i]);
  }
  free(get_double_buffer());
}

void clear_game() {

}

void update_game() {

}

void draw_game(Tile * tiles[], const unsigned tile_no, Player * players[]) {
  draw_pixmap(get_background(), 0, 0, true, PREDEF_COLOR, "");
  for (unsigned int i = 0; i < tile_no; i++) {
    draw_tile(tiles[i]);
  }
  
  draw_player(players[0]);
  draw_player(players[1]);
  //move(player1, 'w');
  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
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

  const unsigned tile_no = 9;
  Tile * tiles[9];
  create_board(tiles, tile_no);

  Player * players[2];
  players[0] = create_player(get_xres()/2 - 70, get_yres()/2 - 20, get_red_ball_animation(), 0);
  players[1] = create_player(get_xres()/2 + 70, get_yres()/2 - 20, get_red_ball_animation(), 3);

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
            execute_event(&s, tiles, tile_no);
          }
          if (msg.m_notify.interrupts & irq_timer0) {   // Timer0 interrupt received
            timer_int_handler();
            if (timer_counter % (sys_hz()/fr_rate) == 0){
              frame_counter++;
              switch(s) {
                case MAIN_MENU:
                  draw_main_menu();
                  break;
                case TUTORIAL:
                  draw_tutorial();
                  break;
                case PAUSE:
                  draw_pause_menu();
                  break;
                case GAME:
                  update_game();
                  draw_game(tiles, tile_no, players);
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
              execute_event(&s, tiles, tile_no);
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

