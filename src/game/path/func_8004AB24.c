#include "ctx.h"

typedef struct Seg {
    f32 x;        /* 0x00 */
    f32 y;        /* 0x04 */
    f32 z;        /* 0x08 */
    f32 f0c;
    f32 f10;
    f32 dx;       /* 0x14 */
    f32 dz;       /* 0x18 */
    u8 pad1c[0x46 - 0x1c];
    s16 h46;      /* 0x46 */
    s16 h48;
    s16 h4a;      /* 0x4a */
    u8 pad4c[0x6c - 0x4c];
} Seg;

extern Seg *D_800EE498;

s32 func_8004A788(Vec3f *pos, s32 start, s32 count, u8 mode);

void func_8004AB24(Vec3f *pos, s32 *area, f32 *t, s32 p4)
{
    s32 i;
    Seg *s;
    Seg *tab;
    Seg *a;
    Seg *b;
    f32 fa;
    f32 fb;
    f32 r;
    f32 d, m1, m2, m3, m4;

    i = func_8004A788(pos, p4, 4, 1);
    tab = D_800EE498;
    s = (Seg *)(i * sizeof(Seg) + (u32)tab);
    if ((pos->z - s->z) * s->dx - (pos->x - s->x) * s->dz < 0.0f) {
        i = s->h4a;
    }
    *area = i;
    a = (Seg *)(i * sizeof(Seg) + (u32)D_800EE498);
    b = (Seg *)(a->h46 * sizeof(Seg) + (u32)D_800EE498);
    d = b->z - pos->z;
    m1 = d * b->dx;
    d = b->x - pos->x;
    m2 = d * b->dz;
    d = a->z - pos->z;
    m3 = d * a->dx;
    d = a->x - pos->x;
    m4 = d * a->dz;
    fb = m1 - m2;
    fa = m3 - m4;
    r = fa / (fa - fb);
    *t = r;
    if (r < 0.0f) {
        *t = 0.0f;
    }
    if (1.0f < *t) {
        *t = 1.0f;
    }
}
