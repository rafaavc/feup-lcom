#ifndef LAB3
#define LAB3
#endif

#include <lcom/lcf.h>

#include <stdint.h>


extern unsigned int sys_inb_counter;

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  // Get least significant byte
  val = val << 8;
  val = val >> 8;
  *lsb = val;

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  // Get most significant byte
  val = val >> 8;
  *msb = val;

  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t *b = (uint32_t *) value;

  if (sys_inb(port, b) != 0) return 1;

  *value = (uint8_t) *b;

  #ifdef LAB3
  sys_inb_counter++;
  #endif

  return 0;
}

uint8_t (util_get_MSbit)(uint8_t val){
  return val >> 7;
}
