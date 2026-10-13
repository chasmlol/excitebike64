#include "ctx.h"

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fc;
    s16 f10;
    u16 f12;
    s32 f14;
    s32 f18;
    s32 f1c;
    s32 f20;
} Rec;

typedef struct {
    u8 ram[0x10000];
    u8 pad[0x4100];
    u16 pc;
    u8 a, x, y, s, v, b, n, d, i, z, c, p;
    u8 pad2[0x1E];
    Rec ch[5];
} Nes;

#define NES (*(Nes *)0x80380000)

extern u16 D_800D05D8[];
extern s32 D_800D05E0[];

void func_800A1BA0(s32 idx, s32 mode)
{
    Rec *r;
    s32 per;
    s32 a0;
    s32 v;
    u8 *p;

    per = NES.ram[idx * 4 + 0x4002] | ((NES.ram[idx * 4 + 0x4003] & 7) << 8);
    r = &NES.ch[idx];
    if (per == 0) {
        per = 1;
    }
    per = 0x36EE8 / per;
    per = (per << 15) / 22050;
    r->f0 = per;
    a0 = NES.ram[idx * 4 + 0x4000] >> 6;
    r->f12 = D_800D05D8[a0];
    a0 = NES.ram[idx * 4 + 0x4003] >> 3;
    if (a0 & 1) {
        a0 >>= 1;
        if (a0 == 0) {
            r->f14 = 0x7E;
        } else {
            r->f14 = a0;
        }
    } else {
        r->f14 = D_800D05E0[a0 >> 1];
    }
    if (NES.ram[0x4000 + idx * 4] & 0x20) {
        r->f1c = 0;
        r->f18 = 0;
    } else {
        r->f1c = 1;
    }
    if (mode == 3) {
        r->f18 = 0;
    }
    if ((NES.ram[0x4015] >> idx) & 1) {
        r->f20 |= 8;
    } else {
        r->f20 &= ~8;
    }
    if (mode == 0) {
        r->f8 = 0xFFFF;
    }
    p = &NES.ram[0x4000 + idx * 4];
    a0 = *p & 0x10;
    v = *p & 0xF;
    if (a0) {
        r->f8 = (v << 16) / 16;
        r->fc = 0;
        r->f20 &= ~0x10;
        return;
    }
    r->fc = 0x4000 / (v + 1);
    if (*p & 0x20) {
        r->f20 |= 0x10;
        return;
    }
    r->f20 &= ~0x10;
}
