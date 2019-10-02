#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  *st = (TIMER_RB_CMD|TIMER_RB_COUNT_|TIMER_RB_SEL(0));

  if (sys_outb(TIMER_0, *st) != 0)
    return 1;

  if (util_sys_inb(TIMER_0, st) != 0)
    return 1;

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  

  union timer_status_field_val value;
  
  switch(field) {
    case 0:
      value.byte = st;
      break;
    case 1:
      st = st << 2;
      st = st >> 6;
      switch (st) {
        case 0:
          value.in_mode = INVAL_val;
          break;
        case 1:
          value.in_mode = LSB_only;
          break;
        case 2:
          value.in_mode = MSB_only;
          break;
        case 3:
          value.in_mode = MSB_after_LSB;
          break;
        default:
          return 1;
      }
      break;
    case 2:
      st = st << 4;
      st = st >> 5;
      switch (st) {
        case 0:
          value.count_mode = 0;
          break;
        case 1:
          value.count_mode = 1;
          break;
        case 2: case 6:
          value.count_mode = 2;
          break;
        case 3: case 7:
          value.count_mode = 3;
          break;
        case 4:
          value.count_mode = 4;
          break;
        case 5:
          value.count_mode = 5;
          break;
        default:
          return 1;
      }
      break;
    case 3:
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
