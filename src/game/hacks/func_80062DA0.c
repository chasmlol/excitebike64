#include "ctx.h"

extern u8 *D_800F0B58;

s32 func_80062DA0(s32 a0)
{
    if (D_800F0B58 != 0) {
        a0 = ((s32 (*)(s32))(*(u32 *)(D_800F0B58 + 0x14)))(a0);
    }
    return a0;
}
