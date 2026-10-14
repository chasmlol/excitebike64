#include "boot.h"

void boot_func_80000450(void)
{
    __osInitialize_common();
    __osInitialize_autodetect();
    osCreateThread(&D_80011600, 1, boot_func_800004A8, NULL, &D_800137B0, 8);
    osStartThread(&D_80011600);
    for (;;) {
    }
}

void boot_func_800004A8(s32 param_1) {
    osCreateThread(&D_800137B0, 3, (void *)boot_func_800010D0, param_1, &D_80015960, 10);
    osStartThread(&D_800137B0);
    osSetThreadPri((void *)0, 0);
    while (1) {
    }
}
