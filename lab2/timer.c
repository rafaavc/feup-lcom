#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int hook_id = TIMER_SEL0;
unsigned int counter;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (freq < 19 || freq > TIMER_FREQ)
    return 1;

  uint8_t current_conf;

  if (timer_get_conf(timer, &current_conf) != 0) // gets current timer conf so that the 4 least significant bits aren't changed
    return 1;

  uint8_t control_word = current_conf << 4;
  control_word = control_word >> 4;  // now control_word holds the 4 least significant bits (BCD/binary and operating mode)

  control_word |= TIMER_LSB_MSB;  // initializes lsb followed by msb
  uint8_t lsb, msb;
  uint16_t count_value = TIMER_FREQ / freq;

  util_get_LSB(count_value, &lsb);
  util_get_MSB(count_value, &msb);

  switch (timer) {
    case 0:
      control_word |= TIMER_SEL0;
      if (sys_outb(TIMER_CTRL, control_word) != 0) // writing to control
        return 1;
      if (sys_outb(TIMER_0, lsb) != 0) // giving lsb
        return 1;
      if (sys_outb(TIMER_0, msb) != 0) // and now msb
        return 1;
      break;
    case 1:
      control_word |= TIMER_SEL1;
      if (sys_outb(TIMER_CTRL, control_word) != 0)
        return 1;
      if (sys_outb(TIMER_1, lsb) != 0)
        return 1;
      if (sys_outb(TIMER_1, msb) != 0)
        return 1;
      break;
    case 2:
      control_word |= TIMER_SEL2;
      if (sys_outb(TIMER_CTRL, control_word) != 0)
        return 1;
      if (sys_outb(TIMER_2, lsb) != 0)
        return 1;
      if (sys_outb(TIMER_2, msb) != 0)
        return 1;
      break;
    default:
      return 1;
  }

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  hook_id = (int) *bit_no; // saves bit_no value
  
  // subscribe a notification on interrupts
  if (sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id) != 0)
    return 1;
  
  *bit_no = (uint8_t) BIT(*bit_no);

  return 0;
}

int (timer_unsubscribe_int)() {
  // removes notification subscriptions on interrupts
  if (sys_irqrmpolicy(&hook_id) != 0)
    return 1;

  return 0;
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  *st = (TIMER_RB_CMD|TIMER_RB_COUNT_|TIMER_RB_SEL(timer)); // read-back command

  if (sys_outb(TIMER_CTRL, *st) != 0) // writing to control
    return 1;
 
  if (timer == 0) {
    if (util_sys_inb(TIMER_0, st) != 0) // getting conf from timer
      return 1;
  } else if (timer == 1) {
    if (util_sys_inb(TIMER_1, st) != 0)
      return 1;
  } else if (timer == 2) {
    if (util_sys_inb(TIMER_2, st) != 0)
      return 1;
  } else {
    return 1;
  }
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  
  if (timer > 2 || timer < 0)
    return 1;

  union timer_status_field_val value;
  
  switch(field) {
    case 0:     // checks status
      value.byte = st;
      break;
    case 1:     // checks initialization mode
      st = st << 2;
      st = st >> 6;
      switch (st) {
        case 0:
          value.in_mode = INVAL_val;    // init mode not valid
          break;
        case 1:
          value.in_mode = LSB_only;     // init mode is LSB only
          break;
        case 2:
          value.in_mode = MSB_only;     // init mode is MSB only
          break;
        case 3:
          value.in_mode = MSB_after_LSB;// init mode is MSB after LSB
          break;
        default:
          return 1;
      }
      break;
    case 2:     // checks counting mode
      st = st << 4;
      st = st >> 5;
      if (st != 6 && st != 7) {
        value.count_mode = st;
      } else {
        value.count_mode = st-4;
      }
      break;
    case 3:     // checks counting base
      if (st % 2 == 0) {
        value.bcd = false;
      } else {
        value.bcd = true;
      }
      break;

    default: 
      return 1;

  }
  timer_print_config(timer, field, value);

  return 0;
}
