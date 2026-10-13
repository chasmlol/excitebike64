#include "ctx.h"

void func_800139A8(s32 *src, u16 *aux, s16 *dst)
{
    u16 *h = (u16 *)src;
    dst[0] = src[0] >> 14;
    dst[16] = h[1] << 2;
    dst[1] = src[1] >> 14;
    dst[17] = h[3] << 2;
    dst[2] = src[2] >> 14;
    {
        u16 t = h[5];
        dst[3] = 0;
        dst[19] = 0;
        dst[18] = t << 2;
    }
    dst[4] = src[3] >> 14;
    dst[20] = h[7] << 2;
    dst[5] = src[4] >> 14;
    dst[21] = h[9] << 2;
    dst[6] = src[5] >> 14;
    {
        u16 t = h[11];
        dst[7] = 0;
        dst[23] = 0;
        dst[22] = t << 2;
    }
    dst[8] = src[6] >> 14;
    dst[24] = h[13] << 2;
    dst[9] = src[7] >> 14;
    dst[25] = h[15] << 2;
    dst[10] = src[8] >> 14;
    {
        u16 t = h[17];
        dst[11] = 0;
        dst[27] = 0;
        dst[26] = t << 2;
    }
    {
        u16 t = aux[1];
        dst[28] = 0;
        dst[12] = t;
    }
    {
        u16 t = aux[3];
        dst[29] = 0;
        dst[13] = t;
    }
    {
        u16 t = aux[5];
        dst[30] = 0;
        dst[15] = 1;
        dst[31] = 0;
        dst[14] = t;
    }
}
