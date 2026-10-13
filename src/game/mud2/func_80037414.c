#include "ctx.h"

typedef struct {
    s16 life;   /* 0x00 */
    s16 max;    /* 0x02 */
    u8 type;    /* 0x04 */
    u8 color;   /* 0x05 */
    u8 r, g, b; /* 0x06 */
    u8 pad9[3];
    Vec3f pos;  /* 0x0C */
    Vec3f vel;  /* 0x18 */
} Part;

typedef struct {
    u8 w;
    u8 h;
    u8 pad[2];
    u8 *img;
} TexEnt;

typedef struct {
    u32 n;
    u32 pad;
    TexEnt e[1];
} TexTab;

typedef struct {
    u8 pad[0x2C];
    u8 r1, g1, b1, a1; /* 0x2C */
    u8 r2, g2, b2;     /* 0x30 */
    u8 pad33[0xD];
} ColDef;

typedef struct {
    s32 x, y, z;
    u8 pad0C[0xC];
    s32 rx, ry, rz; /* 0x18 */
    u8 pad24[0x5B0 - 0x24];
    u32 flags; /* 0x5B0 */
    u8 pad5B4[0x654 - 0x5B4];
    u8 f654;
    u8 pad655[0x7E0 - 0x655];
} Bike;

typedef struct {
    s32 x, y, z;
} Vec3i;

typedef struct {
    u8 pad[0x2C];
    s32 f2C;
} Cfg;

extern s32 D_800E1898;
extern Part *D_800E9214;
extern TexTab *D_800E92A0;
extern TexTab *D_800E9270;
extern TexTab *D_800E929C;
extern TexTab *D_800E926C;
extern TexTab *D_800E9290;
extern s32 D_800E9228[];
extern Bike D_800E37C0[];
extern ColDef D_800C9080[];
extern Vec3i D_800E8090[];
extern Cfg *D_800EFD64;
extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern s32 D_800C2594;
extern s32 D_800E3710;

void func_80012DC8(s32 *param_1, s32 *param_2, s32 *param_3);
void func_80011A58(s32 *param_1, s32 param_2, s32 param_3, s32 param_4);
s32 func_80033AC8(s32 x, s32 y, s32 z, s32 *p4, s32 *p5, s32 *p6, s32 *p7, s32 p8, s32 p9);

static inline s32 project(Vec3f *p, s32 view, s32 arg2, s32 *sx, s32 *sy, s32 *sz, s32 *w) {
    if (D_800EFD64->f2C == 0) {
        return func_80033AC8(p->x * 1000.0f * 0.0625f, p->y * 1000.0f * 0.0625f, p->z * 1000.0f * 0.0625f, sx, sy,
                             sz, w, arg2, view);
    } else {
        return func_80033AC8(p->x * 1000.0f * 0.0625f - (D_800E8090[view].x >> 4),
                             p->y * 1000.0f * 0.0625f - (D_800E8090[view].y >> 4),
                             p->z * 1000.0f * 0.0625f - (D_800E8090[view].z >> 4), sx, sy, sz, w, arg2, view);
    }
}

