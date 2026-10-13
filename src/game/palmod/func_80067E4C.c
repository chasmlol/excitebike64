#include "ctx.h"

typedef struct {
    s32 a, b, c, d;
} R16;

typedef struct {
    R16 r[4];
} Ent40;

typedef struct {
    s32 f0, f4, f8, fC;
    u8 pad[0x50 - 0x10];
} Ent50;

typedef struct {
    u8 pad0[0x44];
    u8 f44;
    u8 pad45[0x6E8 - 0x45];
    u8 f6E8;
    u8 pad6E9[0x7A5 - 0x6E9];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Racer;

extern s32 D_800C2388;
extern s32 D_800C2594;
extern Ent40 D_800E1718[];
extern s32 D_800CCCF0;
extern s32 D_800CCCF4;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern Racer D_800E37C0[];
extern s32 D_800E76D0;
extern u8 D_800F1478[];
extern u8 D_800CCCF8[];
Gfx *func_800685D8(Gfx *gdl, s32 idx, s32 half);

Gfx *func_80067E4C(Gfx *gdl)
{
    s32 i;
    s32 n = D_800C2594;

    if (D_800C2388 == 0) {
        D_800CCCF0 = D_800E1718[n - 1].r[0].a + 4;
        D_800CCCF4 = D_800E1718[n - 1].r[1].a + D_800E1718[n - 1].r[1].c - 8;
    } else {
        D_800CCCF0 = D_800E1718[n - 1].r[0].b + 6;
        D_800CCCF4 = D_800E1718[n - 1].r[n - 1].b + D_800E1718[n - 1].r[n - 1].d - 12;
    }

    gDPPipeSync(gdl++);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetTexturePersp(gdl++, G_TP_NONE);

    if (D_800C2388 == 0) {
        gDPLoadTextureBlock(gdl++, D_800F1478, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_WRAP, G_TX_WRAP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, D_800CCCF0 << 2, 116 << 2, D_800CCCF4 << 2, 124 << 2,
                            G_TX_RENDERTILE, 0, 0, 0, 0x400);
    } else {
        gDPLoadTextureBlock(gdl++, D_800CCCF8, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 1, 0,
                            G_TX_WRAP, G_TX_WRAP, 3, 0, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, (D_800F13EC * 252 / 512) << 2, D_800CCCF0 << 2,
                            (D_800F13EC * 260 / 512) << 2, D_800CCCF4 << 2,
                            G_TX_RENDERTILE, 0, 0, 0x80000 / D_800F13EC, 0);
    }

    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);

    for (i = 0; i < 6; i++) {
        if (D_800E37C0[i].f7A5 == 0) {
            if (D_800E37C0[i].f44 == 0 || D_800E76D0 % 8 >= 4) {
                if (D_800E37C0[i].f6E8 != 0) {
                    gdl = func_800685D8(gdl, i, 0);
                }
            }
        }
    }
    for (i = 0; i < 6; i++) {
        if (D_800E37C0[i].f7A5 == 0) {
            if (D_800E37C0[i].f44 == 0 || D_800E76D0 % 8 >= 4) {
                if (D_800E37C0[i].f6E8 == 0) {
                    gdl = func_800685D8(gdl, i, 1);
                }
            }
        }
    }
    gDPPipeSync(gdl++);
    return gdl;
}
