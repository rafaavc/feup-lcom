#include <lcom/lcf.h>
#include <minix/driver.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t data;
    void * next;
} charqueue_node;


typedef struct {
    charqueue_node * first;
    charqueue_node * last;
    unsigned size;
} charqueue;


charqueue * create_charqueue();

void charqueue_print(charqueue *q);

void charqueue_push(charqueue *q, uint8_t value);

uint8_t charqueue_pop(charqueue *q);

uint8_t charqueue_front(charqueue *q);

void charqueue_make_empty(charqueue *q);

bool charqueue_empty(charqueue *q);

unsigned charqueue_size(charqueue *q);

