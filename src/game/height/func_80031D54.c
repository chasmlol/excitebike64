#include "ctx.h"

s32 func_80031D54(s32 *p1, s32 *p2, s32 x, s32 y)
{
    s32 b0 = p2[0];
    s32 a1 = p1[1];
    s32 a0 = p1[0];
    s32 b1 = p2[1];
    s32 a2;
    s32 b2;

    if ((y - b0) * (a1 - a0) - (x - a0) * (b1 - b0) <= 0) {
        return 0;
    }
    a2 = p1[2];
    b2 = p2[2];
    if ((y - b1) * (a2 - a1) - (x - a1) * (b2 - b1) <= 0) {
        return 0;
    }
    return (y - b2) * (a0 - a2) - (x - a2) * (b0 - b2) > 0;
}
