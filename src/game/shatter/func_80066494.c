#include "ctx.h"

typedef struct {
    s16 x, y, z;
    s16 pad6;
    s32 f8, fC;
} MVtx; /* 0x10 */

typedef struct {
    u32 n;
    MVtx *v;
} Mesh;

typedef struct {
    Mesh *mesh;
    s32 f4, f8, fC, f10;
    u8 pad14[0xC];
} Ent; /* 0x20 */

typedef struct {
    u32 n;
    Ent e[1];
} Mdl;

typedef struct {
    s16 qx, qy, qz;
    s16 px, py, pz;
    s16 padC[3];
    s16 x[3];
    s16 y[3];
    s16 z[3];
    s16 pad24[4];
} Tri3; /* 0x2C */

typedef struct {
    Ent *ent;
    s32 n;
    s32 f8;
    s32 fC, f10, f14, f18;
    s32 min, max;
    s32 dx, dz;
    Tri3 *tri;
} Seg; /* 0x30 */

extern Mdl *D_800F1440;
extern Seg *D_800F1444;
extern s32 D_800CCCE0;
extern s32 *D_800E7834;
extern u8 *D_800C29E0;
extern u8 D_800D8D60[];

s32 func_80066320(Tri3 *t, s32 *outI, s32 *outN, s32 *ox, s32 *oy, s32 *oz);
s32 func_8000EB18(u32 r);
void *func_800BC908(s32 size, u32 flags, s32 owner, s32 tag);

void func_80066494(Mdl *p)
{
    s32 x, y, z;
    s32 a, b;
    s32 i;
    s32 j, k, m, cnt;
    s32 vy;
    Seg *s;
    Seg *sp;
    Mesh *me;
    Ent *e;
    Tri3 *t;

    j = 0;
    D_800F1440 = p;
    for (i = 0; i < p->n; i++) {
        if (D_800E7834[i] & 1) {
            j++;
        }
    }
    D_800CCCE0 = j;
    if (j == 0) {
        return;
    }
    sp = D_800F1444 = func_800BC908(j * 0x30, 0, (s32)D_800D8D60, 0x74);
    j = 0;
    for (i = 0; i < p->n; i++) {
        if (D_800E7834[i] & 1) {
            sp[j++].ent = &p->e[i];
        }
    }
    for (i = 0; i < D_800CCCE0; i++) {
        s = &D_800F1444[i];
        s->fC = s->ent->f4;
        e = s->ent;
        s->f10 = e->fC;
        s->f14 = e->f8;
        s->f18 = e->f10;
        x = s->f18 - s->f14;
        z = s->fC - s->f10;
        y = func_8000EB18(x * x + z * z) + 1;
        s->min = 999999;
        s->max = -999999;
        s->dx = (x << 14) / y;
        s->dz = (z << 14) / y;
    }
    for (i = 0; i < D_800CCCE0; i++) {
        s = &D_800F1444[i];
        me = s->ent->mesh;
        s->n = me->n / 3;
        s->tri = func_800BC908(s->n * 0x2C, 0, (s32)D_800D8D60, 0x96);
        for (j = 0; j < s->n; j++) {
            x = y = z = 0;
            for (a = 0; a < 3; a++) {
                vy = me->v[j * 3 + a].y;
                x += me->v[j * 3 + a].x;
                y += vy;
                z += me->v[j * 3 + a].z;
                if (s->max < vy) {
                    s->max = vy;
                }
                if (vy < s->min) {
                    s->min = vy;
                }
            }
            x = (x * 16) / 3;
            y = (y * 16) / 3;
            z = (z * 16) / 3;
            s->tri[j].px = x;
            s->tri[j].py = y;
            s->tri[j].pz = z;
            s->tri[j].qx = s->tri[j].px;
            s->tri[j].qy = s->tri[j].py;
            s->tri[j].qz = s->tri[j].pz;
            for (a = 0; a < 3; a++) {
                s->tri[j].x[a] = me->v[j * 3 + a].x * 16 - x;
                s->tri[j].y[a] = me->v[j * 3 + a].y * 16 - y;
                s->tri[j].z[a] = me->v[j * 3 + a].z * 16 - z;
            }
        }
        s->f8 = -1;
    }
    for (i = 0; i < D_800CCCE0; i++) {
        s = &D_800F1444[i];
        for (j = 0; j < s->n; j += cnt) {
            t = &s->tri[j];
            if (func_80066320(t, &a, &b, &x, &y, &z) == 0) {
                k = j + 1;
                for (m = k; m < s->n; m++) {
                    s32 x2, y2, z2;
                    Tri3 *u = &s->tri[m];
                    if (func_80066320(u, &a, &b, &x2, &y2, &z2) == 0 &&
                        x == x2 && y == y2 && z == z2) {
                        __builtin_memcpy(D_800C29E0, u, sizeof(Tri3));
                        __builtin_memcpy(u, &s->tri[k], sizeof(Tri3));
                        __builtin_memcpy(&s->tri[k], D_800C29E0, sizeof(Tri3));
                        __builtin_memcpy(D_800C29E0, &s->ent->mesh->v[m * 3], 0x30);
                        __builtin_memcpy(&s->ent->mesh->v[m * 3], &s->ent->mesh->v[k * 3], 0x30);
                        __builtin_memcpy(&s->ent->mesh->v[k * 3], D_800C29E0, 0x30);
                        k++;
                    }
                }
                cnt = k - j;
                if (cnt != 1) {
                    x = 0;
                    y = 0;
                    z = 0;
                    for (a = 0; a < cnt; a++) {
                        x += t[a].px;
                        y += t[a].py;
                        z += t[a].pz;
                    }
                    x /= cnt;
                    y /= cnt;
                    z /= cnt;
                    for (a = 0; a < cnt; a++) {
                        for (b = 0; b < 3; b++) {
                            t[a].x[b] += t[a].px;
                            t[a].y[b] += t[a].py;
                            t[a].z[b] += t[a].pz;
                        }
                        t[a].qx = t[a].px = x;
                        t[a].qy = t[a].py = y;
                        t[a].qz = t[a].pz = z;
                        for (b = 0; b < 3; b++) {
                            t[a].x[b] -= t[a].px;
                            t[a].y[b] -= t[a].py;
                            t[a].z[b] -= t[a].pz;
                        }
                    }
                }
            }
        }
    }
}
