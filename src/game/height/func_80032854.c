#include "ctx.h"

typedef struct {
    s32 flags;  /* 0x00 */
    s32 f4;
    s32 f8;
    s32 fC;
    s32 f10;
    s32 x;      /* 0x14 */
    s32 y;      /* 0x18 */
    s32 f1C;
    s32 tex;    /* 0x20 */
} Ent;

typedef struct {
    s32 f0;
    s32 n;
    Ent e[1];
} EntList;

typedef struct {
    u8 w;
    u8 h;
    u8 pad[2];
    u8 *img;
} TexInfo;

typedef struct {
    s16 x, y, z;
    u16 flag;
    s16 s, t;
    u8 c[4];
} V;

typedef struct {
    s32 x, y, w, h;
} Rect;

typedef struct {
    u8 pad[0x2C];
    s32 f2C;
} Cfg;

extern EntList *D_800C9860;
extern s32 D_800C2280;
extern Vp D_800C25E8;
extern s16 D_800C2C60[];
extern u8 D_800C9844[];
extern s32 D_800C9850[];
extern Rect D_800E1718;
extern s32 D_800E19CC;
extern TexInfo *D_800E8E80;
extern V *D_800E8E88[];
extern s32 D_800E8E90[];
extern s32 D_800E8E9C;
extern Cfg *D_800EFD64;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;

s32 func_8000EB18(u32 r);

#define SINS(a) D_800C2C60[((a) >> 4) & 0xFFF]
#define COSS(a) D_800C2C60[(((a) >> 4) - 0x400) & 0xFFF]

static inline V *allocVtx(s32 idx, s32 n) {
    s32 c = D_800E8E90[idx];
    if (c + n > 128) {
        return NULL;
    }
    D_800E8E90[idx] = c + n;
    return &D_800E8E88[idx][c];
}

