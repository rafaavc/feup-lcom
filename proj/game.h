#include "board_tile.h"
#include "mouse_trigger.h"
#include <stdio.h>
#include "player.h"
#include "Macros.h"

enum State {
    MAIN_MENU,
    TUTORIAL,
    PAUSE,
    GAME,
    CHOOSING_MODE,
    CHOOSING_HOST,
    WAITING_FOR_CONNECTION,
    GAME_MOVING_BLOCKS,
    GAME_BLOCKS_MOVED
};

//Tile * create_game(const unsigned tile_no);

void draw_text_button(bool *added_mouse_events, MouseTrigger * mouse_trigger[], bool needs_to_set_added_mouse_events, enum Event event, char* s, int ssize, uint16_t x, uint16_t y, uint16_t max_length_per_line, uint32_t color, uint32_t color_over, char * relative_size);

void execute_event(enum State *s, Tile * tiles[], unsigned tile_no, Player * players[], int board[BOARD_SIZE][BOARD_SIZE]);

void draw_main_menu();

void draw_tutorial();

void draw_pause_menu();

void draw_choosing_mode_menu();

void draw_player1_prompt();

void handle_keyboard_events(enum State *s, Player * players[]);

void handle_mouse_events(enum State *s, struct packet *mouse_data, int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[]);

void free_allocated_memory(Tile * tiles[], unsigned tile_no, Player * players[]);

void clear_game(Tile * tiles[], unsigned tile_no, Player * players[], int board[BOARD_SIZE][BOARD_SIZE]);

void change_tile_position(int i, int j, int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[]);

void update_game(Player * players[], int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[], enum State *s);

void draw_game(int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[], const unsigned tile_no, Player *ps[]);

// Function for debug purposes
void print_game_board(int board[BOARD_SIZE][BOARD_SIZE]);

void draw_grid();

int game();


