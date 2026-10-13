#include "ctx.h"

extern s32 D_800F47C0;
void func_800BFDD4(s32 a);

void func_80087E00(void)
{
    func_800BFDD4(D_800F47C0);
    D_800F47C0 = 0;
}
