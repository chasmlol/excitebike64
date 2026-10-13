#include "game.h"

void func_80073200(void)
{
    func_800B6AA8();
    func_800B6D80();
    D_800F4780 = 0;
    func_800B1B40(&D_800F2220, 1, (void *)func_80073260, 0, &D_800F25D0, 8);
    func_800B1DE0(&D_800F2220);
    for (;;) {}
}

void func_80073260(s32 param_1) {
    func_800B1B40(D_800F45D0, 3, (void *)func_8000D2E0, param_1, X_800F45D0, 10);
    func_800B1DE0(D_800F45D0);
    func_800B1D10(NULL, 0);
    func_8000E9B8();
}
