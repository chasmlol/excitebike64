#include "ctx.h"

extern u32 D_A0000200;

s32 func_80065AF0(void) {
    return D_A0000200 == 0xAC290000;
}
