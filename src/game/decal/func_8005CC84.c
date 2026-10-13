#include "ctx.h"

typedef struct {
    u8 pad[0x48];
    Vec3f pos;
} Obj;

extern s32 D_800F0750;
extern Vec3f *D_800F0754;

void func_8005CC84(Obj *o)
{
    s32 n;
    s32 ni;
    Vec3f **gp;
    Vec3f *p;
    Vec3f *q;
    f32 dx;
    f32 dy;
    f32 dz;

    if (D_800F0750 < 2000) {
        gp = &D_800F0754;
        n = D_800F0750;
        p = (Vec3f *)((u32)(n * 12) + (u32)*gp);
        dx = p->x - o->pos.x;
        dy = p->y - o->pos.y;
        dz = p->z - o->pos.z;
        if (1.0f < dx * dx + dy * dy + dz * dz) {
            ni = n + 1;
            q = (Vec3f *)((u32)(ni * 12) + (u32)*gp);
            D_800F0750 = ni;
            *q = o->pos;
        }
    }
}
