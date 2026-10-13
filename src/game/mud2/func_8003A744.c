#include "ctx.h"

extern Vec3f D_800E9278;
extern s32 D_800C9998;
extern s32 D_800E76D0;
extern s32 D_800E9294;

void func_8003A744(Vec3f *src, s32 val)
{
    Vec3f *dst = &D_800E9278;
    *dst = *src;
    D_800C9998 = val;
    D_800E9294 = D_800E76D0;
}
