#include "game.h"

typedef struct {
    s16 x, z;
} Pt;

typedef struct {
    u16 f0, f2, f4;
    u16 n;
    Pt pts[1];
} Poly;

typedef struct { u16 n; u16 id[1]; } Cell;
typedef struct { s32 id; s32 n; s32 v[3][3]; } CTri;

typedef struct {
    u8 pad[4];
    s16 x, z;
} Ent_28;

typedef struct {
    s32 x, y, z, w, h, f14, f18;
} Node_5;

typedef struct { s16 x, z, y, h; } Vtx8;
extern Poly **sSurfacePolys;
extern Cell **sHeightGrid;
extern CTri gGroundTris[];
extern Ent_28 *sWallEdges[];
extern Node_5 sDesertObjs[];

static inline void tri(s32 id, s32 n, s32 ax, s32 az, s32 bx, s32 bz, s32 cx, s32 cz) {
    gGroundTris[gGroundTriCount].id = id;
    gGroundTris[gGroundTriCount].n = n;
    gGroundTris[gGroundTriCount].v[0][0] = ax * 2000;
    gGroundTris[gGroundTriCount].v[0][1] = gDesertHeightMap[((az & 0x1FF) << 9) | (ax & 0x1FF)] << 6;
    gGroundTris[gGroundTriCount].v[0][2] = az * 2000;
    gGroundTris[gGroundTriCount].v[1][0] = bx * 2000;
    gGroundTris[gGroundTriCount].v[1][1] = gDesertHeightMap[((bz & 0x1FF) << 9) | (bx & 0x1FF)] << 6;
    gGroundTris[gGroundTriCount].v[1][2] = bz * 2000;
    gGroundTris[gGroundTriCount].v[2][0] = cx * 2000;
    gGroundTris[gGroundTriCount].v[2][1] = gDesertHeightMap[((cz & 0x1FF) << 9) | (cx & 0x1FF)] << 6;
    gGroundTris[gGroundTriCount].v[2][2] = cz * 2000;
    gGroundTriCount++;
}

typedef struct {
    void *hdr;
    s32 x0, z0, x1, z1;
    void *grid;
    s32 f18;
    s32 f1C;
} Sub_7;

struct Area_2 {
    u32 count;
    Sub_7 subs[1];
};

typedef struct { u16 n; u16 ids[1]; } List_5;

extern List_5 **sHeightGrid_2;

