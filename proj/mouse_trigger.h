enum Event {
    NO_EVENT,
    START_GAME,
    QUIT_GAME,
    PAUSE_GAME,
    OPEN_TUTORIAL,
    END_GAME,
    OPEN_MAIN_MENU
};

typedef struct {
    unsigned x, y, width, height;
    enum Event event;
    bool mouse_over;
} MouseTrigger;


MouseTrigger * create_mouse_trigger(unsigned x, unsigned y, unsigned width, unsigned height, enum Event event);

unsigned mt_get_xpos(MouseTrigger *t);

unsigned mt_get_ypos(MouseTrigger *t);

unsigned mt_get_endxpos(MouseTrigger *t);

unsigned mt_get_endypos(MouseTrigger *t);

enum Event mt_get_event(MouseTrigger *t);

bool check_mouse_overlap(MouseTrigger *t);

bool mt_get_mouse_over(MouseTrigger *t);

void mt_set_mouse_over(MouseTrigger *t);

