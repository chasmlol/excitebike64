#include "ctx.h"

typedef struct Rec {
    s32 f0;
    s32 key;
    s32 x, z;
    s32 nx, nz;
} Rec;

typedef struct Seg {
    u8 pad[0x28];
    s32 flags;
    u8 pad2[0x40 - 0x2C];
} Seg;

typedef struct Gate {
    u8 pad[0x20];
    s16 a[3];
    u8 pad26[0x30 - 0x26];
    s16 b[3];
    u8 pad36[0x40 - 0x36];
} Gate;

typedef struct Cell {
    s32 f0, f4;
    s32 x0, z0, x1, z1;
    u8 pad[0x20 - 0x18];
} Cell;

typedef struct Bike {
    u8 pad0[0x44];
    u8 f44;
    u8 pad45[0x370 - 0x45];
    s32 f370[1];
    u8 pad374[0x458 - 0x374];
    s32 f458;
    u8 pad45C[0x494 - 0x45C];
    f32 x, y, z;          /* 0x494 */
    u8 pad4A0[0x4AC - 0x4A0];
    f32 f4AC;
    f32 f4B0;
    f32 f4B4;
    u8 pad4B8[0x4E4 - 0x4B8];
    Seg *seg;             /* 0x4E4 */
    u8 pad4E8[0x4F0 - 0x4E8];
    f32 f4F0;
    u8 pad4F4[0x7A5 - 0x4F4];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Bike;

typedef struct { u8 pad[0x2C]; s32 mode; } Mode;

extern Bike D_800E37C0[];
extern s32 D_800E8508;
extern Cell *D_800E8948;
extern s32 D_800E8958;
extern Seg *D_800E9298;
extern s32 D_800EE140;
extern Mode *D_800EFD64;
extern Rec *D_800F0774;
extern s32 D_800F0778[];
extern Rec *D_800F0798[];
extern Seg *D_800F07B8[];
extern s32 D_800F07D8[];
extern Gate *D_800F07F8;
extern Seg *D_800F0800[];
extern s32 D_800F0828[];

void func_8004262C(s32 a, s32 b, s32 *q, s32 *r);
void func_80013650(s32 *m, s32 *q);
s32 func_8000EB18(u32 r);
s32 func_8002E920(s32 x, s32 y, s32 z);
Rec *func_8005DCFC(s32 p1, s32 *p2, s32 *p3, s32 *p4, s32 *p5, s32 p6);

void func_8005D444(void)
{
    s32 pos[3];
    s32 a[3];
    s32 b[3];
    s32 d[3];
    s32 m[10];
    s32 c[3];
    s32 e[3];
    s32 qr[4];
    s32 i, chg, key, idx, ox, oz, dx, dz, nx, nz, len, k;
    Bike *p;
    Seg *t;
    Rec *r;
    s32 n;
    s32 l2;
    Cell *ce;

    for (i = 0; i < 8; i++) {
        if (D_800E37C0[i].f7A5 != 0) continue;
        p = &D_800E37C0[i];
        if (D_800F0828[i] != 0) D_800F0828[i]--;
        if (D_800E37C0[i].f4F0 != 0.0f) {
            D_800F07D8[i] = 1;
        } else if (D_800F07D8[i] != 0) {
            D_800F07D8[i]--;
        }
        if (!(p->seg->flags & 1)) D_800F07D8[i] = 0;
        t = D_800F07B8[i];
        if (p->seg != t) {
            chg = 1;
            if (t != NULL && (t->flags & 0x40) && p->f458 <= 0) {
                D_800F0800[i] = t;
                D_800F07D8[i] = 45;
                D_800F0828[i] = 45;
            }
        } else {
            chg = 0;
        }
        D_800F07B8[i] = p->seg;
        if (p->f44 != 0) D_800F07D8[i] = 0;
        if (D_800EE140 != 0) D_800F07D8[i] = 0;
        if (D_800F07D8[i] == 0) {
            D_800F0798[i] = NULL;
            continue;
        }
        pos[0] = p->x * 1000.0f;
        pos[1] = p->y * 1000.0f;
        pos[2] = p->z * 1000.0f;
        func_80013650(m, p->f370);
        d[0] = -p->f4B4 * 1000.0f;
        d[2] = p->f4AC * 1000.0f;
        len = func_8000EB18(d[0] * d[0] + d[2] * d[2]);
        if (len == 0) {
            d[1] = 0;
            d[0] = m[0];
            d[2] = m[2];
            len = func_8000EB18(m[0] * m[0] + m[2] * m[2]);
        }
        if (len != 0) {
            d[0] = d[0] * 50 / len;
            d[2] = d[2] * 50 / len;
        }
        a[0] = pos[0] - d[0];
        a[2] = pos[2] - d[2];
        b[0] = pos[0] + d[0];
        b[2] = pos[2] + d[2];
        a[1] = func_8002E920(a[0], pos[1], a[2]);
        b[1] = func_8002E920(b[0], pos[1], b[2]);
        if (D_800E8508 == 0) continue;
        key = D_800E8958;
        if (D_800EFD64->mode == 2) {
            func_8004262C(pos[0], 128000, &qr[0], &qr[1]);
            func_8004262C(pos[2], 128000, &qr[2], &qr[3]);
            key = (qr[0] << 16) | (u16)qr[2];
        }
        if (D_800F0798[i] != NULL && (u32)(a[1] - D_800F0778[i] + 500) > 1000) {
            D_800F0798[i] = NULL;
            D_800F07D8[i] = 0;
            continue;
        }
        idx = p->seg - D_800E9298;
        if (D_800F0828[i] != 0) idx = D_800F0800[i] - D_800E9298;
        if (D_800F0798[i] != NULL) {
            k = D_800F0798[i]->key;
            if (D_800EFD64->mode == 1) {
                ce = &D_800E8948[k];
                ox = (ce->x0 + ce->x1) / 2;
                oz = (ce->z0 + ce->z1) / 2;
            } else if (D_800EFD64->mode == 2) {
                ox = (k >> 16) * 8000 + 4000;
                oz = (s16)k * 8000 + 4000;
            } else {
                ox = oz = 0;
            }
            r = D_800F0798[i];
            dx = pos[0] - r->x;
            dz = pos[2] - r->z;
            nx = r->nx;
            nz = r->nz;
            l2 = func_8000EB18(dx * dx + dz * dz);
            if (l2 != 0) {
                dx = (dx << 14) / l2;
                dz = (dz << 14) / l2;
            }
            if ((nx * dx + nz * dz) >> 14 < 0x3F07 || key != D_800F0798[i]->key || chg) {
                Gate *g;
                s16 *q, *q2;
                len = D_800F0798[i] - D_800F0774;
                g = (Gate *)((len << 6) + (u32)D_800F07F8);
                q = g->a;
                c[0] = q[0] + ox;
                c[1] = q[1];
                c[2] = q[2] + oz;
                q2 = g->b;
                e[0] = q2[0] + ox;
                e[1] = q2[1];
                e[2] = q2[2] + oz;
                c[0] *= 16;
                c[1] *= 16;
                c[2] *= 16;
                e[0] *= 16;
                e[1] *= 16;
                e[2] *= 16;
                D_800F0798[i] = func_8005DCFC(key, c, e, a, b, idx);
                D_800F0778[i] = a[1];
            } else {
                D_800F0798[i]->x = pos[0];
                D_800F0798[i]->z = pos[2];
                {
                Gate *g;
                len = D_800F0798[i] - D_800F0774;
                g = &D_800F07F8[len];
                g->a[0] = a[0] / 16 - ox;
                g->a[1] = a[1] / 16;
                g->a[2] = a[2] / 16 - oz;
                g->b[0] = b[0] / 16 - ox;
                g->b[1] = b[1] / 16;
                g->b[2] = b[2] / 16 - oz;
                }
                D_800F0778[i] = a[1];
            }
        } else {
            D_800F0798[i] = func_8005DCFC(key, a, b, a, b, idx);
            D_800F0778[i] = a[1];
        }
    }
}