void height_init(Area_2 *area, s32 b, s32 c, u32 *polys)
{
    u32 i;
    s32 n;
    s32 cell;
    s32 x, z;
    s32 j, k, total;
    u16 *out;
    Sub_7 *s;
    Area_2 *r;
    Poly *p;
    Poly **tbl;
    s16 *buf;

    sHeightAreas_2 = area;
    sWallMesh = c;
    gGroundArea = -1;
    if (area != NULL) {
        for (i = 0; i < sHeightAreas_2->count; i++) {
            s = (Sub_7 *)((u8 *)sHeightAreas_2 + (i * 32 + 4));
            if (s->x0 < sHeightAreaMin[0] || i == 0) sHeightAreaMin[0] = s->x0;
            if (s->z0 < sHeightAreaMin[1] || i == 0) sHeightAreaMin[1] = s->z0;
            if (s->x1 > sHeightAreaMax[0] || i == 0) sHeightAreaMax[0] = s->x1;
            if (s->z1 > sHeightAreaMax[1] || i == 0) sHeightAreaMax[1] = s->z1;
        }
        mem_fill((u8 *)gZBuffer_2, 0, 0x200);
        i = 0;
        total = 0;
        out = gZBuffer_2 + 0x100;
        for (k = 0; k < 16; k++) {
            for (j = 0; j < 16; j++) {
                for (n = 0; n < sHeightAreas_2->count; n++) {
                    r = (Area_2 *)((u8 *)sHeightAreas_2 + n * 32);
                    x = r->subs[0].x0 << 4;
                    x >>= 4;
                    x = ((x - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[0] - sHeightAreaMin[0]);
                    z = r->subs[0].z0 << 4;
                    z >>= 4;
                    if (x < 0) x = 0;
                    if (x >= 16) x = 15;
                    z = ((z - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[1] - sHeightAreaMin[1]);
                    if (z < 0) z = 0;
                    if (z >= 16) z = 15;
                    cell = z * 16 + x;
                    z = cell >> 4;
                    if ((cell & 0xF) > j) continue;
                    if (z > k) continue;
                    z = r->subs[0].x1 << 4;
                    z >>= 4;
                    z = ((z - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[0] - sHeightAreaMin[0]);
                    x = r->subs[0].z1 << 4;
                    x >>= 4;
                    if (z < 0) z = 0;
                    if (z >= 16) z = 15;
                    x = ((x - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[1] - sHeightAreaMin[1]);
                    if (x < 0) x = 0;
                    if (x >= 16) x = 15;
                    cell = x * 16 + z;
                    z = cell >> 4;
                    if ((cell & 0xF) < j) continue;
                    if (z < k) continue;
                    gZBuffer_2[i]++;
                    total++;
                    *out++ = n;
                }
                i++;
            }
        }
        sHeightGridData = mem_alloc_dbg((total + 0x100) * 2, 0, (s32)D_800D6C20, 0xDA);
        sHeightGrid_2 = mem_alloc_dbg(0x400, 0, (s32)D_800D6C20, 0xDB);
        n = 0;
        out = gZBuffer_2 + 0x100;
        for (i = 0; (s32)i < 0x100; i++) {
            sHeightGrid_2[i] = (List_5 *)(sHeightGridData + n);
            sHeightGrid_2[i]->n = gZBuffer_2[i];
            memcpy(sHeightGrid_2[i]->ids, out, sHeightGrid_2[i]->n * 2);
            n++;
            n += sHeightGrid_2[i]->n;
            out += sHeightGrid_2[i]->n;
        }
    } else {
        sHeightGridData = NULL;
        sHeightGrid_2 = NULL;
    }
    sSurfacePolys = NULL;
    sAreaSurfacePolys = NULL;
    if (polys != NULL) {
        sSurfacePolys = mem_alloc_dbg(*polys * 4, 0, (s32)D_800D6C20, 0xF6);
        i = 0;
        p = (Poly *)(polys + 1);
        z = *polys;
        if (z != 0) {
            do {
                sSurfacePolys[i] = p;
                p = (Poly *)((u8 *)p + (p->n * 4 + 8));
            } while (++i < *polys);
        }
        mem_fill((u8 *)gZBuffer_2, 0, sHeightAreas_2->count * 2);
        i = 0;
        out = gZBuffer_2 + sHeightAreas_2->count;
        for (; i < sHeightAreas_2->count; i++) {
            for (cell = 0; cell < *polys; cell++) {
                p = sSurfacePolys[cell];
                for (n = 0; n < p->n; n++) {
                    if (height_test_area(i, p->pts[n].x << 6, 0, p->pts[n].z << 6)) break;
                }
                if (n != p->n) {
                    *out++ = cell;
                    gZBuffer_2[i]++;
                }
            }
        }
        i = 0;
        sAreaSurfacePolys = mem_alloc_dbg(sHeightAreas_2->count * 4, 0, (s32)D_800D6C20, 0x11C);
        mem_fill((u8 *)sAreaSurfacePolys, 0, sHeightAreas_2->count * 4);
        out = gZBuffer_2 + sHeightAreas_2->count;
        for (; i < sHeightAreas_2->count; i++) {
            if (gZBuffer_2[i] != 0) {
                buf = mem_alloc_dbg((gZBuffer_2[i] + 1) * 2, 0, (s32)D_800D6C20, 0x123);
                sAreaSurfacePolys[i] = buf;
                memcpy(buf, out, gZBuffer_2[i] * 2);
                out += gZBuffer_2[i];
                sAreaSurfacePolys[i][gZBuffer_2[i]] = -1;
            }
        }
    }
}

void height_free(void)
{
    u32 i;
    if (gWallEdgeFlags != 0) {
        mem_free(gWallEdgeFlags);
        gWallEdgeFlags = 0;
    }
    if (sHeightGrid != 0) {
        mem_free(sHeightGrid);
        sHeightGrid = 0;
    }
    if (sHeightGridData != 0) {
        mem_free(sHeightGridData);
        sHeightGridData = 0;
    }
    if (sSurfacePolys != 0) {
        mem_free(sSurfacePolys);
        sSurfacePolys = 0;
    }
    if (sAreaSurfacePolys != 0) {
        i = 0;
        if (*sHeightAreas_3 != 0) {
            do {
                if (sAreaSurfacePolys[i] != 0) {
                    mem_free(sAreaSurfacePolys[i]);
                }
                i++;
            } while (i < (u32)*sHeightAreas_3);
        }
        mem_free(sAreaSurfacePolys);
        sAreaSurfacePolys = 0;
    }
}

typedef struct {
    s32 f0;
    Vtx16 *verts;
} Hdr_3;

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
    Hdr_3 *hdr;
    s32 x0, z0, x1, z1;
    Grid *grid;
    s32 f18;
} Sub_8;

s32 height_test_area(s32 id, s32 x, s32 y, s32 z)
{
    Sub_8 *a;
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
    a = (Sub_8 *)((u8 *)sHeightAreas + (id * 32 + 4));
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
    if (!(grid->flags & 1)) fatal_halt(1);
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
    start = gGroundTri;
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
                        if (h <= y + 500 && h >= sGroundBestHeight) {
                            sGroundBestHeight = h;
                            gGroundFound = 1;
                            gGroundArea = id;
                            gGroundTri = i;
                            gGroundHeight = h;
                            gGroundSurface = tri->flags;
                        }
                    } else if (sGroundBestHeight == -99999) {
                        gGroundFound = 1;
                        gGroundArea = id;
                        gGroundTri = i;
                        gGroundHeight = h;
                        gGroundSurface = tri->flags;
                        if (sAreaSurfacePolys != NULL) {
                            if (sAreaSurfacePolys[id] == NULL) return 1;
                            k = 0;
                            while (sAreaSurfacePolys[id][k] != -1) {
                                if (point_in_poly_xz(sSurfacePolys_2[sAreaSurfacePolys[id][k]], x, z)) {
                                    gGroundSurface = *sSurfacePolys_2[sAreaSurfacePolys[id][k]];
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

s32 height_get_ground(s32 x, s32 y, s32 z)
{
    s32 ix, iz, i, k;

    sGroundBestHeight = -99999;
    gGroundFound = 0;
    gGroundSurface = 0;
    D_800E8938++;
    if (gCurTrack_5->mode == 2) {
        return height_get_desert(x, z);
    }
    if (gGroundArea != -1 && height_test_area(gGroundArea, x, y, z) != 0) {
        return gGroundHeight;
    }
    ix = x >> 4;
    ix = (ix - sHeightAreaMin_2.a) * 16 / (sHeightAreaMax_2.a - sHeightAreaMin_2.a);
    iz = z >> 4;
    if (ix < 0) ix = 0;
    if (ix > 15) ix = 15;
    iz = (iz - sHeightAreaMin_2.a) * 16 / (sHeightAreaMax_2.b - sHeightAreaMin_2.b);
    if (iz < 0) iz = 0;
    if (iz > 15) iz = 15;
    k = iz * 16 + ix;
    for (i = 0; i < sHeightGrid[k]->n; i++) {
        if (sHeightGrid[k]->id[i] != gGroundArea && height_test_area(sHeightGrid[k]->id[i], x, y, z) != 0) {
            return gGroundHeight;
        }
    }
    if (sGroundBestHeight != -99999) {
        return sGroundBestHeight;
    }
    return gGroundHeight;
}

s32 height_get_desert(s32 x, s32 z)
{
    struct {
        s32 q;
        s32 rx;
        s32 rz;
    } L;
    s32 *qp;
    s32 ix, lt, v, fx, fz, h0, h1, h2;

    qp = &L.q;
    gGroundFound = 1;
    divmod_floor(x, 2000, qp, &L.rx);
    ix = L.q;
    divmod_floor(z, 2000, qp, &L.rz);
    z = L.q;
    gGroundArea = ((ix >> 2) & 0xffff) + ((z >> 2) << 16);
    v = ((ix & 3) + (z & 3) * 4) * 2;
    gGroundTri = v;
    if (L.rx < 1999 - L.rz) {
        gGroundTri = v + 1;
    }
    ix &= 0x1ff;
    z &= 0x1ff;
    fx = L.rx;
    fz = L.rz;
    if (fx < 1999 - fz) {
        h0 = gDesertHeightMap[z * 0x200 + ix];
        h1 = gDesertHeightMap[((z + 1) & 0x1ff) * 0x200 + ix];
        h2 = gDesertHeightMap[z * 0x200 + ((ix + 1) & 0x1ff)];
        h0 <<= 6;
        h1 <<= 6;
        h2 <<= 6;
        L.q = h0 + ((h1 - h0) * fz + (h2 - h0) * fx) / 2000;
    } else {
        h0 = gDesertHeightMap[((z + 1) & 0x1ff) * 0x200 + ((ix + 1) & 0x1ff)];
        h1 = gDesertHeightMap[((z + 1) & 0x1ff) * 0x200 + ix];
        h2 = gDesertHeightMap[z * 0x200 + ((ix + 1) & 0x1ff)];
        h0 <<= 6;
        h1 <<= 6;
        h2 <<= 6;
        L.q = h0 + ((h1 - h0) * (2000 - L.rx) + (h2 - h0) * (2000 - L.rz)) / 2000;
    }
    return L.q;
}

float height_get_desert_f(float x, float z)
{
    s32 ix, iz, h0, h1, h2;
    float rz, f0, f2;

    x *= 0.5f;
    z *= 0.5f;
    ix = x;
    gGroundFound = 1;
    if (ix < 0) {
        ix--;
    }
    iz = z;
    x -= ix;
    if (iz < 0) {
        iz--;
    }
    gGroundArea = ((ix >> 2) & 0xffff) + ((iz >> 2) << 16);
    z -= iz;
    rz = 1.0f - z;
    gGroundTri = ((ix & 3) + (iz & 3) * 4) * 2;
    if (x < rz) {
        gGroundTri = gGroundTri + 1;
    }
    ix &= 0x1ff;
    iz &= 0x1ff;
    if (x < rz) {
        float f1;
        h0 = gDesertHeightMap[iz * 0x200 + ix];
        h1 = gDesertHeightMap[((iz + 1) & 0x1ff) * 0x200 + ix];
        h0 <<= 6;
        h1 <<= 6;
        f0 = h0;
        f1 = h1;
        f1 -= f0;
        f2 = gDesertHeightMap[iz * 0x200 + ((ix + 1) & 0x1ff)] << 6;
        return (f0 + f1 * z + (f2 - f0) * x) / 1000.0f;
    } else {
        float f1, d;
        h0 = gDesertHeightMap[((iz + 1) & 0x1ff) * 0x200 + ((ix + 1) & 0x1ff)];
        h1 = gDesertHeightMap[((iz + 1) & 0x1ff) * 0x200 + ix];
        h0 <<= 6;
        h1 <<= 6;
        f0 = h0;
        f1 = h1;
        d = f1 - f0;
        f1 = 1.0f - x;
        f2 = gDesertHeightMap[iz * 0x200 + ((ix + 1) & 0x1ff)] << 6;
        return (f0 + d * f1 + (f2 - f0) * rz) / 1000.0f;
    }
}

typedef struct { u8 pad[8]; u16 v[3]; u16 pad2; } Tri3;
typedef struct { u16 flags; u16 pad; Tri3 t[1]; } Shape;
typedef struct { u32 pad; Vtx_t *vtx; } Mesh_3;
typedef struct { Mesh_3 *mesh; s32 x0, z0, x1, z1; Shape *shape; u32 pad[2]; } Ent_26;
typedef struct { u32 n; Ent_26 e[1]; } EntList;

extern EntList *sHeightAreas_4;

void height_get_ground_color(s32 x, s32 z, s32 *out)
{
    s32 m[3][4];
    Ent_26 *e;
    Shape *s;
    Tri3 *t;
    Vtx_t *vb;
    s32 ee, k, q, tt, a, a2, b, d, num, sx, sz, px, pz;

    if (gGroundArea < 0 || gGroundArea >= sHeightAreas_4->n) return;
    e = &sHeightAreas_4->e[gGroundArea];
    s = e->shape;
    if (!(s->flags & 1)) return;
    t = &s->t[gGroundTri];
    sx = (e->x1 + e->x0) * 8;
    sz = (e->z1 + e->z0) * 8;
    if (s->flags & 4) {
        px = x - sx;
        pz = z - sz;
    } else {
        px = x;
        pz = z;
    }
    vb = e->mesh->vtx;
    m[0][0] = vb[t->v[0]].ob[0];
    m[0][1] = vb[t->v[1]].ob[0];
    m[0][2] = vb[t->v[2]].ob[0];
    m[2][0] = vb[t->v[0]].ob[2];
    m[2][1] = vb[t->v[1]].ob[2];
    m[2][2] = vb[t->v[2]].ob[2];
    m[0][1] -= m[0][0];
    m[2][1] -= m[2][0];
    m[0][2] -= m[0][0];
    m[2][2] -= m[2][0];
    m[0][0] <<= 4;
    m[2][0] <<= 4;
    for (k = 0; k < 3; k++) {
        m[1][0] = vb[t->v[0]].cn[k];
        m[1][1] = vb[t->v[1]].cn[k];
        m[1][2] = vb[t->v[2]].cn[k];
        m[1][1] -= m[1][0];
        m[1][2] -= m[1][0];
        tt = m[2][1] * m[1][2];
        a = m[1][1] * m[2][2] - tt;
        b = px - m[0][0];
        q = b * a;
        tt = m[1][1] * m[0][2];
        a2 = m[0][1] * m[1][2] - tt;
        d = m[0][1] * m[2][2];
        q += (pz - m[2][0]) * a2;
        ee = m[2][1] * m[0][2];
        q = q / (d - ee);
        q /= 16; q += m[1][0];
        if (q < 0) q = 0;
        if (q > 255) q = 255;
        out[k] = q;
    }
}

#define IDX(a, b) (((((a) & 0x1FF)) << 9) | ((b) & 0x1FF))

typedef struct { u8 pad[8]; u16 vi[3]; u16 pad2; } MTri;
typedef struct { u16 flags; u16 pad; MTri tri[1]; } Mesh_4;
typedef struct { u8 pad[4]; Vtx16 *verts; } Model_3;
typedef struct { Model_3 *model; s32 a, b, c, d; Mesh_4 *mesh; } Obj_23;

s32 height_get_tri_verts(s32 p1, s32 p2, Vec3i *v)
{
    s32 n, q, u12, u13, lo4;
    Obj_23 *o;
    Mesh_4 *m;
    Vtx16 *vt;
    s32 k;
    u32 f; s32 j;
    u16 *pb, *pv;

    if (*(s32 *)(gCurTrack + 0x2c) == 2) {
        n = p2 / 2;
        lo4 = (p1 << 16) >> 14;
        q = n / 4;
        u12 = lo4 + (n - q * 4);
        u13 = (p1 >> 16) * 4 + q;
        if (p2 & 1) {
            v[0].x = u12 * 2000;
            v[0].z = u13 * 2000;
            v[0].y = gDesertHeightMap[IDX(u13, u12)] << 6;
            v[1].x = (u12 + 1) * 2000;
            v[1].z = u13 * 2000;
            v[1].y = gDesertHeightMap[IDX(u13, u12 + 1)] << 6;
            v[2].x = u12 * 2000;
            v[2].z = (u13 + 1) * 2000;
            v[2].y = gDesertHeightMap[IDX(u13 + 1, u12)] << 6;
        } else {
            v[0].x = (u12 + 1) * 2000;
            v[0].z = (u13 + 1) * 2000;
            v[0].y = gDesertHeightMap[IDX(u13 + 1, u12 + 1)] << 6;
            v[1].x = u12 * 2000;
            v[1].z = (u13 + 1) * 2000;
            v[1].y = gDesertHeightMap[IDX(u13 + 1, u12)] << 6;
            v[2].x = (u12 + 1) * 2000;
            v[2].z = u13 * 2000;
            v[2].y = gDesertHeightMap[IDX(u13, u12 + 1)] << 6;
        }
        return 0;
    }

    o = (Obj_23 *)(sHeightAreas + (p1 * 0x20 + 4));
    m = o->mesh;
    f = m->flags;
    if (!(f & 1)) {
        goto hang;
    }
    pb = (u16 *)m;
    vt = o->model->verts;
    if (f & 4) {
        u12 = (o->c + o->a) * 8;
        n = (o->d + o->b) * 8;
    } else {
        n = 0;
        u12 = 0;
    }
    pv = pb + (p2 * 8 + 2);
    for (p1 = 0; p1 < 3; p1++) {
        v[p1].x = vt[pv[p1 + 4]].x * 16 + u12;
        v[p1].y = vt[pv[p1 + 4]].y * 16;
        v[p1].z = vt[pv[p1 + 4]].z * 16 + n;
    }
    return 0;
hang:
    for (;;) {
    }
}

void height_get_ground_normal(f32 *param_1)
{
    s32 v[9];
    f32 len;
    height_get_tri_verts(gGroundArea, gGroundTri, v);
    v[3] -= v[0];
    v[6] -= v[0];
    v[4] -= v[1];
    v[5] -= v[2];
    v[7] -= v[1];
    v[8] -= v[2];
    param_1[0] = (f32)v[5] * (f32)v[7] - (f32)v[4] * (f32)v[8];
    param_1[1] = (f32)v[3] * (f32)v[8] - (f32)v[5] * (f32)v[6];
    param_1[2] = (f32)v[4] * (f32)v[6] - (f32)v[3] * (f32)v[7];
    len = param_1[0] * param_1[0] + param_1[1] * param_1[1] + param_1[2] * param_1[2];
    len = sqrtf(len);
    param_1[0] = param_1[0] / len;
    param_1[1] = param_1[1] / len;
    param_1[2] = param_1[2] / len;
}

typedef struct { s32 pad; Vtx16 *v; } Mesh_5;
typedef struct { u32 m0, m1; u16 v[3]; u16 flags; } Ent_27;
typedef struct { u16 flags; u16 n; Ent_27 e[1]; } TList;
typedef struct { Mesh_5 *m; s32 x0, z0, x1, z1; TList *tl; u8 pad[8]; } Obj_24;

void height_collect_ground_tris(s32 x0, s32 z0, s32 x1, s32 z1, s32 hy) {

    s32 r[4];
    union { u16 h[4]; u32 w[2]; } mask;
    s32 code[3];
    s32 idx;
    s32 hmin;
    s32 x, z, gx, gz, cx, cz, cell, k, j, n;
    s32 ox, oz, dx, dz;
    s32 bx0, bz0, bx1, bz1, bit;
    u16 f;
    Obj_24 *o;
    TList *tl;
    Ent_27 *en;
    Vtx16 *vt;
    u16 *vi;
    u16 *mp;
    CTri *t;
    s32 *p;

    x0 >>= 4;
    x1 >>= 4;
    z0 >>= 4;
    z1 >>= 4;
    gGroundTriCount = 0;
    hy >>= 4;
    if (gCurTrack_4->f2C == 2) {
        divmod_floor(x0, 125, &r[0], &idx);
        divmod_floor(z0, 125, &r[2], &idx);
        divmod_floor(x1 + 124, 125, &r[1], &idx);
        divmod_floor(z1 + 124, 125, &r[3], &idx);
        for (k = r[2]; k < r[3]; k++) {
            for (n = r[0]; n < r[1]; n++) {
                tri(idx, n * 2 + k * 8, n, k, n + 1, k, n + 1, k + 1);
                if (gGroundTriCount == 24) return;
                tri(idx, n * 2 + k * 8, n, k, n + 1, k + 1, n, k + 1);
                if (gGroundTriCount == 24) return;
            }
        }
        return;
    }
    gx = (x0 + x1) >> 1;
    gz = (z0 + z1) >> 1;
    gx = ((gx - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[0] - sHeightAreaMin[0]);
    if (gx < 0) gx = 0;
    if (gx >= 16) gx = 15;
    gz = ((gz - sHeightAreaMin[0]) << 4) / (sHeightAreaMax[1] - sHeightAreaMin[1]);
    if (gz < 0) gz = 0;
    if (gz >= 16) gz = 15;
    cell = gz * 16 + gx;
    for (idx = 0; idx < sHeightGrid[cell]->n; idx++) {
        o = (Obj_24 *)(sHeightAreas + ((sHeightGrid[cell]->id[idx] << 5) + 4));
        if (x1 < o->x0) continue;
        if (z1 < o->z0) continue;
        if (o->x1 < x0) continue;
        if (o->z1 < z0) continue;
        if (!(o->tl->flags & 1)) fatal_halt(1);
        tl = o->tl;
        if (tl->flags & 4) {
            cx = (o->x1 + o->x0) / 2;
            cz = (o->z1 + o->z0) / 2;
        } else {
            cx = cz = 0;
        }
        r[0] = x0 - cx;
        r[1] = x1 - cx;
        r[2] = z0 - cz;
        r[3] = z1 - cz;
        dx = o->x1 - o->x0;
        ox = o->x0;
        bx0 = (x0 - ox) * 8 / dx;
        oz = o->z0;
        dz = o->z1 - oz;
        if (bx0 >= 8) bx0 = 7;
        if (bx0 < 0) bx0 = 0;
        bz0 = (z0 - oz) * 8 / dz;
        if (bz0 >= 8) bz0 = 7;
        if (bz0 < 0) bz0 = 0;
        bx1 = (x1 - ox) * 8 / dx;
        if (bx1 >= 8) bx1 = 7;
        if (bx1 < 0) bx1 = 0;
        bz1 = (z1 - oz) * 8 / dz;
        if (bz1 >= 8) bz1 = 7;
        if (bz1 < 0) bz1 = 0;
        mp = mask.h;
        for (n = bz0; n <= bz1; n++) {
            for (k = bx0; k <= bx1; k++) {
                bit = n * 8 + k;
                mp[bit / 16] |= 1 << (bit % 16);
            }
        }
        for (n = 0; n < tl->n; n++) {
            en = &tl->e[n];
            if (!((en->m0 & mask.w[0]) || (en->m1 & mask.w[1]))) continue;
            if (en->flags & 0x100) continue;
            vi = en->v;
            vt = o->m->v;
            for (k = 0; k < 3; k++) {
                code[k] = 0;
                if (vt[vi[k]].x < r[0]) code[k] = 1;
                if (r[1] < vt[vi[k]].x) code[k] |= 2;
                if (vt[vi[k]].z < r[2]) code[k] |= 4;
                if (r[3] < vt[vi[k]].z) code[k] |= 8;
            }
            if (code[0] & code[1] & code[2]) continue;
            if (en->flags & 0x400) {
                for (k = 0; k < 3; k++) {
                    if (!(hy < vt[vi[k]].y)) break;
                }
                if (k == 3) continue;
                for (k = 0; k < 3; k++) {
                    if (hy - 250 < vt[vi[k]].y) break;
                }
                if (k == 3) continue;
            }
            { CTri *b = gGroundTris; t = &b[gGroundTriCount]; }
            t->n = n;
            t->id = idx;
            for (k = 0; k < 3; k++) {
                gGroundTris[gGroundTriCount].v[k][0] = (cx + vt[vi[k]].x) * 16;
                gGroundTris[gGroundTriCount].v[k][1] = vt[vi[k]].y << 4;
                gGroundTris[gGroundTriCount].v[k][2] = (cz + vt[vi[k]].z) * 16;
            }
            if (++gGroundTriCount == 24) return;
        }
    }
}

typedef struct {
    u8 pad[0x1C];
    u8 *base;
} Hdr_4;

extern Hdr_4 *sWallMesh_2;

void height_push_off_wall(f32 *pos)
{
    s32 i;
    Ent_28 *e;
    f32 f;

    height_gather_walls((pos[0] - 1.0f) * 1000.0f, (pos[2] - 1.0f) * 1000.0f,
                  (pos[0] + 1.0f) * 1000.0f, (pos[2] + 1.0f) * 1000.0f);
    for (i = 0; i < sWallEdgeCount; i++) {
        e = sWallEdges[i];
        if (!(gWallEdgeFlags[((u8 *)e - sWallMesh_2->base) >> 3] & 0x20)) {
            f = __builtin_sqrtf(e->x * e->x + e->z * e->z);
            pos[0] += e->x / f;
            pos[2] += e->z / f;
            return;
        }
    }
}

typedef struct {
    s16 x, y, z;
} Face;

extern Face sDesertBoxNormals[];

s32 height_raycast_desert_objs(f32 *p1, f32 *p2, f32 *p3, f32 *p4)
{
    Face *bestFace;
    s32 best;
    s32 ax, ay, az, bx, by, bz;
    s32 n, i;
    s32 ex, ey, ez;

    ax = (s32)(p1[0] * 1000.0f);
    ay = (s32)(p1[1] * 1000.0f);
    az = (s32)(p1[2] * 1000.0f);
    bx = (s32)(p2[0] * 1000.0f);
    by = (s32)(p2[1] * 1000.0f);
    bz = (s32)(p2[2] * 1000.0f);
    best = 0;
    bestFace = 0;
    for (n = 0; n < sDesertObjCount; n++) {
        Node_5 *nd = &sDesertObjs[n];
        for (i = 0; i < 5; i++) {
            Face *f = &sDesertBoxNormals[i];
            s32 hw = nd->w;
            s32 hh = nd->h;
            s32 nx = nd->x;
            s32 ny = nd->y;
            s32 nz = nd->z;
            s32 px, py, pz;
            s32 d1, d2, dd;
            f32 t;
            s32 dx;
            px = ((hw * f->x) >> 14) + nx;
            py = ((hh * f->y) >> 14) + ny;
            pz = ((hw * f->z) >> 14) + nz;

            d1 = (bx - px) * f->x + (by - py) * f->y + (bz - pz) * f->z;
            if (d1 >= best) continue;
            pz = (ax - px) * f->x + (ay - py) * f->y + (az - pz) * f->z;
            if (pz < 0) continue;
            py = d1 - pz;
            if (py == 0) continue;
            dx = bx - ax;
            t = -(f32)pz / (f32)py;
            px = (f32)ax + t * (f32)dx;
            dx = by - ay;
            py = (f32)ay + t * (f32)dx;
            dx = bz - az;
            pz = (f32)az + t * (f32)dx;
            if (i < 2) {
                if (ny + hh < py || pz < nz - hw || nz + hw < pz) continue;
            } else if (i < 4) {
                if (ny + hh < py || px < nx - hw || nx + hw < px) continue;
            } else {
                if (px < nx - hw || nx + hw < px || pz < nz - hw || nz + hw < pz) continue;
            }
            bestFace = &sDesertBoxNormals[i];
            best = d1;
        }
    }
    if (bestFace) {
        *p4 = -((f32)best / 16384000.0f);
        p3[0] = bestFace->x * 6.1035156e-05f;
        p3[1] = bestFace->y * 6.1035156e-05f;
        p3[2] = bestFace->z * 6.1035156e-05f;
        return 1;
    }
    return 0;
}

void height_push_out_of_desert_objs(f32 *p)
{
    s32 nx, ny, i, dx, dy, ax, d2, lim;
    f32 r;

    nx = (s32)(p[0] * 1000.0f);
    ny = (s32)(p[2] * 1000.0f);
    height_gather_walls(nx - 0x1000, ny - 0x1000, nx + 0x1000, ny + 0x1000);
    if (sDesertObjCount > 0) {
    i = 0;
    lim = 0x900000;
    do {
        dx = sDesertObjs[i].x - nx;
        if (abs(dx) < 0xc01) {
            dy = sDesertObjs[i].z - ny;
            if (abs(dy) < 0xc01) {
                d2 = dx * dx + dy * dy;
                if (d2 <= lim) {
                    r = sqrtf((f32)d2);
                    nx = (s32)((f32)sDesertObjs[i].x - (f32)(dx * 0xc00) / r);
                    ny = (s32)((f32)sDesertObjs[i].z - (f32)(dy * 0xc00) / r);
                }
            }
        }
    } while (++i < sDesertObjCount);
    }
    p[0] = (f32)nx / 1000.0f;
    p[2] = (f32)ny / 1000.0f;
}

typedef struct { s16 a, b, nx, nz; } Edge;
typedef struct { s16 a, b, c, nx, ny, nz; } Tri12;
typedef struct { u8 pad[0x14]; Vtx8 *verts; u8 pad2[4]; Edge *edges; } Track_8;

extern Track_8 *sWallMesh_3;
extern Tri12 *sWallTris[];
extern Edge *sWallHitEdge;

u8 height_raycast_walls(f32 *start, f32 *end, f32 *out, f32 *tmax, s32 wantFlag)
{
    u8 result = 0;
    f32 sx, sy, sz, ex, ey, ez;
    s32 i;
    f32 t;
    f32 vx, vy, vz;
    f32 d1, d0, px;
    Vtx8 *a, *b;
    s32 iax, iaz, ibx, ibz;
    f32 unused[3];

    if (gCurTrack_5->mode == 2) {
        return height_raycast_desert_objs(start, end, out, tmax);
    }
    if (sWallMesh_3 == NULL) {
        return 0;
    }
    sx = start[0] * 1000.0f;
    sy = start[1] * 1000.0f;
    sz = start[2] * 1000.0f;
    ex = end[0] * 1000.0f;
    ey = end[1] * 1000.0f;
    ez = end[2] * 1000.0f;
    gWallHitFlags = 0;
    if (sWallEdgeCount != 0) {
        for (i = 0; i < sWallEdgeCount; i++) {
            Edge *e = sWallEdges[i];
            u8 fl = gWallEdgeFlags[(e - sWallMesh_3->edges)];
            f32 ax, az, fax, faz, dz, dx, py, y0, y1, len;

            if (fl & 0x20) continue;
            if (wantFlag && !(fl & 0x40)) continue;
            a = &sWallMesh_3->verts[e->a];
            iax = a->x;
            fax = iax;
            iaz = a->z;
            faz = iaz;
            ax = fax * 64.0f;
            az = faz * 64.0f;
            b = &sWallMesh_3->verts[e->b];
            ibz = b->z;
            dz = ibz - iaz;
            ibx = b->x;
            dx = iax - ibx;
            vx = ex - ax;
            vz = ez - az;
            d1 = vx * dz + vz * dx;
            if (d1 >= 0.0f) continue;
            vx = sx - ax;
            vz = sz - az;
            d0 = vx * dz + vz * dx;
            if (d0 < 0.0f) continue;
            t = d0 / (d0 - d1);
            if (*tmax < t) continue;
            px = start[0] + (end[0] - start[0]) * t;
            py = start[1] + (end[1] - start[1]) * t;
            vz = start[2] + (end[2] - start[2]) * t;
            px *= 15.625f;
            py *= 1000.0f;
            vz *= 15.625f;
            if (__builtin_abs(e->nz) > __builtin_abs(e->nx)) {
                if (px < fax && px < ibx) continue;
                if (fax < px && ibx < px) continue;
                vz = (px - fax) / (ibx - fax);
            } else {
                if (vz < faz && vz < ibz) continue;
                if (faz < vz && ibz < vz) continue;
                vz = (vz - faz) / (ibz - faz);
            }
            y0 = a->y + vz * (b->y - a->y);
            y0 *= 16.0f;
            y1 = a->h + vz * (b->h - a->h);
            y1 *= 16.0f;
            if (py < y0) continue;
            if (y0 + y1 <= py) continue;
            *tmax = t;
            len = __builtin_sqrtf(dz * dz + dx * dx);
            out[0] = dz / len;
            out[1] = 0.0f;
            out[2] = dx / len;
            result = 1;
            sWallHitEdge = e;
            gWallHitFlags = gWallEdgeFlags[(e - sWallMesh_3->edges)];
            D_800E8E78 = (gWallHitFlags >> 1) & 3;
        }
    }
    if (sWallTriCount != 0) {
        for (i = 0; i < sWallTriCount; i++) {
            Tri12 *tr = sWallTris[i];
            Vtx8 *v = sWallMesh_3->verts;
            f32 fax, faz, ax, ay, az, nx, ny, nz, c, len;
            Vtx8 *cc;
            s32 pos, neg, jz;
            a = &v[tr->a];

            ibx = a->x;
            fax = ibx;
            ax = fax * 64.0f;
            ay = (a->y + a->h) * 16.0f;
            jz = a->z;
            faz = jz;
            az = faz * 64.0f;
            nx = tr->nx;
            ny = tr->ny;
            nz = tr->nz;
            b = &v[tr->b];
            vx = ex - ax;
            vy = ey - ay;
            vz = ez - az;
            d1 = vx * nx + vy * ny + vz * nz;
            cc = &v[tr->c];
            if (d1 >= 0.0f) continue;
            vx = sx - ax;
            vy = sy - ay;
            vz = sz - az;
            d0 = vx * nx + vy * ny + vz * nz;
            if (d0 < 0.0f) continue;
            t = d0 / (d0 - d1);
            if (*tmax < t) continue;
            pos = neg = 0;
            px = (sx + (ex - sx) * t) * 0.015625f;
            vz = (sz + (ez - sz) * t) * 0.015625f;
            c = (b->x - ibx) * (vz - faz) - (b->z - jz) * (px - fax);
            if (c > 0.0f) pos++; else if (c < 0.0f) neg++;
            c = (cc->x - b->x) * (vz - b->z) - (cc->z - b->z) * (px - b->x);
            if (c > 0.0f) pos++; else if (c < 0.0f) neg++;
            c = (a->x - cc->x) * (vz - cc->z) - (a->z - cc->z) * (px - cc->x);
            if (c > 0.0f) pos++; else if (c < 0.0f) neg++;
            if (pos == 3 || neg == 3) {
                len = __builtin_sqrtf(tr->nx * tr->nx + tr->ny * tr->ny + tr->nz * tr->nz);
                out[0] = tr->nx / len;
                out[1] = tr->ny / len;
                out[2] = tr->nz / len;
                result = 1;
                *tmax = t;
            }
        }
    }
    return result;
}

typedef struct {
    s32 x, y, z, r, a, b, c;
} Ent_30;

typedef struct {
    s32 cx;             /* 0x00 */
    s32 cz;             /* 0x04 */
    s32 f8, fC;
    s32 off;            /* 0x10 */
    u8 pad14[0x320 - 0x14];
    s32 count;          /* 0x320 */
    Ent_30 e[8];           /* 0x324 */
} Tile;

typedef struct {
    s16 v0, v1;
    s16 f4;
    s16 flags;          /* 0x06 */
} CEdge;

typedef struct {
    s16 v[3];
    s16 f6, f8;
    s16 flags;          /* 0x0A */
} CTri_2;

typedef struct {
    u16 *tris;
    u16 *edges;
} CCell;

typedef struct {
    s32 minx, minz, maxx, maxz;
    s32 f10;
    Vtx8 *verts;        /* 0x14 */
    s32 f18;
    CEdge *edges;       /* 0x1C */
    s32 f20;
    CTri_2 *tris;         /* 0x24 */
    CCell cells[257];   /* 0x28 */
} Grid_2;

extern Tile *gDesertTiles_2;
extern Ent_30 sDesertObjs_2[];
extern Grid_2 *sWallMesh_4;
extern CEdge *sWallEdges_2[];
extern CTri_2 *sWallTris_2[];

void height_gather_walls(s32 x0, s32 z0, s32 x1, s32 z1)
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
    CTri_2 *t;
    Vtx8 *a, *b;

    sDesertObjCount = 0;
    if (gCurTrack_5->mode == 2) {
        sDesertObjCount = 0;
        for (i = 0; i < 16; i++) {
            if (gDesertTiles_2[i].off != 0) continue;
            if (x1 < gDesertTiles_2[i].cx * 256000) continue;
            if (z1 < gDesertTiles_2[i].cz * 256000) continue;
            if ((gDesertTiles_2[i].cx + 1) * 256000 < x0) continue;
            if ((gDesertTiles_2[i].cz + 1) * 256000 < z0) continue;
            for (j = 0; j < gDesertTiles_2[i].count; j++) {
                if (x1 >= gDesertTiles_2[i].e[j].x - gDesertTiles_2[i].e[j].r
                    && z1 >= gDesertTiles_2[i].e[j].z - gDesertTiles_2[i].e[j].r
                    && gDesertTiles_2[i].e[j].x + gDesertTiles_2[i].e[j].r >= x0
                    && gDesertTiles_2[i].e[j].z + gDesertTiles_2[i].e[j].r >= z0) {
                    sDesertObjs_2[sDesertObjCount] = gDesertTiles_2[i].e[j];
                    sDesertObjCount++;
                    if (sDesertObjCount == 8) return;
                }
            }
            if (sDesertObjCount == 8) return;
        }
    } else if (sWallMesh_4 != NULL) {
        sWallEdgeCount = 0;
        sWallTriCount = 0;
        ix0 = ((x0 / 16 - sWallMesh_4->minx) * 16) / (sWallMesh_4->maxx - sWallMesh_4->minx);
        iz0 = ((z0 / 16 - sWallMesh_4->minz) * 16) / (sWallMesh_4->maxz - sWallMesh_4->minz);
        ix1 = ((x1 / 16 - sWallMesh_4->minx) * 16) / (sWallMesh_4->maxx - sWallMesh_4->minx);
        iz1 = ((z1 / 16 - sWallMesh_4->minz) * 16) / (sWallMesh_4->maxz - sWallMesh_4->minz);
        if (ix0 > 16 || iz0 > 16 || ix1 < 0 || iz1 < 0) return;
        if (ix0 < 0) ix0 = 0;
        if (ix0 > 15) ix0 = 15;
        if (iz0 < 0) iz0 = 0;
        if (iz0 > 15) iz0 = 15;
        if (ix1 > 15) ix1 = 15;
        if (iz1 > 15) iz1 = 15;
        for (i = ix0; i <= ix1; i++) {
            for (j = iz0; j <= iz1; j++) {
                cell = &sWallMesh_4->cells[j * 16 + i];
                n = (u32)sWallMesh_4->cells[j * 16 + i + 1].edges - (u32)cell->edges;
                n = (u32)n >> 1;
                for (k = 0; k < n; k++) {
                    if (sWallEdgeCount >= 0x100) break;
                    e = &sWallMesh_4->edges[cell->edges[k]];
                    if (e->flags & 1) continue;
                    a = &sWallMesh_4->verts[e->v0];
                    b = &sWallMesh_4->verts[e->v1];
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
                            sWallEdges_2[sWallEdgeCount] = e;
                            sWallEdgeCount++;
                            e->flags |= 1;
                        }
                    }
                }
                cell = &sWallMesh_4->cells[j * 16 + i];
                n = (u32)sWallMesh_4->cells[j * 16 + i + 1].tris - (u32)cell->tris;
                n = (u32)n >> 1;
                for (k = 0; k < n + ((n & 1) >> 1); k++) {
                    if (sWallTriCount >= 0x100) break;
                    t = &sWallMesh_4->tris[cell->tris[k]];
                    if (t->flags & 1) continue;
                    mask = 0xF;
                    for (m = 0; m < 3; m++) {
                        a = &sWallMesh_4->verts[t->v[m]];
                        v = a->x << 6;
                        if (v >= x0) mask &= ~1;
                        if (x1 >= v) mask &= ~2;
                        v = a->z << 6;
                        if (v >= z0) mask &= ~4;
                        if (z1 >= v) mask &= ~8;
                    }
                    if (mask == 0) {
                        sWallTris_2[sWallTriCount++] = t;
                        t->flags |= 1;
                    }
                }
            }
        }
        for (i = 0; i < sWallEdgeCount; i++) {
            sWallEdges_2[i]->flags &= ~1;
        }
        for (i = 0; i < sWallTriCount; i++) {
            sWallTris_2[i]->flags &= ~1;
        }
        if (gDrawFlags & 0x200) {
            for (i = 0; i < sWallEdgeCount; i++) {
                px[0] = sWallMesh_4->verts[sWallEdges_2[i]->v0].x << 6;
                px[1] = sWallMesh_4->verts[sWallEdges_2[i]->v0].x << 6;
                px[2] = sWallMesh_4->verts[sWallEdges_2[i]->v1].x << 6;
                pz[0] = sWallMesh_4->verts[sWallEdges_2[i]->v0].z << 6;
                pz[1] = sWallMesh_4->verts[sWallEdges_2[i]->v0].z << 6;
                pz[2] = sWallMesh_4->verts[sWallEdges_2[i]->v1].z << 6;
                height_get_ground(px[0], 10000, pz[0]);
                height_get_ground(px[2], 10000, pz[2]);
                py[0] = sWallMesh_4->verts[sWallEdges_2[i]->v0].y * 16;
                py[1] = (sWallMesh_4->verts[sWallEdges_2[i]->v0].y + sWallMesh_4->verts[sWallEdges_2[i]->v0].h) * 16;
                py[2] = sWallMesh_4->verts[sWallEdges_2[i]->v1].y * 16;
                px[0] = sWallMesh_4->verts[sWallEdges_2[i]->v0].x << 6;
                px[1] = sWallMesh_4->verts[sWallEdges_2[i]->v1].x << 6;
                px[2] = sWallMesh_4->verts[sWallEdges_2[i]->v1].x << 6;
                pz[0] = sWallMesh_4->verts[sWallEdges_2[i]->v0].z << 6;
                pz[1] = sWallMesh_4->verts[sWallEdges_2[i]->v1].z << 6;
                pz[2] = sWallMesh_4->verts[sWallEdges_2[i]->v1].z << 6;
                py[0] = (sWallMesh_4->verts[sWallEdges_2[i]->v0].y + sWallMesh_4->verts[sWallEdges_2[i]->v0].h) * 16;
                py[1] = (sWallMesh_4->verts[sWallEdges_2[i]->v1].y + sWallMesh_4->verts[sWallEdges_2[i]->v1].h) * 16;
            }
            for (i = 0; i < sWallTriCount; i++) {
                px[0] = sWallMesh_4->verts[sWallTris_2[i]->v[0]].x << 6;
                px[1] = sWallMesh_4->verts[sWallTris_2[i]->v[1]].x << 6;
                px[2] = sWallMesh_4->verts[sWallTris_2[i]->v[2]].x << 6;
                py[0] = (sWallMesh_4->verts[sWallTris_2[i]->v[0]].y + sWallMesh_4->verts[sWallTris_2[i]->v[0]].h) * 16;
                py[1] = (sWallMesh_4->verts[sWallTris_2[i]->v[1]].y + sWallMesh_4->verts[sWallTris_2[i]->v[1]].h) * 16;
                py[2] = (sWallMesh_4->verts[sWallTris_2[i]->v[2]].y + sWallMesh_4->verts[sWallTris_2[i]->v[2]].h) * 16;
                qz[0] = sWallMesh_4->verts[sWallTris_2[i]->v[0]].z << 6;
                qz[1] = sWallMesh_4->verts[sWallTris_2[i]->v[1]].z << 6;
                qz[2] = sWallMesh_4->verts[sWallTris_2[i]->v[2]].z << 6;
            }
        }
    }
}

typedef struct {
    s16 x;
    s16 y;
    s32 pad;
} Pt_2;

typedef struct {
    s16 a;
    s16 b;
    s16 nx;
    s16 ny;
} Edge_2;

typedef struct {
    u8 pad[10];
    s16 flags;
} Rec_16;

struct Hdr_5 {
    s32 pad[5];
    Pt_2 *pts;
    s32 nedges;
    Edge_2 *edges;
    s32 nrecs;
    Rec_16 *recs;
    Pair tab[0x101];
};

void height_init_walls(Hdr_5 **param_1, s32 param_2)
{
    Hdr_5 *h = *param_1;
    s32 i;
    s32 dx;
    s32 dy;
    s32 len;

    if (param_2 != 0) {
        h->pts = (Pt_2 *)((s32)h->pts + (s32)h);
        h->edges = (Edge_2 *)((s32)h->edges + (s32)h);
        h->recs = (Rec_16 *)((s32)h->recs + (s32)h);
        for (i = 0; i < 0x101; i++) {
            h->tab[i].a += (s32)h;
            h->tab[i].b += (s32)h;
        }
    }
    gWallEdgeFlags = mem_alloc_dbg(h->nedges, 0, D_800D6C20, 0x87F);
    mem_fill(gWallEdgeFlags, 0, h->nedges);
    for (i = 0; i < h->nedges; i++) {
        gWallEdgeFlags[i] |= 1;
        if (h->edges[i].nx == -0x8000) {
            gWallEdgeFlags[i] = ((u8 *)&h->edges[i])[7] ^ 1;
        }
        dx = h->pts[h->edges[i].b].x - h->pts[h->edges[i].a].x;
        dy = h->pts[h->edges[i].b].y - h->pts[h->edges[i].a].y;
        len = isqrt_u32_4(dx * dx + dy * dy);
        if (len != 0) {
            dx = (dx * 0x4000) / len;
            dy = (dy * 0x4000) / len;
        }
        h->edges[i].nx = dy;
        h->edges[i].ny = -dx;
    }
    for (i = 0; i < h->nedges; i++) {
        h->edges[i].ny &= ~1;
    }
    for (i = 0; i < h->nrecs; i++) {
        h->recs[i].flags &= ~1;
    }
}

void height_init_walls_reloc(int *param_1)
{
    height_init_walls(param_1, 1);
}

void height_free_walls(void)
{
    mem_free(gWallEdgeFlags);
    gWallEdgeFlags = 0;
}

s32 point_in_tri_xz(s32 *p1, s32 *p2, s32 x, s32 y)
{
    s32 b0 = p2[0];
    s32 a1 = p1[1];
    s32 a0 = p1[0];
    s32 b1 = p2[1];
    s32 a2;
    s32 b2;

    if ((y - b0) * (a1 - a0) - (x - a0) * (b1 - b0) <= 0) {
        return 0;
    }
    a2 = p1[2];
    b2 = p2[2];
    if ((y - b1) * (a2 - a1) - (x - a1) * (b2 - b1) <= 0) {
        return 0;
    }
    return (y - b2) * (a0 - a2) - (x - a2) * (b0 - b2) > 0;
}

int height_get_grid_cell(int x, int y)
{
    int a, b;

    b = y >> 4;
    a = x >> 4;
    a = (a - sHeightAreaMin[0]) * 16 / (sHeightAreaMax[0] - sHeightAreaMin[0]);
    if (a < 0) a = 0;
    if (a > 15) a = 15;
    b = (b - sHeightAreaMin[0]) * 16 / (sHeightAreaMax[1] - sHeightAreaMin[1]);
    if (b < 0) b = 0;
    if (b > 15) b = 15;
    return b * 16 + a;
}

struct Poly_2 {
    u8 hdr[6];
    u16 n;
    Pt pts[1];
};

s32 point_in_poly_xz(Poly_2 *poly, s32 px, s32 pz)
{
    s32 pos = 0;
    s32 neg;
    s32 i = 0, j, n, cr;

    n = poly->n;
    neg = 0;
    px = px >> 6;
    pz = pz >> 6;
    if (n != 0) {
        do {
            j = (i + 1) % n;
            cr = (pz - poly->pts[i].z) * (poly->pts[j].x - poly->pts[i].x);
            cr -= (px - poly->pts[i].x) * (poly->pts[j].z - poly->pts[i].z);
            if (cr >= 0) {
                pos++;
            }
            if (cr <= 0) {
                neg++;
            }
            n = poly->n;
            i++;
        } while (i < n);
    }
    if (pos == poly->n) {
        goto ret1;
    }
    if (neg != poly->n) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

s32 func_80031FA0(s32 p1, s32 p2, s32 p3)
{
    s32 t, v;

    if (p3 >= D_800E8518.b && p3 >= D_800E8518.d) {
        return 0;
    }
    if (p3 < D_800E8518.b && p3 < D_800E8518.d) {
        return 0;
    }
    t = ((p3 - D_800E8518.b) << 8) / (D_800E8518.d - D_800E8518.b);
    v = D_800E8518.a + ((D_800E8518.c - D_800E8518.a) * t) / 256;
    if (v < p1 || p2 < v) {
        return 0;
    }
    return 1;
}

s32 func_8003207C(s32 p1, s32 p2, s32 p3)
{
    s32 t, v;

    if (p3 >= D_800E8518.a && p3 >= D_800E8518.c) {
        return 0;
    }
    if (p3 < D_800E8518.a && p3 < D_800E8518.c) {
        return 0;
    }
    t = ((p3 - D_800E8518.a) << 8) / (D_800E8518.c - D_800E8518.a);
    v = D_800E8518.b + ((D_800E8518.d - D_800E8518.b) * t) / 256;
    if (v < p1 || p2 < v) {
        return 0;
    }
    return 1;
}
