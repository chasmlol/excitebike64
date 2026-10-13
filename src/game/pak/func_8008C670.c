#include "ctx.h"

extern u8 D_800CDE74[];

void func_8008C670(u8 *buf, s32 n)
{
    s32 i = 0;
    u8 *p;
    if (n > 0) {
        do {
            p = buf + i;
            if ((u8)(*p - 0xF) < 0x33) {
                *p = D_800CDE74[*p - 0xF];
            } else {
                if (*p != 0) {
                    goto nonzero;
                }
                *p = 0;
                goto next;
            nonzero:
                *p = 0x2A;
            }
        next:
            i++;
        } while (i < n);
    }
}
