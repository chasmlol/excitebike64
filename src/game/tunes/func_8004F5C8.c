#include "ctx.h"

extern s32 D_800C8CE8;
extern s32 D_800CCD64;
extern s32 D_800C2358;
extern s32 D_800CA858;
extern u32 D_800EFD00[];

static inline void push(u32 v)
{
    if (D_800C8CE8 == 0 && D_800CCD64 == 0 && D_800C2358 == 0 && D_800CA858 < 8) {
        D_800EFD00[D_800CA858] = v;
        D_800CA858++;
    }
}

void func_8004F5C8(s32 on)
{
    if (on) {
        push(0x92);
    } else {
        push(0x93);
    }
}
