#include "ctx.h"

typedef struct { s16 a, b, c, d; } V4;
typedef struct { s16 a, b, c, d, e, f; } V6;

typedef struct {
    s16 *a;
    s16 *b;
} GridPtr;

typedef struct Geo {
    s32 xmin, ymin, xmax, ymax;
    s32 nv; V4 *v;
    s32 ne; V4 *e;
    s32 nt; V6 *t;
    GridPtr cell[256];
} Geo;

typedef struct {
    u32 nv;
    Vtx *v;
    s32 f8;
    void *fC;
    s32 f10, f14;
} Mesh;

typedef struct {
    Mesh *mesh;
    s32 x0, y0, x1, y1;
    s16 *tex;
    s32 f18, f1C;
} Entry;

typedef struct {
    u32 n;
    Entry e[1];
} Set;

typedef struct {
    s16 dim[2];
    u8 pad4[8];
    u16 flags;
    u8 padE[0x22];
    Set *s30;
    Set *s34;
    s32 *r38;
    Geo *p3C;
} Ent;

typedef struct {
    s16 x, y;
    u16 flags;
    s16 rot;
    s16 type;
    s16 pad;
} Piece;

typedef struct {
    s32 n;
    s32 f4;
    s32 mode;
    u8 padC[8];
    u16 rec[8][15];
    Piece pc[1];
} Tab;

typedef struct {
    Ent *e;
    Entry *a;
    Entry *b;
    s16 rot, col, row, ox, oy;
    s16 pad;
} Cell;

typedef struct {
    s32 x, y;
    s32 f8;
    s16 next, fE, prev, f12;
    u8 pad[0x18];
} Rec;

typedef struct {
    s16 ix, iy, ox, oy, sx, sy;
} Rot;

typedef struct RecSet {
    s32 n;
    Rec r[1];
} RecSet;

typedef struct {
    Set *f00;
    s32 f04;
    Set *f08;
    s32 f0C;
    s16 *f10;
    Vtx *f14;
    Mesh *f18;
    Vtx *f1C;
    Mesh *f20;
    struct RecSet *f24;
    Geo *f28;
} Blk;

typedef struct {
    u8 pad[0x10];
    Geo *f10;
} Hdr;

typedef struct {
    u8 pad[0x68];
    Vec3f v68;
} Obj;

extern Tab *D_800C9570;
extern Ent D_800CE280[];
extern Hdr D_800CE26C;
extern Blk D_800F5CC0;
extern u16 *D_800C29E0;
extern u32 *D_800D0680;
extern s32 D_800D1348, D_800D134C, D_800D1350, D_800D1370;
extern Rot D_800D0684[];
extern u8 D_800DB040[];
extern u8 D_800D06B4[];
extern Obj *D_800EFD64;

void func_8000ED10(void *a0, u8 a1, s32 a2);
void *func_800BC908(s32 size, u32 flags, void *owner, s32 tag);
void func_800BCB04(void *ptr);
void func_80031A78(Geo **p, s32 b);
u32 func_800BF79C(u32 *pa0, s32 pa1);
void func_800AEC34(void *dst, void *src, s32 n);

#define PC (D_800C9570->pc[i])
#define EN (D_800CE280[D_800C9570->pc[i].type])
#define EP (D_800CE280[p->type])
#define R (D_800D0684[grid[c].rot])
#define G (grid[gy * size[0] + gx])
#define W (EP.dim[0])
#define H (EP.dim[1])
#define K0 (row * W + col)
#define K1 (col * W + W - 1 - row)
#define K2 ((H - 1 - row) * W + W - 1 - col)
#define K3 ((H - 1 - col) * W + row)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define REV(x) ((((x) >> 7) & 1) | (((x) >> 5) & 2) | (((x) >> 3) & 4) | (((x) >> 1) & 8) |                 (((x) << 1) & 0x10) | (((x) << 3) & 0x20) | (((x) << 5) & 0x40) | (((x) << 7) & 0x80))
/* transpose terms: output bit (7 - i) from source bit k */
#define BL(i, k) ((ts[i] << (7 - (i) - (k))) & (0x80 >> (i)))
#define BR(i, k) ((ts[i] >> ((k) - 7 + (i))) & (0x80 >> (i)))
#define T80 ((ts[0] & 0x80) | ((ts[1] & 0x80) >> 1) | ((ts[2] & 0x80) >> 2) | ((ts[3] & 0x80) >> 3) |              ((ts[4] & 0x80) >> 4) | ((ts[5] & 0x80) >> 5) | ((ts[6] & 0x80) >> 6) | ((ts[7] & 0x80) >> 7))
#define T01 (((ts[0] & 1) << 7) | ((ts[1] & 1) << 6) | ((ts[2] & 1) << 5) | ((ts[3] & 1) << 4) |              ((ts[4] & 1) << 3) | ((ts[5] & 1) << 2) | ((ts[6] & 1) << 1) | (ts[7] & 1))
#define BX(i, m, k) ((7 - (i)) > (k) ? BL(i, k) : (7 - (i)) == (k) ? (ts[i] & (m)) : BR(i, k))
#define TM(m, k) (((ts[0] & (m)) << (7 - (k))) | BX(1, m, k) | BX(2, m, k) | BX(3, m, k) | BX(4, m, k) |                   BX(5, m, k) | BX(6, m, k) | ((ts[7] & (m)) >> (k)))
#define TS(m, k) (((ts[0] & (m)) << (7 - (k))) | BX(1, m, k) | BX(2, m, k) | BX(3, m, k) | BX(4, m, k) | BX(5, m, k) | BX(6, m, k) | ({ s32 z; if (ts[7] & (m)) z = 1; else z = 0; z; }))
#define COPY8 td[8] = ts[8]; td[9] = ts[9]; td[10] = ts[10]; td[11] = ts[11];               td[12] = ts[12]; td[13] = ts[13]; td[14] = ts[14]; td[15] = ts[15]

