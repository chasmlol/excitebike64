#include "ctx.h"

typedef struct {
    s32 x, y, z, r, a, b, c;
} Ent;

typedef struct {
    s32 cx;             /* 0x00 */
    s32 cz;             /* 0x04 */
    s32 f8, fC;
    s32 off;            /* 0x10 */
    u8 pad14[0x320 - 0x14];
    s32 count;          /* 0x320 */
    Ent e[8];           /* 0x324 */
} Tile;

typedef struct {
    s16 x, z, y, h;
} CVtx;

typedef struct {
    s16 v0, v1;
    s16 f4;
    s16 flags;          /* 0x06 */
} CEdge;

typedef struct {
    s16 v[3];
    s16 f6, f8;
    s16 flags;          /* 0x0A */
} CTri;

typedef struct {
    u16 *tris;
    u16 *edges;
} CCell;

typedef struct {
    s32 minx, minz, maxx, maxz;
    s32 f10;
    CVtx *verts;        /* 0x14 */
    s32 f18;
    CEdge *edges;       /* 0x1C */
    s32 f20;
    CTri *tris;         /* 0x24 */
    CCell cells[257];   /* 0x28 */
} Grid;

typedef struct {
    u8 pad[0x2C];
    s32 mode;
} Game;

extern Game *D_800EFD64;
extern s32 D_800E850C;
extern Tile *D_800EE138;
extern Ent D_800E8D78[];
extern Grid *D_800E8D6C;
extern s32 D_800E84FC;
extern s32 D_800E8504;
extern CEdge *D_800E8538[];
extern CTri *D_800E8960[];
extern s32 D_800C2350;

s32 func_8002E920(s32 x, s32 y, s32 z);

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

