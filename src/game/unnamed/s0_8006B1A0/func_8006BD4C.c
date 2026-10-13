#include "ctx.h"

typedef struct { u8 pad[0x28]; u32 flags; u8 rest[0x14]; } Ent64;
typedef struct { f32 x, y, z; } V3;
typedef struct {
    u8 pad0[0x50];
    f32 f50;
    u8 pad54[0x8];
    f32 f5C;
    u8 pad60[0x4E4 - 0x60];
    Ent64 *e;
    u8 pad4E8[0x7A9 - 0x4E8];
    u8 b7A9;
} Obj;

extern u32 D_800E84F8;
extern Ent64 D_800C9080[];
extern f32 D_800C8E00;

f32 func_800228A0(V3 *p);

s32 func_8006BD4C(Obj *o, s32 n)
{
    V3 a, b;
    f32 z, bx, r1, r2, sgn, step;
    s32 i;
    u32 fl;

    if (o->e->flags & 0x400) {
        return 0;
    }
    a.x = (f32)n * 2.178f;
    bx = (f32)o->b7A9 * 2.178f;
    z = 0.0f;
    a.y = z;
    b.y = z;
    b.x = bx;
    a.z = o->f50;
    b.z = a.z;
    r1 = func_800228A0(&b);
    r2 = func_800228A0(&a);
    if (r1 != r2) {
ret0:
        return 0;
    }
    sgn = 1.0f;
    if (z < o->f5C) {
        sgn = -1.0f;
    }
    step = o->f5C * D_800C8E00 / 60.0f - sgn - sgn;
    for (i = 0; i < 5; i++) {
        b = a;
        b.z += (f32)i * step * 0.25f + sgn;
        func_800228A0(&b);
        if (D_800C9080[(D_800E84F8 >> 4) & 0xF].flags & 0x400) {
            goto ret0;
        }
    }
    fl = D_800C9080[(D_800E84F8 >> 4) & 0xF].flags & 0x400;
    return fl == 0;
}
