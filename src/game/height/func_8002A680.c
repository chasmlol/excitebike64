#include "ctx.h"

extern s32 D_800C9758;
extern s32 D_800C975C;
extern s32 D_800C9684;

typedef struct {
    s32 x, y, z, w3, w4, w5;
} TblEnt;
extern TblEnt D_800E8030[];

typedef struct {
    f32 x, y, z;
} V3;

typedef struct {
    u8 pad0[0x34];
    u16 s34;
    u8 pad1[0x0E];
    u8 f44;
    u8 pad2[0x03];
    V3 pos;
    u8 pad3[0x5D0];
    u16 c624;
} Obj;

void func_80012DC8(s32 *a, s32 *b, s32 *c);
f32 func_8002A4EC(u8 *p);
void func_80011A58(s32 *a, s32 b, s32 c, s32 d);
void func_8002B348(s32 *p);
void func_8001254C(s32 a, s32 b, s16 *c, s32 *d);

void func_8002A680(Obj *p, s32 *out)
{
    s32 v[4];
    s32 r[4];
    s32 m[10];
    s16 ang;
    s32 dist;
    TblEnt *tb;
    TblEnt *base;
    s32 z2, a, b;
    V3 *pv;
    s32 z, d;
    s16 rot, h;

    if (p->f44 == 0) {
        rot = p->s34;
        h = func_8002A4EC((u8 *)p);
        func_80011A58(m, h, 0, rot);
        v[0] = 0;
        v[1] = 0;
        v[2] = -D_800C9758;
        func_80012DC8(v, m, r);
        pv = &p->pos;
        out[0] = r[0] + pv->x * 1000.0f;
        out[1] = r[1] + pv->y * 1000.0f + D_800C975C;
        out[2] = r[2] + pv->z * 1000.0f;
        out[3] = h;
        out[4] = 0;
        out[5] = rot;
    } else {
        base = D_800E8030;
        tb = &base[D_800C9684];
        out[0] = tb->x / 16;
        out[1] = tb->y / 16;
        z = tb->z;
        if (z < 0) {
            z += 0xF;
        }
        z2 = z >> 4;
        out[2] = z2;
        a = out[0] - (s32)(p->pos.x * 1000.0f);
        b = (s32)(p->pos.z * 1000.0f) - z2;
        d = (s32)(p->pos.y * 1000.0f) + D_800C975C - out[1];
        func_8001254C(a, b, &ang, &dist);
        if (dist > 10000) {
            p->c624 += 3;
        }
        out[5] = ang;
        func_8001254C(-d, dist, &ang, &dist);
        out[3] = ang;
        out[4] = 0;
    }
    func_8002B348(out);
}
