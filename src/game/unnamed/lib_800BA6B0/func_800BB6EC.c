#include "ctx.h"

extern u8 *D_800FE8D4;

s32 func_800BB6EC(s32 a0)
{
    return *(s32 *)(D_800FE8D4 + a0 * 0x30 + 8);
}
