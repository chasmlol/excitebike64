#include "ctx.h"

u8 *func_800C1F04(u8 *p)
{
    if (p != 0) {
        p[0x20] = 0;
        *(s32 *)(p + 0x10) = 0;
        *(s32 *)(p + 0x14) = 0;
        *(s32 *)(p + 0x18) = 0;
        *(s32 *)(p + 0x1C) = 0;
    }
    return p;
}
