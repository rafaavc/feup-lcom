#include <lcom/lcf.h>

#include <stdint.h>

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

  if (sys_inb(port, b) != 0) {
    return 1;
  }

  *value = (uint8_t) *b;

  return 0;
}

int (util_sys_in2b)(int port, uint16_t *value) {
  uint32_t *b = (uint32_t *) value;

  if (sys_inb(port, b) != 0) {
    return 1;
  }

  *value = (uint16_t) *b;

  return 0;
}
