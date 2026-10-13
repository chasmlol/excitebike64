#include "ctx.h"

extern s32 D_800E9228[];
extern u32 func_8000EFE8(void);

void func_8003AA8C(s32 param_1)
{
    u32 r = func_8000EFE8();
    D_800E9228[param_1 * 2 + (r & 1)] = 0xF;
}
