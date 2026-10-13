#include "ctx.h"

extern u8 *D_800CA930;
extern s32 D_800CADE4[];

s32 func_8005B55C(void)
{
    s32 idx = *(s16 *)(D_800CA930 + 0x64);
    return D_800CADE4[idx * 10 + 7] != 0;
}
