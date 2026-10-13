#include "ctx.h"

extern u8 D_800E32A8[];
extern s32 D_800E76D0;

void func_80010E28(s32 a0, s32 a1, s32 a2, s32 a3)
{
    u8 *base;
    u8 *rec;
    s32 off;
    s32 v;

    base = D_800E32A8;
    off = a0 * 100;
    v = D_800E76D0;
    rec = base + off;
    *(s32 *)(rec + 0x40) = a1;
    *(s32 *)(rec + 0x38) = v;
}
