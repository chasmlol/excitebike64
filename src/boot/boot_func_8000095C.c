#include "ctx.h"

extern s32 D_8000CDF0;
extern char D_80011240[];
extern s32 boot_func_8000292C(char *, s32);
extern void boot_func_80002B78(s32, u8 *, u32);

void boot_func_8000095C(u8 *param_1, u32 param_2)
{
    if (D_8000CDF0 == -1) {
        D_8000CDF0 = boot_func_8000292C(D_80011240, 0);
    }
    boot_func_80002B78(D_8000CDF0, param_1, param_2);
}
