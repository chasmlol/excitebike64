#include "ctx.h"

extern s16 D_800C2C60[];

s32 func_8009EDCC(s32 mode, s32 v, s32 d, s32 f)
{
    s32 iv1;
    s32 iv2;
    s32 q;
    s32 num;
    s32 r;
    s32 res;
    s32 ta;
    s32 tb;
    volatile s32 pad[2];

    res = 0;
    if (v != 0) {
        if (mode == 1) {
            goto m1;
        }
        if (mode == 0) {
            goto m0;
        }
        if (mode == 2) {
            goto m2;
        }
        if (mode == 3) {
            goto m3;
        }
        iv1 = 0;
        iv2 = 0;
        goto dv;
    m0:
        iv1 = 0x4000;
        iv2 = 0x4000;
        v = v << 1;
        goto common;
    m1:
        iv1 = 0x4000;
        iv2 = 0x8000;
        v = v << 1;
        goto common;
    m2:
        iv1 = 0x8000;
        iv2 = 0x4000;
        goto common;
    m3:
        num = v << 10;
        q = num / d;
        r = num % d;
        if (f == 0) {
            q = q + r;
        }
        return q;
    dv:
        v = iv1;
    common:
        q = (iv1 << 10) / d;
        ta = (s32)D_800C2C60[(((q * f) >> 10) + iv2 >> 4) - 0x400 & 0xFFF];
        f = f + 1;
        tb = (s32)D_800C2C60[(((q * f) >> 10) + iv2 >> 4) - 0x400 & 0xFFF];
        r = ta - tb;
        res = (r * v) >> 5;
    }
    return res;
}
