#include <lcom/lcf.h>
#include "Macros.h"

/** @defgroup Player Player
 * @{
 * Contains functions for Player
 */

/**
 * @brief struct for a player animation
 */
typedef struct {
    unsigned counter /** counter used to change player xpm only every 4 frames */, animation_frame /** current animation frame of the player */;
    int board_i /** board position on x axis */ , board_j /** board position on y axis */;
    bool moving;                    /**< Player moving or stoped */
    xpm_image_t animation_idle[12]; /**< all the player's animation frames */
    char * name;                    /**< name of the player */
    int next_x /** Pixels remainig to move in x axis */, next_y /** Pixels remaining to move in y axis */;
    char last_movement;             /**< Player's last movement (x if first movement of the turn) */
} Player;

/**
 * @brief creates a Player
 * 
 * @param x tile position of the player on the x axis
 * @param y tile position of the player on the y axis
 * @param animation_idle all the player's animation frames
 * @param starting_an initial animation frame
 * 
 * @return pointer to Player created
 */
Player * create_player(unsigned x, unsigned y, xpm_image_t animation_idle[12], unsigned starting_an);

/**
 * @brief get method for player board_j
 * 
 * @param p Player to get from
 * 
 * @return Player's board_j
 */
int p_get_j(Player * p);

/**
 * @brief get method for player board_i
 * 
 * @param p Player to get from
 * 
 * @return Player's board_i
 */
int p_get_i(Player * p);

/**
 * @brief set method for Player's name
 * 
 * @param p Player to change name
 * @param name the name to give the player
 */
void p_set_name(Player * p, char * name);

/**
 * @brief get method for Player's name
 * @param p Player to get name
 * @return player's name
 */
char * p_get_name(Player * p);

/**
 * @brief updates animation frame to next one
 * 
 * @param p Player to update
 * 
 * @return new animation frame postion
 */
unsigned next_animation_frame(Player *p);

/**
 * @brief updates player position in case it's moving and draws it
 * 
 * @param p Player to draw 
 */
void draw_player(Player *p);

/**
 * @brief draws player in custom x and y coordinates
 * 
 * @param p Player to draw 
 * @param x coordinate x to draw player in
 * @param y coordinate y to draw player in
 */
void draw_player_custom(Player *p, unsigned x, unsigned y);

/**
 * @brief move the player on any directon
 * 
 * @param p Player to move
 * @param direction char with the direction of the movement (w, a, s or d)
 * @param board game's board
 * 
 * @returns true if player is able to make his move and false otherwise
 */
bool move(Player *p, char direction, int board[BOARD_SIZE][BOARD_SIZE]);

/**
 * @brief moves the player on positive y direction
 * 
 * @param p Player to move
 */
void player_move_w(Player * p);

/**
 * @brief moves the player on negative x direction
 * 
 * @param p Player to move
 */
void player_move_a(Player * p);

/**
 * @brief moves the player on negative y direction
 * 
 * @param p Player to move
 */
void player_move_s(Player * p);

/**
 * @brief moves the player on positive x direction
 * 
 * @param p Player to move
 */
void player_move_d(Player * p);

/**
 * @brief set method for Player's last movement
 * 
 * @param p Player to change
 * @param lmov last movement
 */
void p_set_last_movement(Player *p, char lmov);

/**
 * @brief get method for Player's last movement
 * 
 * @param p Player to get from
 * 
 * @return Player's last movement
 */
char p_get_last_movement(Player *p);

/** @}*/
