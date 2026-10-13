#include "ctx.h"

void func_8000BBC8(u8 *param_1, u8 *param_2, u8 *param_3, s32 param_4, s32 param_5)
{
    s32 total = param_5 * 3;
    s32 i = 0;
    u8 *pa = param_1;
    u8 *pb = param_2;
    u8 *po = param_3;
    if (total > 0) {
        s32 inv = 0xFF - param_4;
        s32 lim = total;
        do {
            *po = ((s32)*pa * inv + (s32)*pb * param_4) >> 8;
            pa++;
            i++;
            pb++;
            po++;
        } while (i < lim);
    }
}
