#include "ctx.h"

typedef struct {
    u8 pad0[0x44];
    u8 f44;
    u8 pad1[0x6E8 - 0x45];
    u8 f6E8;
    u8 pad2[0x7A5 - 0x6E9];
    u8 f7A5;
    u8 pad3[0x7E0 - 0x7A6];
} Ent;

typedef struct {
    s32 f0;
    u8 pad[0x48];
    s32 f4C;
} Ctl;

extern s32 D_800CA934;
extern s32 D_800F1458;
extern Ctl D_800EFD70;
extern s32 D_800C2384;
extern s32 D_800C2594;
extern s32 D_800E76D0;
extern s32 D_800C8CE0;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern s32 D_800F145C;
extern s32 D_800F14B8;
extern s32 D_800F1460;
extern s32 D_800F14BC;
extern u16 D_800F1452;
extern u16 D_800F1456;
extern Ent D_800E37C0[];

Gfx *func_80068484(Gfx *gdl, s32 player, s32 big);
Gfx *func_80067E4C(Gfx *gdl);

#define WD(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_80068910(Gfx *gdl) {
    Ent *e;
    s32 i;

    if (D_800CA934 != 0 || D_800F1458 == 0 || (D_800EFD70.f0 & 0x81) || D_800C2384 != 0) {
        return gdl;
    }
    if (D_800C2594 >= 2) {
        return func_80067E4C(gdl);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    WD(gdl++, 0xE200001C, 0x00404240);
    WD(gdl++, 0xFCFF97FF, 0xFF2CFE7F);
    gDPSetTextureFilter(gdl++, G_TF_BILERP);
    gDPSetBlendColor(gdl++, 0, 0, 0, 0x10);
    gDPSetAlphaCompare(gdl++, G_AC_THRESHOLD);
    gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 0x60);
    gDPLoadTextureBlock(gdl++, D_800F1458 + 8, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTextureRectangle(gdl++, D_800F145C * 4, D_800F1460 * 4,
                        (D_800F145C + D_800F14B8) * 4, (D_800F1460 + D_800F14BC) * 4,
                        G_TX_RENDERTILE, 0, 0, D_800F1452, D_800F1456);
    gDPPipeSync(gdl++);
    WD(gdl++, 0xE200001C, 0x0C084000);
    WD(gdl++, 0xFCFFFFFF, 0xFFFDF6FB);

    e = D_800E37C0;
    for (i = 0; i < 6; i++) {
        if (D_800E37C0[i].f7A5 != 0) continue;
        if (D_800E37C0[i].f44 != 0 && D_800E76D0 % 8 < 4) continue;
        if (D_800E37C0[i].f6E8 != 0 || (D_800EFD70.f4C == 8 && i != D_800C8CE0)) {
            gdl = func_80068484(gdl, i, 0);
        }
    }
    e = D_800E37C0;
    for (i = 0; i < 6; i++) {
        if (D_800E37C0[i].f7A5 != 0) continue;
        if (D_800E37C0[i].f44 != 0 && D_800E76D0 % 8 < 4) continue;
        if (D_800E37C0[i].f6E8 != 0) continue;
        if (D_800EFD70.f4C == 8 && i != D_800C8CE0) continue;
        gdl = func_80068484(gdl, i, 1);
    }
    gDPPipeSync(gdl++);
    return gdl;
}
