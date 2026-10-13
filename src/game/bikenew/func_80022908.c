#include "ctx.h"

typedef struct {
    f32 x, y, z, w;
} Vec4f;

typedef struct {
    u8 pad0[0x48];
    Vec3f pos;
    u8 pad54[0xA0 - 0x54];
    f32 mtx[10];
    Vec3f pts[8];
} Obj;

extern Vec4f D_800C94C8[];
extern u32 D_800E76D0;
extern u32 D_800E7708;

void func_8001327C(f32 *v, f32 *m, f32 *o);

void func_80022908(Obj *p)
{
    s32 i;
    Vec3f out;

    D_800E7708 = D_800E76D0;
    for (i = 6; i < 8; i++) {
        func_8001327C(&D_800C94C8[i].x, p->mtx, &out.x);
        p->pts[i].x = out.x + p->pos.x;
        p->pts[i].y = out.y + p->pos.y;
        p->pts[i].z = out.z + p->pos.z;
    }
}
