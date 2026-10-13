#include "ctx.h"

extern s32 D_800EE490;
extern u8 *D_800EE498;

void func_8002D964(s32 a0, f32 f, s16 *out)
{
    s16 i;
    s32 i1, i2;
    f32 r;
    u8 *base;
    i = a0;
    if (0.0f <= f) {
        i2 = i;
        i1 = i + 1;
        if (D_800EE490 <= i1) {
            i1 = i1 - D_800EE490;
        }
    } else {
        s32 t;
        t = i;
        i2 = t - 1;
        i1 = t;
        if (i2 < 0) {
            i2 = i2 + D_800EE490;
        }
        f = f + 1.0f;
    }
    base = D_800EE498;
    r = *(f32 *)(base + i2 * 0x6c + 0x24) * (1.0f - f) + *(f32 *)(base + i1 * 0x6c + 0x24) * f;
    *out = (s16)((r * 32768.0f) / 3.1415927f);
}
