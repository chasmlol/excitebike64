#include "ctx.h"

extern s32 D_800CCFFC;
extern void func_800BCB04(s32 a);

void func_80072A9C(void)
{
    if (D_800CCFFC != 0) {
        func_800BCB04(D_800CCFFC);
        D_800CCFFC = 0;
    }
}
