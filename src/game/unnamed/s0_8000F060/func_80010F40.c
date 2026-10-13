#include "ctx.h"

extern void func_80011098(s32 *p);
extern u8 D_800E32A8[];

void func_80010F40(void) {
    s32 i;
    u8 *p;
    u8 *hb;

    i = 0;
    hb = (u8 *)0x800C0000;
    p = D_800E32A8;
    do {
        if (*(s32 *)(hb + 0x2B64) != 0) {
            func_80011098((s32 *)(p + 0xC));
            func_80011098((s32 *)(p + 0x4));
            func_80011098((s32 *)(p + 0x44));
            func_80011098((s32 *)(p + 0x50));
            func_80011098((s32 *)(p + 0x4C));
            func_80011098((s32 *)(p + 0x48));
        }
        i++;
        p += 0x64;
    } while (i < 8);
}
