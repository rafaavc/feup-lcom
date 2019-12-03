#include <lcom/lcf.h>

typedef struct {
    int x, y, xvel, yvel;
    unsigned animation_frame;
    xpm_image_t * animation_idle[];
} Player;
