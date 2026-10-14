#include "game.h"

void func_80073200(void)
{
    __osInitialize_common();
    __osInitialize_autodetect();
    D_800F4780 = 0;
    osCreateThread(&D_800F2220, 1, (void *)func_80073260, 0, &D_800F25D0, 8);
    osStartThread(&D_800F2220);
    for (;;) {}
}

void func_80073260(s32 param_1) {
    osCreateThread(&D_800F45D0, 3, (void *)func_8000D2E0, param_1, X_800F45D0, 10);
    osStartThread(&D_800F45D0);
    osSetThreadPri(NULL, 0);
    func_8000E9B8();
}
