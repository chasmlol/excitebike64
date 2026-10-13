#include "ctx.h"

extern u32 *D_800FE860;

void func_800B9F4C(void)
{
    ((void (*)(s32))(*(u32 *)((u8 *)D_800FE860 + 0x70)))(0);
}
