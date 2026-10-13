#include "ctx.h"

typedef struct {
    s32 f0;
    u32 pal;            /* 0x04 */
    u8 img[1];          /* 0x08 */
} Pic;

typedef struct {
    u8 pad0[0x54];
    s32 mode;           /* 0x54 */
    u16 fade;           /* 0x58 */
    s16 timer;          /* 0x5A */
    u8 pad5C[0x36C - 0x5C];
    Pic *imgs[1];       /* 0x36C */
} Glob;

typedef struct {
    u8 pad0[0xC];
    u8 *tbl;            /* 0x0C */
    u8 pad10[6];
    s16 type;           /* 0x16 */
    u8 pad18[4];
    s32 f1C;            /* 0x1C */
    s32 *idx;           /* 0x20 */
    Pic *img;           /* 0x24 */
} Obj;

typedef struct {
    s16 x, y, w, h;
} Rect;


extern s32 D_800EFDC4;
extern Glob D_800EFD70;
extern Obj *D_800F47A4;
extern Obj *D_800F47C4;
extern s32 D_800F47A0;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;
extern Gfx D_800C2A38[];
extern Gfx D_800C2A00[];
extern Vp D_800CDA60;
extern Mtx D_800C25F8;
extern u32 D_800C29E0;
extern u32 D_800C29D8[];
extern Vp D_800F4790;
extern Rect D_800CDB64;
extern u8 D_800CA80F[];

void func_80011490(Mtx *m, s32 rx, s32 ry, s32 rz, s32 tx, s32 ty, s32 tz, s32 scale);
void func_800AE240(Mtx *m, f32 l, f32 r, f32 b, f32 t, f32 n, f32 f, f32 s);
void func_800AE540(Mtx *m, u16 *pn, f32 fovy, f32 aspect, f32 n, f32 f, f32 s);
void func_800120A4(Mtx *a, Mtx *b, Mtx *o);
s32 func_800C2224(void);
Gfx *func_8000A690(s32 x, s32 y, void *img, Gfx *gdl, s32 a4, s32 sx, s32 sy, s32 flags, s32 dw);
Gfx *func_8007CEDC(Mtx *m, s32 fb, Gfx *gdl, u8 *tbl, s32 e);

#define IMG(s) ((s)->type == 4 ? D_800EFD70.imgs[*(s)->idx] : (s)->img)

void func_80087310(Mtx *m, s32 fb, Gfx **pgdl)
{
    u8 unk28[0x70];
    Mtx persp;
    u16 pn;
    Gfx *gdl;
    Obj *s;
    u8 *tbl;
    u16 alpha;

    gdl = *pgdl;
    if (D_800EFDC4 == 0) {
        s = D_800F47A4;
    } else {
        s = D_800F47C4;
    }
    if (s != NULL) {
        if (D_800EFDC4 == 0) {
            tbl = s->tbl + D_800F47A0 * 16;
        } else {
            tbl = s->tbl;
            if (s->idx != NULL) {
                tbl += *s->idx * 16;
            }
        }
    }
    alpha = 0xFF;
    if (D_800EFD70.mode == 1) goto c1;
    if (D_800EFD70.mode == 0) goto c0;
    if (D_800EFD70.mode == 2) goto c2;
    goto done;
c1:
    alpha = 0;
    goto done;
c0:
    alpha = D_800EFD70.fade;
    goto done;
c2:
    alpha = 0xFF - D_800EFD70.fade;
done:
    func_80011490(&m[16], 0, 0, 0, 0, 0, 0, 0x100);
    func_800AE240(&m[12], -D_800F13EC / 2, D_800F13EC / 2, -D_800F13F0 / 2, D_800F13F0 / 2, 50.0f, 4192.0f, 1.0f);
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, D_800C2A38);
    gSPDisplayList(gdl++, D_800C2A00);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    gSPViewport(gdl++, &D_800CDA60);
    gDPPipeSync(gdl++);
    func_800AE540(&persp, &pn, 33.0f, 1.9512196f, 25.0f, 4192.0f, 1.0f);
    func_800120A4(&D_800C25F8, &persp, m);
    gSPPerspNormalize(gdl++, pn);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29E0);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetFillColor(gdl++, 0xFFFCFFFC);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, D_800F13EC, D_800C29D8[fb]);
    if (D_800EFDC4 == 1 || s == NULL || s->f1C == 0) {
        gDPSetFillColor(gdl++, 0);
        gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    }
    gDPPipeSync(gdl++);
    gDPSetDepthImage(gdl++, D_800C29E0);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gSPSetGeometryMode(gdl++, G_CULL_BACK);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
    gDPSetTextureFilter(gdl++, G_TF_BILERP);
    gSPMatrix(gdl++, &m[16], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &m[12], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    func_800C2224();
    gDPPipeSync(gdl++);
    if (D_800EFD70.mode == 1) {
        if (D_800EFD70.timer > 0 || D_800EFD70.timer == -1) {
            D_800EFD70.timer--;
        }
    } else {
        if (s->type == 4 || s->f1C != 0) {
            gDPLoadTLUT_pal256(gdl++, s->type == 4 ? D_800EFD70.imgs[*s->idx]->pal : s->img->pal);
            gdl = func_8000A690(0, 0, IMG(s)->img, gdl, fb, 1, 1, 3, 0);
        }
        if (s->type == 0) {
            Rect *r = &D_800CDB64;
            D_800F4790.vp.vscale[2] = 0x1FF;
            D_800F4790.vp.vscale[3] = 0;
            D_800F4790.vp.vtrans[2] = 0x1FF;
            D_800F4790.vp.vtrans[3] = 0;
            D_800F4790.vp.vscale[0] = r->w * 2;
            D_800F4790.vp.vscale[1] = r->h * 2;
            D_800F4790.vp.vtrans[0] = (r->x + r->w / 2) * 4;
            D_800F4790.vp.vtrans[1] = (r->y + r->h / 2) * 4;
            gSPViewport(gdl++, &D_800F4790);
            gdl = func_8007CEDC(m, fb, gdl, tbl, 0);
        } else if (s != NULL) {
            gdl = func_8007CEDC(m, fb, gdl, tbl, 0);
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
            D_800CA80F[0] = alpha;
        }
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, D_800F13EC, D_800F13F0);
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}
