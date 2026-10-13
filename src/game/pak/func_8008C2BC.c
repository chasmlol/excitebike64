#include "ctx.h"

extern u32 D_800E2004;
extern u8 D_800E1C78[];
extern s32 func_800B4090(void *, s32, u8, s32, s32, u8 *);
extern void func_8000CD00(void);

s32 func_8008C2BC(u32 param_1, s32 param_2, u32 param_3, u32 param_4, u8 *param_5)
{
    s32 r;

    if ((D_800E2004 & (1 << param_1)) == 0) {
        return -1;
    }
    func_8000CD00();
    r = func_800B4090((u8 *)D_800E1C78 + param_1 * 0x68, param_2, 0, param_3, param_4, param_5);
    if (r == 0) {
        return 0;
    }
    return -r;
}
