#include "ctx.h"

extern f32 D_800C98D0[];

f32 func_80036108(f32 x)
{
    f32 r;
    f32 lo;
    s32 i;

    if (x < 0.0f) {
        return 0.0f;
    }
    if (14500.0f < x) {
        x = 14500.0f;
    }
    i = (s32)((x * 28.0f) / 14500.0f);
    lo = D_800C98D0[i];
    r = lo;
    if (i < 0x1C) {
        r = r + ((x - (f32)((i * 0x38A4) / 0x1C)) * (D_800C98D0[i + 1] - r)) / 517.0f;
    }
    return r;
}
