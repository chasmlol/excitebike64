#include "ctx.h"

extern u32 *D_800FE860;

void func_800B9A68(void)
{
    ((void (*)(void))(*(u32 *)((u8 *)D_800FE860 + 0x64)))();
}
