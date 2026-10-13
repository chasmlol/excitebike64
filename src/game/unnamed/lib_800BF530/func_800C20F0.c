#include "ctx.h"

void func_800C20F0(u8 *s)
{
    u8 c = *s;
    while (c != 0) {
        if ((u32)(c - 0x61) < 0x1A) {
            *s = c - 0x20;
        }
        s = s + 1;
        c = *s;
    }
}
