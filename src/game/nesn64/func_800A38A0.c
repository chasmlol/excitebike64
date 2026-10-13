#include "ctx.h"

extern s32 D_800F5BB0;
void func_800A2178(s32 a, u32 b);

void func_800A38A0(void) {
    s32 a = D_800F5BB0;
    u32 v = *(u8 *)0x80394103;
    if (a < 0x2000) {
        ((u8 *)0x80380000)[a & 0x7FF] = v;
    } else if (a < 0x8000) {
        func_800A2178(a, v);
    }
}