void func_80030ECC(s32 x0, s32 z0, s32 x1, s32 z1)
{
    s32 px[3];
    s32 py[3];
    s32 pz[3];
    s32 qz[3];
    s32 i, j, k, m;
    s32 ix0, iz0, ix1, iz1;
    s32 n, mask, v, lo, hi;
    CCell *cell;
    CEdge *e;
    CTri *t;
    CVtx *a, *b;

    D_800E850C = 0;
    if (D_800EFD64->mode == 2) {
        D_800E850C = 0;
        for (i = 0; i < 16; i++) {
            if (D_800EE138[i].off != 0) continue;
            if (x1 < D_800EE138[i].cx * 256000) continue;
            if (z1 < D_800EE138[i].cz * 256000) continue;
            if ((D_800EE138[i].cx + 1) * 256000 < x0) continue;
            if ((D_800EE138[i].cz + 1) * 256000 < z0) continue;
            for (j = 0; j < D_800EE138[i].count; j++) {
                if (x1 >= D_800EE138[i].e[j].x - D_800EE138[i].e[j].r
                    && z1 >= D_800EE138[i].e[j].z - D_800EE138[i].e[j].r
                    && D_800EE138[i].e[j].x + D_800EE138[i].e[j].r >= x0
                    && D_800EE138[i].e[j].z + D_800EE138[i].e[j].r >= z0) {
                    D_800E8D78[D_800E850C] = D_800EE138[i].e[j];
                    D_800E850C++;
                    if (D_800E850C == 8) return;
                }
            }
            if (D_800E850C == 8) return;
        }
    } else if (D_800E8D6C != NULL) {
        D_800E84FC = 0;
        D_800E8504 = 0;
        ix0 = ((x0 / 16 - D_800E8D6C->minx) * 16) / (D_800E8D6C->maxx - D_800E8D6C->minx);
        iz0 = ((z0 / 16 - D_800E8D6C->minz) * 16) / (D_800E8D6C->maxz - D_800E8D6C->minz);
        ix1 = ((x1 / 16 - D_800E8D6C->minx) * 16) / (D_800E8D6C->maxx - D_800E8D6C->minx);
        iz1 = ((z1 / 16 - D_800E8D6C->minz) * 16) / (D_800E8D6C->maxz - D_800E8D6C->minz);
        if (ix0 > 16 || iz0 > 16 || ix1 < 0 || iz1 < 0) return;
        if (ix0 < 0) ix0 = 0;
        if (ix0 > 15) ix0 = 15;
        if (iz0 < 0) iz0 = 0;
        if (iz0 > 15) iz0 = 15;
        if (ix1 > 15) ix1 = 15;
        if (iz1 > 15) iz1 = 15;
        for (i = ix0; i <= ix1; i++) {
            for (j = iz0; j <= iz1; j++) {
                cell = &D_800E8D6C->cells[j * 16 + i];
                n = (u32)D_800E8D6C->cells[j * 16 + i + 1].edges - (u32)cell->edges;
                n = (u32)n >> 1;
                for (k = 0; k < n; k++) {
                    if (D_800E84FC >= 0x100) break;
                    e = &D_800E8D6C->edges[cell->edges[k]];
                    if (e->flags & 1) continue;
                    a = &D_800E8D6C->verts[e->v0];
                    b = &D_800E8D6C->verts[e->v1];
                    if (a->x < b->x) {
                        if ((a->x << 6) >= x1 || x0 >= (b->x << 6)) continue;
                    } else {
                        if ((b->x << 6) >= x1 || x0 >= (a->x << 6)) continue;
                    }
                    if (a->z < b->z) {
                        if ((a->z << 6) >= z1 || z0 >= (b->z << 6)) continue;
                    } else {
                        if ((b->z << 6) >= z1 || z0 >= (a->z << 6)) continue;
                    }
                    {
                        {
                            D_800E8538[D_800E84FC] = e;
                            D_800E84FC++;
                            e->flags |= 1;
                        }
                    }
                }
                cell = &D_800E8D6C->cells[j * 16 + i];
                n = (u32)D_800E8D6C->cells[j * 16 + i + 1].tris - (u32)cell->tris;
                n = (u32)n >> 1;
                for (k = 0; k < n + ((n & 1) >> 1); k++) {
                    if (D_800E8504 >= 0x100) break;
                    t = &D_800E8D6C->tris[cell->tris[k]];
                    if (t->flags & 1) continue;
                    mask = 0xF;
                    for (m = 0; m < 3; m++) {
                        a = &D_800E8D6C->verts[t->v[m]];
                        v = a->x << 6;
                        if (v >= x0) mask &= ~1;
                        if (x1 >= v) mask &= ~2;
                        v = a->z << 6;
                        if (v >= z0) mask &= ~4;
                        if (z1 >= v) mask &= ~8;
                    }
                    if (mask == 0) {
                        D_800E8960[D_800E8504++] = t;
                        t->flags |= 1;
                    }
                }
            }
        }
        for (i = 0; i < D_800E84FC; i++) {
            D_800E8538[i]->flags &= ~1;
        }
        for (i = 0; i < D_800E8504; i++) {
            D_800E8960[i]->flags &= ~1;
        }
        if (D_800C2350 & 0x200) {
            for (i = 0; i < D_800E84FC; i++) {
                px[0] = D_800E8D6C->verts[D_800E8538[i]->v0].x << 6;
                px[1] = D_800E8D6C->verts[D_800E8538[i]->v0].x << 6;
                px[2] = D_800E8D6C->verts[D_800E8538[i]->v1].x << 6;
                pz[0] = D_800E8D6C->verts[D_800E8538[i]->v0].z << 6;
                pz[1] = D_800E8D6C->verts[D_800E8538[i]->v0].z << 6;
                pz[2] = D_800E8D6C->verts[D_800E8538[i]->v1].z << 6;
                func_8002E920(px[0], 10000, pz[0]);
                func_8002E920(px[2], 10000, pz[2]);
                py[0] = D_800E8D6C->verts[D_800E8538[i]->v0].y * 16;
                py[1] = (D_800E8D6C->verts[D_800E8538[i]->v0].y + D_800E8D6C->verts[D_800E8538[i]->v0].h) * 16;
                py[2] = D_800E8D6C->verts[D_800E8538[i]->v1].y * 16;
                px[0] = D_800E8D6C->verts[D_800E8538[i]->v0].x << 6;
                px[1] = D_800E8D6C->verts[D_800E8538[i]->v1].x << 6;
                px[2] = D_800E8D6C->verts[D_800E8538[i]->v1].x << 6;
                pz[0] = D_800E8D6C->verts[D_800E8538[i]->v0].z << 6;
                pz[1] = D_800E8D6C->verts[D_800E8538[i]->v1].z << 6;
                pz[2] = D_800E8D6C->verts[D_800E8538[i]->v1].z << 6;
                py[0] = (D_800E8D6C->verts[D_800E8538[i]->v0].y + D_800E8D6C->verts[D_800E8538[i]->v0].h) * 16;
                py[1] = (D_800E8D6C->verts[D_800E8538[i]->v1].y + D_800E8D6C->verts[D_800E8538[i]->v1].h) * 16;
            }
            for (i = 0; i < D_800E8504; i++) {
                px[0] = D_800E8D6C->verts[D_800E8960[i]->v[0]].x << 6;
                px[1] = D_800E8D6C->verts[D_800E8960[i]->v[1]].x << 6;
                px[2] = D_800E8D6C->verts[D_800E8960[i]->v[2]].x << 6;
                py[0] = (D_800E8D6C->verts[D_800E8960[i]->v[0]].y + D_800E8D6C->verts[D_800E8960[i]->v[0]].h) * 16;
                py[1] = (D_800E8D6C->verts[D_800E8960[i]->v[1]].y + D_800E8D6C->verts[D_800E8960[i]->v[1]].h) * 16;
                py[2] = (D_800E8D6C->verts[D_800E8960[i]->v[2]].y + D_800E8D6C->verts[D_800E8960[i]->v[2]].h) * 16;
                qz[0] = D_800E8D6C->verts[D_800E8960[i]->v[0]].z << 6;
                qz[1] = D_800E8D6C->verts[D_800E8960[i]->v[1]].z << 6;
                qz[2] = D_800E8D6C->verts[D_800E8960[i]->v[2]].z << 6;
            }
        }
    }
}
