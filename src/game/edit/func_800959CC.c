#include "ctx.h"

typedef struct {
    s32 x, y, w, h;
} Rect;

typedef struct {
    s32 f0;
    u32 pal;            /* 0x04 */
    u8 img[1];          /* 0x08 */
} Pic;

typedef struct {
    s16 f0;
    s16 n;              /* 0x02 */
    s32 f4;
    s32 f8;
    u16 fC;             /* 0x0C */
    u16 fE;
    u8 pad10[0xC];
    char *name;         /* 0x1C */
    u8 pad20[0x20];
} Trk;

typedef struct {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    Trk *trk;           /* 0x14 */
} Sel;

typedef struct {
    u8 pad0[0x170];
    u32 mode;           /* 0x170 */
    u8 pad174[0x10];
    Sel *sel;           /* 0x184 */
    u8 pad188[0x16BA - 0x188];
    char name[0x26];    /* 0x16BA */
    s32 f16E0;
    s32 f16E4;
    u32 f16E8;
    s32 f16EC;
    s32 f16F0;
    s32 f16F4;
    s32 f16F8;
    s32 f16FC;
    u8 pad1700[0x18];
    s32 *font2;         /* 0x1718 */
    s32 *font;          /* 0x171C */
    u8 pad1720[0x80];
    Vp vp0;             /* 0x17A0 */
    u8 pad17B0[0x18];
    Vp vp[3];           /* 0x17C8 */
    u8 pad17F8[0xC];
    Pic *pic1804;       /* 0x1804 */
    Pic *pic1808;       /* 0x1808 */
} Menu;

typedef struct {
    s32 f0;
    u8 *img;
} Res;

extern Menu *D_800CEA40;
extern Res *D_800CE200;
extern Rect D_800CE9C0[4];
extern Rect D_800CEA00[4];
extern Trk D_800CE280[];
extern char *D_800CEBB0[];
extern char *D_800CEBCC[];
extern s32 D_800F55B0;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern u32 D_800C29E0;
extern u32 D_800C29D8[];
extern s32 D_800E2038;
extern u8 D_800A9D00[];
extern u8 D_800D15C0[];
extern char D_800DABE8[];

void func_8008EFFC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);
Gfx *func_8000B9CC(Gfx *gdl, s32 a1, s32 a2, void *a3, void *a4);
void func_8000EE30(s32 v, char *out);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
Gfx *func_800C1394(Gfx *gdl);
Gfx *func_80008B1C(Gfx *gdl, s32 a1, s32 a2);
Gfx *func_8009559C(Gfx *gdl, s32 arg1);
Gfx *func_8000A690(s32 x, s32 y, void *img, Gfx *gdl, s32 a4, s32 sx, s32 sy, s32 flags, s32 dw);
void func_800C0ACC(s32 *p, s32 x, s32 y);
Gfx *func_8008FB08(s32 a0, Gfx *gdl, s32 a2, s32 a3, s32 a4);
Gfx *func_800978A8(s32 a0, s32 a1, Gfx *gdl);

