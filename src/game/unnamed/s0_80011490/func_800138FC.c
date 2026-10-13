#include "ctx.h"

extern f32 func_800AEA30(f32 x);

u32 func_800138FC(f32 *p) {
    f32 c, d;
    s32 i;
    u32 r;

    c = func_800AEA30(*p + 4.0f) * 256.0f;
    i = (s32)c;
    c = c - (f32)i;
    if (c < 0.0f) {
        c = -c;
    }
    d = c * 256.0f;
    *p = c;
    if (!(2147483648.0f <= d)) {
        r = (s32)d;
    } else {
        r = (s32)(d - 2147483648.0f);
        r = r | 0x80000000;
    }
    return r;
}
