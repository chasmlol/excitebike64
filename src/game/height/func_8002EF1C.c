#include "ctx.h"

typedef struct { u8 pad[8]; u16 v[3]; u16 pad2; } Tri3;
typedef struct { u16 flags; u16 pad; Tri3 t[1]; } Shape;
typedef struct { u32 pad; Vtx_t *vtx; } Mesh;
typedef struct { Mesh *mesh; s32 x0, z0, x1, z1; Shape *shape; u32 pad[2]; } Ent;
typedef struct { u32 n; Ent e[1]; } EntList;

extern s32 D_800E8958;
extern EntList *D_800E8948;
extern s32 D_800E8D64;

void func_8002EF1C(s32 x, s32 z, s32 *out)
{
    s32 m[3][4];
    Ent *e;
    Shape *s;
    Tri3 *t;
    Vtx_t *vb;
    s32 ee, k, q, tt, a, a2, b, d, num, sx, sz, px, pz;

    if (D_800E8958 < 0 || D_800E8958 >= D_800E8948->n) return;
    e = &D_800E8948->e[D_800E8958];
    s = e->shape;
    if (!(s->flags & 1)) return;
    t = &s->t[D_800E8D64];
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
