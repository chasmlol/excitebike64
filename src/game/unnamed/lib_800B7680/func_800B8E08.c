#include "ctx.h"

extern s32 D_800D33F0;
extern s32 *D_800FE860;

void func_800B8E08(void)
{
    if (D_800D33F0 != 0) {
        ((void (*)(void))(((s32 *)D_800FE860)[0x1C / 4]))();
    }
}
