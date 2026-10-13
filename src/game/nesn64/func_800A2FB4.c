#include "ctx.h"

extern s32 D_800F5BB8;

void func_800A2FB4(void) {
    u8 *nes = (u8 *)0x80390000;
    u32 sum = nes[0x4102];
    u32 old = sum;
    u32 m = 0xFFFFFF80;
    sum = sum + D_800F5BB8;
    if (nes[0x410C] != 0) {
        sum = sum + 1;
    }
    nes[0x410B] = (sum & 0xFF) == 0;
    nes[0x4108] = sum & m;
    nes[0x410C] = (sum & 0xFFFFFF00) != 0;
    nes[0x4102] = sum;
    nes[0x4106] = (old ^ sum) & m;
}
