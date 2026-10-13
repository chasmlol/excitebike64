#include "ctx.h"

extern s32 D_800F5BB0;
extern s32 D_800F5BB8;
void func_800A2178(s32 a0, s32 a1);

void func_800A2928(void)
{
    s32 a2 = D_800F5BB0;

    if (a2 == -1) {
        u8 *g = (u8 *)0x80390000;
        u8 b = g[0x4102];
        g[0x410C] = b & 1;
        b = b >> 1;
        g[0x4102] = b;
        g[0x410B] = (b == 0);
        g[0x4108] = b & 0x80;
        return;
    } else {
        u8 *pb = (u8 *)&D_800F5BB8;
        u8 lo = pb[3];
        s32 w;

        *(u8 *)0x8039410C = lo & 1;
        w = D_800F5BB8 >> 1;
        D_800F5BB8 = w;
        if (a2 < 0x2000) {
            *(u8 *)(0x80380000 + (a2 & 0x7FF)) = (u8)w;
        } else if (a2 <= 0x7FFF) {
            func_800A2178(a2, w);
        }
    }
    {
        u8 *t = (u8 *)0x80390000;
        t[0x410B] = ((D_800F5BB8 & 0xFF) == 0);
        t[0x4108] = ((u8 *)&D_800F5BB8)[3] & 0x80;
    }
}
