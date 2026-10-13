#include "ctx.h"

u32 func_800AEC10(u8 *s);
s32 func_8000ECE0(s32 a0);

u32 func_800C02E4(u8 *p)
{
    u32 i = 0;
    u32 sum = 0;
    u32 len;
    u8 c;

    for (;;) {
        len = func_800AEC10(p);
        if (len <= i || (s32)i >= 16) {
            break;
        }
        c = p[i];
        i = i + 1;
        sum = sum + func_8000ECE0(c);
    }
    return sum & 0xFF;
}
