#include "ctx.h"

extern u16 D_800F5BBA;

void func_800A3180(void)
{
    u8 *nes;
    u16 h;
    s32 c;

    nes = (u8 *)0x80390000;
    if (nes[0x410b] == 0) {
        h = D_800F5BBA;
        c = *(s32 *)(nes + 0x4110) + 2;
        *(u16 *)(nes + 0x4100) = h;
        *(s32 *)(nes + 0x4110) = c;
    }
}
