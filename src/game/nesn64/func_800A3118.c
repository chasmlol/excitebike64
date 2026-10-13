#include "ctx.h"

extern u32 D_800F5BB8;

void func_800A3118(void) {
    u8 *nes = (u8 *)0x80390000;
    nes[0x410B] = ((nes[0x4102] & D_800F5BB8) == 0);
    {
        u8 b = ((u8 *)&D_800F5BB8)[3];
        nes[0x4108] = b & 0x80;
        nes[0x4106] = b & 0x40;
    }
}
