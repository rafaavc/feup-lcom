#include "queue.h"


charqueue * create_charqueue() {
    charqueue * q = (charqueue *) malloc(sizeof(*q));
    (*q).first = NULL;
    (*q).last = NULL;
    (*q).size = 0;
    return q;
}

void charqueue_print(charqueue *q) {

    while (!charqueue_empty(q)) {
        printf("%c,", charqueue_pop(q));
    }
    printf("|\n");
}

void charqueue_push(charqueue *q, uint8_t value) {
    charqueue_node * qn = (charqueue_node *) malloc(sizeof(*qn));
    (*qn).next = NULL;
    (*qn).data = value;
    if ((*q).last == NULL) {
        (*q).first = qn;
        (*q).last = qn;
    } else {
        (*((*q).last)).next = qn;
        (*q).last = qn;
    }
    (*q).size = (*q).size + 1;
}

uint8_t charqueue_pop(charqueue *q) {
    if ((*q).first == NULL) return 0;

    uint8_t value = (*((*q).first)).data;
    if ((*((*q).first)).next == NULL) {
        (*q).first = NULL;
        (*q).last = NULL;
        return value;
    }
    charqueue_node * new_first = (*((*q).first)).next;
    free((*q).first);
    (*q).first = new_first;

    (*q).size = (*q).size - 1;

    return value;
}

uint8_t charqueue_front(charqueue *q) {
    if ((*q).first == NULL) return 0;

    return (*((*q).first)).data;
}

bool charqueue_empty(charqueue *q) {
    return ((*q).first == (*q).last) && ((*q).last == NULL);
}

void charqueue_make_empty(charqueue *q) {
    while (!charqueue_empty(q)) {
        charqueue_pop(q);
    }
}

unsigned charqueue_size(charqueue *q) {
    return (*q).size;
}

void charqueue_remove_last(charqueue *q) {
    if (charqueue_empty(q)) return;

    charqueue *q1 = create_charqueue();
    while (!charqueue_empty(q)) {
        if ((*q).first == (*q).last) {
            charqueue_pop(q);
        } else {
            charqueue_push(q1, charqueue_pop(q));
        }
    }
    while (!charqueue_empty(q1)) {
        charqueue_push(q, charqueue_pop(q1));
    }

    free(q1);
}

char * charqueue_to_string(charqueue *q) {
    if (charqueue_empty(q)) return "";
    char * str = malloc(sizeof(char[charqueue_size(q)+1]));

    charqueue *q1 = create_charqueue();
    unsigned i = 0;
    char c;
    while (!charqueue_empty(q)) {
        charqueue_push(q1, charqueue_pop(q));
    }
    
    while (!charqueue_empty(q1)) {
        c = charqueue_pop(q1);
        str[i] = c;
        charqueue_push(q, c);
        i++;
    }
    str[i] = '\0';
    free(q1);
    return str;

}
