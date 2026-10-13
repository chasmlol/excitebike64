#include "ctx.h"

typedef struct { u8 active; u8 col; u8 pad[2]; s32 id; u8 rest[0x10]; } Ent;
typedef struct { u8 pad[0x34]; u8 r, g, b; u8 pad2[9]; } Col;
typedef struct { u32 mask; u8 pad[0x1C]; } Obj;
typedef struct { u32 count; u8 pad[0x1C]; Obj obj[1]; } ObjList;
typedef struct { u8 pad[0x2C]; s32 mode; } Game;

extern s32 D_800EE140;
extern Game *D_800EFD64;
extern s32 D_800EB9EC;
extern Ent *D_800F0774;
extern s32 D_800ED728[];
extern s32 D_800ED6A8[];
extern Mtx D_800EA9E8[][32];
extern Vtx *D_800F07F8;
extern s32 D_800F0770;
extern Col *D_800E9298;
extern ObjList *D_800E8948;
extern Mtx **D_800E77D4;
extern void *D_800F0820;

#define RAW(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_8005E090(Gfx *gdl, s32 arg1, Mtx *arg2, s32 arg3) {
    s32 i, j, k, m, a, idx, started, v, hi, lo;
    Col *c;

    if (D_800EE140 != 0) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    RAW(gdl++, 0xE200001C, 0x00404A50);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    RAW(gdl++, 0xFC119623, 0xFF2FFFFF);
    gDPLoadTextureBlock(gdl++, D_800F0820, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 1, 0,
                        G_TX_CLAMP, G_TX_NOMIRROR | G_TX_WRAP, 3, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    if (D_800EFD64->mode == 2) {
        for (i = 0; i < D_800EB9EC; i++) {
            started = 0;
            for (j = 0; j < 128; j++) {
                if (!D_800F0774[j].active) continue;
                v = D_800F0774[j].id;
                hi = v >> 16;
                lo = (s16)v;
                if (hi == D_800ED6A8[i] && lo == D_800ED728[i]) {
                    k = j + 1;
                    if (!started) {
                        gSPMatrix(gdl++, &D_800EA9E8[arg1][i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                        started = 1;
                    }
                    for (; k < 128; k++) {
                        if (k >= j + 8) break;
                        if (!D_800F0774[k].active) break;
                        v = D_800F0774[k].id;
                        hi = v >> 16;
                        lo = (s16)v;
                        if (hi != D_800ED6A8[i] || lo != D_800ED728[i]) break;
                    }
                    k -= j;
                    gSPVertex(gdl++, &D_800F07F8[j * 4], k * 4, 0);
                    for (m = 0; m < k; m++) {
                        idx = m + j;
                        a = idx - D_800F0770;
                        if (a < 0) a += 128;
                        if (a >= 16) {
                            gDPSetPrimColor(gdl++, 0, 0, D_800E9298[D_800F0774[idx].col].r, D_800E9298[D_800F0774[idx].col].g, D_800E9298[D_800F0774[idx].col].b, a);
                            gSP2Triangles(gdl++, m * 4, m * 4 + 1, m * 4 + 2, 0, m * 4 + 2, m * 4 + 1, m * 4 + 3, 0);
                        }
                    }
                    j += k - 1;
                }
            }
        }
    } else if (D_800EFD64->mode == 1) {
        ObjList *l = D_800E8948;
        u32 bit;
        {
            for (i = 0; i < D_800E8948->count; i++) {
                if (l->obj[i].mask & (1 << (arg3 * 2))) {
                started = 0;
                for (j = 0; j < 128; j++) {
                    if (D_800F0774[j].active && D_800F0774[j].id == i) {
                        k = j + 1;
                        if (!started) {
                            gSPMatrix(gdl++, D_800E77D4[i * 4 + arg3], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                            started = 1;
                        }
                        for (; k < 128; k++) {
                            if (k >= j + 8) break;
                            if (!D_800F0774[k].active) break;
                            if (D_800F0774[k].id != i) break;
                        }
                        k -= j;
                        gSPVertex(gdl++, &D_800F07F8[j * 4], k * 4, 0);
                        for (m = 0; m < k; m++) {
                            idx = m + j;
                            a = idx - D_800F0770;
                            if (a < 0) a += 128;
                            if (a >= 16) {
                                gDPSetPrimColor(gdl++, 0, 0, D_800E9298[D_800F0774[idx].col].r, D_800E9298[D_800F0774[idx].col].g, D_800E9298[D_800F0774[idx].col].b, a);
                                gSP2Triangles(gdl++, m * 4, m * 4 + 1, m * 4 + 2, 0, m * 4 + 2, m * 4 + 1, m * 4 + 3, 0);
                            }
                        }
                        j += k - 1;
                    }
                }
                }
            }
        }
    } else {
        for (i = 0; i < 128; i += 8) {
            gSPVertex(gdl++, &D_800F07F8[i * 4], 32, 0);
            for (j = 0; j < 8; j++) {
                idx = i + j;
                if (D_800F0774[idx].active) {
                    if (D_800E8948->obj[D_800F0774[idx].id].mask & (1 << (arg3 * 2))) {
                        a = idx - D_800F0770;
                        if (a < 0) a += 128;
                        if (a >= 16) {
                            gDPSetPrimColor(gdl++, 0, 0, D_800E9298[D_800F0774[idx].col].r, D_800E9298[D_800F0774[idx].col].g, D_800E9298[D_800F0774[idx].col].b, a);
                            gSP2Triangles(gdl++, j * 4, j * 4 + 1, j * 4 + 2, 0, j * 4 + 2, j * 4 + 1, j * 4 + 3, 0);
                        }
                    }
                }
            }
        }
    }
    gSPMatrix(gdl++, arg2 + 16, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(gdl++);
    return gdl;
}
