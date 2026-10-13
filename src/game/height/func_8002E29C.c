#include "ctx.h"

typedef struct {
    s16 x, y, z;
    s16 pad[5];
} Vtx16;

typedef struct {
    s32 f0;
    Vtx16 *verts;
} Hdr;

typedef struct {
    u16 bits[4];
    u16 v[3];
    u16 flags;
} GTri;

typedef struct {
    u16 flags;
    u16 count;
    GTri tris[1];
} Grid;

typedef struct {
    Hdr *hdr;
    s32 x0, z0, x1, z1;
    Grid *grid;
    s32 f18;
} Sub;

typedef struct {
    s32 f0;
    Sub sub;
} Area;

extern Area *D_800E8948;
extern s32 D_800E8E7C;
extern s32 D_800E8D64;
extern s32 D_800E80D0;
extern s32 D_800E80D4;
extern s32 D_800E8508;
extern s32 D_800E8958;
extern s32 D_800E84F8;
extern s16 **D_800E8500;
extern s32 **D_800E8E6C;

void func_8000ECB8(s32 x) __attribute__((noreturn));
s32 func_80031EC4(void *poly, s32 px, s32 pz);

s32 func_8002E29C(s32 id, s32 x, s32 y, s32 z)
{
    Sub *a;
    Grid *g2;
    s32 px, pz;
    s32 mask, word, count;
    GTri *tri;
    Vtx16 *verts;
    s32 x0, z0, x1, z1;
    s32 w, d;
    s32 i, start;
    s32 v[3][4];
    Grid *grid;
    s32 dxp, dzp;
    s32 nx, ny, nz, h;
    s32 k;
    s32 c;

    D_800E8E7C++;
    a = (Sub *)((u8 *)D_800E8948 + (id * 32 + 4));
    x0 = a->x0;
    if (x < x0 * 16) return 0;
    z0 = a->z0;
    if (z < z0 * 16) return 0;
    x1 = a->x1;
    if (x1 * 16 < x) return 0;
    z1 = a->z1;
    if (z1 * 16 < z) return 0;
    grid = a->grid;
    if (grid == NULL) return 0;
    if (!(grid->flags & 1)) func_8000ECB8(1);
    px = x;
    g2 = grid;
    pz = z;
    if (g2->flags & 4) {
        px = x - (x1 + x0) * 8;
        pz = z - (z1 + z0) * 8;
    }
    w = (x1 - x0) * 16;
    if (w == 0) return 0;
    d = (z1 - z0) * 16;
    if (d == 0) return 0;
    i = ((z - z0 * 16) * 8 / d) * 8 + (x - x0 * 16) * 8 / w;
    verts = a->hdr->verts;
    word = i / 16;
    mask = 1 << (i % 16);
    start = D_800E8D64;
    count = g2->count;
    if (start < 0) start = 0;
    if (start >= count) start = 0;
    i = start;
    do {
        tri = &g2->tris[i];
        if (tri->bits[word] & mask) {
            v[0][0] = verts[tri->v[0]].x * 16;
            v[0][1] = verts[tri->v[1]].x * 16;
            v[0][2] = verts[tri->v[2]].x * 16;
            v[2][0] = verts[tri->v[0]].z * 16;
            dzp = pz - v[2][0];
            v[2][1] = verts[tri->v[1]].z * 16;
            dxp = px - v[0][0];
            v[2][2] = verts[tri->v[2]].z * 16;
            if ((c = dzp * (v[0][1] - v[0][0]) - dxp * (v[2][1] - v[2][0])) >= 0 &&
                (c = (pz - v[2][1]) * (v[0][2] - v[0][1]) - (px - v[0][1]) * (v[2][2] - v[2][1])) >= 0 &&
                (c = (pz - v[2][2]) * (v[0][0] - v[0][2]) - (px - v[0][2]) * (v[2][0] - v[2][2])) >= 0) {
                v[1][0] = verts[tri->v[0]].y * 16;
                v[1][1] = verts[tri->v[1]].y * 16;
                v[1][2] = verts[tri->v[2]].y * 16;
                v[0][1] -= v[0][0];
                v[0][2] -= v[0][0];
                v[1][1] -= v[1][0];
                v[1][2] -= v[1][0];
                v[2][1] -= v[2][0];
                v[2][2] -= v[2][0];
                v[0][1] >>= 4;
                v[1][1] >>= 4;
                v[2][1] >>= 4;
                v[0][2] >>= 4;
                v[2][2] >>= 4;
                v[1][2] >>= 4;
                nx = v[2][1] * v[1][2] - v[1][1] * v[2][2];
                ny = v[0][1] * v[2][2] - v[2][1] * v[0][2];
                nz = v[1][1] * v[0][2] - v[0][1] * v[1][2];
                if (ny != 0) {
                    c = -nz;
                    h = (dxp * -nx + dzp * c) / ny + v[1][0];
                    if (tri->flags & 0x400) {
                        if (h <= y + 500 && h >= D_800E80D0) {
                            D_800E80D0 = h;
                            D_800E8508 = 1;
                            D_800E8958 = id;
                            D_800E8D64 = i;
                            D_800E80D4 = h;
                            D_800E84F8 = tri->flags;
                        }
                    } else if (D_800E80D0 == -99999) {
                        D_800E8508 = 1;
                        D_800E8958 = id;
                        D_800E8D64 = i;
                        D_800E80D4 = h;
                        D_800E84F8 = tri->flags;
                        if (D_800E8500 != NULL) {
                            if (D_800E8500[id] == NULL) return 1;
                            k = 0;
                            while (D_800E8500[id][k] != -1) {
                                if (func_80031EC4(D_800E8E6C[D_800E8500[id][k]], x, z)) {
                                    D_800E84F8 = *D_800E8E6C[D_800E8500[id][k]];
                                    break;
                                }
                                k++;
                            }
                        }
                        return 1;
                    }
                }
            }
        }
        i++;
        if (i >= count) i = 0;
    } while (i != start);
    return 0;
}