Gfx *func_80037414(Gfx *gdl, s32 view, s32 arg2) {
    s32 mtx[9];
    s32 v[3];
    s32 out[3];
    Vec3f pos;
    s32 sx, sy, sz, w;
    s32 i, size, k, hh, dsdx, dtdy, alpha, a, f, last;
    Bike *b;

    if (D_800E1898 >= 4) {
        return gdl;
    }

    gDPPipeSync(gdl++);
    gDPSetColorDither(gdl++, G_CD_NOISE);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x30B261, 0xFF67FFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetDepthSource(gdl++, G_ZS_PRIM);
    gDPLoadTextureBlock(gdl++, D_800E92A0->e[0].img, G_IM_FMT_IA, G_IM_SIZ_8b, D_800E92A0->e[0].w * 8,
                        D_800E92A0->e[0].h, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    for (i = 0; i < 96; i++) {
        if (D_800E9214[i].life >= 0 && D_800E9214[i].type == 0) {
            if (project(&D_800E9214[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (D_800C2594 == 1) {
                    D_800E9214[i].life = -1;
                }
            } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                if (D_800C2594 == 1) {
                    D_800E9214[i].life = -1;
                }
            } else if (w < 5001 && w != 0) {
                size = 15000 / w;
                if (D_800C2594 == 2) {
                    size = size * 2 / 3;
                } else if (D_800C2594 >= 3) {
                    size /= 2;
                }
                if (size > 48) {
                    size = 48;
                }
                if (D_800E9214[i].vel.y > 0.0f) {
                    k = ((D_800E9214[i].max - D_800E9214[i].life) << 10) / D_800E9214[i].max + 0x80;
                    if (k > 0x400) {
                        k = 0x400;
                    }
                    if (D_800E3710 == 0) {
                        D_800E9214[i].vel.y -= 0.005f;
                    }
                } else {
                    k = ((D_800E9214[i].max - D_800E9214[i].life) << 9) / D_800E9214[i].max + 0x100;
                    if (k > 0x200) {
                        k = 0x200;
                    }
                }
                size = size * k / 256;
                if (size < 3) {
                    size = 3;
                }
                dsdx = (D_800E92A0->e[0].w << 13) / size;
                a = D_800E9214[i].color;
                hh = size * 2 / 3;
                alpha = 0xFF - D_800C9080[a].a1;
                alpha = (((D_800E9214[i].life << 7) / D_800E9214[i].max + 0x7F) * alpha) >> 8;
                dtdy = (D_800E92A0->e[0].h << 10) / hh;
                if (alpha != 0) {
                    gDPSetPrimColor(gdl++, 0, 0, (D_800C9080[a].r1 * D_800E9214[i].r) >> 8,
                                    (D_800C9080[a].g1 * D_800E9214[i].g) >> 8, (D_800C9080[a].b1 * D_800E9214[i].b) >> 8,
                                    alpha);
                    gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                    gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                        (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                }
            }
        }
    }

    for (i = 0; i < 96; i++) {
        if (D_800E9214[i].life >= 0 && D_800E9214[i].type == 6) {
            if (project(&D_800E9214[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (D_800C2594 == 1) {
                    D_800E9214[i].life = -1;
                }
            } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                if (D_800C2594 == 1) {
                    D_800E9214[i].life = -1;
                }
            } else if (w != 0) {
                k = ((D_800E9214[i].max - D_800E9214[i].life) * 0x2800) / D_800E9214[i].max + 0x500;
                size = 15000 / w;
                if (k > 0x400) {
                    k = 0x400;
                }
                size = size * k / 256;
                if (size < 20) {
                    size = 20;
                }
                if (size > 100) {
                    size = 100;
                }
                dsdx = (D_800E92A0->e[0].w << 13) / size;
                alpha = (D_800E9214[i].life * 0xC0) / D_800E9214[i].max + 0x3F;
                hh = size * 2 / 3;
                a = 0xFF - (w * 0xFF) / 15000;
                dtdy = (D_800E92A0->e[0].h << 10) / hh;
                if (a < 0) {
                    a = 0;
                }
                if (a > 255) {
                    a = 255;
                }
                alpha = (alpha * a) >> 8;
                if (alpha != 0) {
                    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, alpha);
                    gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                    gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                        (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                }
            }
        }
    }

    for (i = 0; i < 96; i++) {
        if (D_800E9214[i].life >= 0 && D_800E9214[i].type == 3) {
            if (project(&D_800E9214[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (D_800C2594 == 1) {
                    D_800E9214[i].life = -1;
                }
            } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                if (D_800C2594 == 1) {
                    D_800E9214[i].life = -1;
                }
            } else if (w != 0) {
                size = 7500 / w;
                if (D_800C2594 >= 3) {
                    size /= 2;
                }
                if (size > 48) {
                    size = 48;
                }
                k = ((D_800E9214[i].max - D_800E9214[i].life) << 10) / D_800E9214[i].max + 0x100;
                if (k > 0x400) {
                    k = 0x400;
                }
                size = size * k / 256;
                if (size < 3) {
                    size = 3;
                }
                dsdx = (D_800E9270->e[0].w << 13) / size;
                hh = size * 2 / 3;
                dtdy = (D_800E9270->e[0].h << 10) / hh;
                gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, (D_800E9214[i].life * 245) / D_800E9214[i].max + 20);
                k = (u32)(i * 123 + D_800E9214[i].max - D_800E9214[i].life) % D_800E9270->n;
                gDPLoadTextureBlock(gdl++, D_800E9270->e[k].img, G_IM_FMT_IA, G_IM_SIZ_8b, D_800E9270->e[k].w * 8,
                                    D_800E9270->e[k].h, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
                gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                    (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
            }
        }
    }

    for (i = 0; i < 16; i++) {
        if (D_800E9228[i] != 0) {
            k = ((15 - D_800E9228[i]) * D_800E929C->n) / 15;
            if (k < 0) {
                k = 0;
            }
            if (k >= D_800E929C->n) {
                k = D_800E929C->n - 1;
            }
            gDPLoadTextureBlock(gdl++, D_800E929C->e[k].img, G_IM_FMT_IA, G_IM_SIZ_8b, D_800E929C->e[k].w * 8,
                                D_800E929C->e[k].h, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
            b = &D_800E37C0[i / 2];
            func_80011A58(mtx, b->rx, b->ry, b->rz);
            v[0] = 100;
            v[1] = 700;
            v[2] = (D_800E9228[i] - 15) * 40 - 650;
            func_80012DC8(v, mtx, out);
            pos.x = (f32)(out[0] + b->x) / 1000.0f;
            pos.y = (f32)(out[1] + b->y) / 1000.0f;
            pos.z = (f32)(out[2] + b->z) / 1000.0f;
            if (project(&pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (D_800C2594 == 1) {
                    D_800E9214[i].life = -1;
                }
            } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                if (D_800C2594 == 1) {
                    D_800E9214[i].life = -1;
                }
            } else if (w != 0) {
                size = 10000 / w;
                if (D_800C2594 >= 2) {
                    size /= 2;
                }
                k = ((15 - D_800E9228[i]) << 8) / 15;
                k += 0x100;
                size = size * k / 256;
                if (size > 48) {
                    size = 48;
                }
                if (size < 3) {
                    size = 3;
                }
                if (D_800E37C0[i / 2].f654 != 0) {
                    size = size * 3 / 2;
                    k = (D_800E9228[i] * 63) / 15 + 0xC0;
                    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, k);
                } else {
                    if (D_800E37C0[i / 2].flags & 0x20000) {
                        size = size * 3 / 2;
                    }
                    k = (D_800E9228[i] * 127) / 15 + 0x80;
                    gDPSetPrimColor(gdl++, 0, 0, 0x60, 0x60, 0x80, k);
                }
                dsdx = (D_800E929C->e[0].w << 13) / size;
                hh = size * 2 / 3;
                dtdy = (D_800E929C->e[0].h << 10) / hh;
                gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                    (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
            }
        }
    }

    for (i = 0; i < 96; i++) {
        if (D_800E9214[i].life >= 0 && D_800E9214[i].type == 1) {
            if (project(&D_800E9214[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (D_800C2594 == 1) {
                    D_800E9214[i].life = -1;
                }
            } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                if (D_800C2594 == 1) {
                    D_800E9214[i].life = -1;
                }
            } else {
                gDPLoadTextureBlock(gdl++, D_800E926C->e[i & 3].img, G_IM_FMT_IA, G_IM_SIZ_8b,
                                    D_800E926C->e[i & 3].w * 8, D_800E926C->e[i & 3].h, 0, G_TX_CLAMP, G_TX_CLAMP, 5,
                                    5, G_TX_NOLOD, G_TX_NOLOD);
                if (w != 0) {
                    size = 15000 / w;
                    size /= 6;
                    if (D_800C2594 >= 3) {
                        size /= 2;
                    }
                    if (size > 24) {
                        size = 24;
                    }
                    k = ((D_800E9214[i].max - D_800E9214[i].life) << 9) / D_800E9214[i].max + 0x100;
                    if (k > 400) {
                        k = 400;
                    }
                    size = size * k / 256;
                    if (size < 3) {
                        size = 3;
                    }
                    dsdx = (D_800E926C->e[0].w << 13) / size;
                    hh = size * 2 / 3;
                    dtdy = (D_800E926C->e[0].h << 10) / hh;
                    a = D_800E9214[i].color;
                    gDPSetPrimColor(gdl++, 0, 0, (D_800C9080[a].r2 * D_800E9214[i].r) >> 8,
                                    (D_800C9080[a].g2 * D_800E9214[i].g) >> 8, (D_800C9080[a].b2 * D_800E9214[i].b) >> 8,
                                    (D_800E9214[i].life << 7) / D_800E9214[i].max + 0x40);
                    gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                    gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                        (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                }
            }
        }
    }

    last = -1;
    if (D_800C2594 == 1) {
        gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
        for (i = 0; i < 96; i++) {
            if (D_800E9214[i].life >= 0 && D_800E9214[i].type == 5) {
                if (project(&D_800E9214[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                    if (D_800C2594 == 1) {
                        D_800E9214[i].life = -1;
                    }
                } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                    if (D_800C2594 == 1) {
                        D_800E9214[i].life = -1;
                    }
                } else {
                    alpha = (D_800E9214[i].max - D_800E9214[i].life) / 2;
                    if (alpha >= 3) {
                        alpha = 5 - alpha;
                    }
                    if (alpha >= 0) {
                        if (last != alpha) {
                            last = alpha;
                            gDPLoadTextureBlock(gdl++, D_800E9290->e[alpha].img, G_IM_FMT_IA, G_IM_SIZ_8b,
                                                D_800E9290->e[alpha].w * 8, D_800E9290->e[alpha].h, 0, G_TX_CLAMP, G_TX_CLAMP,
                                                5, 5, G_TX_NOLOD, G_TX_NOLOD);
                        }
                        if (w != 0) {
                            size = 30000 / w;
                            size /= 6;
                            if (D_800C2594 >= 3) {
                                size /= 2;
                            }
                            if (size > 24) {
                                size = 24;
                            }
                            if (size < 15) {
                                size = 15;
                            }
                            dsdx = (D_800E9290->e[alpha].w << 13) / size;
                            hh = size * 2 / 3;
                            dtdy = (D_800E9290->e[alpha].h << 10) / hh;
                            gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                            gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                                (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                        }
                    }
                }
            }
        }
    }

    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x442230, 0);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    for (i = 0; i < 96; i++) {
        if (D_800E9214[i].life >= 0 && D_800E9214[i].type == 2) {
            if (project(&D_800E9214[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (D_800C2594 == 1) {
                    D_800E9214[i].life = -1;
                }
            } else if (sx < 0 || D_800F13EC < sx || sy < 0 || D_800F13F0 < sy) {
                if (D_800C2594 == 1) {
                    D_800E9214[i].life = -1;
                }
            } else {
                w = 0x7F - (w << 7) / 2000;
                if (w < 0) {
                    w = 0;
                }
                if (w > 127) {
                    w = 127;
                }
                gDPSetPrimColor(gdl++, 0, 0, w + 0x80, w + 0x80, 0, 255);
                gDPSetPrimDepth(gdl++, sz - 0x100, 0);
                gDPFillRectangle(gdl++, sx, sy, sx + 1, sy + 1);
            }
        }
    }

    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetDepthSource(gdl++, G_ZS_PIXEL);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    return gdl;
}
