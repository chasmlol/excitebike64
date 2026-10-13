#include "ctx.h"

s32 func_80044530(s32 param_1, s32 param_2)
{
    return ((param_1 * (param_2 + 0x7B) * 0x1C8) << (param_1 % 0x315A)) + param_2;
}
