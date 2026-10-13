#include "ctx.h"

extern u32 *D_800FE860;

void func_800B8E70(void)
{
    ((void (*)(void))(*(u32 *)((u8 *)D_800FE860 + 0x24)))();
}
