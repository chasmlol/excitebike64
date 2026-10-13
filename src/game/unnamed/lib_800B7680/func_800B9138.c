#include "ctx.h"

extern s32 *D_800FE860;

void func_800B9138(void)
{
    ((void (*)(void))(((s32 *)D_800FE860)[0x68 / 4]))();
}
