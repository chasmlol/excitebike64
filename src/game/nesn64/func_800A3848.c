#include "ctx.h"

extern s32 D_800F5BB0;
void func_800A2178();

void func_800A3848(void)
{
    u8 *nes = (u8 *)0x80390000;
    s32 a = D_800F5BB0;
    u32 v = nes[0x4102];
    if (a < 0x2000) {
        *(u8 *)((a & 0x7FF) + 0x80380000) = v;
    } else if (a < 0x8000) {
        func_800A2178(a, v);
    }
}
