#include "ctx.h"

extern s32 *D_800FE860;

void func_800B9F24(void)
{
    ((void (*)(s32))(((s32 *)D_800FE860)[0x70 / 4]))(1);
}
