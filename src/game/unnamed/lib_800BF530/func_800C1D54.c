#include "ctx.h"

extern s32 D_800D5758;
extern s32 D_800D575C;
extern s32 D_800D5760;
extern s32 D_800D5764;
extern s32 D_800D5768;
extern s32 D_800D576C;

void func_800C1D54(s32 a, s32 b, s32 c, s32 d)
{
    D_800D5758 = a;
    D_800D575C = b;
    D_800D5768 = c;
    D_800D576C = d;
    D_800D5760 = 0x4000000 / a;
    D_800D5764 = 0x4000000 / b;
}
