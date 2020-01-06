#include <lcom/lcf.h>

#include <stdint.h>

#include "Macros.h"


int timer_counter = 0;
extern unsigned timer_counter_play;

int hook_id_timer = TIMER_SEL0;

void (timer_int_handler)() {
  timer_counter++;
  timer_counter_play++;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  hook_id_timer = (int) *bit_no; // saves bit_no value
  
  // subscribe a notification on interrupts
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id_timer) != 0)
    return 1;
  
  *bit_no = (uint8_t) BIT(*bit_no);

  return 0;
}

int (timer_unsubscribe_int)() {
  // removes notification subscriptions on interrupts
  if (sys_irqrmpolicy(&hook_id_timer) != 0)
    return 1;

  return 0;
}

