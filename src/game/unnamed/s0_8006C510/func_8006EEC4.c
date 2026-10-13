#include "ctx.h"

extern s32 D_800E7728;
extern s32 D_800D0DEC;
extern s32 D_800D0FB0;
extern s32 D_800D10F8;
extern s32 D_800D0770;
extern s32 D_800EFD6C;
extern s32 D_800F16E0;
extern s32 D_800F18E8;
extern s32 D_800F18EC;
extern u32 D_800F18F0[];
s32 func_800BF79C(s32 a, s32 b);
void func_80014B84(u32 *p1, u32 a1, u32 a2, u32 a3);

void func_8006EEC4(void)
{
    u32 pa;
    D_800EFD6C = func_800BF79C(D_800E7728, D_800D0DEC);
    D_800F18E8 = func_800BF79C(D_800E7728, D_800D0FB0);
    pa = (u32)&D_800F18EC;
    D_800F18EC = func_800BF79C(D_800E7728, D_800D10F8);
    D_800F16E0 = func_800BF79C(D_800E7728, D_800D0770);
    func_80014B84(D_800F18F0, D_800F18E8, pa, 12);
}
