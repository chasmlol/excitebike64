#include "ctx.h"

extern u16 D_800C6C64[];

f32 func_8001318C(f32 p) {
    s32 x;
    s32 v;
    s32 r;

    x = (s32)(p * 16384.0f);
    if (x <= 0) {
        goto neg;
    }
    if (x < 0x4000) {
        goto tbl;
    }
    r = 0x4000;
    goto done;
tbl:
    {
        u16 *tb;
        v = x + 2;
        tb = D_800C6C64;
        if (v < 0) {
            v = x + 5;
        }
        r = tb[v >> 2];
    }
    goto done;
neg:
    if (x < -0x3FFF) {
        r = -0x4000;
        goto done;
    }
    {
        u16 *tb;
        v = 2 - x;
        tb = D_800C6C64;
        if (v < 0) {
            v = v + 3;
        }
        r = -tb[v >> 2];
    }
done:
    return (f32)(s16)r * 3.1415927f * 3.0517578e-05f;
}
