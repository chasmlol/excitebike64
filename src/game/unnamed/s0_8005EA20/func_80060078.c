#include "ctx.h"

s32 func_80060078(u8 *s, u32 *a1, u32 *a2, u32 *a3)
{
    u32 v;

    if (s[0] != 0x7B) {
        return 1;
    }
    *a1 = s[6];
    *a2 = s[7];
    v = s[0x14];
    *a3 = v;
    v = v + (s[0x15] << 8);
    *a3 = v;
    *a3 = v + (s[0x16] << 16);
    return 0;
}
