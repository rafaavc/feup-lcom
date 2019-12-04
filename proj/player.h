#include <lcom/lcf.h>

typedef struct {
    int x, y, xvel, yvel;
    unsigned animation_frame;
    xpm_image_t * animation_idle[];
} Player;

Player * create_player(unsigned x, unsigned y, xpm_image_t * animation_idle[12]);
int p_get_xpos(Player * p);
int p_get_ypos(Player * p);
int p_set_xvel(Player * p);
int p_set_yvel(Player * p);
unsigned next_animation_frame(Player *p);
void draw_player(Player *p);
