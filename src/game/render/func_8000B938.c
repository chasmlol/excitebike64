#include "ctx.h"

extern s32 *D_800EFD64;
extern s32 D_800E8090[];
void func_80011490();

void func_8000B938(s32 p1, s32 p2, s32 p3, s32 p4, s32 p5, s32 p6, s32 p7, s32 p8, s32 p9)
{
    s32 *t;
    if (D_800EFD64[11] == 0) {
        func_80011490(p1, p2, p3, p4, p5, p6, p7, p8);
    } else {
        s32 *tb = D_800E8090;
        t = tb + p9 * 3;
        func_80011490(p1, p2, p3, p4, p5 - t[0], p6 - t[1], p7 - t[2], p8);
    }
}
