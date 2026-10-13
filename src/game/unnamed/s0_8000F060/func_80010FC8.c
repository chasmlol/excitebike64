#include "ctx.h"

extern s32 D_800D11D0;
extern s32 D_800D1230;
extern s32 D_800D1240;
extern s32 D_800D11CC;
extern s32 D_800E3290[];
extern s32 func_800B781C(s32 a);

void func_80010FC8(void)
{
    D_800E3290[0] = func_800B781C(D_800D11D0);
    D_800E3290[1] = func_800B781C(D_800D1230);
    D_800E3290[2] = func_800B781C(D_800D1240);
    D_800E3290[3] = func_800B781C(D_800D11CC);
}
