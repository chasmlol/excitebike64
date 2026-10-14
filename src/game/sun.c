#include "game.h"

typedef struct {
    s32 x, d, y;
} Pt_3;

extern Pt_3 *sStars;

typedef struct {
    s32 flags;
    u8 pad[0x20];
} Ent_31;

typedef struct {
    s32 f0;
    s32 n;
    Ent_31 e[1];
} Hdr_6;

typedef struct {
    u8 pad[4];
    s16 v;
    u8 pad2[0xA];
} Cell_3;

typedef struct {
    u8 w, h;
    u8 pad[2];
    u8 *buf;
} Item_4;

typedef struct {
    u32 n;
    u32 pad;
    Item_4 it[1];
} Tab;

typedef struct {
    u8 pad[0x2C];
    s32 mode;
    u8 pad2[0x20];
    s32 flags;
} Cfg_6;

extern Hdr_6 *sSunList_2;
extern Cell_3 *sSunVtxBufs_2[2];
extern Tab *sFlareTextures;
extern Cfg_6 *gCurTrack_23;

void sun_init(void)
{
    s32 v;
    s32 i, j, k, n, a, b, c;
    s32 x, y;

    sStars = 0;
    sSunVtxBufs_2[0] = 0;
    sSunVtxBufs_2[1] = 0;
    sSunList_2 = 0;
    if (gCurTrack_23->mode == 2) {
        v = D_800D0F78;
    } else {
        v = track_res_find(D_800D14EC);
    }
    if (v < 0) {
        sSunList_2 = 0;
        return;
    }
    a = 0;
    sSunList_2 = res_load(D_800E7728, v);
    for (; a < sSunList_2->n; a++) {
        sSunList_2->e[a].flags &= ~0x18;
    }
    for (j = 0; j < 2; j++) {
        sSunVtxBufs_2[j] = mem_alloc_dbg(0x800, 0, D_800D6C80, 0xCB);
        for (n = 0; n < 0x80; n++) {
            sSunVtxBufs_2[j][n].v = 1000;
        }
    }
    sFlareTextures = res_load(D_800E7728, sSunList_2->f0);
    for (a = 0; a < sFlareTextures->n; a++) {
        n = sFlareTextures->it[a].w * sFlareTextures->it[a].h * 8;
        for (k = 0; k < n; k++) {
            sFlareTextures->it[a].buf[k] |= 0xF0;
        }
    }
    if (gCurTrack_23->flags & 0x20) {
        sStarTexture = res_load(D_800E7728, D_800D0E00);
        sStars = mem_alloc_dbg(0x300, 0, D_800D6C80, 0xE3);
        for (i = 0; i < 0x40; i++) {
            x = (random_byte() - 0x80) * 16;
            y = (random_byte() - 0x80) * 16;
            sStars[i].d = isqrt_u32_4(0x400000 - (x * x + y * y));
            sStars[i].x = x;
            sStars[i].y = y;
        }
    }
    gSunGlare = 0;
}

typedef struct {
    u32 flags;
    s32 a1, a2, a3;
    s32 p10;
    s32 v14, v18, v1C;
    s32 p20;
} Rec_17;

typedef struct {
    s32 h0;
    s32 count;
    Rec_17 r[1];
} Tab_2;

extern Tab_2 *sSunList_3;

void sun_update_screen_pos(s32 param_1, s32 param_2, s32 arg2)
{
    s32 A, B, C, D, E;
    s32 i;
    Rec_17 *rp;
    u32 fl;

    if (sSunList_3 != NULL) {
        for (i = 0; i < sSunList_3->count; i++) {
            rp = &sSunList_3->r[i];
            fl = rp->flags;
            rp->flags = fl & ~8;
            if (fl & 1) {
                if (sky_get_sun_screen_pos(&A, &B, param_1, param_2) == 0) {
                    rp->v1C = 0x7FFF;
                    rp->flags |= 8;
                    rp->v14 = A;
                    rp->v18 = B;
                }
            } else if (fl & 2) {
                if (project_to_screen(0, 0x14, -0x12C, &A, &B, &C, &D, param_1, 0) == 0) {
                    rp->v1C = 0x7FFF;
                    rp->flags |= 8;
                    rp->v14 = A;
                    rp->v18 = B;
                }
            } else {
                if (project_to_screen(rp->a1, rp->a2, rp->a3, &A, &B, &C, &E, param_1, param_2) == 0) {
                    rp->v14 = A;
                    rp->v18 = B;
                    rp->v1C = C;
                    rp->flags |= 8;
                }
            }
            if (rp->v14 < -0x32 || rp->v18 < -0x32 ||
                gScreenWidth + 0x32 < rp->v14 || gScreenHeight + 0x32 < rp->v18) {
                rp->flags &= ~8;
            }
        }
    }
}

typedef struct { u32 flags; s32 pad[4]; s32 x, y, z; s32 res; } Obj_25;
typedef struct { s32 pad; s32 n; Obj_25 o[1]; } Lst_2;

