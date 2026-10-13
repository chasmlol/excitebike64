#include "ctx.h"

extern u8 D_800C9A40[];

s32 func_8003F534(s32 a0, s32 *a1)
{
    u8 *base;
    u8 *rec;

    base = D_800C9A40;
    rec = base + a0 * 0x3C;
    *a1 = *(s32 *)rec;
    return *(s32 *)(*(u8 **)(rec + 0xC) + 4);
}
