#include "game.h"

typedef struct { u8 pad[0x198]; u8 f198; u8 rest[0x1A8 - 0x199]; } B1A8;

typedef struct {
    s32 x, y, z;
    u8 pad[0x1C - 12];
} Elem;

extern B1A8 *D_800ED7AC;
extern Elem D_800EA7C8[];

typedef struct { s32 *name; s32 val; } NP;

extern NP D_800CA3BC[];

void func_8003F6A0(void)
{
    s32 i;

    D_800C23F0 = 0x4000;
    D_800E783C = func_800BF79C(D_800E7728, D_800D0B68);
    for (i = 0; D_800CA3BC[i].name != 0; i++) {
        D_800CA3BC[i].val = func_800BF79C(D_800E7728, *D_800CA3BC[i].name);
    }
    D_800E781C = func_800BF79C(D_800E7728, D_800D1014);
    D_800ED7AC = func_800BC908(0x19e10, 0, D_800D6FD0, 0xe9);
    for (i = 0; i < 0xfa; i++) {
        D_800ED7AC[i].f198 = 0;
    }
    D_800EA990 = func_800BC908(0x40000, 0, D_800D6FD0, 0xed);
    func_80044314(D_800EA990);
    func_80044358();
    for (i = 0; i < 0x10; i++) {
        D_800EA9A8[i] = func_800422AC(i);
    }
    func_8000ED10_2(D_800EBE08, -1, 0x1800);
    func_8000ED10_2(D_800EA998, 0, 0xc);
    D_800EB9E8 = 0xfa;
    for (i = 0xf9; i >= 0; i--) {
        D_800ED7C8[i] = i;
    }
    D_800EBDF8[0] = func_800BC908(0x400, 0, D_800D6FD0, 0xfe);
    D_800EBDF8[1] = func_800BC908(0x400, 0, D_800D6FD0, 0xff);
    D_800EBE04 = func_800BF79C(D_800E7728, D_800D0DF0);
    D_800EA988 = func_800BF79C(D_800E7728, D_800D0C50);
    D_800EDBB4 = func_800BC908(0x80, 0, D_800D6FD0, 0x104);
    D_800EDFD0 = func_800BF79C(D_800E7728, D_800D07C4);
    D_800EA98C = func_800BF79C(D_800E7728, D_800D0BC8);
}

typedef struct {
    u8 pad[0x19C];
    s16 x;
    s16 y;
    u8 slot;
    s8 active;
    u8 pad2[0x1A8 - 0x1A2];
} Obj_32;

extern Obj_32 *D_800ED7AC_2;

void func_8003F8FC(s32 arg0)
{
    s32 i;
    s32 j;
    Elem *e;
    Mtx m1;
    Mtx m2;

    D_800CA3B8 = D_800E76D0 / 4;
    func_800443F0(100);
    D_800ED698 = D_800E8090_2.x;
    D_800ED69C = D_800E8090_2.y;
    D_800ED6A0 = D_800E8090_2.z;
    func_8004262C(D_800ED698, 0x1F400, &D_800EDFD4, &D_800EBDF0);
    func_8004262C(D_800ED6A0, 0x1F400, &D_800EDFD8, &D_800EBDF4);
    func_80042830(D_800ED698, D_800ED6A0);
    func_80040DBC();
    func_800417B8();
    D_800EB9EC = 0;
    for (i = 0; i < 250; i++) {
        if (D_800ED7AC_2[i].active != 0) {
            for (j = 0; j < D_800EB9EC; j++) {
                if (D_800ED6A8[j] == D_800ED7AC_2[i].x && D_800ED728[j] == D_800ED7AC_2[i].y) {
                    D_800ED7AC_2[i].slot = j;
                    break;
                }
            }
            if (j == D_800EB9EC) {
                D_800ED6A8[j] = D_800ED7AC_2[i].x;
                D_800ED728[j] = D_800ED7AC_2[i].y;
                func_80011490(&D_800EA9E8[D_800E30A8][j], 0, 0, 0,
                              (D_800ED7AC_2[i].x - D_800EDFD4) * 0x1F400 - D_800EBDF0 + 64000,
                              -D_800ED69C,
                              (D_800ED7AC_2[i].y - D_800EDFD8) * 0x1F400 - D_800EBDF4 + 64000,
                              0x100);
                D_800ED7AC_2[i].slot = D_800EB9EC;
                D_800EB9EC++;
            }
        }
    }
    D_800ED608 = func_80043F24(D_800ED698, D_800ED6A0, D_800E8000_2, 0x1F400, D_800EA7C8, 0x10);
    for (i = 0; i < D_800ED608; i++) {
        func_80011490(&D_800EBDF8[D_800E30A8][i], 0, 0, 0,
                      D_800EA7C8[i].x - D_800ED698, D_800EA7C8[i].y - D_800ED69C, D_800EA7C8[i].z - D_800ED6A0, 0x10);
    }
    if (D_800CA3B0 >= 0) {
        func_80011490(&m1, 0, 0, D_800CA3B0, 0, 0, 0, 0x10);
        func_80011490(&m2, -0x1000, 0, 0, -70, -55, 300, 10);
        func_800120A4(&m1, &m2, &D_800EDBB4_2[D_800E30A8]);
    }
    func_80011490(&D_800ED610[D_800E30A8], 0, 0, D_800E8000_2, 0, 0, 1000, 0x100);
}

