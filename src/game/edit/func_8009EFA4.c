#include "ctx.h"

extern u32 D_800CE210;
extern u32 D_800CE21C;
extern u32 D_800CE240;

void func_8009EFA4(void)
{
    u32 *dst = &D_800CE21C;
    u32 *src = &D_800CE210;
    u32 a, b, c;

    a = src[0];
    b = src[1];
    c = dst[2];
    D_800CE240 = 0;
    dst[0] = a;
    dst[1] = b;
    dst[2] = ((c + 0x800) >> 13) << 13;
}
