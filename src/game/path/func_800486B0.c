#include "ctx.h"

typedef struct Rec {
    f32 x, y, z, w;
    s32 pad10;
    f32 nx;
    f32 ny;
    u8 pad1[0x46 - 0x1C];
    s16 a;
    s16 b;
    s16 c;
    u8 pad2[0x6C - 0x4C];
} Rec;

extern s32 D_800EE490;
extern Rec *D_800EE498;

void func_800486B0(void)
{
    s32 i;
    Rec *p;
    Rec *a;
    Rec *c;
    f32 sz;
    f32 sx;
    f32 len;

    for (i = 0; i < D_800EE490; i++) {
        p = (Rec *)(i * sizeof(Rec) + (u32)D_800EE498);
        a = (Rec *)(p->a * sizeof(Rec) + (u32)D_800EE498);
        c = (Rec *)(p->c * sizeof(Rec) + (u32)D_800EE498);
        sz = (a->z - p->z) / p->w;
        sz = sz * p->w + ((p->z - c->z) / c->w) * c->w;
        sx = -(a->x - p->x) / p->w;
        sx = sx * p->w + (-(p->x - c->x) / c->w) * c->w;
        len = __builtin_sqrtf(sz * sz + sx * sx);
        if (0.0f < len) {
            D_800EE498[i].nx = sz / len;
            D_800EE498[i].ny = sx / len;
        }
    }
}
