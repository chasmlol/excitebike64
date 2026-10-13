#include "ctx.h"

extern s32 D_800E7728;
extern s32 D_800F073C;
s32 func_800BF79C(s32 a, s32 b);

s32 func_8005AF4C(s32 *p) {
    s32 r = 0;
    if (p != NULL) {
        D_800F073C = func_800BF79C(D_800E7728, *p);
        r = D_800F073C != 0;
    }
    return r;
}