extern Lst_2 *sSunList_4;

void sun_check_occlusion(s32 a0)
{
    s32 i, r, c;
    s32 x, y, thr, v, cnt, tot;
    s32 x0, y0, w, h, xe, ye;
    Obj_25 *o;
    u16 *tb;

    if (sSunList_4 == NULL) {
        return;
    }
    osInvalDCache(0, 0x400000);
    w = gViewRects_2.w;
    h = gViewRects_2.h;
    x0 = gViewRects_2.x;
    y0 = gViewRects_2.y;
    i = 0;
    if (i < sSunList_4->n) {
        do {
            o = &sSunList_4->o[i];
            o->res = -1;
            if (!(o->flags & 8)) {
                o->flags &= ~0x10;
            } else {
                tot = 0;
                cnt = 0;
                x = o->x - 4;
                y = o->y - 4;
                thr = o->z - 0x19;
                for (r = 0; r < 8; r++) {
                    if (y < y0) {
                        y = y0;
                    }
                    if (y >= y0 + h) {
                        y = y0 + h - 1;
                    }
                    for (c = 0; c < 8; c++) {
                        if (x < x0) {
                            x = x0;
                        }
                        if (x >= x0 + w) {
                            x = x0 + w - 1;
                        }
                        tb = &gZBuffer_2[y * gScreenWidth + x];
                        v = zbuffer_decode(*tb);
                        if (v > 0x7FFF) {
                            v = 0x7FFF;
                        }
                        if (v >= thr) {
                            cnt++;
                        }
                        tot++;
                        x++;
                    }
                    x -= 8;
                    y++;
                }
                if (tot != 0) {
                    cnt = cnt * 7 / tot;
                }
                cnt--;
                if (cnt > 5) {
                    cnt = 5;
                }
                o->res = cnt;
                o->flags |= 0x10;
            }
            i++;
        } while (i < sSunList_4->n);
    }
}

#define SINS(a) gCosTable[((a) >> 4) & 0xFFF]
#define COSS(a) gCosTable[(((a) >> 4) - 0x400) & 0xFFF]

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
} Ent_32;

typedef struct {
    s32 f0;
    s32 n;
    Ent_32 e[1];
} EntList_2;

typedef struct {
    s16 x, y, z;
    u16 flag;
    s16 s, t;
    u8 c[4];
} V;

extern EntList_2 *sSunList_5;
extern V *sSunVtxBufs_3[];

static inline V *allocVtx(s32 idx, s32 n) {
    s32 c = sSunVtxUsed[idx];
    if (c + n > 128) {
        return NULL;
    }
    sSunVtxUsed[idx] = c + n;
    return &sSunVtxBufs_3[idx][c];
}

