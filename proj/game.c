#include <stdio.h>
#include <lcom/lcf.h>
#include <minix/driver.h>
#include <sys/mman.h> 
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "video.h"
#include "keyboard.h"
#include "timer.h"
#include "utils.h"
#include "xpm_includes.h"
#include "mouse.h"
#include "game.h"
#include "menus.h"
#include "sp.h"
#include "rtc.h"


extern uint8_t kbd_code, timer_counter, mouse_code, bytes_read[], p1_kbd_code, rtc[];
extern int mouse_xvariance, mouse_yvariance, hook_id_mouse, p1_mouse_xvariance, p1_mouse_yvariance;
extern bool p1_mouse_lb, opponent_quit;
bool mouse_lb_pressed = false;

uint8_t irq_com1 = 0, irq_com2 = 5;

unsigned grid_height = 23, grid_width = 37, current_player = 0, move_count = 0; // current_player -> 0 or 1 for player 1 or 2

bool on = true, added_mouse_events_main_menu = false, added_mouse_events_pause = false, added_mouse_events_tutorial = false, added_mouse_events_choosing_menu = false, game_ends = false, added_mouse_events_settings = false;

const unsigned triggers_mm_no = 4, triggers_p_no = 2, triggers_t_no = 1, triggers_g_no = 2, triggers_cm_no = 3, triggers_s_no = 3;

MouseTrigger * mouse_triggers_main_menu[3];
MouseTrigger * mouse_triggers_pause[2];
MouseTrigger * mouse_triggers_choosing_menu[3];
MouseTrigger * mouse_triggers_tutorial[1];
MouseTrigger * mouse_triggers_game[2] = {NULL, NULL}; // the the block(s) that need to be moved
MouseTrigger * mouse_triggers_settings[3];

Player * main_menu_animated_ball;

unsigned error = 0, error_timer = 0, play_time = 0, timer_counter_play = 0;
unsigned paused_time[2];  // holds at 0 the seconds that the counter has before the game being paused and at 1 the timer ticks that it was at

// When the player disconnects during serial port mode, delay is needed so we can send the command to the opponent
unsigned turn_off_delay_time_count = 0;
bool turn_off = false;


int blocks_to_move[2][2] = {{DEFAULT_BLOCK_COORDINATE, DEFAULT_BLOCK_COORDINATE}, {DEFAULT_BLOCK_COORDINATE, DEFAULT_BLOCK_COORDINATE}};  // blocks_to_move will hold the coordinates of the blocks that need to be moved ([[i, j], [i1, j1]])
Tile * t_being_dragged = NULL;
bool only_one_move = false; unsigned tile_move_count = 0;

enum Event current_event = NO_EVENT;

                    //      text    text_over   other
uint32_t color_palette[] = {WHITE, DIRTY_WHITE, BLACK};

// RTC
extern bool dark_mode;
extern bool override_dark_mode;

// Serial port variables
bool multi_computer = false; // Whether playing serial port mode or not
bool host = false; // Whether this machine is the host, case multi_computer == true
bool sp_on = false; // Whether the sp is subscribed or not
bool connected = false;  // Whether the two players are connected
extern bool received_player_data;   // whether this pc has received data from the other player
bool sp_used = false;   // Whether the serial port was used during this instance of the app

charqueue * player1_name;
charqueue * player2_name;
bool enter_key = false;

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
      *mouse_trigger = create_mouse_trigger(x - (get_string_width_normal(s, ssize)/(module_size*2)), y, get_string_width_normal(s, ssize)/module_size, height, event);
      if (needs_to_set_added_mouse_events) {
        *added_mouse_events = true;
      }
    }
  }
}

void execute_event(enum State *s, Tile * tiles[], unsigned tile_no, Player * players[], int board[BOARD_SIZE][BOARD_SIZE]) {
  int prev_i, prev_j;
  switch (current_event) {
    case NO_EVENT:
      break;
    case MAKE_DARK_MODE_AUTO:
      override_dark_mode = false;
      make_dm_auto();
      current_event = NO_EVENT;
      break;
    case MAKE_DARK_MODE_CUSTOM_TOGGLE:
      override_dark_mode = true;
      if (!dark_mode) {
        dark_mode = true;
      } else {
        dark_mode = false;
      }
      make_dm_custom(dark_mode);
      draw_bg_buffer(get_background(), 0, 0, true, PREDEF_COLOR, ""); // updates the bg
      current_event = NO_EVENT;
      break;
    case OPEN_SETTINGS:
      *s = SETTINGS;
      current_event = NO_EVENT;
      break;
    case START_CHOOSING_MODE:
      *s = CHOOSING_MODE;
      current_event = NO_EVENT;
      break;
    case START_GAME_NO_SP:
      *s = PLAYER1_PROMPT;
      play_time = 0, timer_counter_play = 0;
      current_event = NO_EVENT;
      multi_computer = false;
      break;
    case START_GAME_SP:
      *s = CHOOSING_HOST;
      multi_computer = true;
      current_event = NO_EVENT;
      break;
    case RESUME_GAME:
      *s = GAME;
      current_event = NO_EVENT;
      play_time = paused_time[0];
      timer_counter_play = paused_time[1];
      if (multi_computer && !opponent_quit && !turn_off) {
        transmit_critical_event("resume_game");
      }
      break;
    case CONNECT_SP:
      if (!sp_on) {
        sp_init();
        sp_used = true;
      }
      sp_send_character('a');
      *s = WAITING_FOR_CONNECTION;
      current_event = NO_EVENT;
      //sp_sync();
      break;
    case QUIT_GAME:
      free_allocated_memory(tiles, tile_no, players);
      on = false;
      current_event = NO_EVENT;
      break;
    case OPEN_TUTORIAL:
      *s = TUTORIAL;
      current_event = NO_EVENT;
      break;
    case END_GAME:
      clear_game(tiles, tile_no, players, board);
      *s = MAIN_MENU;
      current_event = NO_EVENT;
      if (sp_on) {
        turn_off = true;
        if (!opponent_quit) {
          transmit_critical_event("disconnect");
        }
      }
      break;
    case OPEN_MAIN_MENU:
      *s = MAIN_MENU;
      current_event = NO_EVENT;
      break;
    case PAUSE_GAME:
      if (multi_computer && !opponent_quit && !turn_off) {
        transmit_critical_event("pause_game");
      }
      *s = PAUSE;
      current_event = NO_EVENT;
      paused_time[0] = play_time;
      paused_time[1] = timer_counter_play;
      break;
    case PLAYER_MOVE_W:
      prev_i = p_get_i(players[current_player]);
      prev_j = p_get_j(players[current_player]);
      if (move(players[current_player], 'w', board)) {
        if(blocks_to_move[0][0] == DEFAULT_BLOCK_COORDINATE) {
          blocks_to_move[0][0] = prev_i;
          blocks_to_move[0][1] = prev_j;
        } else if (blocks_to_move[1][0] == DEFAULT_BLOCK_COORDINATE) {
          blocks_to_move[1][0] = prev_i;
          blocks_to_move[1][1] = prev_j;
        } else {
          printf("An error has occurred. (blocks to move are filled and player is trying to move)");
        }
      }
      current_event = NO_EVENT;
      break;
    case PLAYER_MOVE_A:
      prev_i = p_get_i(players[current_player]);
      prev_j = p_get_j(players[current_player]);
      if (move(players[current_player], 'a', board)) {
        if(blocks_to_move[0][0] == DEFAULT_BLOCK_COORDINATE) {
          blocks_to_move[0][0] = prev_i;
          blocks_to_move[0][1] = prev_j;
        } else if (blocks_to_move[1][0] == DEFAULT_BLOCK_COORDINATE) {
          blocks_to_move[1][0] = prev_i;
          blocks_to_move[1][1] = prev_j;
        } else {
          printf("An error has occurred. (blocks to move are filled and player is trying to move)");
        }
      }
      current_event = NO_EVENT;
      break;
    case PLAYER_MOVE_S:
      prev_i = p_get_i(players[current_player]);
      prev_j = p_get_j(players[current_player]);
      if (move(players[current_player], 's', board)) {
        if(blocks_to_move[0][0] == DEFAULT_BLOCK_COORDINATE) {
          blocks_to_move[0][0] = prev_i;
          blocks_to_move[0][1] = prev_j;
        } else if (blocks_to_move[1][0] == DEFAULT_BLOCK_COORDINATE) {
          blocks_to_move[1][0] = prev_i;
          blocks_to_move[1][1] = prev_j;
        } else {
          printf("An error has occurred. (blocks to move are filled and player is trying to move)");
        }
      }
      current_event = NO_EVENT;
      break;
    case PLAYER_MOVE_D:
      prev_i = p_get_i(players[current_player]);
      prev_j = p_get_j(players[current_player]);
      if (move(players[current_player], 'd', board)) {
        if(blocks_to_move[0][0] == DEFAULT_BLOCK_COORDINATE) {
          blocks_to_move[0][0] = prev_i;
          blocks_to_move[0][1] = prev_j;
        } else if (blocks_to_move[1][0] == DEFAULT_BLOCK_COORDINATE) {
          blocks_to_move[1][0] = prev_i;
          blocks_to_move[1][1] = prev_j;
        } else {
          printf("An error has occurred. (blocks to move are filled and player is trying to move)");
        }
      }
      current_event = NO_EVENT;
      break;
    default:
      break;
  }
}

