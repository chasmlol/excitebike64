#include "ctx.h"

typedef struct {
    u8 pad0[0xC];
    Vec3f pos;
    Vec3f vel;
    u8 pad24[0x1C];
    Vec3f dir;
    Vec3f ndir;
} Body;

typedef struct { u8 p0[0x648]; s32 f648; u8 p1[0x7E0 - 0x64C]; } Ent;

extern Body D_800F1510;
extern u8 D_800CCDE0[];
extern Ent D_800E37C0[];
extern f32 D_800E76C8;

s32 func_800453D8(Body *b, void *t, s32 n, u8 m);
f32 sqrtf(f32 x);

void func_8006C340(void)
{
    Body *b = &D_800F1510;
    Vec3f v;
    f32 len;
    s32 i;

    b->vel.x = -b->pos.x / 15.0f;
    b->vel.y = -b->pos.y / 15.0f - 7.3500004f;
    b->vel.z = -b->pos.z / 15.0f;
    v = b->dir;
    len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len != 0.0f) {
        v.x = v.x / len;
        v.y = v.y / len;
        v.z = v.z / len;
        b->ndir.x = -v.x;
        b->ndir.y = -v.y;
        b->ndir.z = -v.z;
    }
    func_800453D8(b, D_800CCDE0, 0x14, 1);
    b->pos.x += b->vel.x * D_800E76C8;
    b->pos.y += b->vel.y * D_800E76C8;
    b->pos.z += b->vel.z * D_800E76C8;
    b->dir.x += b->ndir.x * D_800E76C8;
    b->dir.y += b->ndir.y * D_800E76C8;
    b->dir.z += b->ndir.z * D_800E76C8;
    for (i = 3; i >= 0; i--) {
        D_800E37C0[i].f648 = 0;
    }
}
