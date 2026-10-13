#include "ctx.h"

typedef struct {
    u8 b[0x100];
} Page;

typedef struct {
    u8 ram[0x10000];
    u8 vram[0x4000];
    u8 oam[0x100];
    u16 pc;
    u8 a, x, y, s, v, b, n, d, i, z, c, p;
    u8 pad0[6];
    u16 vaddr;
    u16 pad1;
    u32 ctl;
    u8 sx;
    u8 sy;
    u8 pad2;
    u8 oamaddr;
    u32 pad3;
    u32 strobe;
} Nes;

#define NES (*(Nes *)0x80380000)

void func_800A1BA0(s32 idx, s32 mode);
void func_800A1DCC(s32 n, s32 m);
void func_800A1F8C(s32 n, s32 m);

void func_800A2178(s32 addr, s32 value)
{
    s32 k;
    Page *src;

    if (addr == 0x2007) {
        NES.vram[NES.vaddr] = value;
        if (NES.ram[0x2000] & 4) {
            NES.vaddr = (NES.vaddr + 32) % 0x4000;
        } else {
            NES.vaddr = (NES.vaddr + 1) % 0x4000;
        }
    } else if (addr == 0x2006) {
        if (NES.ctl & 1) {
            NES.vaddr = (NES.vaddr & 0xFF00) | value;
            NES.ctl |= 2;
        } else {
            NES.vaddr = (NES.vaddr & 0xFF) | (value << 8);
        }
        NES.ctl ^= 1;
    } else if (addr == 0x4016) {
        if (value & 1) {
            NES.strobe = 0x7B;
        } else if (NES.strobe == 0x7B) {
            NES.strobe = 0;
        }
    } else if (addr == 0x2005) {
        if (NES.ctl & 4) {
            NES.sy = value;
        } else {
            NES.sx = value;
        }
        NES.ctl ^= 4;
    } else if (addr == 0x2003) {
        NES.oamaddr = value;
    } else if (addr == 0x2004) {
        NES.oam[NES.oamaddr++] = value;
    } else if (addr == 0x4014) {
        src = (Page *)&NES.ram[value << 8];
        *(Page *)NES.oam = *src;
    } else {
        k = addr - 0x4000;
        if ((u32)k < 0x16) {
            NES.ram[addr] = value;
            k >>= 2;
            if (k < 2) {
                func_800A1BA0(k, addr & 3);
            } else if (k == 2) {
                func_800A1DCC(2, addr & 3);
            } else if (k == 3) {
                func_800A1F8C(3, addr & 3);
            }
        }
    }
    NES.ram[addr] = value;
}
