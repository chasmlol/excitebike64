#include "ctx.h"

extern s32 D_800EE128;

u8 func_800444C4(void)
{
    s32 x;
    D_800EE128 = (D_800EE128 * 13791 + 0x3038) % 65531;
    x = D_800EE128;
    return x & 0xff;
}