void draw_main_menu() {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  draw_pixmap(get_logo(), get_xres()/2 - 230, 55, false, color_palette[0], "");

  unsigned block_ball_posx = get_xres()/2 + 150;
  unsigned block_ball_posy = 130;
  draw_pixmap(get_block(), block_ball_posx, block_ball_posy, false, PREDEF_COLOR, "");
  draw_player_custom(main_menu_animated_ball, block_ball_posx + 14, block_ball_posy - 45);

  draw_text_button(&added_mouse_events_main_menu, &mouse_triggers_main_menu[0], false, START_CHOOSING_MODE, "Start Game", 10, get_xres()/2, 275, 800, PREDEF_COLOR, PREDEF_COLOR, "");

  draw_text_button(&added_mouse_events_main_menu, &mouse_triggers_main_menu[1], false, OPEN_TUTORIAL, "Help", 4, get_xres()/2, 350, 800, PREDEF_COLOR, PREDEF_COLOR, "");
  
  draw_text_button(&added_mouse_events_main_menu, &mouse_triggers_main_menu[2], false, OPEN_SETTINGS, "Settings", 8, get_xres()/2, 425, 800, PREDEF_COLOR, PREDEF_COLOR, "");

  draw_text_button(&added_mouse_events_main_menu, &mouse_triggers_main_menu[3], true, QUIT_GAME, "Quit", 4, get_xres()/2, 530, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_tutorial() {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  draw_string_centered("HELP", 4, get_xres()/2, 30, 800, color_palette[0], "");

  draw_string_centered("In game, use WASD to move your character. You have two moves.", 61, get_xres()/2, 120, 800, color_palette[0], "smaller");
  
  draw_pixmap(get_wasd(), get_xres()/2 - 60, 155, false, color_palette[0], "");

  draw_string_centered("If you only want to move once, press ENTER after the first movement.", 68, get_xres()/2, 290, 800, color_palette[0], "smaller");
  draw_string_centered("After you make your move(s),", 28, get_xres()/2, 320, 800, color_palette[0], "smaller");
  draw_string_centered("you need to move the tiles you went over to a position that suits your strategy.", 80, get_xres()/2, 350, 800, color_palette[0], "smaller");
  draw_string_centered("Press ESC to pause the game.", 28, get_xres()/2, 380, 800, color_palette[0], "smaller");

  draw_string_centered("The objective of the game is to get at the same position as your opponent,", 74, get_xres()/2, 440, 800, color_palette[0], "smaller");
  draw_string_centered("of course without letting him get to your position first.", 57, get_xres()/2, 470, 800, color_palette[0], "smaller");

  draw_text_button(&added_mouse_events_tutorial, &mouse_triggers_tutorial[0], true, OPEN_MAIN_MENU, "Return", 6, get_xres()/2, get_yres()-70, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_settings() {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  draw_string_centered("Settings", 8, get_xres()/2, 150, 800, color_palette[0], "");
  uint32_t dma_color = PREDEF_COLOR, dma_color_over = PREDEF_COLOR;

  if (!override_dark_mode) {
    dma_color = 0xf9b909;
    dma_color_over = dma_color;
  }

  draw_text_button(&added_mouse_events_settings, &mouse_triggers_settings[0], false, MAKE_DARK_MODE_AUTO, "Make Dark Mode automatic (based on time of day)", 47, get_xres()/2, 280, 800, dma_color, dma_color_over, "small");
  draw_string_centered("If dark mode is automatic, it will turn on at 8PM and off at 8AM.", 65, get_xres()/2, 320, 800, color_palette[0], "smaller");
  if (!dark_mode) {
    draw_text_button(&added_mouse_events_settings, &mouse_triggers_settings[1], false, MAKE_DARK_MODE_CUSTOM_TOGGLE, "Turn on Dark Mode", 17, get_xres()/2, 360, 800, PREDEF_COLOR, PREDEF_COLOR, "small");
  } else {
    draw_text_button(&added_mouse_events_settings, &mouse_triggers_settings[1], false, MAKE_DARK_MODE_CUSTOM_TOGGLE, "Turn off Dark Mode", 18, get_xres()/2, 360, 800, PREDEF_COLOR, PREDEF_COLOR, "small");
  }

  draw_text_button(&added_mouse_events_settings, &mouse_triggers_settings[2], true, OPEN_MAIN_MENU, "Return", 6, get_xres()/2, 530, 800, PREDEF_COLOR, PREDEF_COLOR, "small");
  
  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_pause_menu() {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  draw_string_centered("PAUSED", 6, get_xres()/2, 150, 800, color_palette[0], "");
  if (multi_computer)
    draw_text_button(&added_mouse_events_pause, &mouse_triggers_pause[0], false, RESUME_GAME, "Resume Game", 11, get_xres()/2, 280, 800, PREDEF_COLOR, PREDEF_COLOR, "small");
  else
    draw_text_button(&added_mouse_events_pause, &mouse_triggers_pause[0], false, RESUME_GAME, "Resume Game", 11, get_xres()/2, 280, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  draw_text_button(&added_mouse_events_pause, &mouse_triggers_pause[1], true, END_GAME, "End Game", 8, get_xres()/2, 320, 800, PREDEF_COLOR, PREDEF_COLOR, "small");
  
  if (multi_computer) {
    if (opponent_quit) {
      draw_string_centered("Opponent disconnected", 21, get_xres()/2, 500, 800, color_palette[0], "small");
    }
  }

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_choosing_mode_menu() {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  draw_string_centered("CHOOSE GAME MODE", 16, get_xres()/2, 150, 800, color_palette[0], "");

  draw_text_button(&added_mouse_events_choosing_menu, &mouse_triggers_choosing_menu[0], false, START_GAME_NO_SP, "Multiplayer on this computer", 28, get_xres()/2, 280, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  if (!sp_used) {
    draw_text_button(&added_mouse_events_choosing_menu, &mouse_triggers_choosing_menu[1], false, START_GAME_SP, "Multiplayer on two computers", 28, get_xres()/2, 320, 800, PREDEF_COLOR, PREDEF_COLOR, "small");
  } else {
    draw_string_centered("If you want to play on two computers again, please reload the game.", 67, get_xres()/2, 327, 800, color_palette[0], "smaller");
  }
  draw_text_button(&added_mouse_events_choosing_menu, &mouse_triggers_choosing_menu[2], true, END_GAME, "Return", 6, get_xres()/2, 360, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_choosing_host_menu() {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  draw_string_centered("CHOOSE PLAYER", 13, get_xres()/2, 150, 800, color_palette[0], "");

  draw_string_centered("Type 1 or 2.", 12, get_xres()/2, 300, 800, color_palette[0], "small");
  
  draw_string_centered("IMPORTANT! You need to make sure your opponent chooses a different player.", 74, get_xres()/2, 350, 800, color_palette[0], "smaller");

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_waiting_for_connection() {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  draw_string_centered("Waiting for connection...", 25, get_xres()/2, 300, 800, color_palette[0], "small");

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_string_input(unsigned x, unsigned y, uint32_t color, charqueue * input) {
  char * input_str;
  input_str = charqueue_to_string(input);
  unsigned ssize = strlen(input_str);

  unsigned string_width = get_string_width_normal(input_str, ssize) / 2;

  draw_string_centered(input_str, ssize, x, y - 35, 800, color, "small");
  if (string_width < 100) {
    draw_hline(x - 50, y, 100, color);
    draw_hline(x - 50, y+1, 100, color);
    draw_hline(x - 50, y+2, 100, color);
  } else {
    draw_hline(x - (string_width/2), y, string_width, color);
    draw_hline(x - (string_width/2), y+1, string_width, color);
    draw_hline(x - (string_width/2), y+2, string_width, color);
  }
  free(input_str);
}

void draw_player1_prompt() {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  draw_string_centered("Player 1", 8, get_xres()/2, 150, 800, color_palette[0], "");
  draw_string_centered("Insert your name:", 17, get_xres()/2, 250, 800, color_palette[0], "small");
  
  draw_string_input(get_xres()/2, 370, color_palette[0], player1_name);

  if (multi_computer) {
    if (enter_key) {
      draw_string_centered("Waiting for opponent...", 23, get_xres()/2, 500, 800, color_palette[0], "small");
    }
    if (opponent_quit) {
      draw_string_centered("Opponent disconnected", 21, get_xres()/2, 550, 800, color_palette[0], "small");
    }
  }

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_player2_prompt() {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  draw_string_centered("Player 2", 8, get_xres()/2, 150, 800, color_palette[0], "");
  draw_string_centered("Insert your name:", 17, get_xres()/2, 250, 800, color_palette[0], "small");
  
  draw_string_input(get_xres()/2, 370, color_palette[0], player2_name);

  if (multi_computer) {
    if (enter_key) {
      draw_string_centered("Waiting for opponent...", 23, get_xres()/2, 500, 800, color_palette[0], "small");
    }
    if (opponent_quit) {
      draw_string_centered("Opponent disconnected", 21, get_xres()/2, 550, 800, color_palette[0], "small");
    }
  }

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void get_letter_input(charqueue * q) {
  if (enter_key) return;
  if (strlen(charqueue_to_string(q)) == MAX_NAME_SIZE) {
    enter_key = true;
    return;
  }
  switch (kbd_code) {
    case A_break:
      charqueue_push(q, 'A');
      break;
    case B_break:
      charqueue_push(q, 'B');
      break;
    case C_break:
      charqueue_push(q, 'C');
      break;
    case D_break:
      charqueue_push(q, 'D');
      break;
    case E_break:
      charqueue_push(q, 'E');
      break;
    case F_break:
      charqueue_push(q, 'F');
      break;
    case G_break:
      charqueue_push(q, 'G');
      break;
    case H_break:
      charqueue_push(q, 'H');
      break;
    case I_break:
      charqueue_push(q, 'I');
      break;
    case J_break:
      charqueue_push(q, 'J');
      break;
    case K_break:
      charqueue_push(q, 'K');
      break;
    case L_break:
      charqueue_push(q, 'L');
      break;
    case M_break:
      charqueue_push(q, 'M');
      break;
    case N_break:
      charqueue_push(q, 'N');
      break;
    case O_break:
      charqueue_push(q, 'O');
      break;
    case P_break:
      charqueue_push(q, 'P');
      break;
    case Q_break:
      charqueue_push(q, 'Q');
      break;
    case R_break:
      charqueue_push(q, 'R');
      break;
    case S_break:
      charqueue_push(q, 'S');
      break;
    case T_break:
      charqueue_push(q, 'T');
      break;
    case U_break:
      charqueue_push(q, 'U');
      break;
    case V_break:
      charqueue_push(q, 'V');
      break;
    case W_break:
      charqueue_push(q, 'W');
      break;
    case X_break:
      charqueue_push(q, 'X');
      break;
    case Y_break:
      charqueue_push(q, 'Y');
      break;
    case Z_break:
      charqueue_push(q, 'Z');
      break;
    case BACKSPACE_break:
      charqueue_remove_last(q);
      break;
    case ENTER_break:
      if (!charqueue_empty(q)) {
        enter_key = true;
      }
      break;
    default:
      //printf("That key isn't mapped, %x\n", kbd_code);
      break;
  }
}

void handle_keyboard_events(enum State *s, Player * players[]) {
  switch (*s) {
    case MAIN_MENU:
      if (kbd_code == ESC_break) {
        current_event = QUIT_GAME;
      }
      break;
    case SETTINGS:
      if (kbd_code == ESC_break) {
        current_event = OPEN_MAIN_MENU;
      }
      break;
    case TUTORIAL:
      if (kbd_code == ESC_break) {
        current_event = OPEN_MAIN_MENU;
      }
      break;
    case PAUSE:
      if (kbd_code == ESC_break) {
        current_event = END_GAME;
      }
      break;
    case CHOOSING_MODE:
      if (kbd_code == ESC_break) {
        current_event = END_GAME;
      }
      break;
    case CHOOSING_HOST:
      if (kbd_code == ESC_break) {
        current_event = OPEN_MAIN_MENU;
      } else if (kbd_code == K1_break) {
        host = true;
        current_event = CONNECT_SP;
      } else if (kbd_code == K2_break) {
        host = false;
        current_event = CONNECT_SP;
      }
      break;
    case WAITING_FOR_CONNECTION:
      if (kbd_code == ESC_break) {
        current_event = END_GAME;
      } 
      break;
    case PLAYER1_PROMPT:
      if (kbd_code == ESC_break) {
        current_event = END_GAME;
      } else {
        get_letter_input(player1_name);
      }
      break;
    case PLAYER2_PROMPT:
      if (kbd_code == ESC_break) {
        current_event = END_GAME;
      } else {
        get_letter_input(player2_name);
      }
      break;
    case GAME:
      if (multi_computer) {
        if (kbd_code == ESC_break) {
          current_event = PAUSE_GAME;
          paused_time[0] = timer_counter_play;
          paused_time[1] = play_time;
        }
        if ((host && (current_player == 0)) || (!host && (current_player == 1))) {
          if (kbd_code == W_break && !game_ends) {
            current_event = PLAYER_MOVE_W;
          } else if (kbd_code == A_break && !game_ends) {
            current_event = PLAYER_MOVE_A;
          } else if (kbd_code == S_break && !game_ends) {
            current_event = PLAYER_MOVE_S;
          } else if (kbd_code == D_break && !game_ends) {
            current_event = PLAYER_MOVE_D;
          } else if (kbd_code == ENTER_break && move_count == 1){
            move_count++;
            only_one_move = true;
          }
        } else if ((host && (current_player == 1)) || (!host && (current_player == 0))) {
          if (p1_kbd_code == W_break && !game_ends) {
            current_event = PLAYER_MOVE_W;
          } else if (p1_kbd_code == A_break && !game_ends) {
            current_event = PLAYER_MOVE_A;
          } else if (p1_kbd_code == S_break && !game_ends) {
            current_event = PLAYER_MOVE_S;
          } else if (p1_kbd_code == D_break && !game_ends) {
            current_event = PLAYER_MOVE_D;
          } else if (p1_kbd_code == ENTER_break && move_count == 1){
            move_count++;
            only_one_move = true;
          }
        }
      } else {
        if (kbd_code == ESC_break) {
          current_event = PAUSE_GAME;
          paused_time[0] = timer_counter_play;
          paused_time[1] = play_time;
        } else if (kbd_code == W_break && !game_ends) {
          current_event = PLAYER_MOVE_W;
        } else if (kbd_code == A_break && !game_ends) {
          current_event = PLAYER_MOVE_A;
        } else if (kbd_code == S_break && !game_ends) {
          current_event = PLAYER_MOVE_S;
        } else if (kbd_code == D_break && !game_ends) {
          current_event = PLAYER_MOVE_D;
        } else if (kbd_code == ENTER_break && move_count == 1){
          move_count++;
          only_one_move = true;
        }
      }

      break;
    case GAME_MOVING_BLOCKS: case GAME_BLOCKS_MOVED:
      if (kbd_code == ESC_break) {
        current_event = PAUSE_GAME;
        paused_time[0] = timer_counter_play;
        paused_time[1] = play_time;
      }
    default:
      break;
  }
}

void change_tile_position(int i1, int j1, int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[]) {
  //print_game_board(board);
  for (unsigned i = 0; i < BOARD_SIZE; i++) {
    for (unsigned j = 0; j < BOARD_SIZE; j++) {
      if (tiles[board[i][j]] == t_being_dragged) {
        int n = board[i][j];
        board[i][j] = -1;
        board[i1][j1] = n;
      }
    }
  }
}

void handle_mouse_events(enum State *s, struct packet *mouse_data, int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[]) {
  unsigned n;
  if (!mouse_data->lb) {
    mouse_lb_pressed = false;
  } 
  switch (*s) {
    case MAIN_MENU:
      for (unsigned i = 0; i < triggers_mm_no; i++) {
        if (check_mouse_overlap(mouse_triggers_main_menu[i])) {
          mt_set_mouse_over(mouse_triggers_main_menu[i]);
          break;
        }
      }
      if (mouse_data->lb && !mouse_lb_pressed) {
        for (unsigned i = 0; i < triggers_mm_no; i++) {
          if (check_mouse_overlap(mouse_triggers_main_menu[i])) {
            current_event = mt_get_event(mouse_triggers_main_menu[i]);
            timer_counter_play = 0;
            play_time = 0;
            break;
          }
        }
      }
      break;
    case SETTINGS:
      for (unsigned i = 0; i < triggers_s_no; i++) {
        if (check_mouse_overlap(mouse_triggers_settings[i])) {
          mt_set_mouse_over(mouse_triggers_settings[i]);
          break;
        }
      }
      if (mouse_data->lb && !mouse_lb_pressed) {
        for (unsigned i = 0; i < triggers_s_no; i++) {
          if (check_mouse_overlap(mouse_triggers_settings[i])) {
            current_event = mt_get_event(mouse_triggers_settings[i]);
            timer_counter_play = 0;
            play_time = 0;
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
      if (mouse_data->lb && !mouse_lb_pressed) {
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
      if (mouse_data->lb && !mouse_lb_pressed) {
        for (unsigned i = 0; i < triggers_p_no; i++) {
          if (check_mouse_overlap(mouse_triggers_pause[i])) {
            current_event = mt_get_event(mouse_triggers_pause[i]);
            timer_counter_play = paused_time[0];
            play_time = paused_time[1];
            break;
          }
        }
      }
      break;
    case CHOOSING_MODE:
      for (unsigned i = 0; i < triggers_cm_no; i++) {
        if (check_mouse_overlap(mouse_triggers_choosing_menu[i])) {
          mt_set_mouse_over(mouse_triggers_choosing_menu[i]);
          break;
        }
      }
      if (mouse_data->lb && !mouse_lb_pressed) {
        for (unsigned i = 0; i < triggers_cm_no; i++) {
          if (check_mouse_overlap(mouse_triggers_choosing_menu[i])) {
            current_event = mt_get_event(mouse_triggers_choosing_menu[i]);
            break;
          }
        }
      }
      break;
    case GAME_MOVING_BLOCKS:
      n = triggers_g_no;
      if (only_one_move) {
        n--;
      }
      if (multi_computer && !mouse_lb_pressed) {
        if ((host && (current_player == 0)) || (!host && (current_player == 1))) {
          if (mouse_data->lb) {
            for (unsigned i = 0; i < n; i++) {
              if (check_mouse_overlap(mouse_triggers_game[i])) {
                if (mt_get_obj(mouse_triggers_game[i]) != NULL && t_being_dragged == NULL) {
                  t_being_dragged = (Tile *) mt_get_obj(mouse_triggers_game[i]);
                  start_dragging_tile(t_being_dragged);
                }
              }
            }
          }
          if (!mouse_data->lb) {
            if (t_being_dragged != NULL) {
              bool changed = false;
              for (unsigned i = 1; i < BOARD_SIZE-1; i++) {
                for (unsigned j = 1; j < BOARD_SIZE-1; j++) {
                  if (board[i][j] == -1) {
                    if ((board[i+1][j+1] != -1 && tiles[board[i+1][j+1]] != t_being_dragged) || (board[i+1][j-1] != -1 && tiles[board[i+1][j-1]] != t_being_dragged) || (board[i-1][j+1] != -1 && tiles[board[i-1][j+1]] != t_being_dragged) || (board[i-1][j-1] != -1 && tiles[board[i-1][j-1]] != t_being_dragged)) {
                      if (check_mouse_overlap(create_mouse_trigger(get_xres()/2 + (j*grid_width - (((BOARD_SIZE - 1) * grid_width) / 2)) - (grid_width/2), (get_yres()/2) + (i*grid_height - (((BOARD_SIZE - 1) * grid_height) / 2)) - (grid_height/2) - 20, grid_width, grid_height, NO_EVENT))) {
                        changed = true;
                        change_tile_position(i, j, board, tiles);
                        stop_dragging_tile(t_being_dragged, true);
                        t_being_dragged = NULL;
                        tile_move_count++;
                      }
                    }
                  }
                }
              }
              if (!changed) {
                stop_dragging_tile(t_being_dragged, false);
                t_being_dragged = NULL;
              }
            }
          }
        } else if ((host && (current_player == 1)) || (!host && (current_player == 0))) {
          if (p1_mouse_lb) {
            for (unsigned i = 0; i < n; i++) {
              if (p1_check_mouse_overlap(mouse_triggers_game[i])) {
                if (mt_get_obj(mouse_triggers_game[i]) != NULL && t_being_dragged == NULL) {
                  t_being_dragged = (Tile *) mt_get_obj(mouse_triggers_game[i]);
                  start_dragging_tile(t_being_dragged);
                }
              }
            }
          }
          if (!p1_mouse_lb) {
            if (t_being_dragged != NULL) {
              bool changed = false;
              for (unsigned i = 1; i < BOARD_SIZE-1; i++) {
                for (unsigned j = 1; j < BOARD_SIZE-1; j++) {
                  if (board[i][j] == -1) {
                    if ((board[i+1][j+1] != -1 && tiles[board[i+1][j+1]] != t_being_dragged) || (board[i+1][j-1] != -1 && tiles[board[i+1][j-1]] != t_being_dragged) || (board[i-1][j+1] != -1 && tiles[board[i-1][j+1]] != t_being_dragged) || (board[i-1][j-1] != -1 && tiles[board[i-1][j-1]] != t_being_dragged)) {
                      if (p1_check_mouse_overlap(create_mouse_trigger(get_xres()/2 + (j*grid_width - (((BOARD_SIZE - 1) * grid_width) / 2)) - (grid_width/2), (get_yres()/2) + (i*grid_height - (((BOARD_SIZE - 1) * grid_height) / 2)) - (grid_height/2) - 20, grid_width, grid_height, NO_EVENT))) {
                        changed = true;
                        change_tile_position(i, j, board, tiles);
                        stop_dragging_tile(t_being_dragged, true);
                        t_being_dragged = NULL;
                        tile_move_count++;
                      }
                    }
                  }
                }
              }
              if (!changed) {
                stop_dragging_tile(t_being_dragged, false);
                t_being_dragged = NULL;
              }
            }
          }
        }
      } else {
        if (mouse_data->lb && !mouse_lb_pressed) {
          for (unsigned i = 0; i < n; i++) {
            if (check_mouse_overlap(mouse_triggers_game[i])) {
              if (mt_get_obj(mouse_triggers_game[i]) != NULL && t_being_dragged == NULL) {
                t_being_dragged = (Tile *) mt_get_obj(mouse_triggers_game[i]);
                start_dragging_tile(t_being_dragged);
              }
            }
          }
        }
        if (!mouse_data->lb) {
          if (t_being_dragged != NULL) {
            bool changed = false;
            for (unsigned i = 1; i < BOARD_SIZE-1; i++) {
              for (unsigned j = 1; j < BOARD_SIZE-1; j++) {
                if (board[i][j] == -1) {
                  if ((board[i+1][j+1] != -1 && tiles[board[i+1][j+1]] != t_being_dragged) || (board[i+1][j-1] != -1 && tiles[board[i+1][j-1]] != t_being_dragged) || (board[i-1][j+1] != -1 && tiles[board[i-1][j+1]] != t_being_dragged) || (board[i-1][j-1] != -1 && tiles[board[i-1][j-1]] != t_being_dragged)) {
                    if (check_mouse_overlap(create_mouse_trigger(get_xres()/2 + (j*grid_width - (((BOARD_SIZE - 1) * grid_width) / 2)) - (grid_width/2), (get_yres()/2) + (i*grid_height - (((BOARD_SIZE - 1) * grid_height) / 2)) - (grid_height/2) - 20, grid_width, grid_height, NO_EVENT))) {
                      changed = true;
                      change_tile_position(i, j, board, tiles);
                      stop_dragging_tile(t_being_dragged, true);
                      t_being_dragged = NULL;
                      tile_move_count++;
                    }
                  }
                }
              }
            }
            if (!changed) {
              stop_dragging_tile(t_being_dragged, false);
              t_being_dragged = NULL;
            }
          }
        }
      }
      break;
    default:
      break;
  }
  if (mouse_data->lb) {
    mouse_lb_pressed = true;
  }
}

void free_allocated_memory(Tile * tiles[], unsigned tile_no, Player * players[]) {
  for (unsigned i = 0; i < triggers_mm_no; i++) {
    free(mouse_triggers_main_menu[i]);
  }
  for (unsigned i = 0; i < triggers_p_no; i++) {
    free(mouse_triggers_pause[i]);
  }
  for (unsigned i = 0; i < triggers_t_no; i++) {
    free(mouse_triggers_tutorial[i]);
  }
  for (unsigned i = 0; i < triggers_g_no; i++) {
    free(mouse_triggers_game[i]);
  }
  for (unsigned i = 0; i < triggers_cm_no; i++) {
    free(mouse_triggers_choosing_menu[i]);
  }
  for (unsigned i = 0; i < triggers_s_no; i++) {
    free(mouse_triggers_settings[i]);
  }
  for (unsigned i = 0; i < tile_no; i++) {
    free(tiles[i]);
  }
  for (unsigned i = 0; i < 2; i++) {
    free(players[i]);
  }
  free(player1_name);
  free(player2_name);
  free(get_background_buffer());
  free(get_double_buffer());
}

void clear_game(Tile * tiles[], unsigned tile_no, Player * players[2], int board[BOARD_SIZE][BOARD_SIZE]) {
  create_tiles(tiles, tile_no);
  create_board(board);

  players[0] = create_player(8, 6, get_red_ball_animation(), 0);
  players[1] = create_player(8, 10, get_blue_ball_animation(), 3);
  current_player = 0;
  opponent_quit = false;

  only_one_move = game_ends = false;
  tile_move_count = move_count = 0;
  blocks_to_move[0][0] = DEFAULT_BLOCK_COORDINATE;
  blocks_to_move[1][0] = DEFAULT_BLOCK_COORDINATE;
  blocks_to_move[0][1] = DEFAULT_BLOCK_COORDINATE;
  blocks_to_move[1][1] = DEFAULT_BLOCK_COORDINATE;
  p_set_last_movement(players[0],'x');
  p_set_last_movement(players[1],'x');

  if (player1_name != NULL) {
    charqueue_make_empty(player1_name);
    free(player1_name);
  }
  if (player2_name != NULL) {
    charqueue_make_empty(player2_name);
    free(player2_name);
  }

  player1_name = create_charqueue();
  player2_name = create_charqueue();
}

void update_game(Player * players[], int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[], enum State *s) {
  if ((only_one_move && tile_move_count == 1) || tile_move_count == 2) {
    *s = GAME_BLOCKS_MOVED;
  }
  if (((p_get_i(players[0]) == p_get_i(players[1])) && (p_get_j(players[0]) == p_get_j(players[1])))) {
    game_ends = true;
  } else if ((((play_time == PLAY_TIME) && ((*s) == GAME)) || ((play_time == PLAY_TIME_MOVING_BLOCKS) && ((*s) == GAME_MOVING_BLOCKS))) && !game_ends){
    game_ends = true;
    blocks_to_move[0][0] = DEFAULT_BLOCK_COORDINATE;
    blocks_to_move[0][1] = DEFAULT_BLOCK_COORDINATE;
    blocks_to_move[1][0] = DEFAULT_BLOCK_COORDINATE;
    blocks_to_move[1][1] = DEFAULT_BLOCK_COORDINATE;
    if (current_player == 0){
      current_player = 1;
    } else {
      current_player = 0;
    }
  } else if (opponent_quit) {
    game_ends = true;
    blocks_to_move[0][0] = DEFAULT_BLOCK_COORDINATE;
    blocks_to_move[0][1] = DEFAULT_BLOCK_COORDINATE;
    blocks_to_move[1][0] = DEFAULT_BLOCK_COORDINATE;
    blocks_to_move[1][1] = DEFAULT_BLOCK_COORDINATE;
    if (!host){
      current_player = 1;
    } else {
      current_player = 0;
    }
  } else {
    if (move_count == 2) {
      if (*s == GAME_BLOCKS_MOVED) {
        p_set_last_movement(players[current_player],'x');  // resets player's last move

        // toggles player
        if (current_player == 0) {
          current_player = 1;
        } else {
          current_player = 0;
        }

        tile_move_count = 0;    // resets count of tile movements
        only_one_move = false;  // resets whether the player did only one move
        move_count = 0;         // resets the move count
        play_time = 0;

        // resets the blocks to move to the predefined values
        blocks_to_move[0][0] = DEFAULT_BLOCK_COORDINATE;
        blocks_to_move[0][1] = DEFAULT_BLOCK_COORDINATE;
        blocks_to_move[1][0] = DEFAULT_BLOCK_COORDINATE;
        blocks_to_move[1][1] = DEFAULT_BLOCK_COORDINATE;

        // resets the mouse triggers
        free(mouse_triggers_game[0]);
        free(mouse_triggers_game[1]);
        mouse_triggers_game[0] = NULL;
        mouse_triggers_game[1] = NULL;

        // evolves the state machine
        *s = GAME;

      } else if (*s == GAME) {
        play_time = 0;

        // Getting the coordinates for the mouse trigger
        int x = blocks_to_move[0][1]*grid_width - (((BOARD_SIZE - 1) * grid_width) / 2);
        int y = blocks_to_move[0][0]*grid_height - (((BOARD_SIZE - 1) * grid_height) / 2);
        x = get_xres()/2 + x -(grid_width/2);
        y = get_yres()/2 + y -(grid_height/2);

        mouse_triggers_game[0] = create_mouse_trigger(x, y - 20, grid_width, grid_height, NO_EVENT);
        mt_set_obj(mouse_triggers_game[0], tiles[board[blocks_to_move[0][0]][blocks_to_move[0][1]]]);
        toggle_need_to_be_moved(tiles[board[blocks_to_move[0][0]][blocks_to_move[0][1]]]);

        if (blocks_to_move[1][0] != DEFAULT_BLOCK_COORDINATE) {
          // Getting the coordinates for the mouse trigger
          int x = blocks_to_move[1][1]*grid_width - (((BOARD_SIZE - 1) * grid_width) / 2);
          int y = blocks_to_move[1][0]*grid_height - (((BOARD_SIZE - 1) * grid_height) / 2);
          x = get_xres()/2 + x -(grid_width/2);
          y = get_yres()/2 + y -(grid_height/2);

          mouse_triggers_game[1] = create_mouse_trigger(x, y - 20, grid_width, grid_height, NO_EVENT);
          mt_set_obj(mouse_triggers_game[1], tiles[board[blocks_to_move[1][0]][blocks_to_move[1][1]]]);
          toggle_need_to_be_moved(tiles[board[blocks_to_move[1][0]][blocks_to_move[1][1]]]);
        }
        
        *s = GAME_MOVING_BLOCKS;
      }
    }
  }
}

void draw_game(int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[], const unsigned tile_no, Player * players[], enum State *s) {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  draw_pixmap(get_wasd(), 50, get_yres()-110, false, color_palette[0], "small");

  for (unsigned i = 0; i < BOARD_SIZE; i++) {
    for (unsigned j = 0; j < BOARD_SIZE; j++) {
      if (board[i][j] != -1) {
        if (!get_dragging(tiles[board[i][j]]) && tiles[board[i][j]] != t_being_dragged) {
          draw_tile(tiles[board[i][j]], grid_width*j, grid_height*i);
        }
      }
    }
  }
  if (t_being_dragged != NULL) {
    if (!multi_computer) {
      draw_tile(t_being_dragged, mouse_xvariance - grid_width, mouse_yvariance - grid_height);
    } else {
      if ((host && (current_player == 0)) || (!host && (current_player == 1))) {
        draw_tile(t_being_dragged, mouse_xvariance - grid_width, mouse_yvariance - grid_height);
      } else {
        draw_tile(t_being_dragged, p1_mouse_xvariance - grid_width, p1_mouse_yvariance - grid_height);
      }
    }
  }

  
  if (p_get_i(players[0]) > p_get_i(players[1])) {
    draw_player(players[1]);
    draw_player(players[0]);
  } else if (game_ends && (p_get_i(players[0]) == p_get_i(players[1]))){
    draw_player(players[current_player]);
  } else {
    draw_player(players[0]);
    draw_player(players[1]);
  }

  if (!game_ends){
    if ((*s) == GAME) {
      if (play_time < PLAY_TIME){
        char s[10];
        if (PLAY_TIME - play_time >= 10) {
          sprintf(s, "%d", PLAY_TIME - play_time);
        } else {
          sprintf(s, "0%d", PLAY_TIME - play_time);
        }
        if (current_player == 0) {
          draw_string(s, 2, 30, 100, 400, color_palette[0], "smaller");
        } else {
          draw_string(s, 2, 770 - (get_string_width_normal(s, strlen(s))/2), 100, 400, color_palette[0], "smaller");
        }
      } else {
        if (current_player == 0) {
          draw_string("0", 1, 30, 100, 400, color_palette[0], "smaller");
        } else {
          draw_string("0", 1, 770 - (get_string_width_normal("0", 1)/2), 100, 400, color_palette[0], "smaller");
        }
      }
    } else if ((*s) == GAME_MOVING_BLOCKS) {
      if (play_time < PLAY_TIME_MOVING_BLOCKS){
        char s[10];
        if (PLAY_TIME_MOVING_BLOCKS - play_time >= 10) {
          sprintf(s, "%d", PLAY_TIME_MOVING_BLOCKS - play_time);
        } else {
          sprintf(s, "0%d", PLAY_TIME_MOVING_BLOCKS - play_time);
        }
        if (current_player == 0) {
          draw_string(s, 2, 30, 100, 400, color_palette[0], "smaller");
        } else {
          draw_string(s, 2, 770 - (get_string_width_normal(s, strlen(s))/2), 100, 400, color_palette[0], "smaller");
        }
      } else {
        if (current_player == 0) {
          draw_string("0", 1, 30, 100, 400, color_palette[0], "smaller");
        } else {
          draw_string("0", 1, 770 - (get_string_width_normal("0", 1)/2), 100, 400, color_palette[0], "smaller");
        }
      }
    }
    switch(error){
    case 1:
      draw_string_centered("Can't return to initial position!", 33, get_xres()/2, 80, 800, color_palette[0], "small");
      break;
    default:
      break;
    }
  }
  char * p1_name = p_get_name(players[0]);
  char * p2_name = p_get_name(players[1]);
  draw_string(p1_name, strlen(p1_name), 30, 50, 400, color_palette[0], "small");
  draw_string(p2_name, strlen(p2_name), 770 - (get_string_width_normal(p2_name, strlen(p2_name))/2), 50, 400, color_palette[0], "small");
  
  if (error != 0 && error_timer == 120) {
    error = 0;
    error_timer = 0;
  }

  if (game_ends) {
    char *winner;
    if (current_player == 0) {
      winner = p1_name;
    } else {
      winner = p2_name;
    }
    draw_string_centered(winner, strlen(winner), get_xres()/2, 80, 800, color_palette[0], "");
    draw_string_centered("wins!", 5, get_xres()/2, 140, 800, color_palette[0], "small");
  }
  if (multi_computer) {
    if (opponent_quit) {
      draw_string_centered("Opponent disconnected", 21, get_xres()/2, 500, 800, color_palette[0], "small");
    } else {
      if ((host && (current_player == 1)) || (!host && (current_player == 0))) {
        draw_pixmap(get_mouse_secondary(), p1_mouse_xvariance, p1_mouse_yvariance, false, PREDEF_COLOR, "");
      }
    }
  }
  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}


int game() {
  int ipc_status;   // gets ipc_status
  int r;   // return value of driver receive
  message msg;

  uint8_t irq_kbd = BIT(0), irq_timer0 = BIT(1), irq_mouse = BIT(2), irq_rtc = 3; // IRQ's of keyboard, timer, mouse, rtc and com1
  unsigned int frame_counter = 0;

  uint8_t fr_rate = 60;

  unsigned int byte_counter = 0;
  struct packet mouse_data;


  load_pixmaps();

  /* IO Setup */
  // Mouse
  if (mouse_subscribe_int(& irq_mouse) != 0) return 1;  // Subscribes mouse interruptions
  sys_irqdisable(&hook_id_mouse);
  //  ler command byte e ver se mouse está a 1 & enable stream mode
  send_command_to_mouse(ENABLE_DATA_REPORTING);
  sys_irqenable(&hook_id_mouse);

  // RTC
  if (rtc_subscribe_int(&irq_rtc) != 0) return 1; // Subscries RTC interrupts on update

  // Keyboard
  if (kbd_subscribe_int(& irq_kbd) != 0) return 1;  // Subscribes keyboard interruptions

  // Timer
  if (timer_subscribe_int(& irq_timer0) != 0) return 1;  // Subscribes Timer0 interruptions

  // Video Card
  if (vg_init(0x115) == NULL) return 1;

  

  /* GAME Setup */

  // Gets the background buffer ready
  draw_bg_buffer(get_background(), 0, 0, true, PREDEF_COLOR, "");

  // Setting up the game
  const unsigned tile_no = 9;
  Tile * tiles[9];

  int board[BOARD_SIZE][BOARD_SIZE]; // Each of the board's positions will hold either -1 or the position of the tile in the tiles array that occupies the position

  Player * players[2];
  main_menu_animated_ball = create_player(0, 0, get_blue_ball_animation(), 0);

  // Initializing state machine
  enum State s; 
  s = MAIN_MENU;

  // Creating a clean slate game
  clear_game(tiles, tile_no, players, board);

  //print_game_board(board);

  /* Interrupt loop */
  while (on)
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
            if (!opponent_quit && !turn_off && sp_on && ((host && (current_player == 0)) || (!host && (current_player == 1)))) {
              transmit_kbd_code(kbd_code);
            }
            handle_keyboard_events(&s, players);
            execute_event(&s, tiles, tile_no, players, board);
          }
          if (sp_on && multi_computer) {
            if (msg.m_notify.interrupts & irq_com1) {
              if (multi_computer) {
                switch(sp_ih(COM1)) {
                  case 0:
                    break;
                  case 1:
                    handle_keyboard_events(&s, players);
                    execute_event(&s, tiles, tile_no, players, board);
                    break;
                  case 2:
                    handle_mouse_events(&s, &mouse_data, board, tiles);
                    execute_event(&s, tiles, tile_no, players, board);
                    break;
                  case 3:
                    if ((s == GAME) && !game_ends) {
                      current_event = PAUSE_GAME;
                      execute_event(&s, tiles, tile_no, players, board);
                    }
                    break;
                  case 4:
                    if ((s == PAUSE) && !game_ends) {
                      current_event = RESUME_GAME;
                      execute_event(&s, tiles, tile_no, players, board);
                    }
                    break;
                  default:
                    break;
                }
              } else {
                printf("Serial Port (COM1) interrupt, but not in 'multi computer mode'.\n");
              }
            }
            if (msg.m_notify.interrupts & irq_com2) {
              if (multi_computer) {
                switch(sp_ih(COM2)) {
                  case 0:
                    break;
                  case 1:
                    handle_keyboard_events(&s, players);
                    execute_event(&s, tiles, tile_no, players, board);
                    break;
                  case 2:
                    handle_mouse_events(&s, &mouse_data, board, tiles);
                    execute_event(&s, tiles, tile_no, players, board);
                    break;
                  case 3:
                    if ((s == GAME) && !game_ends) {
                      current_event = PAUSE_GAME;
                      execute_event(&s, tiles, tile_no, players, board);
                    }
                    break;
                  case 4:
                    if ((s == PAUSE) && !game_ends) {
                      current_event = RESUME_GAME;
                      execute_event(&s, tiles, tile_no, players, board);
                    }
                    break;
                  default:
                    break;
                }
              } else {
                printf("Serial Port (COM2) interrupt, but not in 'multi computer mode'.\n");
              }
            }
          }
          if (msg.m_notify.interrupts & irq_rtc){ // RTC interrupt received
            uint8_t *regC = malloc(sizeof(uint8_t));   // needed to do this because weird things were happening
            *regC = 0;
            if (read_rtc(REG_C, regC) != 0) return 1;
            if (*regC & C_UPDATE_FLAG) { // Update interrupt
                if (get_time_rtc()) {
                    if (!override_dark_mode) {
                      if ((rtc[0] < 8 || rtc[0] > 20) && !dark_mode) {
                          dark_mode = true;
                          draw_bg_buffer(get_background(), 0, 0, true, PREDEF_COLOR, ""); // updates the bg
                      }
                      else if ((rtc[0] > 8 && rtc[0] < 20) && dark_mode) {
                          dark_mode = false;
                          draw_bg_buffer(get_background(), 0, 0, true, PREDEF_COLOR, ""); // updates the bg
                      }
                    }
                    //printf("%d:%d:%d\n", rtc[0], rtc[1], rtc[2]);
                }
            }
            if (*regC & C_ALARM_FLAG) { // Alarm interrupt
                printf("RTC alarm interrupt.\n");
            }
            if (*regC & C_PERIODIC_FLAG) { // Periodic interrupt
                printf("RTC periodic interrupt.\n");
            }
            free(regC);
          }
          if (msg.m_notify.interrupts & irq_timer0) {   // Timer0 interrupt received
            timer_int_handler();
            if (timer_counter_play % sys_hz() == 0){
              play_time++;
              if (turn_off) {
                if (turn_off_delay_time_count >= 1) {
                  sp_terminate();
                  multi_computer = false;
                  connected = false;
                  turn_off = false;
                  turn_off_delay_time_count = 0;
                }
                turn_off_delay_time_count++;
              }

            }
            if (timer_counter % (sys_hz()/fr_rate) == 0){
              frame_counter++;
              if (error != 0)
                error_timer++;
              switch(s) {
                case MAIN_MENU:
                  draw_main_menu();
                  break;
                case TUTORIAL:
                  draw_tutorial();
                  break;
                case SETTINGS:
                  draw_settings();
                  break;
                case PAUSE:
                  draw_pause_menu();
                  break;
                case CHOOSING_MODE:
                  draw_choosing_mode_menu();
                  break;
                case CHOOSING_HOST:
                  draw_choosing_host_menu();
                  break;
                case WAITING_FOR_CONNECTION:
                  draw_waiting_for_connection();
                  if (connected) {
                    printf("Connection successful\n");
                    if (host) {
                      s = PLAYER1_PROMPT;
                    } else {
                      s = PLAYER2_PROMPT;
                    }
                    play_time = 0;
                  }
                  break;
                case PLAYER1_PROMPT:
                  draw_player1_prompt();
                  if (enter_key) {
                    p_set_name(players[0], charqueue_to_string(player1_name));
                    if (multi_computer) {
                      if (!opponent_quit && !turn_off) {
                        transmit_player_data();
                      }
                      if (received_player_data) {
                        p_set_name(players[1], charqueue_to_string(player2_name));
                        s = GAME;
                        enter_key = false;
                      }
                    } else {
                      s = PLAYER2_PROMPT;
                      enter_key = false;
                    }
                  }
                  play_time = 0;
                  break;
                case PLAYER2_PROMPT:
                  draw_player2_prompt();
                  if (enter_key) {
                    p_set_name(players[1], charqueue_to_string(player2_name));
                    if (multi_computer) {
                      if (!opponent_quit && !turn_off) {
                        transmit_player_data();
                      }
                      if (received_player_data) {
                        p_set_name(players[0], charqueue_to_string(player1_name));
                        s = GAME;
                        enter_key = false;
                      }
                    } else {
                      s = GAME;
                      enter_key = false;
                    }
                  }
                  play_time = 0;
                  break;
                case GAME: case GAME_MOVING_BLOCKS: case GAME_BLOCKS_MOVED:
                  update_game(players, board, tiles, &s);
                  draw_game(board, tiles, tile_no, players, &s);
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
              parse_packet(&mouse_data, true);
              byte_counter = 0;
              if (!opponent_quit && !turn_off && sp_on && ((host && (current_player == 0)) || (!host && (current_player == 1)))) {
                transmit_mouse_bytes(&mouse_data, mouse_xvariance, mouse_yvariance);
              }      
              handle_mouse_events(&s, &mouse_data, board, tiles);
              execute_event(&s, tiles, tile_no, players, board);
            }
          }
      }
    }
  }  // end of interrupt loop

  if (mouse_unsubscribe_int() != 0) return 1;   // Unsubscribing mouse interruptions
  if (kbd_unsubscribe_int() != 0) return 1;   // Unsubscribing keyboard interruptions
  if (timer_unsubscribe_int() != 0) return 1;  // unsubscribes Timer0 interrupts
  if (rtc_unsubscribe_int() != 0) return 1; // Unsubscriber RTC interrupts
  // Exiting video mode
  vg_exit();

  return 0;
}

