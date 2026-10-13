#include "ctx.h"

typedef struct V3 { s32 x, y, z; } V3;

typedef struct Facet {
    u8 p00[8];
    V3 v[3];
} Facet;

extern s32 D_800E92D4;
extern s32 D_800E92D8;
extern V3 D_800E92E8[];
extern V3 D_800E94F8[];

void func_8003BB1C(Facet *a, s32 lim)
{
    V3 *o;
    V3 *p;
    V3 *q;
    s32 i;
    s32 t;


    D_800E92E8[0] = a->v[0];
    D_800E92E8[1] = a->v[1];
    D_800E92E8[2] = a->v[2];
    D_800E92D4 = 3;
    o = D_800E94F8;
    i = 0;
    D_800E92D8 = 0;
    for (; i < D_800E92D4; i++) {
        p = &D_800E92E8[i];
        q = &D_800E92E8[(i + 1) % D_800E92D4];
        if (p->x <= lim) {
            *o++ = *p;
            D_800E92D8++;
        }
        if ((p->x <= lim && q->x > lim) || (p->x > lim && q->x <= lim)) {
            t = ((lim - p->x) << 8) / (q->x - p->x);
            o->x = lim;
            o->y = p->y + t * (q->y - p->y) / 256;
            o->z = p->z + t * (q->z - p->z) / 256;
            o++;
            D_800E92D8++;
        }
    }

    D_800E92D4 = D_800E92D8;
    o = D_800E92E8;
    i = 0;
    D_800E92D8 = 0;
        for (; i < D_800E92D4; i++) {
            p = &D_800E94F8[i];
            q = &D_800E94F8[(i + 1) % D_800E92D4];
            if (p->x >= -lim) {
                *o++ = *p;
                D_800E92D8++;
            }
            if ((p->x >= -lim && q->x < -lim) || (p->x < -lim && q->x >= -lim)) {
                t = ((-lim - p->x) << 8) / (q->x - p->x);
                o->x = -lim;
                o->y = p->y + t * (q->y - p->y) / 256;
                o->z = p->z + t * (q->z - p->z) / 256;
                o++;
                D_800E92D8++;
            }
        }

    D_800E92D4 = D_800E92D8;
    o = D_800E94F8;
    i = 0;
    D_800E92D8 = 0;
        for (; i < D_800E92D4; i++) {
            p = &D_800E92E8[i];
            q = &D_800E92E8[(i + 1) % D_800E92D4];
            if (p->z <= lim) {
                *o++ = *p;
                D_800E92D8++;
            }
            if ((p->z <= lim && q->z > lim) || (p->z > lim && q->z <= lim)) {
                t = ((lim - p->z) << 8) / (q->z - p->z);
                o->z = lim;
                o->y = p->y + t * (q->y - p->y) / 256;
                o->x = p->x + t * (q->x - p->x) / 256;
                o++;
                D_800E92D8++;
            }
        }

    D_800E92D4 = D_800E92D8;
    o = D_800E92E8;
    i = 0;
    D_800E92D8 = 0;
        for (; i < D_800E92D4; i++) {
            p = &D_800E94F8[i];
            q = &D_800E94F8[(i + 1) % D_800E92D4];
            if (p->z >= -lim) {
                *o++ = *p;
                D_800E92D8++;
            }
            if ((p->z >= -lim && q->z < -lim) || (p->z < -lim && q->z >= -lim)) {
                t = ((-lim - p->z) << 8) / (q->z - p->z);
                o->z = -lim;
                o->y = p->y + t * (q->y - p->y) / 256;
                o->x = p->x + t * (q->x - p->x) / 256;
                o++;
                D_800E92D8++;
            }
        }
}
