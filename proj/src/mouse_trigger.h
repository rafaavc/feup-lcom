/** @defgroup MouseTrigger Mouse Trigger
 * @{
 * Contains functions for MouseTrigger
 */

/**
 * @brief enum for all the possible events to happen in the game
 */
enum Event {
    NO_EVENT,           /**< Standard position when no event is happening */
    START_CHOOSING_MODE,/**< Change game to choosing mode */
    START_GAME_SP,      /**< Change game to serial port mode */
    START_GAME_NO_SP,   /**< Change game to one pc mode */
    CONNECT_SP,         /**< Change game to connecting serial port */
    QUIT_GAME,          /**< Quit game */
    PAUSE_GAME,         /**< Pause game */
    RESUME_GAME,        /**< Resume game */
    OPEN_TUTORIAL,      /**< Open tutorial */
    END_GAME,           /**< End game and return to main menu */
    OPEN_MAIN_MENU,     /**< Open main menu */
    PLAYER_MOVE_W,      /**< Move a player with w key */
    PLAYER_MOVE_A,      /**< Move a player with a key */
    PLAYER_MOVE_S,      /**< Move a player with s key */
    PLAYER_MOVE_D,       /**< Move a player with d key */
    MAKE_DARK_MODE_AUTO, /**< Making dark mode automatic */
    MAKE_DARK_MODE_CUSTOM_TOGGLE, /**< Making dark mode custom with dark mode = !dark_mode */ 
    OPEN_SETTINGS                /**< Open settings menu */
};

/**
 * @brief box where you can click to execute an event
 */
typedef struct {
    unsigned x /** start postion of the box on the x axis */, y /** start postion of the box on the y axis */, width /** widht of the box */, height /** height of the box */;
    enum Event event;   /**< event that happens when clicked */
    bool mouse_over;    /**< true if mouse is over the box and false otherwise */
    void *obj;          /**< MouseTrigger's object */
} MouseTrigger;

/**
 * @brief creates a MouseTrigger
 * 
 * @param x initial position on x axis
 * @param y initial position on y axis
 * @param width width of the mouse trigger
 * @param height height of the mouse trigger
 * @param event event that happens when clicked
 */
MouseTrigger * create_mouse_trigger(unsigned x, unsigned y, unsigned width, unsigned height, enum Event event);

/**
 * @brief get method for leftmost x
 * 
 * @param t MouseTrigger to get from
 * 
 * @return start position on the x axis
 */
unsigned mt_get_xpos(MouseTrigger *t);

/**
 * @brief get method for upper y
 * 
 * @param t MouseTrigger to get from
 * 
 * @return start position on the y axis
 */
unsigned mt_get_ypos(MouseTrigger *t);

/**
 * @brief get method for rightmost x
 * 
 * @param t MouseTrigger to get from
 * 
 * @return start position on the x axis + width
 */
unsigned mt_get_endxpos(MouseTrigger *t);

/**
 * @brief get method for lower y
 * 
 * @param t MouseTrigger to get from
 * 
 * @return start position on the y axis + height
 */
unsigned mt_get_endypos(MouseTrigger *t);

/**
 * @brief get method for the event of the MouseTrigger
 * 
 * @param t MouseTrigger to get from
 * 
 * @return MouseTrigger's event
 */
enum Event mt_get_event(MouseTrigger *t);

/**
 * @brief checks if the mouse is on the MouseTrigger
 * 
 * @param t MouseTrigger to check from
 * 
 * @return true if the mouse is over the MouseTrigger and false otherwise
 */
bool check_mouse_overlap(MouseTrigger *t);

/**
 * @brief checks if the mouse from the other player is on the MouseTrigger (if Serial Port is being used)
 * 
 * @param t MouseTrigger to check from
 * 
 * @return true if the mouse is over the MouseTrigger and false otherwise
 */
bool p1_check_mouse_overlap(MouseTrigger *t);

/**
 * @brief get method for mouse_over
 * 
 * @param t MouseTrigger to get from
 * 
 * @return MouseTrigger's mouse_over
 */
bool mt_get_mouse_over(MouseTrigger *t);

/**
 * @brief sets mouse_over to true
 * 
 * @param t MouseTrigger to set
 */
void mt_set_mouse_over(MouseTrigger *t);

/**
 * @brief sets the mouse trigger object
 * 
 * @param t MouseTrigger to set
 * @param o the object
 */
void mt_set_obj(MouseTrigger *t, void * o);

/**
 * @brief GET method for the mouse trigger object
 * 
 * @param t MouseTrigger to get object from
 */
void * mt_get_obj(MouseTrigger *t);

/** @}*/
