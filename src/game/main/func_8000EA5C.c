#include "ctx.h"

s32 func_8000EA5C(void)
{
    __asm__ volatile("addu $9, $30, $0" : : : "$9");
    while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
    }
    if ((*(volatile u32 *)0xB0000574 & 0xFFFF) != 0x6C07) {
        return 0;
    }
    while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
    }
    return (*(volatile u32 *)0xB0000578 & 0xFFFF) == 0x8965;
}
