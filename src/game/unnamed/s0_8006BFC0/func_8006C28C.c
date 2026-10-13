#include "ctx.h"

typedef struct { f32 q[4]; } Quat;

typedef struct Body {
    Vec3f pos;          /* 0x00 */
    Vec3f vel;          /* 0x0C */
    Vec3f acc;          /* 0x18 */
    u8 pad24[0xC];
    Quat q;             /* 0x30 */
    Vec3f angvel;       /* 0x40 */
    Vec3f angacc;       /* 0x4C */
    f32 m[9];           /* 0x58 */
    s32 f7C;            /* 0x7C */
    Vec3f pts[20];      /* 0x80 */
    f32 f170;           /* 0x170 */
    f32 f174;
    f32 f178;
    u8 type;            /* 0x17C */
} Body;

extern Body D_800F1510;
extern char D_800CC2F4[];
s32 func_80060270(char *p);
void func_8006BFC0(void);


void func_8006C28C(s32 a0)
{
    D_800F1510.pos.x = 0.0f;
    D_800F1510.pos.z = 0.0f;
    D_800F1510.type = 4;
    D_800F1510.pos.y = 1.0f;
    D_800F1510.f174 = 125.0f;
    D_800F1510.f178 = 31.25f;
    D_800F1510.angvel.x = D_800F1510.angvel.y = D_800F1510.angvel.z = D_800F1510.pos.x;
    D_800F1510.vel.x = D_800F1510.vel.y = D_800F1510.vel.z = D_800F1510.pos.x;
    D_800F1510.acc.x = D_800F1510.acc.y = D_800F1510.acc.z = D_800F1510.pos.x;
    D_800F1510.angacc.x = D_800F1510.angacc.y = D_800F1510.angacc.z = D_800F1510.pos.x;
    D_800F1510.q.q[0] = 1.0f;
    D_800F1510.q.q[3] = D_800F1510.q.q[2] = D_800F1510.q.q[1] = D_800F1510.pos.x;
    if (a0 != 0) {
        D_800F1510.f7C = func_80060270(D_800CC2F4);
        func_8006BFC0();
    }
}

