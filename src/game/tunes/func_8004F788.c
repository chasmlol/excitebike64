#include "ctx.h"

extern void func_800BCB04(s32 a);
extern void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_8004F788(u8 *param_1)
{
    if (*(s32 *)(param_1 + 8) != 0) {
        func_800BCB04(*(s32 *)(param_1 + 8));
        func_800BCB04(*(s32 *)(param_1 + 0xC));
        func_8000ED10(param_1, 0, 0x4C);
        *(s32 *)(param_1 + 4) = -1;
        *(s32 *)(param_1 + 0x10) = -1;
        *(s32 *)(param_1 + 0x30) = -1;
    }
}
