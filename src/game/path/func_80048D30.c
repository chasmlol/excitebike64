#include "ctx.h"

typedef struct Ent {
    u8 p0[4];
    f32 f4;
    u8 p8[4];
    f32 fc;
    u8 p10[0x36];
    s16 s46;
    u8 p48[2];
    s16 s4a;
    u8 p4c[0x6c - 0x4c];
} Ent;

extern Ent *D_800EE498;

u8 func_80048D30(f32 x, s32 idx)
{
    s32 c;
    s32 A, B;
    f32 d1, d2, v;
    x = x * x;
    for (c = 0; c < 5; c++) {
        A = D_800EE498[idx].s46;
        B = D_800EE498[idx].s4a;
        d1 = (D_800EE498[idx].f4 - D_800EE498[A].f4) / D_800EE498[A].fc;
        d2 = (D_800EE498[B].f4 - D_800EE498[idx].f4) / D_800EE498[c].fc;
        v = -(((d2 - d1) + (d2 - d1)) / (D_800EE498[A].fc + D_800EE498[c].fc)) * x;
        if (7.3500004f < v) return c & 0xFF;
        idx = A;
    }
    return 0xFF;
}