Gfx *sun_draw_flare(Gfx *gdl, u8 *mtx, s32 idx) {
    s32 i, j, k;
    Ent_32 *e;
    V *v;
    TexInfo *t;
    s32 a, b, p, q, x, y, ang, c, s, m, n, w;
    s32 bb;
    s32 aa;
    V *vv;

    if (sSunList_5 == NULL) {
        return gdl;
    }
    sSunVtxUsed[idx] = 0;
    if (gCheatFlags & 0x40) {
        return gdl;
    }

    gDPPipeSync(gdl++);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPViewport(gdl++, &gScreenViewport);
    gSPMatrix(gdl++, mtx + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, gViewRects_2.x, gViewRects_2.y, gViewRects_2.x + gViewRects_2.w - 1,
                  gViewRects_2.y + gViewRects_2.h - 1);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);

    gSunGlare = 0;
    for (i = 0; i < sSunList_5->n; i++) {
        e = &sSunList_5->e[i];
        if (e->flags & 8) { if (e->flags & 0x10) { if (e->tex >= 0) {
            p = gScreenWidth / 2 - e->x;
            q = gScreenHeight / 2 - e->y;
            a = 0x80 - isqrt_u32_2(p * p + q * q);
            if (a >= 0) {
                a = a * e->tex / 6;
                if (a != 0) {
                    a = a * 192 / 128;
                    if (i == 0) {
                        gSunGlare = a;
                    }
                    v = allocVtx(idx, 28);
                    if (v != NULL) {
                        x = e->x - gScreenWidth / 2;
                        y = e->y - gScreenHeight / 2;
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
                                v[j * 7 + k + 1].x = x + COSS(ang) * sFlareSizes[j] / 16384;
                                v[j * 7 + k + 1].y = b + SINS(ang) * sFlareSizes[j] / 16384;
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
                            gDPSetPrimColor(gdl++, 0, 0, sFlareColors[j * 3], sFlareColors[j * 3 + 1],
                                            sFlareColors[j * 3 + 2], a);

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
    if (gCurTrack_4->f2C == 0) {
        gDPSetTexturePersp(gdl++, G_TP_PERSP);
    } else {
        gDPSetTexturePersp(gdl++, G_TP_NONE);
    }

    for (i = 0; i < sSunList_5->n; i++) {
        e = &sSunList_5->e[i];
        if (e->flags & 8) { if (e->flags & 0x10) { if ((n = e->tex) >= 0) {
            gDPPipeSync(gdl++);
            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
            t = &sFlareTextures_2[n + 1];
            if (gCurTrack_4->f2C == 0) {
                x = e->x - gScreenWidth / 2;
                y = gScreenHeight / 2 - e->y;
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
                if (p >= gScreenWidth) {
                    p = gScreenWidth - 1;
                }
                if (q >= gScreenHeight) {
                    q = gScreenHeight - 1;
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

    gSPSetGeometryMode(gdl++, gCullMode);
    gDPPipeSync(gdl++);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    gDPSetAlphaDither(gdl++, G_AD_DISABLE);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gSPMatrix(gdl++, mtx, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    return gdl;
}

Gfx *sun_draw_stars(Gfx *gdl, s32 arg)
{
    s32 c, i;
    u8 *img;
    s32 sx, sy, sz, sw;

    if (sSunList == 0) return gdl;
    if (*(s32 *)(gCurTrack + 0x50) & 0x20) {
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x0C084000, 0);
        { Gfx *_g = gdl++; _g->words.w0 = 0xFC11FE23; _g->words.w1 = 0xFFFFFFFF; }
        img = *(u8 **)(sStarTexture + 0xC) + 0x28;
        gDPLoadTextureBlock(gdl++, img, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
        for (i = 0, c = 0x80; i < 0x40; i++) {
            if (project_to_screen(sStars_2[i].x + (gCamPos_2.x >> 4), sStars_2[i].y + (gCamPos_2.y >> 4),
                              sStars_2[i].z + (gCamPos_2.z >> 4), &sx, &sy, &sz, &sw, arg, 0) == 0
                && sx >= 0 && sy >= 0 && sx <= gScreenWidth && sy <= gScreenHeight) {
                gDPSetPrimColor(gdl++, 0, 0, c, c, c, 255);
                gSPTextureRectangle(gdl++, (sx - 1) << 2, (sy - 1) << 2, (sx + 1) << 2, (sy + 1) << 2,
                                    0, 0, 0, 0x400, 0x400);
            }
            c += 2;
        }
    }
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    return gdl;
}

s32 project_to_screen(s32 x, s32 y, s32 z, s32 *p4, s32 *p5, s32 *p6, s32 *p7, s32 p8, s32 p9)
{
    f32 m[4][4];
    s16 *ps;
    f32 kx, ky, kz, kw;
    f32 fx, fy, fz, a, b, c, w;
    s32 ret;

    ps = (s16 *)(p9 * 16 + gViewports_2);
    kx = (f32)ps[0] * 0.25f;
    ky = (f32)ps[1] * 0.25f;
    kw = (f32)ps[4] * 0.25f;
    kz = (f32)ps[5] * 0.25f;
    guMtxL2F(&m[0][0], (u32 *)(p8 + p9 * 0x40));

    fx = (f32)x;
    fy = (f32)y;
    fz = (f32)z;
    a = m[0][0] * fx + m[1][0] * fy + m[2][0] * fz + m[3][0];
    b = m[0][1] * fx + m[1][1] * fy + m[2][1] * fz + m[3][1];
    c = m[0][2] * fx + m[1][2] * fy + m[2][2] * fz + m[3][2];
    w = m[0][3] * fx + m[1][3] * fy + m[2][3] * fz + m[3][3];
    ret = 1;
    if (w != 0.0f) {
        a = a / w;
        b = b / w;
        c = c / w;
        if (c <= -1.0f) {
            return ret;
        }
        if (1.0f <= c) {
            return ret;
        }
        *p4 = (s32)(a * kx + kw);
        *p5 = (s32)(b * -ky + kz);
        *p6 = (s32)(c * 1023.0f * 0.5f + 511.5f) << 5;
        if (p7 != 0) {
            *p7 = (s32)w;
        }
        ret = 0;
    }
    return ret;
}

void sun_free(void) {
    if (sStars != 0) {
        mem_free(sStars);
        sStars = 0;
    }
    if (sSunVtxBufs[0] != 0) {
        mem_free(sSunVtxBufs[0]);
        mem_free(sSunVtxBufs[1]);
    }
    sSunList = 0;
}

s32 zbuffer_decode(s32 x) {
    s32 off = x >> 11;
    s32 v = (x >> 2) & 0x7FF;
    u8 *p1 = sZDecodeShift;
    s32 sh = *(s32 *)(p1 + (off & 0x1C));
    s32 add = *(s32 *)(sZDecodeAdd + (off & 0x1C));
    return ((v << sh) + add) >> 3;
}

s32 sun_alloc_vtx(s32 a, s32 idx) {
    s32 n = sSunVtxUsed[idx] + a;
    if (n >= 0x81) {
        return 0;
    }
    sSunVtxUsed[idx] = n;
    return sSunVtxBufs[idx] + (n - a) * 16;
}
