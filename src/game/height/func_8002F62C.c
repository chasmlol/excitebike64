#include "ctx.h"

typedef struct { s16 x, y, z, pad[5]; } MVtx;
typedef struct { s32 pad; MVtx *v; } Mesh;
typedef struct { u32 m0, m1; u16 v[3]; u16 flags; } Ent;
typedef struct { u16 flags; u16 n; Ent e[1]; } TList;
typedef struct { Mesh *m; s32 x0, z0, x1, z1; TList *tl; u8 pad[8]; } Obj;
typedef struct { s32 pad; Obj o; } Rec;
typedef struct { s32 id; s32 n; s32 v[3][3]; } CTri;
typedef struct { u8 pad[0x2C]; s32 f2C; } Cfg;

extern s32 D_800E8D68;
extern CTri D_800E80D8[];
extern s8 *D_800EA990;
extern s32 D_800E8940[2];
extern s32 D_800E8950[2];
typedef struct { u16 n; u16 id[1]; } Cell;
extern Cell **D_800E8D60;
extern u8 *D_800E8948;
extern Cfg *D_800EFD64;

void func_8000ECB8(s32) __attribute__((noreturn));
void func_8004262C(s32 a, s32 b, s32 *q, s32 *r);

static inline void tri(s32 id, s32 n, s32 ax, s32 az, s32 bx, s32 bz, s32 cx, s32 cz) {
    D_800E80D8[D_800E8D68].id = id;
    D_800E80D8[D_800E8D68].n = n;
    D_800E80D8[D_800E8D68].v[0][0] = ax * 2000;
    D_800E80D8[D_800E8D68].v[0][1] = D_800EA990[((az & 0x1FF) << 9) | (ax & 0x1FF)] << 6;
    D_800E80D8[D_800E8D68].v[0][2] = az * 2000;
    D_800E80D8[D_800E8D68].v[1][0] = bx * 2000;
    D_800E80D8[D_800E8D68].v[1][1] = D_800EA990[((bz & 0x1FF) << 9) | (bx & 0x1FF)] << 6;
    D_800E80D8[D_800E8D68].v[1][2] = bz * 2000;
    D_800E80D8[D_800E8D68].v[2][0] = cx * 2000;
    D_800E80D8[D_800E8D68].v[2][1] = D_800EA990[((cz & 0x1FF) << 9) | (cx & 0x1FF)] << 6;
    D_800E80D8[D_800E8D68].v[2][2] = cz * 2000;
    D_800E8D68++;
}

void func_8002F62C(s32 x0, s32 z0, s32 x1, s32 z1, s32 hy) {

    s32 r[4];
    union { u16 h[4]; u32 w[2]; } mask;
    s32 code[3];
    s32 idx;
    s32 hmin;
    s32 x, z, gx, gz, cx, cz, cell, k, j, n;
    s32 ox, oz, dx, dz;
    s32 bx0, bz0, bx1, bz1, bit;
    u16 f;
    Obj *o;
    TList *tl;
    Ent *en;
    MVtx *vt;
    u16 *vi;
    u16 *mp;
    CTri *t;
    s32 *p;

    x0 >>= 4;
    x1 >>= 4;
    z0 >>= 4;
    z1 >>= 4;
    D_800E8D68 = 0;
    hy >>= 4;
    if (D_800EFD64->f2C == 2) {
        func_8004262C(x0, 125, &r[0], &idx);
        func_8004262C(z0, 125, &r[2], &idx);
        func_8004262C(x1 + 124, 125, &r[1], &idx);
        func_8004262C(z1 + 124, 125, &r[3], &idx);
        for (k = r[2]; k < r[3]; k++) {
            for (n = r[0]; n < r[1]; n++) {
                tri(idx, n * 2 + k * 8, n, k, n + 1, k, n + 1, k + 1);
                if (D_800E8D68 == 24) return;
                tri(idx, n * 2 + k * 8, n, k, n + 1, k + 1, n, k + 1);
                if (D_800E8D68 == 24) return;
            }
        }
        return;
    }
    gx = (x0 + x1) >> 1;
    gz = (z0 + z1) >> 1;
    gx = ((gx - D_800E8940[0]) << 4) / (D_800E8950[0] - D_800E8940[0]);
    if (gx < 0) gx = 0;
    if (gx >= 16) gx = 15;
    gz = ((gz - D_800E8940[0]) << 4) / (D_800E8950[1] - D_800E8940[1]);
    if (gz < 0) gz = 0;
    if (gz >= 16) gz = 15;
    cell = gz * 16 + gx;
    for (idx = 0; idx < D_800E8D60[cell]->n; idx++) {
        o = (Obj *)(D_800E8948 + ((D_800E8D60[cell]->id[idx] << 5) + 4));
        if (x1 < o->x0) continue;
        if (z1 < o->z0) continue;
        if (o->x1 < x0) continue;
        if (o->z1 < z0) continue;
        if (!(o->tl->flags & 1)) func_8000ECB8(1);
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
            { CTri *b = D_800E80D8; t = &b[D_800E8D68]; }
            t->n = n;
            t->id = idx;
            for (k = 0; k < 3; k++) {
                D_800E80D8[D_800E8D68].v[k][0] = (cx + vt[vi[k]].x) * 16;
                D_800E80D8[D_800E8D68].v[k][1] = vt[vi[k]].y << 4;
                D_800E80D8[D_800E8D68].v[k][2] = (cz + vt[vi[k]].z) * 16;
            }
            if (++D_800E8D68 == 24) return;
        }
    }
}
