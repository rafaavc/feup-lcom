#include "board_tile.h"
#include "mouse_trigger.h"
#include <stdio.h>
#include "player.h"

enum State {
    MAIN_MENU,
    TUTORIAL,
    PAUSE,
    GAME
};

//Tile * create_game(const unsigned tile_no);

void draw_text_button(bool *added_mouse_events, MouseTrigger * mouse_trigger[], bool needs_to_set_added_mouse_events, enum Event event, char* s, int ssize, uint16_t x, uint16_t y, uint16_t max_length_per_line, uint32_t color, uint32_t color_over, char * relative_size);

void execute_event(enum State *s, Tile * tiles[], unsigned tile_no);

void draw_main_menu();

void draw_tutorial();

void draw_pause_menu();

void handle_keyboard_events(enum State *s);

void handle_mouse_events(enum State *s, struct packet *mouse_data);

void free_allocated_memory();

void clear_game();

void update_game();

void draw_game(Tile * tiles[], const unsigned tile_no, Player *ps[]);

int game();