void func_800959CC(s32 arg0, s32 arg1, Gfx **pgdl)
{
    char buf[0x68];
    char *s;
    Gfx *gdl;
    s32 i;
    u8 *p;
    u8 *img;
    char *str;
    Menu *m;
    Trk *t;
    Trk *e;

    D_800F55B0 = arg0;
    switch (D_800CEA40->f16E8) {
    case 1:
        D_800CEA40->f16F0 = 0;
        break;
    case 2:
        D_800CEA40->f16F0 = 30 - D_800CEA40->f16EC;
        break;
    case 0:
        D_800CEA40->f16F0 = D_800CEA40->f16EC;
        break;
    case 3:
        D_800CEA40->f16F0 = 30;
        break;
    }
    D_800CEA40->f16F0 *= 4;
    D_800CE9C0[1].x = D_800CEA00[1].x - D_800CEA40->f16F0;
    D_800CE9C0[1].w = D_800CEA00[1].w + D_800CEA40->f16F0;
    D_800CE9C0[2].x = D_800CEA00[2].x - D_800CEA40->f16F0;
    D_800CE9C0[3].x = D_800CEA00[3].x - D_800CEA40->f16F0;
    func_8008EFFC(D_800CE9C0[1].x, D_800CE9C0[1].y, D_800CE9C0[1].w - 1, D_800CE9C0[1].h - 1, D_800CEA40->f16F0);
    D_800CEA40->vp[0].vp.vscale[0] = D_800CEA00[1].w * 2;
    D_800CEA40->vp[0].vp.vscale[1] = D_800CEA00[1].h * 2;
    D_800CEA40->vp[0].vp.vtrans[0] = (D_800CEA00[1].x + D_800CEA00[1].w / 2) * 4;
    D_800CEA40->vp[0].vp.vtrans[1] = (D_800CEA00[1].y + D_800CEA00[1].h / 2) * 4;
    D_800CEA40->vp[1].vp.vscale[0] = D_800CE9C0[2].w * 2;
    D_800CEA40->vp[1].vp.vscale[1] = D_800CE9C0[2].h * 2;
    D_800CEA40->vp[1].vp.vtrans[0] = (D_800CE9C0[2].x + D_800CE9C0[2].w / 2) * 4;
    D_800CEA40->vp[1].vp.vtrans[1] = (D_800CE9C0[2].y + D_800CE9C0[2].h / 2) * 4;
    D_800CEA40->vp[2].vp.vscale[0] = D_800CE9C0[3].w * 2;
    D_800CEA40->vp[2].vp.vscale[1] = D_800CE9C0[3].h * 2;
    D_800CEA40->vp[2].vp.vtrans[0] = (D_800CE9C0[3].x + D_800CE9C0[3].w / 2) * 4;
    D_800CEA40->vp[2].vp.vtrans[1] = (D_800CE9C0[3].y + D_800CE9C0[3].h / 2) * 4;

    gdl = *pgdl;
    gdl = func_8000B9CC(gdl, arg1, arg0, D_800A9D00, D_800D15C0);
    gdl = func_80008B1C(gdl, arg1, arg0);
    gDPSetTextureFilter(gdl++, G_TF_AVERAGE);
    gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
    gSPClipRatio(gdl++, FRUSTRATIO_2);
    gSPViewport(gdl++, &D_800CEA40->vp0);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    gDPPipeSync(gdl++);
    gDPSetDepthImage(gdl++, D_800C29E0);
    gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29E0);
    gDPSetFillColor(gdl++, 0xFFFCFFFC);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29D8[arg1]);
    gDPSetFillColor(gdl++, 0);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29D8[arg1]);
    gDPSetDepthSource(gdl++, G_ZS_PIXEL);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800CE9C0[0].x, D_800CE9C0[0].y,
                  D_800CE9C0[0].x + D_800CE9C0[0].w - 1, D_800CE9C0[0].y + D_800CE9C0[0].h - 1);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPPipeSync(gdl++);
    gDPSetFillColor(gdl++, 0x00010001);
    gDPFillRectangle(gdl++, D_800CE9C0[2].x, D_800CE9C0[2].y,
                     D_800CE9C0[2].x + D_800CE9C0[2].w - 1, D_800CE9C0[2].y + D_800CE9C0[2].h - 1);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gdl = func_8008FB08(arg1, gdl, D_800CEA40->f16E0, 0x1A, 0x11);
    gdl = func_800978A8(arg0, arg1, gdl);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetPrimColor(gdl++, 0, 0, 0xB4, 0xB4, 0xB4, 0xFF);

    switch (D_800CEA40->mode) {
    case 10:
    case 11:
        func_800C1E60(D_800CEA40->font, 0x20, 4, 2, 0x21);
        func_800C1F2C(D_800CEA40->font, "Warning");
        func_800C0ACC(D_800CEA40->font, 0x14C, 0x48);
        for (i = 0;; i++) {
            if (*(D_800CEA40->mode == 10 ? &D_800CEBB0[i] : &D_800CEBCC[i]) == NULL) break;
            func_800C1E60(D_800CEA40->font, 0x20, 5, 2, 0x21);
            func_800C1F2C(D_800CEA40->font, *(D_800CEA40->mode == 10 ? &D_800CEBB0[i] : &D_800CEBCC[i]));
            func_800C0ACC(D_800CEA40->font, 0x144 - i * 4, i * 12 + 0x61);
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        img = D_800CEA40->pic1808->img;
        gDPLoadTLUT_pal256(gdl++, D_800CEA40->pic1808->pal);
        gdl = func_8000A690(0xD0, 0x3D, img, gdl, arg1, 1, 1, 3, 0);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        gdl = func_8000A690(0xB6, 0x3A, D_800CEA40->pic1804->img, gdl, arg1, 1, 1, 6, 0);
        break;
    }

    if (D_800CEA40->sel != NULL) {
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        p = D_800CE200->img + 0x58;
        func_800C1E60(D_800CEA40->font, 0x20, 5, 2, 0x21);
        func_800C1F2C(D_800CEA40->font, D_800DABE8);
        func_800C1F2C(D_800CEA40->font, D_800CEA40->name);
        func_800C0ACC(D_800CEA40->font, 0x140, 0x19);
        for (i = 0; i < (0x207 - D_800CE9C0[1].x) / (p[0] * 8); i++) {
            gdl = func_8000A690(D_800CE9C0[1].x + p[0] * (i << 3), 0x11, p, gdl, arg1, 1, 1, 6, 0);
        }
        if (D_800CEA40->mode == 9 && (D_800E2038 & 0x20)) {
            gDPSetPrimColor(gdl++, 0, 0, 0xB4, 0xB4, 0x50, 0xFF);
        }
        for (i = 0; i < (0x207 - D_800CE9C0[1].x) / (p[0] * 8); i++) {
            gdl = func_8000A690(D_800CE9C0[1].x + p[0] * (i << 3), 0xCD, p, gdl, arg1, 1, 1, 6, 0);
        }
        gDPPipeSync(gdl++);
        func_800C1E60(D_800CEA40->font2, 0x40, 5, 2, 0x21);
        i = D_800CEA40->sel->trk - D_800CE280;
        if (D_800CEA40->mode == 9 && D_800CEA40->sel != NULL &&
            (D_800CEA40->sel->f4 != 0 || D_800CEA40->sel->f8 != 0)) {
            func_800C1F2C(D_800CEA40->font2, "< ");
        }
        if (i < 2) {
            func_800C1F2C(D_800CEA40->font2, "Straight ");
            i += 1;
        } else if (i < 6) {
            func_800C1F2C(D_800CEA40->font2, "Turn ");
            i -= 1;
        } else if (i < 18) {
            func_800C1F2C(D_800CEA40->font2, "Jump ");
            i -= 5;
        } else {
            i -= 17;
        }
        if (D_800CEA40->sel->trk - D_800CE280 < 18) {
            s = buf + 0x48;
            func_8000EE30(i, s);
            func_800C1F2C(D_800CEA40->font2, s);
            func_800C1F2C(D_800CEA40->font2, " : ");
        }
        func_800C1F2C(D_800CEA40->font2, D_800CEA40->sel->trk->name);
        if (D_800CEA40->sel->trk - D_800CE280 >= 18) {
            if (D_800CEA40->mode == 9) {
                func_800C1F2C(D_800CEA40->font2, " - Press Z Button To Edit");
            } else if (D_800CEA40->mode == 6 && D_800CE280[D_800CEA40->f16FC + 18].n >= 5 &&
                       !(D_800CE280[D_800CEA40->f16FC + 18].fC & 0x200) &&
                       (D_800CEA40->f16F8 == 1 || D_800CEA40->f16F8 == D_800CE280[D_800CEA40->f16FC + 18].n * 2 - 1)) {
                func_800C1F2C(D_800CEA40->font2, " - R Button Deletes Segment");
            }
        } else if (D_800CEA40->mode == 9 && (*(u32 *)&D_800CEA40->sel->trk->fC & 0x300000) == 0x300000) {
            func_800C1F2C(D_800CEA40->font2, " - R Button Flips");
        }
        if (D_800CEA40->mode == 9 && D_800CEA40->sel != NULL &&
            (D_800CEA40->sel->f4 != 0 || D_800CEA40->sel->f8 != 0)) {
            func_800C1F2C(D_800CEA40->font2, " >");
        }
        func_800C0ACC(D_800CEA40->font2, 0x142, 0xD5);
    }
    if (D_800CEA40->mode == 7 || D_800CEA40->mode == 8) {
        gdl = func_8009559C(gdl, arg1);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800CE9C0[0].x, D_800CE9C0[0].y,
                  D_800CE9C0[0].x + D_800CE9C0[0].w - 1, D_800CE9C0[0].y + D_800CE9C0[0].h - 1);
    gdl = func_800C1394(gdl);
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}
