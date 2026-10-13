#include "ctx.h"

extern s32 D_800E7728;
extern s32 D_800D06D0;
extern s32 D_800D08E0;
extern s32 D_800F18E8;
extern s32 D_800F18EC;
extern s32 D_800F18F0[];

extern s32 func_800BF79C(s32, s32);
extern void func_80014B84(void *, s32, void *, s32);

void func_8006EE58(void)
{
    D_800F18E8 = func_800BF79C(D_800E7728, D_800D06D0);
    D_800F18EC = func_800BF79C(D_800E7728, D_800D08E0);
    func_80014B84(D_800F18F0, D_800F18E8, &D_800F18EC, 0xC);
}
