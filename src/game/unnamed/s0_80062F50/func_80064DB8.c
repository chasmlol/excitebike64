#include "ctx.h"

extern u8 D_800F0C98[];
extern void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_80064DB8(void)
{
    func_8000ED10(D_800F0C98, 0, 8);
    D_800F0C98[1] = 1;
}
