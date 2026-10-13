#include "ctx.h"

extern char D_800D8444[];
extern char D_800D8448[];
extern char D_800D844C[];

void func_8000EE30(s32 v, char *buf);
void func_8000ED30(char *dst, char *src);
s32 func_800AEC10(char *s);

void func_8005313C(s32 param_1, char *param_2)
{
    s32 iVar1;
    char acStack_30[32];

    *param_2 = 0;
    if (param_1 > 0xe0f) {
        func_8000EE30(param_1 / 0xe10, param_2);
        func_8000ED30(param_2, D_800D8448);
    }
    func_8000EE30((param_1 / 0x3c) % 0x3c, acStack_30);
    iVar1 = func_800AEC10(acStack_30);
    if ((iVar1 == 1) && (param_1 > 0xe0f)) {
        func_8000ED30(param_2, D_800D8444);
    }
    func_8000ED30(param_2, acStack_30);
    func_8000ED30(param_2, D_800D844C);
    func_8000EE30(((param_1 % 0x3c) * 100) / 0x3c, acStack_30);
    iVar1 = func_800AEC10(acStack_30);
    if (iVar1 == 1) {
        func_8000ED30(param_2, D_800D8444);
    }
    func_8000ED30(param_2, acStack_30);
    return;
}
