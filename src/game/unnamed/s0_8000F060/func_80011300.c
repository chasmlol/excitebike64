#include "ctx.h"

extern s32 D_800C2B64;
extern u8 D_800E32A8[];

void func_80011098(s32 *p);

void func_80011300(s32 a0)
{
    u8 *s;

    if (D_800C2B64 != 0) {
        s = D_800E32A8 + a0 * 100;
        func_80011098((s32 *)(s + 0xC));
        func_80011098((s32 *)(s + 0x4));
        func_80011098((s32 *)(s + 0x44));
        func_80011098((s32 *)(s + 0x50));
        func_80011098((s32 *)(s + 0x4C));
        func_80011098((s32 *)(s + 0x48));
    }
}
