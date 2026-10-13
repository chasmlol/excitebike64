#include "ctx.h"

extern s32 D_800EE490;

void func_8002C5A4(s16 a, f32 b, u16 *c);

void func_8002D8C4(s16 p1, f32 f, u16 *p3)
{
    u16 loc[2];
    s32 i0, i1;

    i0 = p1 - 1;
    if (i0 < 0) {
        i0 += D_800EE490;
    }
    i1 = p1 + 1;
    if (D_800EE490 <= i1) {
        i1 -= D_800EE490;
    }
    func_8002C5A4((s16)i0, f, &loc[0]);
    func_8002C5A4((s16)i1, f, &loc[1]);
    *p3 = loc[1] - loc[0];
}
