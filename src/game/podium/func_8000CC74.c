#include "ctx.h"

extern s16 D_800C2C60[];

void func_8000CC74(s32 a0, s32 a1, s32 a2, s32 *p4, s32 *p5) {
    s16 *tab = D_800C2C60;
    s32 idx = ((s32)(a0 << 16)) >> 20;
    s32 s1 = tab[idx & 0xFFF];
    s32 m1 = a1 * s1;
    s32 m2 = a2 * s1;
    s32 s2 = tab[(idx - 0x400) & 0xFFF];
    s32 m3 = a1 * s2;
    s32 m4 = a2 * s2;

    *p5 = (m2 + m3) >> 14;
    *p4 = (m1 - m4) >> 14;
}
