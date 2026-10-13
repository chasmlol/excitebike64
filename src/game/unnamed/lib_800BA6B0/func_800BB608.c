#include "ctx.h"

extern s32 D_800FE8D8;
extern s32 D_800FE8D4;
void func_8000EE20(void);
void func_8000EE28(void);

void func_800BB608(s32 param_1, s32 param_2)
{
    s32 iVar1;
    s32 iVar2;

    func_8000EE20();
    iVar1 = param_2 % D_800FE8D8 * 65536;
    iVar2 = iVar1 / D_800FE8D8;
    iVar1 = param_1 * 0x30 + D_800FE8D4;
    *(s32 *)(iVar1 + 0x10) = param_2 / D_800FE8D8;
    *(s32 *)(iVar1 + 0x14) = iVar2 * 65536;
    func_8000EE28();
}
