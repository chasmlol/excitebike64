#include "ctx.h"

typedef struct { s32 f0; s32 len; s32 pad8[2]; void *data; } Clip;
typedef struct { s32 f0; s32 f4; Clip *f8; Clip *fC; } Seg;
typedef struct { u8 r, g, b; } RGB;
typedef struct { s16 x, y, z; RGB c[15]; u8 pad; } Rec34;
typedef struct { s16 x, y, z; u8 c[3]; u8 pad[3]; } Rec12;
typedef struct { s32 m[9]; } Mtx9;
typedef struct {
    u8 pad0[4];
    s32 f4, f8, fC;
    u8 pad10[0x134 - 0x10];
    s32 pos[3];
    Mtx9 rot;
} Bike;
typedef struct {
    s32 f0, f4, f8;
    s32 fC, f10, f14;
    u8 pad18[0x44 - 0x18];
    u8 f44;
    u8 pad45[0x65C - 0x45];
    Bike *bike;
    u8 pad660[0x7A5 - 0x660];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Player;
typedef struct { f32 x, y, z; u8 pad[0x60]; } Spot;

extern Spot *D_800EE498;
extern s32 D_800E1B74;
extern s32 D_800E1BF0[];
extern s32 D_800E1BA0[];
extern s32 D_800E1B9C;
#define LO9C (cam[1])
extern s32 D_800E1B70;
extern s32 D_800E1B7C;
extern s32 D_800E76D0;
extern s16 D_800F05CA;
extern Seg *D_800C28C0[];
extern Player D_800E37C0[];
extern s16 D_800C2C60[];
extern s32 D_800E1BC8[][3];
extern s16 D_800E1B88[][3];

void func_8004F410(s32 p);
void func_8003C4A4(s32 *param_1, s32 param_2);
void func_80012EBC(s32 *param_1, u16 *param_2, u16 *param_3, u16 *param_4);
void func_80011A58(s32 *param_1, s32 param_2, s32 param_3, s32 param_4);
s32 func_8002E920(s32 x, s32 y, s32 z);

static inline void rot(s32 *px, s32 *pz, s32 ang)
{
    s32 c = D_800C2C60[ang & 0xFFF];
    s32 s = D_800C2C60[(ang - 0x400) & 0xFFF];
    s32 t = (*px * c - *pz * s) >> 14;

    *pz = (*pz * c + *px * s) >> 14;
    *px = t;
}

static inline void rot2(s32 *px, s32 *pz, s32 x, s32 z, s32 ang)
{
    s32 c = D_800C2C60[ang & 0xFFF];
    s32 s = D_800C2C60[(ang - 0x400) & 0xFFF];
    s32 xc = x * c, zc = z * c, xs = x * s, zs = z * s;

    *pz = (zc + xs) >> 14;
    *px = (xc - zs) >> 14;
}

void func_8000C450(void)
{
    Mtx9 m;
    s32 vx, vz;
    u16 o0, o1, o2;
    s32 i, j, k, t, ang, c, s;
    Spot *e;
    Player *p;
    Bike *b;
    Rec34 *r;
    u8 *cp;
    u16 *cam;

    D_800E1BF0[0] = (s32)((f32)(s32)D_800EE498[D_800E1B74].x * 1000.0f);
    D_800E1BF0[2] = (s32)((f32)(s32)D_800EE498[D_800E1B74].z * 1000.0f);
    D_800E1BF0[1] = func_8002E920(D_800E1BF0[D_800E1B74], 999999, D_800E1BF0[2]);
    func_80011A58(D_800E1BA0, 0, 0, D_800E1B9C);
    if (D_800E1B70 != 0 && D_800E1B70 < 0x100) {
        D_800E1B70++;
    }
    if (D_800E1B70 == 0 || (D_800E1B70 < 0x100 && !(D_800E76D0 & 1))) {
        D_800E1B7C++;
    }
    if (D_800E1B70 != 0x100) {
        D_800E76D0++;
        if (D_800E76D0 & 1) {
            D_800E1B7C++;
        }
    }
    for (i = 0; i < D_800F05CA; i++) {
        cam = (u16 *)&D_800E1B9C;
        D_800E37C0[i].f7A5 = 0;
        D_800E37C0[i].f44 = 1;
        t = D_800E1B7C;
        b = D_800E37C0[i].bike;
        k = 0;
        while (t >= D_800C28C0[i][k].f8->len) {
            t -= D_800C28C0[i][k].f8->len;
            k++;
            if (D_800C28C0[i][k].f0 == 0) {
                k--;
                t %= D_800C28C0[i][k].f8->len;
                if (D_800E1B70 == 0) {
                    D_800E1B70 = 1;
                    func_8004F410(2);
                }
                break;
            }
        }
        { s32 off = t * sizeof(Rec34); r = (Rec34 *)((u8 *)D_800C28C0[i][k].f8->data + off); }
        { RGB *cc = r->c; for (j = 0; j < 15; j++) {
            s32 *dst = (s32 *)((u8 *)b + j * 0x80 + 0x10);
            s32 x, y, z;
            if (j == 0) dst = (s32 *)((u8 *)b + 0x40);
            x = cc[j].r << 8; y = cc[j].g << 8; z = cc[j].b << 8;
            if (j == 0) z += D_800E1B9C;
            func_80011A58(dst, x, y, z);
        } }
        ang = (s32)(LO9C << 16) >> 20;
        b->f4 = 0;
        b->f8 = 0;
        b->fC = 0;
        vx = r->x;
        vz = r->z;
        rot(&vx, &vz, ang);
        D_800E37C0[i].f0 = D_800E37C0[i].fC = D_800E1BF0[0] + vx;
        D_800E37C0[i].f4 = D_800E37C0[i].f10 = D_800E1BF0[1] + r->y;
        D_800E37C0[i].f8 = D_800E37C0[i].f14 = D_800E1BF0[2] + vz;
        func_8003C4A4((s32 *)b, 15);
        if (k != 0) {
            k = 1;
            t = D_800E1B7C;
            t -= D_800C28C0[i][0].f8->len;
            while (t >= D_800C28C0[i][k].fC->len) {
                t -= D_800C28C0[i][k].fC->len;
                k++;
                if (D_800C28C0[i][k].f4 == 0) {
                    k--;
                    if (i == 0) {
                        t = D_800C28C0[i][k].fC->len - 1;
                    } else {
                        t %= D_800C28C0[i][k].fC->len;
                    }
                    break;
                }
            }
            { s32 off = t * sizeof(Rec12); r = (Rec34 *)((u8 *)D_800C28C0[i][k].fC->data + off); }
            {
                s32 *px = &D_800E1BC8[i][0], *pz = &D_800E1BC8[i][2];
                s32 x = ((Rec12 *)r)->x, z = ((Rec12 *)r)->z;
                s32 a = (s32)(LO9C << 16) >> 20;
                s32 c = D_800C2C60[a & 0xFFF];
                s32 s = D_800C2C60[(a - 0x400) & 0xFFF];
                s32 zs, xs, zc, xc;

                xc = x * c;
                zc = z * c;
                xs = x * s;
                zs = z * s;
                *pz = (zc + xs) >> 14;
                *px = (xc - zs) >> 14;
            }
            D_800E1BC8[i][1] = ((Rec12 *)r)->y;
            D_800E1BC8[i][0] += D_800E1BF0[0];
            D_800E1BC8[i][1] += D_800E1BF0[1];
            D_800E1BC8[i][2] += D_800E1BF0[2];
            cp = (u8 *)((Rec12 *)r)->c;
            D_800E1B88[i][0] = cp[0];
            D_800E1B88[i][1] = cp[1];
            D_800E1B88[i][2] = cp[2] + LO9C;
        } else {
            s32 *q = D_800E1BC8[i];
            q[0] = b->pos[0] + D_800E37C0[i].fC;
            q[1] = b->pos[1] + D_800E37C0[i].f10;
            q[2] = b->pos[2] + D_800E37C0[i].f14;
            q[0] += (b->rot.m[3] * 200) >> 15;
            q[1] += (b->rot.m[4] * 200) >> 15;
            q[2] += (b->rot.m[5] * 200) >> 15;
            q[0] += (b->rot.m[6] * 50) >> 15;
            q[1] += (b->rot.m[7] * 50) >> 15;
            q[2] += (b->rot.m[8] * 50) >> 15;
            m = b->rot;
            func_80012EBC(m.m, &o0, &o1, &o2);
            D_800E1B88[i][0] = o0;
            D_800E1B88[i][1] = o1;
            D_800E1B88[i][2] = o2;
        }
    }
}
