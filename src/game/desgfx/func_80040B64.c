#include "ctx.h"

typedef struct {
    u32 w0, w1;
} Blk;

extern s32 D_800CA3B0;
extern u32 *D_800EBE04;
extern u8 *D_800EDBB4;
extern s32 D_800E30A8;
extern u8 D_800E1938[];
extern s32 D_800EA988;

void *func_80013D70(s32 a0, s32 a1, s32 a2, Blk *a3);

#define PKT(a, b) { Blk *_g = gdl++; _g->w0 = (a); _g->w1 = (b); }

Blk *func_80040B64(Blk *gdl, s32 p2)
{
    s32 off;
    Blk *puVar3;

    if (D_800CA3B0 < 0) {
        return gdl;
    }
    {
        PKT(0xFCFFFFFF, 0xFFFCF87C)
        PKT(0xE200001C, 0x0C084000)
        PKT(0xE3001001, 0x00008000)
        PKT(0xD7000002, 0x80008000)
        PKT(0xE3000C00, 0x00080000)
        PKT(0xD9FFFFFE, 0)
        PKT(0xFD100000, D_800EBE04[1])
        PKT(0xE8000000, 0)
        PKT(0xF5000100, 0x07000000)
        PKT(0xE6000000, 0)
        PKT(0xF0000000, 0x073FC000)
        PKT(0xE7000000, 0)
        off = D_800E30A8 * 0x40;
        PKT(0xDA380003, (u32)(D_800EDBB4 + off))
        PKT(0xDA380007, (u32)(D_800E1938 + off))
        gdl = (Blk *)func_80013D70(0, D_800EA988, (s32)&D_800EBE04, gdl);
        PKT(0xDA380003, p2 + 0x400)
        return gdl;
    }
}
