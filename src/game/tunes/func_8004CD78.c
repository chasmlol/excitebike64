#include "ctx.h"

extern s32 D_800C8CE8;
extern s32 D_800CCD64;
extern s32 D_800C2358;
extern s32 D_800CA858;
extern s32 D_800EFD00[];

static inline void push(s32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00[D_800CA858] = v;
        D_800CA858++;
    }
}

void func_8004CD78(s32 param_1)
{
    if (param_1 == 0) {
        push(0x83);
    } else if (param_1 == 1) {
        push(0x82);
    } else if (param_1 == 2) {
        push(0x81);
    }
}
