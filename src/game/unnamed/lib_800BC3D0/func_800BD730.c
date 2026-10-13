#include "ctx.h"

s32 *func_800BC908(s32 a, s32 b, s32 c, s32 d);

s32 *func_800BD730(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 *p = 0;
    s32 k;
    if (a0 == 0) {
        return 0;
    }
    p = func_800BC908(a0, a1, a2, a3);
    if (p != 0) {
        s32 *q = p;
        k = p[-2] - 1;
        do {
            q[0] = 0;
            q[1] = 0;
            q[2] = 0;
            q[3] = 0;
            q += 4;
            k--;
        } while (k != 0);
    }
    return p;
}
