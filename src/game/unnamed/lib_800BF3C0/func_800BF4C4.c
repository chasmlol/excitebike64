#include "ctx.h"

extern s32 D_80100700;
extern void func_800AD910(s32, s32);
extern s32 func_800BEC20(s32, s32, s32, s32, s32);
extern void func_800BEFB4(s32);

s32 func_800BF4C4(s32 p)
{
    s32 r;

    func_800AD910(p, 0x8000);
    func_800BEC20(D_80100700, 1, 0x08000000, p, 0x8000);
    if (r < 0) {
        return 1;
    }
    func_800BEFB4(r);
    return 0;
}
