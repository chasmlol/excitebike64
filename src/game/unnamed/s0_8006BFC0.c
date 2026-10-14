#include "game.h"

void soccer_ball_init_contacts(void)
{
    s32 i;
    f32 sq, f6;

    for (i = 0; i < 5; i++) {
        f6 = ((f32)i * 6.2831855f) / 5.0f;
        sSoccerBallContacts[i].z = 0.9f;
        sq = sqrtf(0.19000006f);
        sSoccerBallContacts[i].x = sq * cosf(f6);
        sSoccerBallContacts[i].y = sq * sinf(f6);
    }
    for (i = 0; i < 5; i++) {
        f6 = ((f32)i * 6.2831855f) / 5.0f;
        sSoccerBallContacts[i + 5].z = 0.1f;
        sq = sqrtf(0.99f);
        sSoccerBallContacts[i + 5].x = sq * cosf(f6);
        sSoccerBallContacts[i + 5].y = sq * sinf(f6);
    }
    for (i = 0; i < 5; i++) {
        f6 = ((f32)i * 6.2831855f) / 5.0f + 3.1415927f;
        sSoccerBallContacts[i + 10].z = -0.1f;
        sq = sqrtf(0.99f);
        sSoccerBallContacts[i + 10].x = sq * cosf(f6);
        sSoccerBallContacts[i + 10].y = sq * sinf(f6);
    }
    for (i = 0; i < 5; i++) {
        f6 = ((f32)i * 6.2831855f) / 5.0f + 3.1415927f;
        sSoccerBallContacts[i + 15].z = -0.9f;
        sq = sqrtf(0.19000006f);
        sSoccerBallContacts[i + 15].x = sq * cosf(f6);
        sSoccerBallContacts[i + 15].y = sq * sinf(f6);
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

extern Body_5 gSoccerBall_3;

void soccer_ball_reset(s32 a0)
{
    gSoccerBall_3.pos.x = 0.0f;
    gSoccerBall_3.pos.z = 0.0f;
    gSoccerBall_3.type = 4;
    gSoccerBall_3.pos.y = 1.0f;
    gSoccerBall_3.f174 = 125.0f;
    gSoccerBall_3.f178 = 31.25f;
    gSoccerBall_3.angvel.x = gSoccerBall_3.angvel.y = gSoccerBall_3.angvel.z = gSoccerBall_3.pos.x;
    gSoccerBall_3.vel.x = gSoccerBall_3.vel.y = gSoccerBall_3.vel.z = gSoccerBall_3.pos.x;
    gSoccerBall_3.acc.x = gSoccerBall_3.acc.y = gSoccerBall_3.acc.z = gSoccerBall_3.pos.x;
    gSoccerBall_3.angacc.x = gSoccerBall_3.angacc.y = gSoccerBall_3.angacc.z = gSoccerBall_3.pos.x;
    gSoccerBall_3.q.q[0] = 1.0f;
    gSoccerBall_3.q.q[3] = gSoccerBall_3.q.q[2] = gSoccerBall_3.q.q[1] = gSoccerBall_3.pos.x;
    if (a0 != 0) {
        gSoccerBall_3.f7C = collide_build_sphere_tree(D_800CC2F4);
        soccer_ball_init_contacts();
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

extern Ent_75 gRiders_88[];

void soccer_ball_update(void)
{
    Body_6 *b = &gSoccerBall;
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
    phys_body_step(b, sSoccerBallContacts, 0x14, 1);
    b->pos.x += b->vel.x * gPhysDeltaTime;
    b->pos.y += b->vel.y * gPhysDeltaTime;
    b->pos.z += b->vel.z * gPhysDeltaTime;
    b->dir.x += b->ndir.x * gPhysDeltaTime;
    b->dir.y += b->ndir.y * gPhysDeltaTime;
    b->dir.z += b->ndir.z * gPhysDeltaTime;
    for (i = 3; i >= 0; i--) {
        gRiders_88[i].f648 = 0;
    }
}
