#include "ctx.h"

typedef struct {
    u8 ram[0x10000];
    u8 pad[0x4100];
    u16 pc;
    u8 a, x, y, s, v, b, n, d, i, z, c, p;
} Nes;

#define NES (*(Nes *)0x80380000)

void func_800A2B14(void)
{
    NES.p = (NES.n ? 0x80 : 0) | (NES.v ? 0x60 : 0x20)
        | (NES.b ? 0x10 : 0) | (NES.d ? 8 : 0) | (NES.i ? 4 : 0)
        | (NES.z ? 2 : 0) | (NES.c ? 1 : 0);
    NES.ram[0x100 + NES.s] = NES.p;
    NES.s--;
}
