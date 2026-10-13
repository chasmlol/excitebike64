#include "ctx.h"

extern s32 D_800F5BB8;

void func_800A363C(void)
{
    u8 *nes = (u8 *)0x80390000;

    nes[0x4104] = D_800F5BB8;
    nes[0x410B] = (D_800F5BB8 & 0xFF) == 0;
    nes[0x4108] = D_800F5BB8 & 0x80;
}
