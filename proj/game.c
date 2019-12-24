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

extern uint8_t kbd_code, timer_counter, mouse_code, bytes_read[];
extern int mouse_xvariance, mouse_yvariance, hook_id_mouse;

unsigned grid_height = 23, grid_width = 37, current_player = 0, move_count = 0; // current_player -> 0 or 1 for player 1 or 2

bool on = true, added_mouse_events_main_menu = false, added_mouse_events_pause = false, added_mouse_events_tutorial = false, game_ends = false;

const unsigned triggers_mm_no = 3, triggers_p_no = 3, triggers_t_no = 1, triggers_g_no = 3;

MouseTrigger * mouse_triggers_main_menu[3];
MouseTrigger * mouse_triggers_pause[3];
MouseTrigger * mouse_triggers_tutorial[1];
MouseTrigger * mouse_triggers_game[3] = {NULL, NULL, NULL}; // The 0 position will be the pause button, the rest the block(s) that need to be moved

unsigned error = 0, error_timer = 0, play_time = 0, timer_counter_play = 0, paused_time[2];
int blocks_to_move[2][2] = {{DEFAULT_BLOCK_COORDINATE, DEFAULT_BLOCK_COORDINATE}, {DEFAULT_BLOCK_COORDINATE, DEFAULT_BLOCK_COORDINATE}};  // blocks_to_move will hold the coordinates of the blocks that need to be moved ([[i, j], [i1, j1]])
Tile * t_being_dragged = NULL;
bool only_one_move = false; unsigned tile_move_count = 0;

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
    case START_GAME:
      *s = GAME;
      current_event = NO_EVENT;
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
      clear_game();
      *s = MAIN_MENU;
      current_event = NO_EVENT;
      break;
    case OPEN_MAIN_MENU:
      *s = MAIN_MENU;
      current_event = NO_EVENT;
      break;
    case PAUSE_GAME:
      *s = PAUSE;
      current_event = NO_EVENT;
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

  draw_text_button(&added_mouse_events_main_menu, &mouse_triggers_main_menu[0], false, START_GAME, "Start Game", 10, get_xres()/2, 200, 800, PREDEF_COLOR, PREDEF_COLOR, "");

  draw_text_button(&added_mouse_events_main_menu, &mouse_triggers_main_menu[1], false, OPEN_TUTORIAL, "Tutorial", 8, get_xres()/2, 275, 800, PREDEF_COLOR, PREDEF_COLOR, "");

  draw_text_button(&added_mouse_events_main_menu, &mouse_triggers_main_menu[2], true, QUIT_GAME, "Quit", 4, get_xres()/2, 350, 800, PREDEF_COLOR, PREDEF_COLOR, "");

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_tutorial() {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  draw_string_centered("TUTORIAL", 8, get_xres()/2, 30, 800, color_palette[0], "");

  draw_text_button(&added_mouse_events_tutorial, &mouse_triggers_tutorial[0], true, OPEN_MAIN_MENU, "Return", 6, get_xres()/2, get_yres()-70, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void draw_pause_menu() {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  draw_string_centered("PAUSED", 6, get_xres()/2, 150, 800, color_palette[0], "");

  draw_text_button(&added_mouse_events_pause, &mouse_triggers_pause[0], false, START_GAME, "Resume Game", 11, get_xres()/2, 280, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  draw_text_button(&added_mouse_events_pause, &mouse_triggers_pause[1], false, END_GAME, "End Game", 8, get_xres()/2, 320, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  draw_text_button(&added_mouse_events_pause, &mouse_triggers_pause[2], true, QUIT_GAME, "Quit", 4, get_xres()/2, 360, 800, PREDEF_COLOR, PREDEF_COLOR, "small");

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

void handle_keyboard_events(enum State *s, Player * players[]) {
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
      break;
    case GAME_MOVING_BLOCKS: case GAME_BLOCKS_MOVED:
      if (kbd_code == ESC_break) {
        current_event = QUIT_GAME;
      }
    default:
      break;
  }
}

void change_tile_position(int i1, int j1, int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[]) {
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
            timer_counter_play = paused_time[0];
            play_time = paused_time[1];
            break;
          }
        }
      }
      break;
    case GAME_MOVING_BLOCKS:
      for (unsigned i = 1; i < triggers_g_no; i++) {
        if (check_mouse_overlap(mouse_triggers_game[i])) {
          //printf("Mouse over tile");
          break;
        }
      }
      if (mouse_data->lb) {
        unsigned n = triggers_g_no;
        if (only_one_move) {
          n--;
        }
        for (unsigned i = 1; i < n; i++) {
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
      break;
    default:
      break;
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
  for (unsigned i = 0; i < tile_no; i++) {
    free(tiles[i]);
  }
  for (unsigned i = 0; i < 2; i++) {
    free(players[i]);
  }
  free(get_background_buffer());
  free(get_double_buffer());
}

void clear_game() {

}

void update_game(Player * players[], int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[], enum State *s) {
  if ((only_one_move && tile_move_count == 1) || tile_move_count == 2) {
    *s = GAME_BLOCKS_MOVED;
  }
  if (((p_get_i(players[0]) == p_get_i(players[1])) && (p_get_j(players[0]) == p_get_j(players[1])))) {
    game_ends = true;
  } else if (play_time == PLAY_TIME && !game_ends){
    game_ends = true;
    if (current_player == 0){
      current_player = 1;
    } else {
      current_player = 0;
    }
  } else {
    if (move_count == 2 && current_player == 0) {
      if (*s == GAME_BLOCKS_MOVED) {
        tile_move_count = 0;
        only_one_move = false;
        p_set_last_movement(players[current_player],'x');
        current_player = 1;
        move_count = 0;
        blocks_to_move[0][0] = DEFAULT_BLOCK_COORDINATE;
        blocks_to_move[0][1] = DEFAULT_BLOCK_COORDINATE;
        blocks_to_move[1][0] = DEFAULT_BLOCK_COORDINATE;
        blocks_to_move[1][1] = DEFAULT_BLOCK_COORDINATE;
        free(mouse_triggers_game[1]);
        free(mouse_triggers_game[2]);
        mouse_triggers_game[1] = NULL;
        mouse_triggers_game[2] = NULL;
        *s = GAME;
      } else if (*s == GAME) {
        // Getting the coordinates for the mouse trigger
        int x = blocks_to_move[0][1]*grid_width - (((BOARD_SIZE - 1) * grid_width) / 2);
        int y = blocks_to_move[0][0]*grid_height - (((BOARD_SIZE - 1) * grid_height) / 2);
        x = get_xres()/2 + x -(grid_width/2);
        y = get_yres()/2 + y -(grid_height/2);

        mouse_triggers_game[1] = create_mouse_trigger(x, y - 20, grid_width, grid_height, NO_EVENT);
        mt_set_obj(mouse_triggers_game[1], tiles[board[blocks_to_move[0][0]][blocks_to_move[0][1]]]);
        toggle_need_to_be_moved(tiles[board[blocks_to_move[0][0]][blocks_to_move[0][1]]]);

        if (blocks_to_move[1][0] != DEFAULT_BLOCK_COORDINATE) {
          // Getting the coordinates for the mouse trigger
          int x = blocks_to_move[1][1]*grid_width - (((BOARD_SIZE - 1) * grid_width) / 2);
          int y = blocks_to_move[1][0]*grid_height - (((BOARD_SIZE - 1) * grid_height) / 2);
          x = get_xres()/2 + x -(grid_width/2);
          y = get_yres()/2 + y -(grid_height/2);

          mouse_triggers_game[2] = create_mouse_trigger(x, y - 20, grid_width, grid_height, NO_EVENT);
          mt_set_obj(mouse_triggers_game[2], tiles[board[blocks_to_move[1][0]][blocks_to_move[1][1]]]);
          toggle_need_to_be_moved(tiles[board[blocks_to_move[1][0]][blocks_to_move[1][1]]]);
        }
        
        *s = GAME_MOVING_BLOCKS;
      }
    } else if (move_count == 2) {
      if (*s == GAME_BLOCKS_MOVED) {
        p_set_last_movement(players[current_player],'x');
        tile_move_count = 0;
        only_one_move = false;
        current_player = 0;
        move_count = 0;
        blocks_to_move[0][0] = DEFAULT_BLOCK_COORDINATE;
        blocks_to_move[0][1] = DEFAULT_BLOCK_COORDINATE;
        blocks_to_move[1][0] = DEFAULT_BLOCK_COORDINATE;
        blocks_to_move[1][1] = DEFAULT_BLOCK_COORDINATE;
        free(mouse_triggers_game[1]);
        free(mouse_triggers_game[2]);
        mouse_triggers_game[1] = NULL;
        mouse_triggers_game[2] = NULL;
        *s = GAME;
      } else if (*s == GAME) {
        // Getting the coordinates for the mouse trigger
        int x = blocks_to_move[0][1]*grid_width - (((BOARD_SIZE - 1) * grid_width) / 2);
        int y = blocks_to_move[0][0]*grid_height - (((BOARD_SIZE - 1) * grid_height) / 2);
        x = get_xres()/2 + x -(grid_width/2);
        y = get_yres()/2 + y -(grid_height/2);

        mouse_triggers_game[1] = create_mouse_trigger(x, y - 20, grid_width, grid_height, NO_EVENT);
        mt_set_obj(mouse_triggers_game[1], tiles[board[blocks_to_move[0][0]][blocks_to_move[0][1]]]);
        toggle_need_to_be_moved(tiles[board[blocks_to_move[0][0]][blocks_to_move[0][1]]]);

        if (blocks_to_move[1][0] != DEFAULT_BLOCK_COORDINATE) {
          // Getting the coordinates for the mouse trigger
          int x = blocks_to_move[1][1]*grid_width - (((BOARD_SIZE - 1) * grid_width) / 2);
          int y = blocks_to_move[1][0]*grid_height - (((BOARD_SIZE - 1) * grid_height) / 2);
          x = get_xres()/2 + x -(grid_width/2);
          y = get_yres()/2 + y -(grid_height/2);

          mouse_triggers_game[2] = create_mouse_trigger(x, y - 20, grid_width, grid_height, NO_EVENT);
          mt_set_obj(mouse_triggers_game[2], tiles[board[blocks_to_move[1][0]][blocks_to_move[1][1]]]);
          toggle_need_to_be_moved(tiles[board[blocks_to_move[1][0]][blocks_to_move[1][1]]]);
        }
        *s = GAME_MOVING_BLOCKS;
      }

    }
  }
}

void draw_game(int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[], const unsigned tile_no, Player * players[]) {
  memcpy(get_double_buffer(), get_background_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8));

  for (unsigned i = 0; i < BOARD_SIZE; i++) {
    for (unsigned j = 0; j < BOARD_SIZE; j++) {
      if (board[i][j] != -1) {
        if (!get_dragging(tiles[board[i][j]]) && tiles[board[i][j]] != t_being_dragged) {
          draw_tile(tiles[board[i][j]], grid_width*j, grid_height*i, false);
        } 
        else if (get_dragging(tiles[board[i][j]])) {
          draw_tile(tiles[board[i][j]], mouse_xvariance - grid_width, mouse_yvariance - grid_height, true);
        }
      }
    }
  }
  
  if (p_get_i(players[0]) > p_get_i(players[1])) {
    draw_player(players[1]);
    draw_player(players[0]);
  } else {
    draw_player(players[0]);
    draw_player(players[1]);
  }

  if (!game_ends){
    if (play_time < PLAY_TIME){
      char s[10];
      if (PLAY_TIME - play_time >= 10) {
        sprintf(s, "%d", PLAY_TIME - play_time);
      } else {
        sprintf(s, "0%d", PLAY_TIME - play_time);
      }
      
      draw_string_centered(s, 2, get_xres()/2, 30, 800, color_palette[0], "");
    } else {
      draw_string_centered("0", 1, get_xres()/2, 30, 800, color_palette[0], "");
    }
    switch(error){
    case 1:
      draw_string_centered("Can't return to initial position!", 33, get_xres()/2, 80, 800, color_palette[0], "small");
      break;
    default:
      break;
    }
  }
  
  if (error != 0 && error_timer == 120) {
    error = 0;
    error_timer = 0;
  }
  
  #ifdef DEBUG
  draw_grid();
  #endif

  if (game_ends) {
    char * m;
    if (current_player == 0) {
      m = "Player 0 wins!";
    } else {
      m = "Player 1 wins!";
    }
    draw_string_centered(m, 14, get_xres()/2, 60, 800, color_palette[0], "");
  }

  draw_pixmap(get_mouse_simple(), mouse_xvariance, mouse_yvariance, false, PREDEF_COLOR, "");
  memcpy(get_video_mem(), get_double_buffer(), get_xres()*get_yres()*((get_bits_per_pixel()+7)/8)); // copies double buffer to display on screen
}

// Functions for debug purposes
void print_game_board(int board[BOARD_SIZE][BOARD_SIZE]) {
  printf("Here is the game board :)");
  for (unsigned i = 0; i < BOARD_SIZE; i++) {
    printf("\n");
    for (unsigned j = 0; j < BOARD_SIZE; j++) {
      if (board[i][j] != -1)
        printf("%d ", board[i][j]);
      else
        printf(". ");
    }
  }
}
void draw_grid() {
  for (unsigned i = 0; i < 20; i++) {
      if (i < 10) {
        draw_hline(0, 300 + i*grid_height, get_xres(), BLACK);
      } else {
        draw_hline(0, 300 - (i%10)*grid_height, get_xres(), BLACK);
      }
    }
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

  draw_bg_buffer(get_background(), 0, 0, true, PREDEF_COLOR, "");

  const unsigned tile_no = 9;
  Tile * tiles[9];

  int board[BOARD_SIZE][BOARD_SIZE]; // Each of the board's positions will hold either -1 or the position of the tile in the tiles array that occupies the position
  create_tiles(tiles, tile_no);
  create_board(board);

  #ifdef DEBUG
  print_game_board(board);
  #endif

  Player * players[2];
  players[0] = create_player(8, 6, get_red_ball_animation(), 0);
  players[1] = create_player(8, 10, get_blue_ball_animation(), 3);

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
            handle_keyboard_events(&s, players);
            execute_event(&s, tiles, tile_no, players, board);
          }
          if (msg.m_notify.interrupts & irq_timer0) {   // Timer0 interrupt received
            timer_int_handler();
            if (timer_counter_play % sys_hz() == 0){
              play_time++;
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
                case PAUSE:
                  draw_pause_menu();
                  break;
                case GAME: case GAME_MOVING_BLOCKS: case GAME_BLOCKS_MOVED:
                  update_game(players, board, tiles, &s);
                  draw_game(board, tiles, tile_no, players);
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
              handle_mouse_events(&s, &mouse_data, board, tiles);
              execute_event(&s, tiles, tile_no, players, board);
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

