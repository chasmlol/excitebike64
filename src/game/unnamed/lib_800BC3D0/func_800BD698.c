#include "ctx.h"

extern s32 D_80100630;
extern s32 D_80100634;
extern s32 D_80100638;
extern s32 D_8010063C;
extern s32 D_80100640;
extern s32 D_80100644;
extern s32 D_80100648;
s32 func_800BDA54(s32 *a, s32 *b);

s32 func_800BD698(s32 p0, s32 p1, s32 a, s32 b)
{
    s32 sum;
    s32 *pa;
    s32 *pb;

    pa = &D_80100640;
    pb = &D_80100644;
    D_80100638 = 0;
    D_8010063C = 0;
    D_80100634 = 0;
    D_80100630 = 2;
    *pa = a;
    sum = a + b;
    D_80100644 = sum;
    D_80100648 = func_800BDA54(pa, pb);
    D_80100634 = D_80100634 + ((D_80100644 - D_80100640) >> 4);
    return 0;
}
