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

extern RigidBody gSoccerBall;

void soccer_ball_reset(s32 a0)
{
    gSoccerBall.pos.x = 0.0f;
    gSoccerBall.pos.z = 0.0f;
    gSoccerBall.type = 4;
    gSoccerBall.pos.y = 1.0f;
    gSoccerBall.mass = 125.0f;
    gSoccerBall.inertia = 31.25f;
    gSoccerBall.angVel.x = gSoccerBall.angVel.y = gSoccerBall.angVel.z = gSoccerBall.pos.x;
    gSoccerBall.vel.x = gSoccerBall.vel.y = gSoccerBall.vel.z = gSoccerBall.pos.x;
    gSoccerBall.acc.x = gSoccerBall.acc.y = gSoccerBall.acc.z = gSoccerBall.pos.x;
    gSoccerBall.angAcc.x = gSoccerBall.angAcc.y = gSoccerBall.angAcc.z = gSoccerBall.pos.x;
    gSoccerBall.quat[0] = 1.0f;
    gSoccerBall.quat[3] = gSoccerBall.quat[2] = gSoccerBall.quat[1] = gSoccerBall.pos.x;
    if (a0 != 0) {
        gSoccerBall.tree = collide_build_sphere_tree(D_800CC2F4);
        soccer_ball_init_contacts();
    }
}

void soccer_ball_update(void)
{
    RigidBody *b = &gSoccerBall;
    Vec3f v;
    f32 len;
    s32 i;

    b->acc.x = -b->vel.x / 15.0f;
    b->acc.y = -b->vel.y / 15.0f - 7.3500004f;
    b->acc.z = -b->vel.z / 15.0f;
    v = b->angVel;
    len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len != 0.0f) {
        v.x = v.x / len;
        v.y = v.y / len;
        v.z = v.z / len;
        b->angAcc.x = -v.x;
        b->angAcc.y = -v.y;
        b->angAcc.z = -v.z;
    }
    phys_body_step(b, sSoccerBallContacts, 0x14, 1);
    b->vel.x += b->acc.x * gPhysDeltaTime;
    b->vel.y += b->acc.y * gPhysDeltaTime;
    b->vel.z += b->acc.z * gPhysDeltaTime;
    b->angVel.x += b->angAcc.x * gPhysDeltaTime;
    b->angVel.y += b->angAcc.y * gPhysDeltaTime;
    b->angVel.z += b->angAcc.z * gPhysDeltaTime;
    for (i = 3; i >= 0; i--) {
        gRiders[i].engine.heat = 0.0f;
    }
}
