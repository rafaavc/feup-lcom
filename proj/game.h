#include "board_tile.h"
#include "mouse_trigger.h"
#include <stdio.h>
#include "player.h"
#include "Macros.h"


/**
 * @brief enum for all the states in which the game can be
 */
enum State {
    MAIN_MENU,  /**<  used when the game is at the initial menu  */
    TUTORIAL,   /**<  used when the game is at the tutorial page  */
    PAUSE,      /**<  used when the game is paused during a game  */
    GAME,       /**<  used when the players are playing  */
    CHOOSING_MODE,  /**<  used when the players are choosing which mode to play  */
    CHOOSING_HOST,  /**<  used when the player is waiting for the other player to connect  */
    WAITING_FOR_CONNECTION, /**<  used when the player is waiting for the other player to connect  */
    GAME_MOVING_BLOCKS,     /**<  used when it's time for a player to move game blocks  */
    GAME_BLOCKS_MOVED       /**<  used to reset variables associated with moving blocks  */
};

//Tile * create_game(const unsigned tile_no);

/**
 * @brief draws a text that can be clicked by the user
 * 
 * @param added_mouse_events true if all clickable text of the current frame have been drawn and false otherwise
 * @param mouse_trigger variable to store the MouseTrigger to be used later when handling mouse
 * @param needs_to_set_added_mouse_events false if this isn't the last cclickable text from current fram and true otherwise
 * @param event event that will happen when this text is clicked
 * @param s string to draw
 * @param ssize s size
 * @param x initial pixel on x axis
 * @param y initial pixel on y axis
 * @param max_length_per_line max number of pixels per line
 * @param color normal color of the text
 * @param color_over color of the text when mouse is over that text
 * @param relative_size size of the text, can be "" (normal), "small" or "smaller"
 */
void draw_text_button(bool *added_mouse_events, MouseTrigger * mouse_trigger[], bool needs_to_set_added_mouse_events, enum Event event, char* s, int ssize, uint16_t x, uint16_t y, uint16_t max_length_per_line, uint32_t color, uint32_t color_over, char * relative_size);

/**
 * @brief excecutes the current_event of the game
 * 
 * @param s state in which the game is
 * @param tiles all game Tile
 * @param tile_no number of tiles
 * @param players array with the each Player
 * @param board board to play 
 */
void execute_event(enum State *s, Tile * tiles[], unsigned tile_no, Player * players[], int board[BOARD_SIZE][BOARD_SIZE]);

/**
 * @brief draws main menu screen
 * 
 * Draws the background, then all the strings and text buttons for the main menu and lastly the mouse
 */
void draw_main_menu();

/**
 * @brief draws tutorial screen
 * 
 * Draws the background, then all the strings and text buttons for the tutorial and lastly the mouse
 */
void draw_tutorial();

/**
 * @brief draws main menu screen
 * 
 * Draws the background, then all the strings and text buttons for the pause menu and lastly the mouse
 */
void draw_pause_menu();

/**
 * @brief draws main menu screen
 * 
 * Draws the background, then all the strings and text buttons for the choosing mode menu and lastly the mouse
 */
void draw_choosing_mode_menu();

void draw_player1_prompt();

/**
 * @brief handles keyboard events for each state of the game
 * 
 * @param s state of the game
 * @param players array with each Player
 */
void handle_keyboard_events(enum State *s, Player * players[]);

/**
 * @brief handles mouse events for each state of the game
 * 
 * @param s state of the game
 * @param mouse_date organized info received by the mouse interrupt handler
 * @param board board to play
 * @param tiles board's tiles
 */
void handle_mouse_events(enum State *s, struct packet *mouse_data, int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[]);

/**
 * @brief free all alocated memory
 * 
 * @param tiles board's tiles
 * @param tile_no number of tiles
 * @param players array with each Player
 */
void free_allocated_memory(Tile * tiles[], unsigned tile_no, Player * players[]);

/**
 * @brief resets the game for a new start
 * 
 * @param tiles board's tiles
 * @param tile_no number of tiles
 * @param players array with each Player
 * @param board board to play
 */
void clear_game(Tile * tiles[], unsigned tile_no, Player * players[], int board[BOARD_SIZE][BOARD_SIZE]);

/**
 * @brief Sets the tile being dragged on the postion i,j
 * 
 * @param i position x for the tile to be set
 * @param j position y for the tile to be set
 * @param board board to play
 * @param tiles board's tiles
 */
void change_tile_position(int i, int j, int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[]);

/**
 * @brief updates game based on players postion and number of moves done
 * 
 * Checks if the game ended, if not checks if a player has ended his play and prepares everything
 * for the current player to finish his play or to pass the turn to the next player
 * 
 * @param players array with each Player
 * @param board board to play
 * @param tiles board's tiles
 * @param s state of the game
 */
void update_game(Player * players[], int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[], enum State *s);

/**
 * @brief draws gameboard and players 
 * 
 * @param board board to play
 * @param tiles board's tiles
 * @param tile_no number of tiles
 * @param players array with each Player
 */
void draw_game(int board[BOARD_SIZE][BOARD_SIZE], Tile * tiles[], const unsigned tile_no, Player *ps[]); 

// Function for debug purposes
void print_game_board(int board[BOARD_SIZE][BOARD_SIZE]);
// also debug purposes
void draw_grid();

/**
 * @brief main function for the game
 * 
 * This function has the main loop to subscribe and receive all the interrupts generated
 * 
 * @return 0 upon sucess and non-zero otherwise 
 */
int game();


