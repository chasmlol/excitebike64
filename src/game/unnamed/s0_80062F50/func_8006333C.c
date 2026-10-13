#include "ctx.h"

typedef struct Player {
    u8 pad0[0x34];
    s16 ang;
    u8 pad36[0x48 - 0x36];
    f32 x;
    f32 y;
    f32 z;
    u8 pad54[0x62C - 0x54];
    f32 f62C;
    u8 pad630[0x7A5 - 0x630];
    u8 f7A5;
    u8 pad7A6[0x7B0 - 0x7A6];
    s32 f7B0;
    u8 pad7B4[0x7D8 - 0x7B4];
    f32 f7D8;
} Player;

extern s32 D_800E76D0;
void func_8001254C(s32 x, s32 y, s16 *pAng, s32 *pRatio);

s32 func_8006333C(Player *p, Player *q)
{
    s16 ang;
    s32 ratio;
    s32 v;
    s32 dt;
    s32 ret;
    f32 dy;
    f32 dx;
    f32 dz;

    if (q->f7A5 != 0) {
        goto fail;
    }
    dt = D_800E76D0 - q->f7B0;
    if (dt >= 0xB4) {
        goto ok;
    }
fail:
    return 0;
ok:
    if (dt >= 0x385) {
        q->f7D8 = 0.2f;
    }
    dy = q->f62C - p->f62C;
    if (dy < 20.0f && -50.0f < dy) {
        goto fail;
    }
    dx = q->x - p->x;
    dz = q->z - p->z;
    if (dx * dx + dz * dz < 225.0f) {
        goto fail;
    }
    func_8001254C((s32)(-dx * 1024.0f), (s32)(dz * 1024.0f), &ang, &ratio);
    ang -= p->ang;
    if ((ang < 0 ? -ang : ang) < 0x1800) {
        goto fail;
    }
    ret = 1;
    q->f7A5 = ret;
    return ret;
}
