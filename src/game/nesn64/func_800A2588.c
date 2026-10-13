#include "ctx.h"

typedef struct { u8 ram[0x10000]; u8 pad[0x4100]; u16 pc; u8 a, x, y, s, v, b, n, d, i, z, c, p; } Nes;
#define NES (*(Nes *)0x80380000)

extern s32 D_800F5BB0;
extern u32 D_800F5BB8;
void func_800A2178(s32 a, u32 b);

void func_800A2588(void)
{
    s32 addr = D_800F5BB0;

    if (addr == -1) {
        NES.c = NES.a & 0x80;
        NES.a <<= 1;
        NES.z = NES.a == 0;
        NES.n = NES.a & 0x80;
    } else {
        u32 v = D_800F5BB8 << 1;
        NES.c = ((u8 *)&D_800F5BB8)[3] & 0x80;
        D_800F5BB8 = v;
        if (addr < 0x2000) {
            NES.ram[addr & 0x7FF] = v;
        } else if (addr < 0x8000) {
            func_800A2178(addr, v);
        }
        NES.z = (D_800F5BB8 & 0xFF) == 0;
        NES.n = ((u8 *)&D_800F5BB8)[3] & 0x80;
    }
}
