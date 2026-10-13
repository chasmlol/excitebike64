#include "ctx.h"

extern u8 D_800CAAA1;
extern u8 D_800F1BC1;

void func_8006F5F8(s32 x)
{
    u8 buf[0x50];
    if (x == 1) {
        D_800F1BC1 = D_800CAAA1;
    }
}
