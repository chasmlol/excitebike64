#include "ctx.h"

extern s32 *D_800FE860;
extern s32 D_800D33F0;

void func_800B8C8C(void)
{
    ((void (*)(void))(((s32 *)D_800FE860)[8 / 4]))();
    D_800D33F0 = 0;
}
