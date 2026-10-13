#include "ctx.h"

extern s32 D_8000CDF0;
extern char D_80011240[];

s32 boot_func_8000292C(char *s, s32 m);
u32 boot_func_80007460(char *s);
void boot_func_80002B78(s32 h, u8 *p, u32 n);

void boot_func_800009C4(u8 *p)
{
    u32 n;

    if (D_8000CDF0 == -1) {
        D_8000CDF0 = boot_func_8000292C(D_80011240, 0);
    }
    n = boot_func_80007460((char *)p);
    boot_func_80002B78(D_8000CDF0, p, n);
}
