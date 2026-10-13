#include "ctx.h"

void func_800AE240(f32 mf[4][4], f32 l, f32 r, f32 b, f32 t, f32 n, f32 f, f32 scale);
void func_80011490(void *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale);
void func_800120A4(s16 *a, s16 *b, s16 *o);

typedef struct {
    u8 pad0[0x220];
    Gfx *dl220;
    u8 pad224[0x22C - 0x224];
    void *seg22C;
    u8 pad230[0x234 - 0x230];
    void *seg234[2];
    void *mtx23C[(0xA80 - 0x23C) / 4];
} Plr;

typedef struct {
    u8 pad[0x44];
    u8 f44;
    u8 pad45[0x7E0 - 0x45];
} Blk;

extern Vp D_800C99A0;
extern Blk D_800E37C0[];
extern Gfx *D_800E16C0[];
extern Mtx D_800E95E8;
extern Mtx D_800E9630;
extern u8 *D_800E92DC;
extern s32 D_800E30A8;
extern u32 D_800E19CC;
extern Plr D_800DC150[];
extern u32 D_800C2280;

Gfx *func_8003AC18(s32 idx, Gfx *gdl, Mtx *m)
{
    f32 mf[4][4];
    s16 rot[64];
    Blk *blk;
    Gfx **tab;
    s32 i;

    gDPPipeSync(gdl++);
    gSPViewport(gdl++, &D_800C99A0);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 1, 1, 31, 31);
    gSPClipRatio(gdl++, FRUSTRATIO_6);
    blk = &D_800E37C0[idx];
    func_800AE240(mf, -15.0f, 15.0f, -15.0f, 15.0f, 50.0f, 4192.0f, 1.0f);
    func_80011490(rot, -0x4000, 0, 0, 0, 0, 0, 4);
    func_800120A4(rot, (s16 *)mf, (s16 *)&D_800E95E8);
    func_80011490(&D_800E9630, 0, 0, 0, 0, 0, 0, 0x100);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);
    gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
    gSPMatrix(gdl++, &D_800E95E8, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &D_800E9630, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPSetColorImage(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, 32, D_800E92DC + idx * 0x400);

    for (i = 0; i < 6; i++) {
        gSPMatrix(gdl++, (u8 *)D_800DC150[idx].mtx23C[D_800E30A8] + i * 0x40, G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
        gSPDisplayList(gdl++, D_800E16C0[i]);
        gSPPopMatrix(gdl++, G_MTX_MODELVIEW);
    }
    if (blk->f44) {
        gDPSetColorImage(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, 32, D_800E92DC + (idx + 8) * 0x400);
    }
    gSPSegment(gdl++, 1, D_800DC150[0].seg22C);
    gSPSegment(gdl++, 6, D_800DC150[idx].seg234[D_800E30A8]);
    if (!(D_800C2280 & 0x20)) {
        gSPDisplayList(gdl++, D_800DC150[0].dl220);
    }
    gDPPipeSync(gdl++);
    gSPMatrix(gdl++, m + 16, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPSetGeometryMode(gdl++, D_800E19CC);
    return gdl;
}
