#include "ctx.h"

void func_800BFDA4(s32 *p)
{
    s32 i = 0xFF;
    s32 *q = p + 0xFF;
    p[0x143] = 0;
    p[0x102] = 0;
    p[0] = 0;
    do {
        q[2] = 0;
        i = i - 1;
        q = q - 1;
    } while (i >= 0);
    p[1] = 0;
    p[0x144] = 0;
}
