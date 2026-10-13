#include "ctx.h"

s32 func_8003F600(s32 a, s32 b, s32 c) {
    s32 t = (a + b) * (a - b);
    s32 d = c << 1;

    return t / d + c / 2;
}
