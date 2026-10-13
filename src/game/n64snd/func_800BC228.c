#include "ctx.h"

extern u32 D_80100618;
s32 func_800BB7A4(void);

u32 func_800BC228(void) {
    u32 r = func_800BB7A4();
    return r / (D_80100618 >> 1);
}
