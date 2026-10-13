#include "ctx.h"

extern u32 D_800E2004;
extern u8 D_800E1C78[];
void func_8000CD00(void);
s32 func_800B4090(void *blk, s32 a1, s32 a2, s32 a3, s32 a4, u8 *a5);

s32 func_8008C370(u32 p1, s32 p2, u32 p3, u32 p4, u8 *p5)
{
    s32 r;

    if ((D_800E2004 & (1u << p1)) == 0) {
        return -1;
    }
    func_8000CD00();
    r = func_800B4090(D_800E1C78 + p1 * 0x68, p2, 1, p3, p4, p5);
    if (r != 0) {
        return -r;
    }
    return 0;
}
