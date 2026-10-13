#include "ctx.h"

extern s32 D_800F5BB0;

void func_800A4018(void)
{
    u32 a = *(u16 *)0x80394100;
    D_800F5BB0 = *(u8 *)(a + 0x80380000 + 1);
}
