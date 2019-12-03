enum State {
    MAIN_MENU,
    PAUSE,
    GAME
};

//Tile * create_game(const unsigned tile_no);

void draw_main_menu();

void draw_pause_menu();

void handle_mouse_events(enum State *s, struct packet *mouse_data);

void update_game();

void draw_game(Tile * tiles[], const unsigned tile_no);

int game();


