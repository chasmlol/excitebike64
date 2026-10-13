#include "ctx.h"

f32 func_80013774(f32 x)
{
    f32 r, u, p;
    s32 i;

    if (10.0f < x) {
        return 0.0f;
    }
    if (x < -10.0f) {
        return 0.0f;
    }
    if (1.0f < x) {
        return func_80013774(x - 1.0f) * 2.7182817f;
    }
    if (x < -1.0f) {
        return func_80013774(x + 1.0f) / 2.7182817f;
    }
    r = x;
    u = x;
    i = 2;
    do {
        u = u * x / i;
        i++;
        r += u;
        u = u * x / i;
        i++;
        p = u * 65536.0f;
        r += u;
    } while (r > 0.0f ? p < -r : p < r);
    return r + 1.0f;
}
