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
        printf("%c ", charqueue_pop(q));
    }
    printf("\n");
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
    (*q).size++;
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

    (*q).size--;

    return value;
}

uint8_t charqueue_front(charqueue *q) {
    if ((*q).first == NULL) return 0;

    return (*((*q).first)).data;
}

bool charqueue_empty(charqueue *q) {
    return ((*q).first == (*q).last) && ((*q).last == NULL);
}

unsigned charqueue_size(charqueue *q) {
    return (*q).size;
}
