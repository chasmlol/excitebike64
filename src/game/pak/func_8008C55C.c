#include "ctx.h"

extern u8 D_800E1C48[];
extern u8 D_800E1C78[];

s32 func_8000CD00(void);
s32 func_800B4D20(void *a, void *b, s32 c);
s32 func_800B5820(void *a);

s32 func_8008C55C(u32 param_1)
{
    u8 *p;
    s32 r;

    func_8000CD00();
    p = D_800E1C78 + param_1 * 0x68;
    func_800B4D20(D_800E1C48, p, param_1);
    r = func_800B5820(p);
    func_800B4D20(D_800E1C48, p, param_1);
    return r;
}