Gfx *func_80032854(Gfx *gdl, u8 *mtx, s32 idx) {
    s32 i, j, k;
    Ent *e;
    V *v;
    TexInfo *t;
    s32 a, b, p, q, x, y, ang, c, s, m, n, w;
    s32 bb;
    s32 aa;
    V *vv;

    if (D_800C9860 == NULL) {
        return gdl;
    }
    D_800E8E90[idx] = 0;
    if (D_800C2280 & 0x40) {
        return gdl;
    }

    gDPPipeSync(gdl++);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPViewport(gdl++, &D_800C25E8);
    gSPMatrix(gdl++, mtx + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, D_800E1718.x, D_800E1718.y, D_800E1718.x + D_800E1718.w - 1,
                  D_800E1718.y + D_800E1718.h - 1);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);

    D_800E8E9C = 0;
    for (i = 0; i < D_800C9860->n; i++) {
        e = &D_800C9860->e[i];
        if (e->flags & 8) { if (e->flags & 0x10) { if (e->tex >= 0) {
            p = D_800F13EC / 2 - e->x;
            q = D_800F13F0 / 2 - e->y;
            a = 0x80 - func_8000EB18(p * p + q * q);
            if (a >= 0) {
                a = a * e->tex / 6;
                if (a != 0) {
                    a = a * 192 / 128;
                    if (i == 0) {
                        D_800E8E9C = a;
                    }
                    v = allocVtx(idx, 28);
                    if (v != NULL) {
                        x = e->x - D_800F13EC / 2;
                        y = e->y - D_800F13F0 / 2;
                        p = x / 3;
                        q = y / 3;
                        x -= p;
                        y -= q;
                        for (j = 0; j < 4; j++) {
                            v[j * 7].x = x;
                            v[j * 7].y = -y;
                            v[j * 7].z = 50;
                            for (k = 0; k < 6; k++) {
                                b = -y;
                                ang = ((k * 2 + 1) << 15) / 6;
                                v[j * 7 + k + 1].x = x + COSS(ang) * D_800C9850[j] / 16384;
                                v[j * 7 + k + 1].y = b + SINS(ang) * D_800C9850[j] / 16384;
                                v[j * 7 + k + 1].z = 50;
                            }
                            x -= p;
                            y -= q;
                            if (j & 1) {
                                x -= p;
                                y -= q;
                            }
                        }
                        gSPVertex(gdl++, v, 28, 0);
                        for (j = 0; j < 4; j++) {
                            gDPPipeSync(gdl++);
                            gDPSetPrimColor(gdl++, 0, 0, D_800C9844[j * 3], D_800C9844[j * 3 + 1],
                                            D_800C9844[j * 3 + 2], a);
                            
                            for (k = 0; k < 3; k++) {
                                s32 n;
                                w = k * 2 + 2;
                                m = k * 2 + 1;
                                n = w % 6 + 1;
                                gSP2Triangles(gdl++, j * 7, j * 7 + w, j * 7 + m, 0, j * 7, j * 7 + n, j * 7 + w, 0);
                            }
                        }
                    }
                }
            }
        }
    }}}

    gDPPipeSync(gdl++);
    gDPSetCombine(gdl++, 0x11FE23, 0xFFFFF3F9);
    gDPSetColorDither(gdl++, G_CD_NOISE);
    gDPSetAlphaDither(gdl++, G_AD_NOISE);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    if (D_800EFD64->f2C == 0) {
        gDPSetTexturePersp(gdl++, G_TP_PERSP);
    } else {
        gDPSetTexturePersp(gdl++, G_TP_NONE);
    }

    for (i = 0; i < D_800C9860->n; i++) {
        e = &D_800C9860->e[i];
        if (e->flags & 8) { if (e->flags & 0x10) { if ((n = e->tex) >= 0) {
            gDPPipeSync(gdl++);
            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
            t = &D_800E8E80[n + 1];
            if (D_800EFD64->f2C == 0) {
                x = e->x - D_800F13EC / 2;
                y = D_800F13F0 / 2 - e->y;
                q = t->h - 1;
                c = (COSS((x + y) * 32) * q >> 14) * 2 / 3;
                s = (SINS((x + y) * 32) * q >> 14) * 2 / 3;
                vv = allocVtx(idx, 4);
                if (vv != NULL) {
                    vv[0].x = x + c;
                    vv[0].y = y + s;
                    vv[0].s = 0;
                    vv[0].t = 0;
                    vv[1].x = x + s;
                    vv[1].y = y - c;
                    vv[1].s = q << 6;
                    vv[1].t = 0;
                    vv[2].x = x - c;
                    vv[2].y = y - s;
                    vv[2].s = q << 6;
                    vv[2].t = q << 6;
                    vv[3].x = x - s;
                    vv[3].y = y + c;
                    vv[3].s = 0;
                    vv[3].t = q << 6;
                    gSPVertex(gdl++, vv, 4, 0);
                    gDPLoadTextureBlock(gdl++, t->img, G_IM_FMT_IA, G_IM_SIZ_8b, t->w * 8, t->h, 0, G_TX_CLAMP,
                                        G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
                    gSP2Triangles(gdl++, 0, 1, 2, 0, 0, 2, 3, 0);
                }
            } else {
                bb = aa = 0;
                x = e->x - t->w * 4;
                y = e->y - t->h / 2;
                p = x + t->w * 8;
                q = y + t->h;
                if (x < 0) {
                    bb = -x;
                    x = 0;
                }
                if (y < 0) {
                    aa = -y;
                    y = 0;
                }
                if (p >= D_800F13EC) {
                    p = D_800F13EC - 1;
                }
                if (q >= D_800F13F0) {
                    q = D_800F13F0 - 1;
                }
                if (x < p && y < q) {
                    gDPLoadTextureBlock(gdl++, t->img, G_IM_FMT_IA, G_IM_SIZ_8b, t->w * 8, t->h, 0, G_TX_CLAMP,
                                        G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
                    gSPTextureRectangle(gdl++, x << 2, y << 2, p << 2, q << 2, G_TX_RENDERTILE, bb << 5, aa << 5,
                                        0x400, 0x400);
                }
            }
        }
    }}}

    gSPSetGeometryMode(gdl++, D_800E19CC);
    gDPPipeSync(gdl++);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    gDPSetAlphaDither(gdl++, G_AD_DISABLE);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gSPMatrix(gdl++, mtx, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}
