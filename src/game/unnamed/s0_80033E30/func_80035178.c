#include "ctx.h"

extern s32 D_800C2384;
extern s32 D_800C2594;
extern s32 D_800E1774;
extern s32 D_800E8EE4;
Gfx *func_80034100(s32 a, s32 b, s32 *c, Gfx *d);

Gfx *func_80035178(Gfx *gdl, s32 unused, u8 *s)
{
    Gfx *g0, *g1, *g2, *g3, *g4, *g5, *g6, *g7, *g8, *q0, *q1, *q2;
    s32 n;
    s32 i, j;
    s32 local;

    n = s[9];
    if (n == 0) {
        n = 0x100;
    }
    g0 = gdl++; ((u32 *)g0)[0] = 0xFD100000; ((u32 *)g0)[1] = *(u32 *)(s + 4);
    g1 = gdl++; ((u32 *)g1)[0] = 0xE8000000; ((u32 *)g1)[1] = 0;
    g2 = gdl++; ((u32 *)g2)[0] = 0xF5000100; ((u32 *)g2)[1] = 0x07000000;
    g3 = gdl++; ((u32 *)g3)[0] = 0xE6000000; ((u32 *)g3)[1] = 0;
    g4 = gdl++; ((u32 *)g4)[0] = 0xF0000000; ((u32 *)g4)[1] = 0x073FC000;
    g5 = gdl++; ((u32 *)g5)[0] = 0xE7000000; ((u32 *)g5)[1] = 0;
    g6 = gdl++; ((u32 *)g6)[0] = 0xE3000C00; ((u32 *)g6)[1] = 0;
    g7 = gdl++; ((u32 *)g7)[0] = 0xE3001201; ((u32 *)g7)[1] = 0;
    g8 = gdl++; ((u32 *)g8)[0] = 0xE3001801; ((u32 *)g8)[1] = 0x80;

    if (D_800C2384 == 0) {
        for (i = 0; i < n; i += 8) {
            local = *(s32 *)(s + 0xC) + i * 0x100;
            for (j = 0; j < D_800C2594; j++) {
                gdl = func_80034100(i, j, &local, gdl);
            }
        }
    } else {
        D_800E8EE4 = D_800E1774;
        for (j = 0; j < D_800C2594; j++) {
            for (i = 0; i < n; i += 8) {
                local = *(s32 *)(s + 0xC) + i * 0x100;
                gdl = func_80034100(i, j, &local, gdl);
            }
        }
    }

    q0 = gdl++; ((u32 *)q0)[0] = 0xE3001201; ((u32 *)q0)[1] = 0x2000;
    q1 = gdl++; ((u32 *)q1)[0] = 0xE3000C00; ((u32 *)q1)[1] = 0x00080000;
    q2 = gdl++; ((u32 *)q2)[0] = 0xE3001801; ((u32 *)q2)[1] = 0;
    return gdl;
}
