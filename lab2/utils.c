#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (val % 2 == 0)
  {  
    *lsb = 0;
    return 0;
  }
  else if (val % 2 == 1)
  {
    *lsb = 1;
    return 0;
  }

  return 1;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  
  val = val >> 15;

  *msb = val;

  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t *b = (uint32_t *) value;

  if (sys_inb(port, b) != 0) {
    return 1;
  }

  *value = (uint8_t) *b;

  return 0;
}
