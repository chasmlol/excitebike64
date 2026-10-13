#include "ctx.h"

typedef struct {
    u8 pad[0x6C];
    f32 x;
    f32 y;
    f32 z;
    u8 pad2[0x7E0 - 0x78];
} Bike;

typedef struct {
    s32 x, y, z;
} V3i;

typedef struct {
    s16 v;
} Ang;

typedef struct {
    s32 x, y, z;
    s32 pad0C, pad10;
    s32 w14;
} Seg;

extern Bike D_800E37C0[];
extern V3i D_800E8090[];
extern s32 D_800E8000[];
extern Seg D_800E7998[][16];
extern Seg D_800E8030[];

void func_8001254C(s32 a, s32 b, s16 *c, s32 *d);

void func_8002CA70(s32 p1, s32 p2, s32 p3, s32 p4)
{
    Ang a;
    s32 b;
    Ang c;
    s32 dx, dz, d;
    s32 i;

    dx = D_800E37C0[p2].x * 1000.0f;
    dx -= D_800E8090[p1].x;
    dz = D_800E37C0[p2].z * 1000.0f;
    dz -= D_800E8090[p1].z;
    func_8001254C(dx, dz, &a.v, &b);
    func_8001254C(dx - p3, dz - p4, &c.v, &b);
    D_800E8090[p1].x += p3;
    D_800E8090[p1].z += p4;
    D_800E8000[p1] -= (d = c.v - a.v);
    if (b > 0x1000) {
        for (i = 0; i < 16; i++) {
            D_800E7998[p1][i].x += p3;
            D_800E7998[p1][i].z += p4;
            D_800E7998[p1][i].w14 -= d;
        }
        D_800E8030[p1].x += p3 * 16;
        D_800E8030[p1].z += p4 * 16;
    }
}
