#include "ctx.h"

void func_800A1BA0(s32 a, s32 b);
void func_800A1DCC(s32 a, s32 b);
void func_800A1F8C(s32 a, s32 b);

void func_800A4190(s32 p, u8 v)
{
    s32 u;

    ((u8 *)0x80380000)[p] = v;
    u = (p - 0x4000) >> 2;
    if (u < 2) {
        func_800A1BA0(u, p & 3);
    } else if (u == 2) {
        func_800A1DCC(2, p & 3);
    } else if (u == 3) {
        func_800A1F8C(3, p & 3);
    }
}
