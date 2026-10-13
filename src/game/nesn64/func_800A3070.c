#include "ctx.h"

extern s32 func_800B1330(void);
extern s32 D_800D05D4;
extern s32 D_800F5BB8;

void func_800A3070(void)
{
    u8 *st;
    s32 a;
    s32 b;
    u32 addr;

    if (D_800D05D4 != 0) {
        a = func_800B1330();
        addr = ((a << 2) & 0xFFFFF) + 0x80000000;
        b = func_800B1330();
        *(s32 *)addr = b;
    }
    st = (u8 *)0x80390000;
    if (st[0x410C] != 0) {
        *(u16 *)(st + 0x4100) = D_800F5BB8;
        *(s32 *)(st + 0x4110) += 2;
    }
}
