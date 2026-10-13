#include "ctx.h"

extern u32 D_80100650;
extern u8 *D_8010065C;

u32 func_800BDF18(s32 param_1)
{
    u32 r = 0;
    s32 n = param_1;
    n -= 1;

    if (param_1 != 0) {
        do {
            n = n - 1;
            if (D_80100650 == 1) {
                u32 b = *D_8010065C;
                D_8010065C = D_8010065C + 1;
                D_80100650 = b + 0x100;
            }
            r = (r << 1) | (D_80100650 & 1);
            D_80100650 = D_80100650 >> 1;
        } while (n != -1);
    }
    return r;
}
