#include "ctx.h"

extern u32 D_80100700;
void func_800AD7E0();
void func_800BEC20();
void func_800BEFB4();

int func_800BF460(u32 param_1)
{
    s32 x;

    func_800AD7E0(param_1, 0x8000);
    func_800BEC20(D_80100700, 0, 0x8000000, param_1, 0x8000);
    if (x < 0) {
        return 1;
    }
    func_800BEFB4(x);
    return 0;
}
