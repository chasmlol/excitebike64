#include "ctx.h"

extern u32 D_800E2004;
extern u8 D_800E1C78[];
extern void func_8000CD00(void);
extern int func_800B4B10(u32 *, int *);

int func_8008C068(u32 param_1)
{
    int local_10[2];

    if ((D_800E2004 & (1 << param_1)) == 0) {
        return -1;
    }
    func_8000CD00();
    if (func_800B4B10((u32 *)(D_800E1C78 + param_1 * 0x68), local_10) != 0) {
        return -2;
    }
    return local_10[0];
}
