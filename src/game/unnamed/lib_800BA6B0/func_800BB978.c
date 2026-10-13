#include "ctx.h"

extern u8 D_80100500[];

void func_800BB978(s32 k)
{
    s32 i = 0;
    s32 x;
    u8 *dst;
    do {
        x = (s8)i * k;
        dst = D_80100500 + i;
        if (x < 0) {
            x += 0xFF;
        }
        *dst = (u8)(x >> 8);
        i++;
    } while (i < 0x100);
}
