#include "ctx.h"

extern void func_800B1B40(void *a, s32 b, void *c, s32 d, void *e, s32 f);
extern void func_800B1DE0(void *a);
extern void func_800B1D10(void *a, s32 b);
extern void func_8000E9B8(void);
extern void func_8000D2E0(void);
extern u8 D_800F45D0[];
extern u8 X_800F45D0[];

void func_80073260(s32 param_1) {
    func_800B1B40(D_800F45D0, 3, (void *)func_8000D2E0, param_1, X_800F45D0, 10);
    func_800B1DE0(D_800F45D0);
    func_800B1D10(NULL, 0);
    func_8000E9B8();
}
