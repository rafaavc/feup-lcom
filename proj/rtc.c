#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include <minix/driver.h>
#include <sys/mman.h>

#include "rtc.h"
#include "Macros.h"


bool dark_mode = false, override_dark_mode = false;
int hook_id_rtc;
uint8_t rtc[3];

bool get_time_rtc(){
    uint8_t regA, format, regC;

    read_rtc(REG_C, &regC);

    read_rtc(REG_A, &regA);

    if (regA & UPDATE_IN_PROGRESS) return false;

    read_rtc(REG_B, &format);

    read_rtc(HOURS, &rtc[0]);

    read_rtc(MINUTES, &rtc[1]);

    read_rtc(SECONDS, &rtc[2]);

    if (check_BCD()){
        for (int i = 0; i < 3; i++){
            bcd_to_binary(&rtc[i]);
        }
    }
    return true;
}

bool check_BCD(){
    uint8_t regB;
    read_rtc(REG_B, &regB);

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

int read_rtc(uint32_t addr, uint8_t *ret) {
    if (sys_outb(RTC_ADDR_REG, addr) != 0) return 1;
    if (util_sys_inb(RTC_DATA_REG, ret) != 0) return 1;
    return 0;
}

int write_rtc(uint32_t addr, uint8_t value) {
    if (sys_outb(RTC_ADDR_REG, addr) != 0) return 1;
    if (sys_outb(RTC_DATA_REG, value) != 0) return 1;
    return 0;
}

int rtc_subscribe_int(uint8_t* bit_no){
    hook_id_rtc = (int) *bit_no;
    uint8_t regB;

    if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_rtc) != 0) return 1;

    if (read_rtc(REG_B, &regB) != 0) return 1;

    regB = (regB | UPDATE_INTERRUPTS);

    if (write_rtc(REG_B, regB) != 0) return 1;

    uint8_t regC;
    if (read_rtc(REG_C, &regC) != 0) return 1;

    // This will check the dark mode registers for data
    uint8_t override_dm;
    if (read_rtc(REG_DARK_MODE, &override_dm) != 0) return 1;
    if (override_dm == 'C') {
        override_dark_mode = true;
        if (read_rtc(REG_DARK_MODE1, &override_dm) != 0) return 1;

        if (override_dm == 'T') dark_mode = true;
        else dark_mode = false;

    } else override_dark_mode = false;


    *bit_no = (uint8_t) BIT(*bit_no);

    return 0;
}

int (rtc_unsubscribe_int)() {
    // removes notification subscriptions on interrupts
    if (sys_irqrmpolicy(&hook_id_rtc) != 0)
        return 1;

    return 0;
}

int write_dark_mode(uint8_t mode, uint8_t content) {
    if (write_rtc(REG_DARK_MODE, mode) != 0) return 1;
    if (write_rtc(REG_DARK_MODE1, content) != 0) return 1;
    return 0;
}

int make_dm_auto() {
    if (write_dark_mode('A', 0x0) != 0) return 1;
    return 0;
}

int make_dm_custom(bool dm) {
    uint8_t c;
    if (dm) c = 'T';
    else c = 'F';

    if (write_dark_mode('C', c) != 0) return 1;
    return 0;
}

