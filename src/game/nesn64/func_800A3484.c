#include "ctx.h"

extern s32 D_800F5BB0;
extern s32 D_800F5BB8;
void func_800A2178(s32 addr, s32 value);

static inline void wr(s32 a, s32 v) {
    if (a < 0x2000) {
        a &= 0x7FF;
        *(u8 *)(a + 0x80380000) = v;
    } else if (a > 0x7FFF) {
    } else {
        func_800A2178(a, v);
    }
}

void func_800A3484(void) {
    u8 *nes;
    s32 c = D_800F5BB8 + 1;

    D_800F5BB8 = c;
    wr(D_800F5BB0, c);
    nes = (u8 *)0x80390000;
    nes[0x410B] = (D_800F5BB8 & 0xFF) == 0;
    nes[0x4108] = ((u8 *)&D_800F5BB8)[3] & 0x80;
}
