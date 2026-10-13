#include "boot.h"

void boot_func_80000450(void)
{
    func_8000C2D8();
    func_8000C5B0();
    func_800084E0(&D_80011600, 1, boot_func_800004A8, NULL, D_800137B0, 8);
    func_80008780(&D_80011600);
    for (;;) {
    }
}

void boot_func_800004A8(s32 param_1) {
    boot_func_800084E0(&D_800137B0, 3, (void *)boot_func_800010D0, param_1, &D_80015960, 10);
    boot_func_80008780(&D_800137B0);
    boot_func_800086B0((void *)0, 0);
    while (1) {
    }
}
