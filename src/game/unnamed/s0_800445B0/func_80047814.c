#include "ctx.h"

s32 func_8002E920(s32 a, s32 b, s32 c);

f32 func_80047814(f32 *p)
{
    s32 iv;
    f32 d, t, a4, r;

    iv = func_8002E920((s32)(p[0] * 1000.0f), (s32)(p[1] * 1000.0f), (s32)(p[2] * 1000.0f));
    d = p[1] - (f32)iv / 1000.0f;
    if (d < 0.0f) {
        return 0.0f;
    }
    a4 = p[4];
    t = __builtin_sqrtf(a4 * a4 + d * 29.400002f) / 14.700001f;
    r = -(-a4 / 14.700001f) + t;
    if (!(r > 0.0f)) {
        r = t + -a4 / 14.700001f;
    }
    return r;
}
