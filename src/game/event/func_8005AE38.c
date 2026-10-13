#include "ctx.h"

extern u8 *D_800CA930;

s32 func_8005AE38(void)
{
    return *(s16 *)(D_800CA930 + 0x64) != 0;
}
