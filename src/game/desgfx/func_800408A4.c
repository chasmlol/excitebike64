#include "ctx.h"

extern s32 D_800ED608;
extern u32 D_800EBDF8[];
extern u8 D_800ED610[];
extern u32 D_800EA7C8[];
extern u32 D_800CA330[];
extern u32 D_800CA3B4;
extern u32 D_800CA3B8;
extern u32 *D_800EDFD0;

typedef struct { u32 pad[5]; u32 f5; u32 f6; } Ent;

#define PKT(p, a, b) { Gfx *_g = (Gfx *)(p++); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_800408A4(Gfx *gdl, s32 p2, s32 idx)
{
    Gfx *w;
    u32 *g;
    s32 n;
    s32 i;
    Ent *e;
    u32 *s1;
    u8 *s0;
    u32 *puVar3;
    u32 uVar9;
    u32 uVar1;
    u8 r, gg, b;
    u32 col;

    PKT(gdl, 0xD9DFFFFB, 0);
    PKT(gdl, 0xD9FEFFFF, 0);
    PKT(gdl, 0xE200001C, 0x00404A50);
    PKT(gdl, 0xE3001001, 0);
    PKT(gdl, 0xFC30B261, 0xFF67FFFF);

    n = D_800ED608;
    i = 0;
    if (n > 0) {
        w = gdl;
        do {
            u8 *q = (u8 *)D_800EA7C8 + i * 0x1C;
            if (*(u32 *)(q + 0x14) == 2 && *(u32 *)(q + 0x18) == D_800CA3B4) {
                PKT(w, 0xDA380003, D_800EBDF8[idx] + i * 0x40);
                PKT(w, 0xDA380001, (u32)((u8 *)D_800ED610 + idx * 0x40));
                uVar9 = D_800CA3B8 % *(puVar3 = D_800EDFD0);
                uVar1 = uVar9 * 2 + puVar3[1];
                uVar1 = *(u16 *)uVar1;
                r = (uVar1 >> 11) << 3;
                gg = ((uVar1 >> 6) & 0x1F) << 3;
                uVar1 <<= 2;
                col = (r << 24) | (gg << 16);
                uVar1 &= 0xF8;
                col |= uVar1 << 8;
                PKT(w, 0xFA000000, col | 0xFF);
                PKT(w, 0xFD700000, *(u32 *)((u8 *)puVar3 + (uVar9 << 3) + 12));
                PKT(w, 0xF5700000, 0x07098250);
                PKT(w, 0xE6000000, 0);
                PKT(w, 0xF3000000, 0x073FF200);
                PKT(w, 0xE7000000, 0);
                PKT(w, 0xF5680800, 0x00098250);
                PKT(w, 0xF2000000, 0x0007C0FC);
                PKT(w, 0x01004008, (u32)D_800CA330);
                gdl = gdl + 12;
                PKT(w, 0x06000204, 0x406);
            }
            i = i + 1;
        } while (i < D_800ED608);
    }
    return gdl;
}
