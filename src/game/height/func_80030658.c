#include "ctx.h"

typedef struct { s16 x, z, y, h; } Vtx8;
typedef struct { s16 a, b, nx, nz; } Edge;
typedef struct { s16 a, b, c, nx, ny, nz; } Tri12;
typedef struct { u8 pad[0x14]; Vtx8 *verts; u8 pad2[4]; Edge *edges; } Track;
typedef struct { u8 pad[0x2C]; s32 mode; } Game;

extern Game *D_800EFD64;
extern Track *D_800E8D6C;
extern s32 D_800E8D70;
extern s32 D_800E8E78;
extern u8 *D_800C9800;
extern s32 D_800E84FC;
extern s32 D_800E8504;
extern Edge *D_800E8538[];
extern Tri12 *D_800E8960[];
extern Edge *D_800E8530;

#define ABS(x) ((x) < 0 ? -(x) : (x))
s32 func_800300B0(f32 *p1, f32 *p2, f32 *p3, f32 *p4);

u8 func_80030658(f32 *start, f32 *end, f32 *out, f32 *tmax, s32 wantFlag)
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

    if (D_800EFD64->mode == 2) {
        return func_800300B0(start, end, out, tmax);
    }
    if (D_800E8D6C == NULL) {
        return 0;
    }
    sx = start[0] * 1000.0f;
    sy = start[1] * 1000.0f;
    sz = start[2] * 1000.0f;
    ex = end[0] * 1000.0f;
    ey = end[1] * 1000.0f;
    ez = end[2] * 1000.0f;
    D_800E8D70 = 0;
    if (D_800E84FC != 0) {
        for (i = 0; i < D_800E84FC; i++) {
            Edge *e = D_800E8538[i];
            u8 fl = D_800C9800[(e - D_800E8D6C->edges)];
            f32 ax, az, fax, faz, dz, dx, py, y0, y1, len;

            if (fl & 0x20) continue;
            if (wantFlag && !(fl & 0x40)) continue;
            a = &D_800E8D6C->verts[e->a];
            iax = a->x;
            fax = iax;
            iaz = a->z;
            faz = iaz;
            ax = fax * 64.0f;
            az = faz * 64.0f;
            b = &D_800E8D6C->verts[e->b];
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
            D_800E8530 = e;
            D_800E8D70 = D_800C9800[(e - D_800E8D6C->edges)];
            D_800E8E78 = (D_800E8D70 >> 1) & 3;
        }
    }
    if (D_800E8504 != 0) {
        for (i = 0; i < D_800E8504; i++) {
            Tri12 *tr = D_800E8960[i];
            Vtx8 *v = D_800E8D6C->verts;
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