typedef struct Patch {
    Vtx v[25];
    s32 x;
    s32 y;
    u8 type;
    u8 mode;
    u8 vis;
    u8 dl;
    s16 bx;
    s16 by;
    s8 mtx;
    u8 pad1A1[7];
} Patch;

typedef struct TexSet_2 {
    s32 pad0;
    u16 *pal;
    s32 pad8;
    u8 *img;
} TexSet_2;

extern TexSet_2 *D_800E783C_3;

Gfx *func_8003FD4C(Gfx *gdl, Mtx *m, s32 arg2, s32 arg3)
{
    s32 i, j;
    s16 id;
    Patch *p;

    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, 0x200004);
    gDPSetCombine(gdl++, 0x121824, 0xFF33FFFF);
    gDPSetRenderMode(gdl++, 0x552230, 0);
    gDPLoadTLUT_pal256(gdl++, D_800E783C_3->pal);
    D_800EBE00 = -1;
    gDPLoadTextureBlock_4b(gdl++, D_800E783C_3->img, G_IM_FMT_CI, 64, 64, 0,
                           G_TX_WRAP, G_TX_WRAP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    for (j = 0; j < 1; j++) {
        for (i = 0; i < 0x400; i++) {
            id = D_800EBE08[j][i];
            if (id != -1) {
                p = (Patch *)(id * 0x1A8 + (s32)D_800ED7AC);
                if (p->vis == 1) {
                    if (p->mtx != D_800EBE00) {
                        gSPMatrix(gdl++, &D_800EA9E8[D_800E30A8][p->mtx], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                        D_800EBE00 = p->mtx;
                    }
                    gSPVertex(gdl++, p->v, 25, 0);
                    gSPDisplayList(gdl++, D_800EA9A8[p->dl]);
                }
            }
        }
    }
    gSPMatrix(gdl++, &m[16], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    return gdl;
}

#define RAW(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u8 pad[0x58]; u16 fmin[4]; u16 fmax[4]; } FogCfg;
typedef struct { u32 pad0; void *pal; u32 pad8; void *tex; } TexSet_3;
typedef struct { u8 pad[0x19A]; u8 f19A; u8 f19B; u8 pad19C[4]; s8 f1A0; u8 pad1A1[7]; } Obj_33;
typedef struct { u8 pad[0x14]; s32 f14; u8 pad18[4]; } Ent1C_2;

extern TexSet_3 *D_800E783C_4;
extern TexSet_3 *D_800E781C_5;
extern Ent1C_2 D_800EA7C8_2[];

Gfx *func_80040190(Gfx *gdl, s32 a1, s32 a2, s32 a3)
{
    FogCfg *fc;
    s32 i, k, j;
    s16 v;
    Obj_33 *o;

    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPPipeSync(gdl++);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPSetGeometryMode(gdl++, G_FOG);
    gDPSetFogColor(gdl++, D_800DC0E0_2.r, D_800DC0E0_2.g, D_800DC0E0_2.b, 255);
    fc = D_800EFD64;
    gSPFogPosition(gdl++, fc->fmin[D_800C2594 - 1], fc->fmax[D_800C2594 - 1]);
    RAW(gdl++, 0xFC127E24, 0xFFFFF9FC);
    RAW(gdl++, 0xE200001C, 0xC8004030);
    D_800EBE00 = -1;
    gDPLoadTLUT_pal256(gdl++, D_800E783C_4->pal);
    gDPLoadTextureBlock_4b(gdl++, D_800E783C_4->tex, G_IM_FMT_CI, 64, 64, 0,
                           G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPViewport(gdl++, &D_800E19C0[a3 + 4]);
    for (j = 0; j < D_800EB9EC; j++) {
        D_800EBE00 = 0;
        for (k = 0; k < 3; k++) {
            for (i = 0; i < 0x400; i++) {
                v = D_800EBE08[k][i];
                if (v != -1) {
                    o = (Obj_33 *)(v * sizeof(Obj_33) + (u32)D_800ED7AC);
                    if (o->f1A0 == j && (k != 0 || o->f19A == 0)) {
                        if (D_800EBE00 == 0) {
                            gSPMatrix(gdl++, &D_800EA9E8[D_800E30A8][j], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                            D_800EBE00 = 1;
                        }
                        gSPVertex(gdl++, o, 25, 0);
                        { Gfx **dl = D_800EA9A8; gSPDisplayList(gdl++, dl[o->f19B]); }
                    }
                }
            }
        }
    }
    gDPPipeSync(gdl++);
    func_80014A68((s32)D_80042718);
    gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
    gSPClearGeometryMode(gdl++, G_FOG);
    RAW(gdl++, 0xFCFFFFFF, 0xFFFCF87C);
    RAW(gdl++, 0xE200001C, 0x00442230);
    gDPLoadTLUT_pal256(gdl++, D_800E781C_5->pal);
    for (i = 0; i < D_800ED608; i++) {
        gSPMatrix(gdl++, &D_800EBDF8[a2][i], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gdl = func_80013D70(0, D_800CA3BC_2[D_800EA7C8_2[i].f14].f4, &D_800E781C_5, gdl);
    }
    func_80014A68(0);
    gDPPipeSync(gdl++);
    return gdl;
}

#define PKT(p, a, b) { Gfx *_g = (Gfx *)(p++); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct { u32 pad[5]; u32 f5; u32 f6; } Ent_40;

Gfx *func_800408A4(Gfx *gdl, s32 p2, s32 idx, s32 arg3)
{
    Gfx *w;
    u32 *g;
    s32 n;
    s32 i;
    Ent_40 *e;
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
                PKT(w, 0xDA380003, D_800EBDF8_2[idx] + i * 0x40);
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

#define PKT_2(a, b) { Blk_2 *_g = gdl++; _g->w0 = (a); _g->w1 = (b); }

Blk_2 *func_80040B64(Blk_2 *gdl, s32 p2, s32 arg2, s32 arg3)
{
    s32 off;
    Blk_2 *puVar3;

    if (D_800CA3B0 < 0) {
        return gdl;
    }
    {
        PKT_2(0xFCFFFFFF, 0xFFFCF87C)
        PKT_2(0xE200001C, 0x0C084000)
        PKT_2(0xE3001001, 0x00008000)
        PKT_2(0xD7000002, 0x80008000)
        PKT_2(0xE3000C00, 0x00080000)
        PKT_2(0xD9FFFFFE, 0)
        PKT_2(0xFD100000, D_800EBE04_2[1])
        PKT_2(0xE8000000, 0)
        PKT_2(0xF5000100, 0x07000000)
        PKT_2(0xE6000000, 0)
        PKT_2(0xF0000000, 0x073FC000)
        PKT_2(0xE7000000, 0)
        off = D_800E30A8 * 0x40;
        PKT_2(0xDA380003, (u32)(D_800EDBB4 + off))
        PKT_2(0xDA380007, (u32)(D_800E1938_2 + off))
        gdl = (Blk_2 *)func_80013D70(0, D_800EA988, (s32)&D_800EBE04_2, gdl);
        PKT_2(0xDA380003, p2 + 0x400)
        return gdl;
    }
}

#define G D_800EB9F0

void func_80040DBC(void) {
    s32 i, j, a, k;
    s32 x, z, u, v, h;
    s32 s, c, n, xx;
    s32 d = 4000;
    s32 w, m;

    a = D_800ED698 >> 4;
    h = D_800ED6A0 >> 4;
    D_800EA7C0 = xx = a / d;
    D_800ED7A8 = a - D_800EA7C0 * d + (((u32)a & 1) >> 1);
    w = D_800E8002;
    w >>= 4;
    s = D_800C2C60[w] + (((u32)&D_800C2C60[w] & 1) >> 1);
    c = D_800C2C60[(w - 0x400) & 0xFFF];
    if (D_800ED7A8 < 0) {
        D_800ED7A8 += d;
        D_800EA7C0 -= 1;
    }
    D_800EA7C4 = h / d;
    xx = D_800EA7C4 * d;
    D_800ED7B0 = h - xx + (((u32)h & 1) >> 1);
    if (D_800ED7B0 < 0) {
        D_800ED7B0 += d;
        D_800EA7C4 -= 1;
    }
    n = 0;
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 32; j++) {
            G[n] = 0;
            x = ((j - 15) * 8000 - 4000) >> 4;
            k = ((u32)(D_800ED7A8 >> 31) & 1) >> 1;
            z = (i * 8000 - 0x1E460) >> 4;
            x -= D_800ED7A8 % 500;
            z -= D_800ED7B0 % 500;
            k = s * x;
            v = (s * z - c * x) >> 14;
            u = (k + c * z) >> 14;
            if (v < 5000) G[n] = 3;
            if (v < 4000) G[n] |= 4;
            if (i < 8 || i >= 24 || j < 8 || j >= 24) G[n] &= 0xFB;
            if (v < 1500) G[n] &= 0xF1;
            if (v >= -359 && u >= -359 && u < 360 && v < 360) {
                G[n] |= 1;
            } else if (v < 0) {
                G[n] = 0;
            } else {
                v = v / 2;
                if (u < 0) {
                    if (u + 360 < -v) G[n] &= 0xFE;
                    if (u + 720 < -v) G[n] &= 0xF1;
                } else {
                    if (v < u - 360) G[n] &= 0xFE;
                    if (v < u - 720) G[n] &= 0xF1;
                }
            }
            n++;
        }
    }
    D_800ED7A8 /= 500;
    D_800ED7B0 /= 500;
    D_800EDBB0 = D_800ED7A8 & 1;
    D_800EDBBC = D_800ED7B0 & 1;
    D_800EDBB8 = (D_800ED7A8 & 2) >> 1;
    D_800EDBC4 = (D_800ED7B0 & 2) >> 1;
    D_800EDBC0 = D_800ED7A8 & 4;
    D_800EDBC0 /= 4;
    D_800EDBC8 = D_800ED7B0 & 4;
    D_800EDBC8 /= 4;
    for (i = 2; i < 32; i += 2) {
        for (j = 2; j < 32; j += 2) {
            s32 r, kb, kc;
            r = ((i / 2 + 8) << 5) + 8;
            m = r + j / 2;
            n = ((i - D_800EDBBC) << 5) + (j - D_800EDBB0);
            kb = ((i - D_800EDBC4) << 5) + (j - D_800EDBB8);
            kc = ((i - D_800EDBC8) << 5) + (j - D_800EDBC0);
            if (G[n] & G[n + 1] & G[n + 32] & G[n + 33] & 1) G[m] &= 0xFD;
            if (G[kb] & G[kb + 1] & G[kb + 32] & G[kb + 33] & 2) G[m] &= 0xFB;
            if (G[kc] & G[kc + 1] & G[kc + 32] & G[kc + 33] & 4) G[m] &= 0xF7;
            if (G[m] & 2) { G[n] &= 0xFE; G[n + 1] &= 0xFE; G[n + 32] &= 0xFE; G[n + 33] &= 0xFE; }
            if (G[m] & 4) { G[kb] &= 0xFD; G[kb + 1] &= 0xFD; G[kb + 32] &= 0xFD; G[kb + 33] &= 0xFD; }
            if (G[m] & 8) { G[kc] &= 0xFB; G[kc + 1] &= 0xFB; G[kc + 32] &= 0xFB; G[kc + 33] &= 0xFB; }
        }
    }
    func_8000ED10(D_800EDBD0, 0, 0x400);
    for (i = 2; i < 32; i += 2) {
        for (j = 2; j < 32; j += 2) {
            s32 r = (((i >> 1) + 8) << 5) + 8;
            m = r + (j >> 1);
            n = (i - D_800EDBBC) * 32 + (j - D_800EDBB0);
            if (D_800EB9F0[m + 32] & 2) {
                if (D_800EB9F0[n + 32] & 1) D_800EDBD0[n + 32] |= 0x10;
                if (D_800EB9F0[n + 33] & 1) D_800EDBD0[n + 33] |= 0x10;
            }
            if (D_800EB9F0[m - 32] & 2) {
                if (D_800EB9F0[n] & 1) D_800EDBD0[n] |= 0x20;
                if (D_800EB9F0[n + 1] & 1) D_800EDBD0[n + 1] |= 0x20;
            }
            if (D_800EB9F0[m + 1] & 2) {
                if (D_800EB9F0[n + 1] & 1) D_800EDBD0[n + 1] |= 0x40;
                if (D_800EB9F0[n + 33] & 1) D_800EDBD0[n + 33] |= 0x40;
            }
            if (D_800EB9F0[m - 1] & 2) {
                if (D_800EB9F0[n] & 1) D_800EDBD0[n] |= 0x80;
                if (D_800EB9F0[n + 32] & 1) D_800EDBD0[n + 32] |= 0x80;
            }
            n = (i - D_800EDBC4) * 32 + (j - D_800EDBB8);
            if (D_800EB9F0[m + 32] & 4) {
                if (D_800EB9F0[n + 32] & 2) D_800EDBD0[n + 32] |= 1;
                if (D_800EB9F0[n + 33] & 2) D_800EDBD0[n + 33] |= 1;
            }
            if (D_800EB9F0[m - 32] & 4) {
                if (D_800EB9F0[n] & 2) D_800EDBD0[n] |= 2;
                if (D_800EB9F0[n + 1] & 2) D_800EDBD0[n + 1] |= 2;
            }
            if (D_800EB9F0[m + 1] & 4) {
                if (D_800EB9F0[n + 1] & 2) D_800EDBD0[n + 1] |= 4;
                if (D_800EB9F0[n + 33] & 2) D_800EDBD0[n + 33] |= 4;
            }
            if (D_800EB9F0[m - 1] & 4) {
                if (D_800EB9F0[n] & 2) D_800EDBD0[n] |= 8;
                if (D_800EB9F0[n + 32] & 2) D_800EDBD0[n + 32] |= 8;
            }
        }
    }
}

#define REC(h) (&D_800ED7AC_3[h])

typedef struct { u8 pad[0x190]; s32 x; s32 y; s8 life; u8 p199; u8 near; u8 snd; u8 p19c[5]; u8 vis; u8 p1a2[6]; } Rec_23;

extern Rec_23 *D_800ED7AC_3;

static inline void divmod(s32 v, s32 d, s32 *q, s32 *r)
{
    *q = v / d;
    *r = v - *q * d;
    if (*r < 0) {
        *r += d;
        *q -= 1;
    }
}

void func_800417B8(void)
{
    s32 qx0, rx, qy0, ry, qx, qy;
    s32 i, j, h, s3, wx, wy, b, dr, dc;

    divmod(D_800ED698 >> 4, 500, &qx0, &rx);
    divmod(D_800ED6A0 >> 4, 500, &qy0, &ry);

    for (i = 0; i < 3; i++) {
        j = 1 << i;
        D_800EA998_2[i] = ((qy0 / j) * 0x20 + qx0 / j) & 0x3ff;
    }
    D_800ED690 = qx0;
    D_800ED694 = qy0;

    for (i = 0; i < 250; i++) {
        if (REC(i)->life > 0)
            REC(i)->life--;
        REC(i)->vis = 0;
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 0x400; j++) {
            h = D_800EBE08[i][j];
            if (h != -1 && REC(h)->life <= 0)
                D_800EBE08[i][j] = -1;
        }
    }

    D_800EB9E8 = 0;
    for (i = 0; i < 250; i++) {
        if (REC(i)->life <= 0) {
            D_800ED7C8[D_800EB9E8++] = i;
        }
    }

    for (i = 0; i < 3; i++) {
        divmod(D_800ED698 >> 4, 500 << i, &qx, &rx);
        divmod(D_800ED6A0 >> 4, 500 << i, &qy, &ry);
        s3 = D_800EA998_2[i];
        wy = (qy - 16) * (500 << i);
        for (j = 0; j < 0x20; j++) {
            s32 k;
            wx = (qx - 16) * (500 << i);
            for (k = 0; k < 0x20; k++) {
                if ((D_800EB9F0_2[0][j * 32 + k] >> i) & 1) {
                    s32 t = D_800EBE08[i][s3];
                    s32 n;
                    if (t == -1) {
                        if (D_800EB9E8 == 0) goto out; h = D_800ED7C8[--D_800EB9E8]; D_800EBE08[i][s3] = h; func_80041E90(h, wx, wy, i);
                    } else {
                        h = t;
                        if (REC(h)->x != wx || REC(h)->y != wy) {
                            if (D_800EB9E8 == 0) goto out; h = D_800ED7C8[--D_800EB9E8]; D_800EBE08[i][s3] = h; func_80041E90(h, wx, wy, i);
                        } else {
                            REC(h)->life = 2;
                        }
                    }
                    REC(h)->vis = 1;
                    dr = j - 16;
                    if (dr < 0)
                        dr = -dr;
                    dc = k - 16;
                    if (dc < 0)
                        dc = -dc;
                    if (dc + dr < 7)
                        REC(h)->near = 1;
                    else
                        REC(h)->near = 0;
                } else {
                    D_800EBE08[i][s3] = -1;
                }
                s3 = (s3 + 1) & 0x3ff;
                wx += 500 << i;
            }
            wy += 500 << i;
        }
    }
out:
    for (i = 0; i < 250; i++)
        REC(i)->snd = 0;

    for (i = 0; i < 2; i++) {
        s3 = D_800EA998_2[i];
        for (j = 0; j < 0x400; j++) {
            h = D_800EBE08[i][s3];
            if (h != -1) {
                Rec_23 *r = (Rec_23 *)(h * 0x1a8 + (u32)D_800ED7AC_3);
                u8 x = D_800EDBD0[j];
                if (i == 0)
                    r->snd = x >> 4;
                else
                    r->snd = x & 0xf;
            }
            s3 = (s3 + 1) % 0x400;
        }
    }
}

typedef struct Patch_2 {
    Vtx v[25];
    s32 x;
    s32 y;
    u8 type;
    u8 mode;
    s16 pad19A;
    s16 bx;
    s16 by;
    u8 pad1A0[8];
} Patch_2;

extern Patch_2 *D_800ED7AC_4;

static __inline__ void fdiv(s32 x, s32 d, s32 *q, s32 *r)
{
    *q = x / d;
    *r = x - *q * d;
    if (*r < 0) {
        *r += d;
        (*q)--;
    }
}

void func_80041E90(s32 n, s32 x, s32 y, s32 mode)
{
    s32 a, b, c, d;
    s32 tx, ty, px, pz, col, row, k;
    s32 h, s;
    Vtx *v;

    D_800ED7AC_4[n].x = x;
    D_800ED7AC_4[n].y = y;
    fdiv(x, 125, &a, &b);
    tx = a;
    fdiv(y, 125, &a, &b);
    ty = a;
    c = 0;
    d = 0;
    fdiv(x, 8000, &c, &a);
    x = a - 4000;
    fdiv(y, 8000, &d, &a);
    pz = a - 4000;
    k = 0;
    D_800ED7AC_4[n].mode = mode;
    D_800ED7AC_4[n].type = 2;
    D_800ED7AC_4[n].bx = c;
    D_800ED7AC_4[n].by = d;
    row = ty;
    for (a = 0; a < 5; a++) {
        s32 xx = x;
        col = tx;
        for (b = 0; b < 5; b++) {
            D_800ED7AC_4[n].v[k].v.ob[0] = xx;
            D_800ED7AC_4[n].v[k].v.ob[1] = D_800EA990[((row & 0x1FF) << 9) | (col & 0x1FF)] * 4;
            D_800ED7AC_4[n].v[k].v.ob[2] = pz;
            s = D_800EA990[((row & 0x1FF) << 9) | (col & 0x1FF)];
            s = (D_800EA990[(((row + 1) & 0x1FF) << 9) | (col & 0x1FF)] - s) * 8 + 0x80;
            if (s < 0) s = 0;
            if (s > 255) s = 255;
            D_800ED7AC_4[n].v[k].v.cn[0] = s * 245 / 256 + 10;
            D_800ED7AC_4[n].v[k].v.cn[1] = s * 230 / 256 + 25;
            D_800ED7AC_4[n].v[k].v.cn[2] = s * 170 / 256 + 85;
            D_800ED7AC_4[n].v[k].v.tc[0] = (b << 10) << mode;
            D_800ED7AC_4[n].v[k].v.tc[1] = (a << 10) << mode;
            D_800ED7AC_4[n].v[k].v.cn[3] = 255;
            k++;
            xx += 125 << mode;
            col += 1 << mode;
        }
        pz += 125 << mode;
        row += 1 << mode;
    }
}

s32 func_800422AC(s32 mask)
{
    Gfx *gdl = D_800C29E0;
    s32 i, j, k, b, c;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            k = i * 6 - j * 4;
            gSP2Triangles(gdl++, k + 10, k + 11, k + 16, 0, k + 10, k + 6, k + 11, 0);
            gSP2Triangles(gdl++, k + 11, k + 12, k + 16, 0, k + 11, k + 6, k + 12, 0);
        }
    }

    for (i = 0; i < 4; i++) {
        if ((mask >> i) & 1) {
            j = D_800CA3DC[i];
            b = D_800CA3EC[i];
            c = D_800CA3FC[i];
            gSP2Triangles(gdl++, j, j + b + c, j + 2 * b, 0, j + 2 * b, j + 3 * b + c, j + 4 * b, 0);
        } else {
            j = D_800CA3DC[i];
            b = D_800CA3EC[i];
            c = D_800CA3FC[i];
            gSP2Triangles(gdl++, j, j + b + c, j + b, 0, j + b, j + b + c, j + 2 * b, 0);
            gSP2Triangles(gdl++, j + 2 * b, j + 3 * b + c, j + 3 * b, 0, j + 3 * b, j + 3 * b + c, j + 4 * b, 0);
        }
    }

    gSPEndDisplayList(gdl++);
    i = (u8 *)gdl - (u8 *)D_800C29E0;
    gdl = func_800BC908(i, 0, (s32)D_800D6FD0, 0x508);
    func_800AEC34(gdl, D_800C29E0, i);
    return (s32)gdl;
}

void func_80042584(void)
{
    s32 i;
    s32 *p;

    func_800BCB04(D_800ED7AC);
    func_800BCB04(D_800EA990);
    func_800BCB04(D_800EBDF8[0]);
    func_800BCB04(D_800EBDF8[1]);
    i = 0;
    func_800BCB04(D_800EE138);
    func_800BCB04(D_800EDBB4);
    p = D_800EA9A8;
    do {
        s32 v = *p;
        p++;
        i++;
        func_800BCB04(v);
    } while (i < 16);
}

void func_80042620(s32 a0)
{
    D_800CA3B0 = a0;
}

void func_8004262C(s32 a, s32 b, s32 *q, s32 *r)
{
    s32 quo = a / b;
    s32 rem;
    *q = quo;
    rem = a - quo * b;
    *r = rem;
    if (rem < 0) {
        *r = rem + b;
        *q = *q - 1;
    }
}

void func_80042694(s32 param_1, s32 param_2)
{
    if (param_1 < D_800ED7B4) {
        D_800ED7B4 = param_1;
    }
    if (param_2 < D_800ED7B8) {
        D_800ED7B8 = param_2;
    }
    if (D_800ED7BC < param_1) {
        D_800ED7BC = param_1;
    }
    if (D_800ED7C0 < param_2) {
        D_800ED7C0 = param_2;
    }
}

void func_800426EC(void)
{
    D_800ED7B8 = 999;
    D_800ED7B4 = 999;
    D_800ED7C0 = -999;
    D_800ED7BC = -999;
}

#define PKT_3(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_80042718(Gfx *gdl, s32 x)
{
    Gfx *p0;
    Gfx *p1;

    if (x != 0) {
        p0 = gdl++;
        p1 = gdl++;
        p0->words.w0 = 0xE7000000;
        p0->words.w1 = 0;
        p1->words.w0 = 0xFCFFFFFF;
        p1->words.w1 = 0xFFFCF279;
        PKT_3(0xE200001C, 0x230);
        PKT_3(0xF9000000, 0xFE);
        PKT_3(0xE2001E01, 1);
    } else {
        PKT_3(0xE7000000, 0);
        PKT_3(0xE2001E01, 0);
        PKT_3(0xFCFFFFFF, 0xFFFCF87C);
        PKT_3(0xE200001C, 0x442230);
    }
    return gdl;
}

void func_8004281C(s32 x)
{
    D_800CA3B4 = x;
}
