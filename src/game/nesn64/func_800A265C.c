#include "ctx.h"
typedef struct { u8 ram[0x10000]; u8 pad[0x4100]; u16 pc; u8 a, x, y, s, v, b, n, d, i, z, c, p; } Nes;
#define NES (*(Nes *)0x80380000)

extern s32 D_800F5BB0;
extern s32 D_800F5BB8;
void func_800A2178(s32 a, s32 b);

static inline void wr(s32 a, s32 v)
{
    if (a < 0x2000) {
        *(u8 *)((a & 0x7FF) + 0x80380000) = v;
    } else if (a > 0x7FFF) {
    } else {
        func_800A2178(a, v);
    }
}

void func_800A265C(void)
{
    if (D_800F5BB0 == -1) {
        NES.c = NES.a & 1;
        NES.a >>= 1;
        if (NES.a & 0x40) {
            NES.a |= 0x80;
        }
        NES.z = NES.a == 0;
        NES.n = NES.a & 0x80;
    } else {
        NES.c = D_800F5BB8 & 1;
        D_800F5BB8 >>= 1;
        if (D_800F5BB8 & 0x40) {
            D_800F5BB8 |= 0x80;
        }
        wr(D_800F5BB0, D_800F5BB8);
        NES.z = (D_800F5BB8 & 0xFF) == 0;
        NES.n = ((u8 *)&D_800F5BB8)[3] & 0x80;
    }
}
