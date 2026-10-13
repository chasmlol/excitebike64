#include "ctx.h"

extern s16 D_800C4C60[];
f32 func_80013098(f32 p1, f32 p2);

f32 func_80013098(f32 p1, f32 p2)
{
    f32 r = 0.0f;
    if (p2 < 0.0f) {
        r = func_80013098(p1, -p2);
        r = -r;
    } else if (p1 < 0.0f) {
        r = func_80013098(-p1, p2);
        r = 3.1415927f - r;
    } else if (p1 < p2) {
        r = func_80013098(p2, p1);
        r = 1.5707964f - r;
    } else if (p1 != 0.0f || p2 != 0.0f) {
        r = (f32)D_800C4C60[(s32)(p2 * 4096.0f / p1)] * 3.1415927f * 3.0517578e-05f;
    }
    return r;
}
