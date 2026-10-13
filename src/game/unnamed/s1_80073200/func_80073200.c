#include "ctx.h"

extern void func_800B6AA8(void);
extern void func_800B6D80(void);
extern void func_800B1B40(void *a, s32 b, void *c, s32 d, void *e, s32 f);
extern void func_800B1DE0(void *a);
extern void func_80073260(void);
extern s32 D_800F2220;
extern s32 D_800F25D0;
extern s32 D_800F4780;

void func_80073200(void)
{
    func_800B6AA8();
    func_800B6D80();
    D_800F4780 = 0;
    func_800B1B40(&D_800F2220, 1, (void *)func_80073260, 0, &D_800F25D0, 8);
    func_800B1DE0(&D_800F2220);
    for (;;) {}
}
