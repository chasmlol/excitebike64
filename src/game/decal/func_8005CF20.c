#include "ctx.h"

f32 func_8005CF20(f32 x)
{
    f32 a;
    f32 b;
    f32 sx;
    f32 res;

    sx = x;
    a = x;
    if (0.0f < sx) {
        a = x;
    } else {
        a = -x;
    }
    b = 1.0f;
    for (; b < a; b = b * 10.0f) {
    }
    for (; a < b; b = b / 10.0f) {
    }
    res = a;
    if (b == 0.0f) {
        return res;
    }
    a = ((f32)(s32)((a * 100.0f) / b + 0.5f) * b) / 100.0f;
    if (0.0f < sx) {
        return a;
    }
    return -a;
}