void func_800A5A74(void)
{
    Vtx *va;
    Mesh *ma;
    Entry *ea;
    Vtx *vb;
    Mesh *mb;
    Entry *eb;
    s16 *g68;
    s16 *g6C;
    s32 nva;
    s32 nvb;
    s32 nv;
    s32 ne;
    s32 nt;
    s32 ti;
    s32 ncells;
    s32 bmw;
    s32 ebase;
    s32 tbase;
    s32 reps;
    s16 lo[2];
    s16 hi[2];
    s16 size[2];
    Vec3f pos;
    s32 pt[6];
    Cell *grid;
    s32 i, j, k, m;
    s32 gx, gy, col, row;
    s32 cnt, texsz;
    s32 vbase;
    s32 minx, maxx, miny, maxy;
    s32 tminx, tmaxx, tminy, tmaxy;
    s32 tcw, tch;
    s32 cw, ch;
    s32 c, n;
    s32 dx, dy;
    s32 sz1, sz2;
    Ent *e;
    Piece *p;
    s32 v, q, b, l;
    V4 *dv;
    V4 *sv;
    Vtx *src;
    Rec *rec;
    Entry *bb;
    Ent *d;
    s32 rc;
    s32 idx;
    s16 *tex;
    u8 *ts;
    u8 *td;
    s32 w, h;
    s32 kk;
    s32 kr;

    texsz = 0;
    nva = 0;
    nvb = 0;
    size[0] = -32000;
    size[1] = -32000;
    for (i = 0; i < D_800C9570->n; i++) {
        if (size[0] < PC.x + EN.dim[PC.rot & 1]) {
            size[0] = PC.x + EN.dim[PC.rot & 1];
        }
        if (size[1] < PC.y + EN.dim[(PC.rot + 1) & 1]) {
            size[1] = PC.y + EN.dim[(PC.rot + 1) & 1];
        }
    }
    size[0] = (size[0] < 15) ? 15 : size[0];
    size[1] = (size[1] < 15) ? 15 : size[1];
    cnt = 0;
    ncells = size[0] * size[1];
    grid = func_800BC908(ncells * sizeof(Cell), 0, D_800DB040, 0x416);
    func_8000ED10(grid, 0, ncells * sizeof(Cell));
    ne = D_800CE26C.f10->ne;
    nv = D_800CE26C.f10->nv;
    nt = D_800CE26C.f10->nt;
    for (i = 0, p = D_800C9570->pc; i < D_800C9570->n; p++, i++) {
        if (EP.flags & 0x40) {
            cnt += EP.dim[1] * 8;
        } else {
            cnt += *EP.r38 - 1;
        }
        if (EP.p3C != NULL) {
            if (EP.flags & 0x40) {
                m = EP.dim[1] * 2;
                nv += EP.p3C->nv * m;
                nt += EP.p3C->nt * m;
                ne += EP.p3C->ne * m;
            } else {
                nv += EP.p3C->nv;
                nt += EP.p3C->nt;
                ne += EP.p3C->ne;
            }
        }
        lo[0] = p->x;
        lo[1] = p->y;
        hi[0] = lo[0] + EP.dim[p->rot & 1];
        hi[1] = lo[1] + EP.dim[(p->rot + 1) & 1];
        for (gy = lo[1], row = 0; gy < hi[1]; gy++, row++) {
            for (gx = lo[0], col = 0; gx < hi[0]; gx++, col++) {
                G.rot = p->rot;
                G.e = &EP;
                G.col = col;
                G.row = row;
                switch (p->rot) {
                case 0:
                    G.a = &EP.s30->e[K0];
                    G.b = EP.s34 ? &EP.s34->e[K0] : NULL;
                    break;
                case 1:
                    G.a = &EP.s30->e[K1];
                    G.b = EP.s34 ? &EP.s34->e[K1] : NULL;
                    break;
                case 2:
                    G.a = &EP.s30->e[K2];
                    G.b = EP.s34 ? &EP.s34->e[K2] : NULL;
                    break;
                case 3:
                    G.a = &EP.s30->e[K3];
                    G.b = EP.s34 ? &EP.s34->e[K3] : NULL;
                    break;
                }
                bb = G.b;
                if (bb != NULL) {
                    d = &EP;
                    switch (p->rot) {
                    case 0:
                        G.ox = (bb->x0 + bb->x1) / 2 - 250 - col * 500;
                        G.oy = (bb->y0 + bb->y1) / 2 - 250 - row * 500;
                        break;
                    case 1:
                        G.ox = (bb->y0 + bb->y1) / 2 - 250 - col * 500;
                        G.oy = (d->dim[0] - row - 1) * 500 + (250 - (bb->x0 + bb->x1) / 2);
                        break;
                    case 2:
                        G.ox = (d->dim[0] - col - 1) * 500 + (250 - (bb->x0 + bb->x1) / 2);
                        G.oy = (d->dim[1] - row - 1) * 500 + (250 - (bb->y0 + bb->y1) / 2);
                        break;
                    case 3:
                        G.ox = (d->dim[1] - col - 1) * 500 + (250 - (bb->y0 + bb->y1) / 2);
                        G.oy = (bb->x0 + bb->x1) / 2 - 250 - row * 500;
                        break;
                    }
                }
            }
        }
    }
    for (c = 0; c < ncells; c++) {
        if (grid[c].a == NULL) {
            grid[c].rot = 0;
            grid[c].e = &D_800CE280[27];
            grid[c].a = &D_800CE280[27].s30->e[0];
            grid[c].col = 0;
            grid[c].row = 0;
            if (D_800CE280[27].s34 != NULL) {
                grid[c].b = &D_800CE280[27].s34->e[0];
            } else {
                grid[c].b = NULL;
            }
        }
        nva += grid[c].a->mesh->nv * sizeof(Vtx);
        if (grid[c].b != NULL) {
            nvb += grid[c].b->mesh->nv * sizeof(Vtx);
        }
        if (grid[c].a->tex[0] & 1) {
            texsz += (grid[c].a->tex[1] * 16) | 4;
        } else {
            texsz += 0x54;
        }
    }
    switch (D_800C9570->mode) {
    case 0:
        D_800F5CC0.f04 = func_800BF79C(D_800D0680, D_800D1348);
        break;
    case 1:
        D_800F5CC0.f04 = func_800BF79C(D_800D0680, D_800D134C);
        break;
    case 2:
        D_800F5CC0.f04 = func_800BF79C(D_800D0680, D_800D1350);
        break;
    }
    D_800F5CC0.f0C = func_800BF79C(D_800D0680, D_800D1370);
    D_800F5CC0.f00 = func_800BC908(ncells * sizeof(Entry) | 4, 0, D_800DB040, 0x4F5);
    D_800F5CC0.f00->n = ncells;
    D_800F5CC0.f08 = func_800BC908(ncells * sizeof(Entry) | 4, 0, D_800DB040, 0x4F8);
    D_800F5CC0.f08->n = ncells;
    D_800F5CC0.f24 = func_800BC908((cnt / 5 + 1) * sizeof(Rec) + 8, 0, D_800DB040, 0x4FB);
    D_800F5CC0.f28 = func_800BC908(sizeof(Geo), 0, D_800DB040, 0x4FC);
    D_800F5CC0.f28->v = func_800BC908(nv * sizeof(V4), 0, D_800DB040, 0x4FD);
    D_800F5CC0.f28->e = func_800BC908(ne * sizeof(V4), 0, D_800DB040, 0x4FE);
    D_800F5CC0.f28->t = func_800BC908(nt * sizeof(V6), 0, D_800DB040, 0x4FF);
    g68 = func_800BC908(0x8200, 0, D_800DB040, 0x500);
    g6C = func_800BC908(0x10200, 0, D_800DB040, 0x501);
    w = size[0] * 250;
    h = size[1] * 250;
    D_800F5CC0.f10 = func_800BC908(texsz, 0, D_800DB040, 0x508);
    D_800F5CC0.f14 = func_800BC908(nva, 0, D_800DB040, 0x509);
    D_800F5CC0.f18 = func_800BC908(ncells * sizeof(Mesh), 0, D_800DB040, 0x50A);
    D_800F5CC0.f1C = func_800BC908(nvb, 0, D_800DB040, 0x50C);
    D_800F5CC0.f20 = func_800BC908(ncells * sizeof(Mesh), 0, D_800DB040, 0x50D);
    ebase = D_800CE26C.f10->ne;
    vbase = D_800CE26C.f10->nv;
    tbase = D_800CE26C.f10->nt;
    D_800F5CC0.f28->nv = nv;
    D_800F5CC0.f28->nt = nt;
    D_800F5CC0.f28->ne = ne;
    D_800F5CC0.f28->xmax = -1000000;
    D_800F5CC0.f28->ymax = -1000000;
    D_800F5CC0.f28->xmin = 1000000;
    D_800F5CC0.f28->ymin = 1000000;
    for (j = 0; j < D_800CE26C.f10->nv; j++) {
        D_800F5CC0.f28->v[j].a = D_800CE26C.f10->v[j].a;
        D_800F5CC0.f28->v[j].b = D_800CE26C.f10->v[j].b;
        D_800F5CC0.f28->v[j].c = D_800CE26C.f10->v[j].c;
        D_800F5CC0.f28->v[j].d = D_800CE26C.f10->v[j].d;
    }
    for (j = 0; j < D_800CE26C.f10->ne; j++) {
        D_800F5CC0.f28->e[j].a = D_800CE26C.f10->e[j].a;
        D_800F5CC0.f28->e[j].b = D_800CE26C.f10->e[j].b;
        D_800F5CC0.f28->e[j].c = -0x8000;
        D_800F5CC0.f28->e[j].d = 0x40;
    }
    for (j = 0; j < D_800CE26C.f10->nt; j++) {
        D_800F5CC0.f28->t[j].a = D_800CE26C.f10->t[j].a;
        D_800F5CC0.f28->t[j].b = D_800CE26C.f10->t[j].b;
        D_800F5CC0.f28->t[j].c = D_800CE26C.f10->t[j].c;
        D_800F5CC0.f28->t[j].d = D_800CE26C.f10->t[j].d;
        D_800F5CC0.f28->t[j].e = D_800CE26C.f10->t[j].e;
        D_800F5CC0.f28->t[j].f = D_800CE26C.f10->t[j].f;
    }
    bmw = (nv + 15) / 16;
    func_8000ED10(D_800C29E0 + bmw, 0xFF, nv * 2);
    for (i = 0, p = D_800C9570->pc; i < D_800C9570->n; p++, i++) {
        e = &D_800CE280[p->type];
        if (e->p3C != NULL) {
            D_800F5CC0.f28->xmin = -3750;
            D_800F5CC0.f28->xmax = 3750;
            D_800F5CC0.f28->ymin = -3750;
            D_800F5CC0.f28->ymax = 3750;
            if (e->flags & 0x40) {
                reps = e->dim[1] * 2;
            } else {
                reps = 1;
            }
            for (k = 0; k < reps; k++) {
                for (j = 0; j < e->p3C->nv; j++) {
                    D_800C29E0[bmw + vbase + j] = i;
                    D_800F5CC0.f28->v[vbase + j].c = e->p3C->v[j].c;
                    D_800F5CC0.f28->v[vbase + j].d = e->p3C->v[j].d;
                    switch (p->rot) {
                    case 0:
                        D_800F5CC0.f28->v[vbase + j].a = e->p3C->v[j].a;
                        D_800F5CC0.f28->v[vbase + j].b = e->p3C->v[j].b;
                        if (e->p3C->v[j].b == 0) {
                            if (k != 0) {
                                D_800F5CC0.f28->v[vbase + j].d += D_800C9570->rec[e - &D_800CE280[18]][k - 1];
                            }
                        } else if (k != reps - 1) {
                            D_800F5CC0.f28->v[vbase + j].d += D_800C9570->rec[e - &D_800CE280[18]][k];
                        }
                        D_800F5CC0.f28->v[vbase + j].a += p->x * 125 - w / 4;
                        D_800F5CC0.f28->v[vbase + j].b += p->y * 125 - h / 4 + (k * 4000) / 64;
                        break;
                    case 1:
                        D_800F5CC0.f28->v[vbase + j].a = e->p3C->v[j].b;
                        D_800F5CC0.f28->v[vbase + j].b = e->dim[0] * 125 - e->p3C->v[j].a;
                        if (e->p3C->v[j].b == 0) {
                            if (k != 0) {
                                D_800F5CC0.f28->v[vbase + j].d += D_800C9570->rec[e - &D_800CE280[18]][k - 1];
                            }
                        } else if (k != reps - 1) {
                            D_800F5CC0.f28->v[vbase + j].d += D_800C9570->rec[e - &D_800CE280[18]][k];
                        }
                        D_800F5CC0.f28->v[vbase + j].a += p->x * 125 - w / 4 + (k * 4000) / 64;
                        D_800F5CC0.f28->v[vbase + j].b += p->y * 125 - h / 4;
                        break;
                    case 2:
                        D_800F5CC0.f28->v[vbase + j].a = e->dim[0] * 125 - e->p3C->v[j].a;
                        D_800F5CC0.f28->v[vbase + j].b = e->dim[1] * 125 - e->p3C->v[j].b;
                        if (e->p3C->v[j].b == 0) {
                            if (k != 0) {
                                D_800F5CC0.f28->v[vbase + j].d += D_800C9570->rec[e - &D_800CE280[18]][k - 1];
                            }
                        } else if (k != reps - 1) {
                            D_800F5CC0.f28->v[vbase + j].d += D_800C9570->rec[e - &D_800CE280[18]][k];
                        }
                        D_800F5CC0.f28->v[vbase + j].a += p->x * 125 - w / 4;
                        D_800F5CC0.f28->v[vbase + j].b += p->y * 125 - h / 4 - (k * 4000) / 64;
                        break;
                    case 3:
                        D_800F5CC0.f28->v[vbase + j].a = e->dim[1] * 125 - e->p3C->v[j].b;
                        D_800F5CC0.f28->v[vbase + j].b = e->p3C->v[j].a;
                        if (e->p3C->v[j].b == 0) {
                            kk = reps - k - 1;
                            if (kk != 0) {
                                D_800F5CC0.f28->v[vbase + j].d += D_800C9570->rec[e - &D_800CE280[18]][reps - k - 2];
                            }
                        } else if ((kr = reps - k - 1) != reps - 1) {
                            D_800F5CC0.f28->v[vbase + j].d += D_800C9570->rec[e - &D_800CE280[18]][kr];
                        }
                        D_800F5CC0.f28->v[vbase + j].a += p->x * 125 - w / 4 - ((reps - k - 1) * 4000) / 64;
                        D_800F5CC0.f28->v[vbase + j].b += p->y * 125 - h / 4;
                        break;
                    }
                }
                for (j = 0; j < e->p3C->ne; j++) {
                    D_800F5CC0.f28->e[ebase + j].a = e->p3C->e[j].a + vbase;
                    D_800F5CC0.f28->e[ebase + j].b = e->p3C->e[j].b + vbase;
                    switch (p->rot) {
                    case 0:
                        D_800F5CC0.f28->e[ebase + j].c = e->p3C->e[j].c;
                        D_800F5CC0.f28->e[ebase + j].d = e->p3C->e[j].d;
                        break;
                    case 1:
                        D_800F5CC0.f28->e[ebase + j].c = e->p3C->e[j].d;
                        D_800F5CC0.f28->e[ebase + j].d = -e->p3C->e[j].c;
                        break;
                    case 2:
                        D_800F5CC0.f28->e[ebase + j].c = -e->p3C->e[j].c;
                        D_800F5CC0.f28->e[ebase + j].d = -e->p3C->e[j].d;
                        break;
                    case 3:
                        D_800F5CC0.f28->e[ebase + j].c = -e->p3C->e[j].d;
                        D_800F5CC0.f28->e[ebase + j].d = e->p3C->e[j].c;
                        break;
                    }
                    D_800F5CC0.f28->e[ebase + j].c &= ~1;
                    D_800F5CC0.f28->e[ebase + j].d &= ~1;
                }
                for (j = 0; j < e->p3C->nt; j++) {
                    D_800F5CC0.f28->t[tbase + j].a = e->p3C->t[j].a + vbase;
                    D_800F5CC0.f28->t[tbase + j].b = e->p3C->t[j].b + vbase;
                    D_800F5CC0.f28->t[tbase + j].c = e->p3C->t[j].c + vbase;
                    switch (p->rot) {
                    case 0:
                        D_800F5CC0.f28->t[tbase + j].d = e->p3C->t[j].d;
                        D_800F5CC0.f28->t[tbase + j].f = e->p3C->t[j].f;
                        break;
                    case 1:
                        D_800F5CC0.f28->t[tbase + j].d = e->p3C->t[j].f;
                        D_800F5CC0.f28->t[tbase + j].f = -e->p3C->t[j].d;
                        break;
                    case 2:
                        D_800F5CC0.f28->t[tbase + j].d = -e->p3C->t[j].d;
                        D_800F5CC0.f28->t[tbase + j].f = -e->p3C->t[j].f;
                        break;
                    case 3:
                        D_800F5CC0.f28->t[tbase + j].d = -e->p3C->t[j].f;
                        D_800F5CC0.f28->t[tbase + j].f = e->p3C->t[j].d;
                        break;
                    }
                    D_800F5CC0.f28->t[tbase + j].e = e->p3C->t[j].e;
                }
                ebase += e->p3C->ne;
                vbase += e->p3C->nv;
                tbase += e->p3C->nt;
            }
        }
    }
    func_8000ED10(D_800C29E0, 0, bmw * 2);
    for (j = D_800CE26C.f10->nv; j < nv; j++) {
        if (!((D_800C29E0[j / 16] >> (j % 16)) & 1)) {
            for (l = j + 1; l < nv; l++) {
                if (!((D_800C29E0[l / 16] >> (l % 16)) & 1)) {
                    dx = D_800F5CC0.f28->v[j].a - D_800F5CC0.f28->v[l].a;
                    dy = D_800F5CC0.f28->v[j].b - D_800F5CC0.f28->v[l].b;
                    if (dx * dx + dy * dy < 16) {
                        D_800F5CC0.f28->v[l].a += dx / 2;
                        D_800F5CC0.f28->v[l].b += dy / 2;
                        D_800F5CC0.f28->v[j].a -= dx - dx / 2;
                        D_800F5CC0.f28->v[j].b -= dy - dy / 2;
                        D_800C29E0[l / 16] |= 1 << (l % 16);
                        D_800C29E0[j / 16] |= 1 << (j % 16);
                        break;
                    }
                }
            }
        }
    }
    for (c = 0; c < 256; c++) {
        g68[c * 65] = 0;
        g6C[c * 129] = 0;
    }
    for (b = 0; b < D_800F5CC0.f28->ne; b++) {
        pt[0] = D_800F5CC0.f28->v[D_800F5CC0.f28->e[b].a].a + w / 4;
        pt[1] = D_800F5CC0.f28->v[D_800F5CC0.f28->e[b].a].b + h / 4;
        pt[2] = D_800F5CC0.f28->v[D_800F5CC0.f28->e[b].b].a + w / 4;
        pt[3] = D_800F5CC0.f28->v[D_800F5CC0.f28->e[b].b].b + h / 4;
        minx = MIN(pt[0], pt[2]);
        maxx = MAX(pt[0], pt[2]);
        miny = MIN(pt[1], pt[3]);
        maxy = MAX(pt[1], pt[3]);
        cw = size[0] * 125 / 16;
        ch = size[1] * 125 / 16;
        for (gy = miny / ch; gy <= maxy / ch; gy++) {
            for (gx = minx / cw; gx <= maxx / cw; gx++) {
                if (gx >= 0 && gy >= 0 && gx < 16 && gy < 16) {
                    c = gy * 16 + gx;
                    g6C[c * 129 + ++g6C[c * 129]] = b;
                }
            }
        }
    }
    for (b = 0; b < D_800F5CC0.f28->nt; b++) {
        pt[0] = D_800F5CC0.f28->v[D_800F5CC0.f28->t[b].a].a + w / 4;
        pt[1] = D_800F5CC0.f28->v[D_800F5CC0.f28->t[b].a].b + h / 4;
        pt[2] = D_800F5CC0.f28->v[D_800F5CC0.f28->t[b].b].a + w / 4;
        pt[3] = D_800F5CC0.f28->v[D_800F5CC0.f28->t[b].b].b + h / 4;
        pt[4] = D_800F5CC0.f28->v[D_800F5CC0.f28->t[b].c].a + w / 4;
        pt[5] = D_800F5CC0.f28->v[D_800F5CC0.f28->t[b].c].b + h / 4;
        tminx = MIN(MIN(pt[0], pt[2]), pt[4]);
        tmaxx = MAX(MAX(pt[0], pt[2]), pt[4]);
        tminy = MIN(MIN(pt[1], pt[3]), pt[5]);
        tmaxy = MAX(MAX(pt[1], pt[3]), pt[5]);
        tch = size[1] * 125 / size[1];
        tcw = size[0] * 125 / size[0];
        for (gy = tminy / tch; gy <= tmaxy / tch; gy++) {
            for (gx = tminx / tcw; gx <= tmaxx / tcw; gx++) {
                if (gx >= 0 && gy >= 0 && gx < 16 && gy < 16) {
                    c = gy * 16 + gx;
                    g68[c * 65 + ++g68[c * 65]] = b;
                }
            }
        }
    }
    sz2 = 0;
    sz1 = 0;
    for (c = 0; c < 256; c++) {
        sz2 += g6C[c * 129] * 2;
        sz1 += g68[c * 65] * 2;
    }
    D_800F5CC0.f28->cell[0].b = func_800BC908(sz2, 0, D_800DB040, 0x6A8);
    D_800F5CC0.f28->cell[0].a = func_800BC908(sz1, 0, D_800DB040, 0x6A9);
    for (c = 0; c < 257; c++) {
        if (c > 0) {
            D_800F5CC0.f28->cell[c].a = D_800F5CC0.f28->cell[c - 1].a + g68[(c - 1) * 65];
            D_800F5CC0.f28->cell[c].b = D_800F5CC0.f28->cell[c - 1].b + g6C[(c - 1) * 129];
        }
        if (c < 256) {
            if (g68[c * 65] != 0) {
                func_800AEC34(D_800F5CC0.f28->cell[c].a, g68 + c * 65 + 1, g68[c * 65] * 2);
            }
            if (g6C[c * 129] != 0) {
                func_800AEC34(D_800F5CC0.f28->cell[c].b, g6C + c * 129 + 1, g6C[c * 129] * 2);
            }
        }
    }
    func_800BCB04(g68);
    func_800BCB04(g6C);
    func_80031A78(&D_800F5CC0.f28, 0);
    va = D_800F5CC0.f14;
    ma = D_800F5CC0.f18;
    tex = D_800F5CC0.f10;
    vb = D_800F5CC0.f1C;
    mb = D_800F5CC0.f20;
    ea = D_800F5CC0.f00->e;
    eb = D_800F5CC0.f08->e;
    *(s32 *)D_800F5CC0.f24 = 0xAA000003;
    D_800F5CC0.f24 = (RecSet *)((s32 *)D_800F5CC0.f24 + 1);
    D_800F5CC0.f24->n = 0;
    rec = D_800F5CC0.f24->r;
    rc = 0;
    for (i = 0, p = D_800C9570->pc; i < D_800C9570->n; p++, i++) {
        if (EP.flags & 0x40) {
            n = EP.dim[1] * 8;
        } else {
            n = *EP.r38 - 1;
        }
        for (; rc < n; rc += 5) {
            s32 x, y, o;

            if (p->flags & 2) {
                sz2 = n - rc;
            } else {
                sz2 = rc;
            }
            o = sz2 * 44 + 4;
            __builtin_memcpy(rec, (u8 *)EP.r38 + o, sizeof(Rec));
            switch (p->rot) {
            case 0:
                x = rec->x + p->x * 8000 - w * 16;
                y = rec->y + p->y * 8000 - h * 16;
                break;
            case 1:
                x = rec->y + p->x * 8000 - w * 16;
                y = EP.dim[0] * 8000 - rec->x + p->y * 8000 - h * 16;
                break;
            case 2:
                x = EP.dim[0] * 8000 - rec->x + p->x * 8000 - w * 16;
                y = EP.dim[1] * 8000 - rec->y + p->y * 8000 - h * 16;
                break;
            case 3:
                x = EP.dim[1] * 8000 - rec->y + p->x * 8000 - w * 16;
                y = rec->x + p->y * 8000 - h * 16;
                break;
            }
            rec->x = x;
            rec->y = y;
            rec->prev = D_800F5CC0.f24->n - 1;
            rec->next = ++D_800F5CC0.f24->n;
            rec->f12 = -1;
            rec->fE = -1;
            rec++;
        }
        rc -= n;
    }
    D_800F5CC0.f24->r[0].prev = D_800F5CC0.f24->n - 1;
    D_800F5CC0.f24->r[D_800F5CC0.f24->n - 1].next = 0;
    D_800F5CC0.f24 = (RecSet *)((s32 *)D_800F5CC0.f24 - 1);
    c = 0;
    for (gy = 0; gy < size[1]; gy++) {
        for (gx = 0; gx < size[0]; gx++) {
            ea->mesh = ma;
            ea->x0 = gx * 500 - w;
            ea->y0 = gy * 500 - h;
            ea->x1 = gx * 500 - w + 500;
            ea->y1 = gy * 500 - h + 500;
            ea->tex = tex;
            ea->f1C = 0;
            ma->nv = grid[c].a->mesh->nv;
            ma->f8 = grid[c].a->mesh->f8;
            ma->v = va;
            ma->fC = grid[c].a->mesh->fC;
            ma->f10 = 0;
            ma->f14 = 0;
            src = grid[c].a->mesh->v;
            for (v = 0; v < grid[c].a->mesh->nv; v++, src++) {
                va->v.ob[0] = R.ox + (ea->x0 + (src->v.ob[R.ix] + 250) * R.sx);
                va->v.ob[1] = src->v.ob[1];
                va->v.ob[2] = R.oy + (ea->y0 + (src->v.ob[R.iy] + 250) * R.sy);
                va->v.flag = src->v.flag;
                va->v.tc[0] = src->v.tc[0];
                va->v.tc[1] = src->v.tc[1];
                va->v.cn[0] = src->v.cn[R.ix] * R.sx;
                va->v.cn[1] = src->v.cn[1];
                va->v.cn[2] = src->v.cn[R.iy] * R.sy;
                va->v.cn[3] = 0xFF;
                va++;
            }
            if (grid[c].a->tex[0] & 1) {
                *tex = grid[c].a->tex[0] & 0xFFFB;
                tex++;
                *tex = grid[c].a->tex[1];
                tex++;
                for (ti = 0; ti < grid[c].a->tex[1]; ti++) {
                    ts = (u8 *)grid[c].a->tex + (ti * 16 + 4);
                    td = (u8 *)tex;
                    switch (grid[c].rot) {
                    case 0:
                        td[0] = ts[0];
                        td[1] = ts[1];
                        td[2] = ts[2];
                        td[3] = ts[3];
                        td[4] = ts[4];
                        td[5] = ts[5];
                        td[6] = ts[6];
                        td[7] = ts[7];
                        COPY8;
                        break;
                    case 1:
                        td[0] = REV(T80);
                        td[1] = REV(TS(0x40, 6));
                        td[2] = REV(TS(0x20, 5));
                        td[3] = REV(TS(0x10, 4));
                        td[4] = REV(TS(8, 3));
                        td[5] = REV(TS(4, 2));
                        td[6] = REV(TS(2, 1));
                        td[7] = REV((((ts[0] & 1) << 7) | ((ts[1] & 1) * 64) | ((ts[2] & 1) * 32) | ((ts[3] & 1) * 16) | ((ts[4] & 1) * 8) | ((ts[5] & 1) * 4) | ((ts[6] & 1) * 2) | (ts[7] & 1)));
                        COPY8;
                        break;
                    case 2:
                        td[0] = REV(ts[7]);
                        td[1] = REV(ts[6]);
                        td[2] = REV(ts[5]);
                        td[3] = REV(ts[4]);
                        td[4] = REV(ts[3]);
                        td[5] = REV(ts[2]);
                        td[6] = REV(ts[1]);
                        td[7] = REV(ts[0]);
                        COPY8;
                        break;
                    case 3:
                        {
                            s32 r = T01;

                            td[0] = r;
                        }
                        {
                            s32 r = TM(2, 1);

                            td[1] = r;
                        }
                        {
                            s32 r = TM(4, 2);

                            td[2] = r;
                        }
                        {
                            s32 r = TM(8, 3);

                            td[3] = r;
                        }
                        {
                            s32 r = TM(0x10, 4);

                            td[4] = r;
                        }
                        {
                            s32 r = TM(0x20, 5);

                            td[5] = r;
                        }
                        {
                            s32 r = TM(0x40, 6);

                            td[6] = r;
                        }
                        {
                            s32 r = T80;

                            td[7] = r;
                        }
                        COPY8;
                        break;
                    }
                    tex += 8;
                }
            }
            ea++;
            ma++;
            if (grid[c].b != NULL) {
                eb->mesh = mb;
                eb->x0 = gx * 500 - w;
                eb->y0 = gy * 500 - h;
                eb->x1 = gx * 500 - w + 500;
                eb->y1 = gy * 500 - h + 500;
                eb->tex = NULL;
                eb->f1C = 0;
                mb->nv = grid[c].b->mesh->nv;
                mb->f8 = grid[c].b->mesh->f8;
                mb->v = vb;
                mb->fC = grid[c].b->mesh->fC;
                mb->f10 = 0;
                mb->f14 = 0;
                src = grid[c].b->mesh->v;
                for (v = 0; v < grid[c].b->mesh->nv; v++, src++) {
                    vb->v.ob[0] = R.ox + (eb->x0 + (src->v.ob[R.ix] + 250) * R.sx);
                    vb->v.ob[1] = src->v.ob[1];
                    vb->v.ob[2] = R.oy + (eb->y0 + (src->v.ob[R.iy] + 250) * R.sy);
                    vb->v.ob[0] += grid[c].ox;
                    vb->v.ob[2] += grid[c].oy;
                    vb->v.flag = src->v.flag;
                    vb->v.tc[0] = src->v.tc[0];
                    vb->v.tc[1] = src->v.tc[1];
                    vb->v.cn[0] = 0xFF;
                    vb->v.cn[1] = 0xFF;
                    vb->v.cn[2] = 0xFF;
                    vb->v.cn[3] = 0xFF;
                    vb++;
                }
            } else {
                eb->mesh = mb;
                eb->x0 = gx * 500 - w;
                eb->y0 = gy * 500 - h;
                eb->x1 = gx * 500 - w + 500;
                eb->y1 = gy * 500 - h + 500;
                eb->tex = NULL;
                eb->f1C = 0;
                mb->nv = 0;
                mb->f8 = 0;
                mb->v = NULL;
                mb->fC = D_800D06B4;
                mb->f10 = 0;
                mb->f14 = 0;
            }
            eb++;
            mb++;
            c++;
        }
    }
    func_800BCB04(grid);
    D_800EFD64->v68.x = pos.x;
    D_800EFD64->v68.y = pos.y;
    D_800EFD64->v68.z = pos.z;
}
