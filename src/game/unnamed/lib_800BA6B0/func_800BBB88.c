#include "ctx.h"

extern u8 D_800FE8F0[];

s32 func_800BBB88(s32 a0)
{
    return *(s32 *)(D_800FE8F0 + a0 * 0x1C);
}
