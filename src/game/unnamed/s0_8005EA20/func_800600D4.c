#include "ctx.h"

s32 func_800600D4(u8 *p, s32 n)
{
    s32 sum;
    s32 i;
    s32 lim;
    u32 hdr;
    u8 *q;

    if (p[0] != 0x7B) {
        return 1;
    }
    if (n != 0x2800) {
        return 1;
    }
    q = p + 4;
    lim = 0x27FC;
    sum = 0;
    i = sum;
    do {
        u8 b = q[i];
        sum += b << (i % 8);
        i++;
    } while (i < lim);
    sum &= 0xFFFFFF;
    hdr = p[1] | (p[2] << 8) | (p[3] << 16);
    return (hdr ^ sum) != 0;
}
