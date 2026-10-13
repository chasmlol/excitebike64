#include "game.h"

void func_8006BFC0(void)
{
    s32 i;
    f32 sq, f6;

    for (i = 0; i < 5; i++) {
        f6 = ((f32)i * 6.2831855f) / 5.0f;
        D_800CCDE0[i].z = 0.9f;
        sq = sqrtf(0.19000006f);
        D_800CCDE0[i].x = sq * func_800ADFC0(f6);
        D_800CCDE0[i].y = sq * func_800AEA30(f6);
    }
    for (i = 0; i < 5; i++) {
        f6 = ((f32)i * 6.2831855f) / 5.0f;
        D_800CCDE0[i + 5].z = 0.1f;
        sq = sqrtf(0.99f);
        D_800CCDE0[i + 5].x = sq * func_800ADFC0(f6);
        D_800CCDE0[i + 5].y = sq * func_800AEA30(f6);
    }
    for (i = 0; i < 5; i++) {
        f6 = ((f32)i * 6.2831855f) / 5.0f + 3.1415927f;
        D_800CCDE0[i + 10].z = -0.1f;
        sq = sqrtf(0.99f);
        D_800CCDE0[i + 10].x = sq * func_800ADFC0(f6);
        D_800CCDE0[i + 10].y = sq * func_800AEA30(f6);
    }
    for (i = 0; i < 5; i++) {
        f6 = ((f32)i * 6.2831855f) / 5.0f + 3.1415927f;
        D_800CCDE0[i + 15].z = -0.9f;
        sq = sqrtf(0.19000006f);
        D_800CCDE0[i + 15].x = sq * func_800ADFC0(f6);
        D_800CCDE0[i + 15].y = sq * func_800AEA30(f6);
    }
}

typedef struct { f32 q[4]; } Quat_3;

typedef struct Body_5 {
    Vec3f pos;          /* 0x00 */
    Vec3f vel;          /* 0x0C */
    Vec3f acc;          /* 0x18 */
    u8 pad24[0xC];
    Quat_3 q;             /* 0x30 */
    Vec3f angvel;       /* 0x40 */
    Vec3f angacc;       /* 0x4C */
    f32 m[9];           /* 0x58 */
    s32 f7C;            /* 0x7C */
    Vec3f pts[20];      /* 0x80 */
    f32 f170;           /* 0x170 */
    f32 f174;
    f32 f178;
    u8 type;            /* 0x17C */
} Body_5;

extern Body_5 D_800F1510_3;

void func_8006C28C(s32 a0)
{
    D_800F1510_3.pos.x = 0.0f;
    D_800F1510_3.pos.z = 0.0f;
    D_800F1510_3.type = 4;
    D_800F1510_3.pos.y = 1.0f;
    D_800F1510_3.f174 = 125.0f;
    D_800F1510_3.f178 = 31.25f;
    D_800F1510_3.angvel.x = D_800F1510_3.angvel.y = D_800F1510_3.angvel.z = D_800F1510_3.pos.x;
    D_800F1510_3.vel.x = D_800F1510_3.vel.y = D_800F1510_3.vel.z = D_800F1510_3.pos.x;
    D_800F1510_3.acc.x = D_800F1510_3.acc.y = D_800F1510_3.acc.z = D_800F1510_3.pos.x;
    D_800F1510_3.angacc.x = D_800F1510_3.angacc.y = D_800F1510_3.angacc.z = D_800F1510_3.pos.x;
    D_800F1510_3.q.q[0] = 1.0f;
    D_800F1510_3.q.q[3] = D_800F1510_3.q.q[2] = D_800F1510_3.q.q[1] = D_800F1510_3.pos.x;
    if (a0 != 0) {
        D_800F1510_3.f7C = func_80060270(D_800CC2F4);
        func_8006BFC0();
    }
}

typedef struct {
    u8 pad0[0xC];
    Vec3f pos;
    Vec3f vel;
    u8 pad24[0x1C];
    Vec3f dir;
    Vec3f ndir;
} Body_6;

typedef struct { u8 p0[0x648]; s32 f648; u8 p1[0x7E0 - 0x64C]; } Ent_75;

extern Ent_75 D_800E37C0_88[];

void func_8006C340(void)
{
    Body_6 *b = &D_800F1510;
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
        D_800E37C0_88[i].f648 = 0;
    }
}
