#include "ctx.h"

extern u8 D_800F0B30;

u8 func_80061FB0(void)
{
    D_800F0B30 = D_800F0B30 ^ 1;
    return D_800F0B30;
}
