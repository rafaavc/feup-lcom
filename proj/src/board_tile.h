
#include <lcom/lcf.h>
#include "Macros.h"

/**
 * @brief struct for all the tiles used in the game
 */
typedef struct {
    unsigned state;         /**< 0 if the tile doesn't need to be moved 1 otherwise */
    xpm_image_t states[2];  /**< xpm for tile that need to be moved and tile that doesn't  */
    bool dragging;          /**< true if the tile is being dragged and false otherwise */
} Tile;


/**
 * @brief creates a tile
 *
 * The tile's state begins at 0 and dragging at false
 */
Tile * create_tile();

/**
 * @brief draw the tile t at position x and y
 * 
 * @param t tile to draw
 * @param x position on x axis
 * @param y position on y axis
 */
void draw_tile(Tile *t, unsigned x, unsigned y);

/**
 * @brief changes state of the tile to draw it as a need to be moved tile (white center)
 * 
 * @param t tile to mark
 */
void toggle_need_to_be_moved(Tile *t);

/** @defgroup BoardTile Board Tile
 * @{
 * Contains functions for Board Tile
 */

/**
 * @brief change dragging tile to true and state to initial
 * 
 * @param t tile to change
 */
void start_dragging_tile(Tile *t);

/**
 * @brief change dragging tile to false
 * 
 * If changed position state is set to 0 otherwise state is set 1
 * 
 * @param t tile to change
 * @param changed_position true if the tile was changed and false if there was an error when changing
 */
void stop_dragging_tile(Tile *t, bool changed_position);

/**
 * @brief creates n tiles
 * 
 * @param t array with all the tiles to create
 * @param n number of tiles to create
 */
void create_tiles(Tile*t[], const unsigned int n);

/**
 * @brief get method for dragging
 * 
 * @param t tile
 */
bool get_dragging(Tile *t);

/**
 * @brief creates a board with the tiles
 * 
 * @param board game's board
 */
void create_board(int board[BOARD_SIZE][BOARD_SIZE]);

/** @}*/
