#include "ctx.h"

typedef struct { s16 x, y, z; s16 pad[5]; } Vtx16;
typedef struct { u8 pad[8]; u16 vi[3]; u16 pad2; } MTri;
typedef struct { s32 x, y, z; } V3;
typedef struct { u16 flags; u16 pad; MTri tri[1]; } Mesh;
typedef struct { u8 pad[4]; Vtx16 *verts; } Model;
typedef struct { Model *model; s32 a, b, c, d; Mesh *mesh; } Obj;

extern u8 *D_800EFD64;
extern s8 *D_800EA990;
extern u8 *D_800E8948;

#define IDX(a, b) (((((a) & 0x1FF)) << 9) | ((b) & 0x1FF))

s32 func_8002F1D0(s32 p1, s32 p2, V3 *v)
{
    s32 n, q, u12, u13, lo4;
    Obj *o;
    Mesh *m;
    Vtx16 *vt;
    s32 k;
    u32 f; s32 j;
    u16 *pb, *pv;

    if (*(s32 *)(D_800EFD64 + 0x2c) == 2) {
        n = p2 / 2;
        lo4 = (p1 << 16) >> 14;
        q = n / 4;
        u12 = lo4 + (n - q * 4);
        u13 = (p1 >> 16) * 4 + q;
        if (p2 & 1) {
            v[0].x = u12 * 2000;
            v[0].z = u13 * 2000;
            v[0].y = D_800EA990[IDX(u13, u12)] << 6;
            v[1].x = (u12 + 1) * 2000;
            v[1].z = u13 * 2000;
            v[1].y = D_800EA990[IDX(u13, u12 + 1)] << 6;
            v[2].x = u12 * 2000;
            v[2].z = (u13 + 1) * 2000;
            v[2].y = D_800EA990[IDX(u13 + 1, u12)] << 6;
        } else {
            v[0].x = (u12 + 1) * 2000;
            v[0].z = (u13 + 1) * 2000;
            v[0].y = D_800EA990[IDX(u13 + 1, u12 + 1)] << 6;
            v[1].x = u12 * 2000;
            v[1].z = (u13 + 1) * 2000;
            v[1].y = D_800EA990[IDX(u13 + 1, u12)] << 6;
            v[2].x = (u12 + 1) * 2000;
            v[2].z = u13 * 2000;
            v[2].y = D_800EA990[IDX(u13, u12 + 1)] << 6;
        }
        return 0;
    }

    o = (Obj *)(D_800E8948 + (p1 * 0x20 + 4));
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
