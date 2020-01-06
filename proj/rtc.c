#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include <minix/driver.h>
#include <sys/mman.h>

#include "rtc.h"
#include "Macros.h"


bool dark_mode = true;
int hook_id_rtc;
uint8_t rtc[3];

bool get_time_rtc(){
    uint8_t regA = UPDATE_IN_PROGRESS, format;

    sys_outb(RTC_ADDR_REG, REG_A);
    util_sys_inb(RTC_DATA_REG, &regA);

    if (regA & UPDATE_IN_PROGRESS) return false;

    sys_outb(RTC_ADDR_REG, REG_B);
    util_sys_inb(RTC_DATA_REG, &format);

    sys_outb(RTC_ADDR_REG, HOURS);
    util_sys_inb(RTC_DATA_REG, &rtc[0]);

    sys_outb(RTC_ADDR_REG, MINUTES);
    util_sys_inb(RTC_DATA_REG, &rtc[1]);

    sys_outb(RTC_ADDR_REG, SECONDS);
    util_sys_inb(RTC_DATA_REG, &rtc[2]);

    if (check_BCD()){
        for (int i = 0; i < 3; i++){
            bcd_to_binary(&rtc[i]);
        }
    }
    return true;
}

bool check_BCD(){
    uint8_t regB;
    sys_outb(RTC_ADDR_REG, REG_B);
    util_sys_inb(RTC_DATA_REG, &regB);

    if (regB & RTC_BINARY) return false;
    return true;
}

void bcd_to_binary(uint8_t* number){
    uint8_t tmp = *number & 0x0F;
    *number = *number & 0xF0;
    *number = *number >> 4;
    *number *= 10;
    *number += tmp;
}

int rtc_subsrcibe_int(uint8_t* bit_no){
    hook_id_rtc = (int) *bit_no;
    uint8_t regB;

    if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_rtc) != 0) return 1;

    if (sys_outb(RTC_ADDR_REG, REG_B) != 0) return 1;
    if (util_sys_inb(RTC_DATA_REG, &regB) != 0) return 1;

    regB = (regB | UPDATE_INTERRUPTS);

    if (sys_outb(RTC_ADDR_REG, REG_B) != 0) return 1;
    if (sys_outb(RTC_DATA_REG, regB) != 0) return 1;

    *bit_no = (uint8_t) BIT(*bit_no);

    return 0;
}

int (rtc_unsubscribe_int)() {
  // removes notification subscriptions on interrupts
  if (sys_irqrmpolicy(&hook_id_rtc) != 0)
    return 1;

  return 0;
}
