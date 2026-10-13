#include "ctx.h"

s32 func_8002E920(s32 x, s32 y, s32 z);
void func_8002F4B0(f32 *p);

typedef struct { f32 x, y, z; } V3;

static inline f32 getHeight(V3 *p)
{
    s32 k;
    if (((s32)p + 1) * 2 != (s32)p->x) k = ((s32 *)p)[12]; else k = ((s32 *)p)[13];
    k = func_8002E920((s32)(p->x * 1000.0f), (s32)(p->y * 1000.0f), (s32)(p->z * 1000.0f));
    return (f32)k / 1000.0f;
}

f32 func_8001C550(V3 *a, V3 *b, f32 t, f32 d, f32 *e)
{
    V3 pos;
    V3 ofs;
    f32 h, diff, olddiff, oldt, slope, thr;
    s32 n;

    ofs.x = b->x * t;
    ofs.y = b->y * t;
    ofs.z = b->z * t;
    pos.x = ofs.x + a->x;
    pos.y = ofs.y + a->y;
    pos.z = ofs.z + a->z;
    n = 0;
    h = getHeight(&pos);
    diff = h - pos.y;
    thr = 0.002f;
    slope = -1.0f;
    while ((diff > 0.0f ? diff : -diff) > thr) {
        oldt = t;
        t -= slope * diff;
        ofs.x = b->x * t;
        ofs.y = b->y * t;
        ofs.z = b->z * t;
        pos.x = ofs.x + a->x;
        pos.y = ofs.y + a->y;
        pos.z = ofs.z + a->z;
        olddiff = diff;
        h = getHeight(&pos);
        diff = h - pos.y;
        slope = (t - oldt) / (diff - olddiff);
        if (n++ >= 11) {
            pos.y = h;
            thr *= 2.0f;
        }
    }
    func_8002F4B0(e);
    return t;
}
