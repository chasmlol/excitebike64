#include "boot.h"

void boot_func_8000095C(u8 *param_1, u32 param_2)
{
    if (D_8000CDF0 == -1) {
        D_8000CDF0 = boot_func_8000292C(D_80011240, 0);
    }
    boot_func_80002B78(D_8000CDF0, param_1, param_2);
}

void boot_func_800009C4(u8 *p)
{
    u32 n;

    if (D_8000CDF0 == -1) {
        D_8000CDF0 = boot_func_8000292C(D_80011240, 0);
    }
    n = boot_func_80007460((char *)p);
    boot_func_80002B78(D_8000CDF0, p, n);
}
