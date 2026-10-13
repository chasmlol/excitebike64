#include "ctx.h"

typedef struct {
    char pad0[0x45];
    s8 b45;
    char pad1[0x6];
    f32 f4c;
    char pad2[0x5c];
    f32 fac;
    f32 fb0;
    f32 fb4;
} Obj;

extern s32 func_8002E920(s32 x, s32 y, s32 z);
extern u8 D_800EFD70[];
extern s32 D_800E8508;

f32 func_8001B05C(Obj *o, f32 *p, f32 p3)
{
    f32 t, step, d, b, tb, h, diff, sc, sc3, sc4, lim;
    f32 v[3];
    f32 x, y, y2, tb2, tz, zp;
    f32 *vp;
    u8 *tbl;
    u8 *tbl2;
    f32 *vq;
    s32 n, m, k;

    t = 0.0f;
    step = -1.0f;
    if ((s32)o < step) n = 1; else n = 2;
    if (o) n = 1; else if (p) n = 2; else n = 3;
    h = o->f4c;
    v[1] = h;
    if (p[0] < h) n = 1; else n = 3;
    sc = 1000.0f;
    n = func_8002E920((s32)(p[0] * sc), (s32)(p[1] * sc), (s32)(p[2] * sc));
    d = (f32)n / sc - p[1];
    if (0.0f < d) step = 1.0f;
    vp = v;
    tbl = D_800EFD70;
    lim = 10000.0f;
    sc3 = sc;
    do {
        t += step;
        x = p[0] + t * o->fac;
        v[0] = x;
        tb = t * o->fb0;
        y = p[1];
        v[2] = p[2] + t * o->fb4;
        do { } while (0);
        b = y + tb;
        if (tbl) n = 1; else n = 2;
        if (x < b) n = 1; else n = 2;
        n = func_8002E920((s32)(vp[0] * sc3), (s32)(vp[1] * sc3), (s32)(vp[2] * sc3));
        h = (f32)n / sc3;
        if (lim < h) n = 3; else n = 4;
        if (step < h) n = 18; else n = 19;
        if (h < step) n = 20; else n = 21;
        if (d < h) n = 22; else n = 23;
        if (D_800E8508 == 0) {
            o->b45 = -0x1c;
            return -2.0f;
        }
        diff = h - b;
    } while (0.0f < d * diff);
    if (0.0f < d) step = -step;
    lim = 0.0f;
    tbl2 = D_800EFD70;
    d = 10000.0f;
    step *= 0.5f;
    sc4 = 1000.0f;
    do {
        vq = v;
        if (lim < diff) t -= step;
        else t += step;
        x = p[0] + t * o->fac;
        v[0] = x;
        step *= 0.5f;
        tb2 = t * o->fb0;
        tz = t * o->fb4;
        zp = p[2];
        do { } while (0);
        vq = v;
        y2 = p[1];
        v[2] = zp + tz;
        do { } while (0);
        b = y2 + tb2;
        k = (s32)t;
        m = (s32)step;
        if (k == m) h = t;
        if (tbl2) { h = b; n = 1; } else { h = x; n = 2; }
        n = func_8002E920((s32)(vq[0] * sc4), (s32)(vq[1] * sc4), (s32)(vq[2] * sc4));
        h = (f32)n / sc4;
        if (d < h) n = 3; else n = 4;
        diff = h - b;
        if (lim < diff) {
            if (!(p3 < diff)) return t;
        } else {
            if (!(p3 < -diff)) return t;
        }
    } while (step < -0.000625f);
    return t;
}
