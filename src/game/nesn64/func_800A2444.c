#include "ctx.h"

extern volatile s32 D_803941AC;

u32 func_800A2444(s32 code, s32 unused, u32 val)
{
    u8 *ram;
    u8 *reg;
    s32 pos;
    s32 v;
    s32 n;
    s32 i;
    u32 mask;

    if (code == 0x2007) {
        ram = (u8 *)0x80380000;
        __asm__ __volatile__("");
        code = *(u16 *)(ram + 0x14114);
        unused = *(s32 *)(ram + 0x14118);
        val = ram[code + 0x10000];
        if (unused & 2) {
            *(s32 *)(ram + 0x14118) = unused & ~2;
        } else {
            if (ram[0x2000] & 4) {
                *(u16 *)(ram + 0x14114) = (code + 0x20) % 0x4000;
            } else {
                *(u16 *)(ram + 0x14114) = (code + 1) % 0x4000;
            }
        }
        return val;
    }
    if (code == 0x4016) {
        reg = (u8 *)0x80390000;
        n = *(s32 *)(reg + 0x4124);
        if (n < 8) {
            *(s32 *)(reg + 0x4124) = n + 1;
            val = (reg[0x4120] >> n) & 1;
        }
        return val;
    }
    if (code == 0x4015) {
        ram = (u8 *)0x80380000;
        val = 0;
        for (i = 0; i < 4; i++) {
            u8 *c = ram + i * 0x24 + 0x10000;
            if ((*(s32 *)(c + 0x414C) & 8) && *(s32 *)(c + 0x4144) < D_803941AC && *(s32 *)(c + 0x4134) != 0) {
                val |= 1 << i;
            }
        }
        return val & *(u8 *)0x80384015;
    }
    return *(u8 *)(code + 0x80380000);
}
