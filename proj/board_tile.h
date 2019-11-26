typedef struct {
    int x, y;
} Tile;



Tile * create_tile(int x, int y);

void draw_tile(Tile *t);

void dragging_tile(Tile *t, int xvariance, int yvariance);

void create_board(Tile*t[], const unsigned int n);

int get_xpos(Tile *t);

int get_ypos(Tile *t);


