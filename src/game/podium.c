#include "game.h"

typedef struct { u8 pad[0x10]; void *p; } Obj_3;

extern Obj_3 *D_800E1B80;
extern Obj_3 *D_800E1B78;

typedef struct Rec_7 {
    s16 x, y, z, a, b, c;
} Rec_7;

typedef struct Seg {
    s32 f00;
    s32 len;
    s32 f08;
    s32 f0c;
    Rec_7 *data;
} Seg;

typedef struct Entry_3 {
    s32 w0;
    Seg *seg;
} Entry_3;

extern Entry_3 D_800C28CC[];

void func_8000BD10(void)
{
    s32 t = D_800E1B7C;
    s32 n = 0;
    Rec_7 *p;
    s16 *tp;
    s32 o;
    u16 *ap;
    s32 ang, s, c, A, B, X, Y, as, bs, y2, x2;

    while (t >= D_800C28CC[n].seg->len) {
        t -= D_800C28CC[n].seg->len;
        n++;
        if (D_800C28CC[n].w0 == 0) {
            n--;
            t = D_800C28CC[n].seg->len - 1;
            break;
        }
    }

    o = t * 12;
    p = (Rec_7 *)((u8 *)D_800C28CC[n].seg->data + o);
    A = p->x;
    D_800E8090_5.x = A;
    B = p->z;
    B = (s16)B;
    D_800E8090_5.z = B;
    tp = D_800C2C60;
    ang = (s16)D_800E1B9C >> 4;
    s = tp[ang & 0xFFF];
    c = tp[(ang - 0x400) & 0xFFF];
    as = A * s;
    bs = B * s;
    Y = (bs + A * c) >> 14;
    X = (as - B * c) >> 14;
    D_800E8090_5.z = Y;
    x2 = D_800E1BF0_2.x;
    y2 = Y + ((volatile Vec *)&D_800E1BF0_2)->z;
    D_800E8090_5.x = X;
    D_800E8090_5.x = X + x2;
    D_800E8090_5.z = y2;
    D_800E8090_5.y = p->y + D_800E1BF0_2.y;
    D_800E7978_2 = p->a;
    D_800E78B8_2 = p->b;
    D_800E8000_2 = p->c + D_800E1B9C;
}

typedef struct { s32 *a; s32 *c; Obj_3 *b; Obj_3 *d; } Rec_8;
typedef struct { s32 *a; Obj_3 *b; } Rec2;

extern Rec_8 *D_800C28C0[];
extern Rec2 D_800C28CC_2[];
extern Obj_3 *D_800E1BC4;

void func_8000BEA0(void)
{
    s32 i;
    Rec_8 *r;
    Obj_3 *o;

    D_800E76D0 = 0;
    D_800E1B7C = 0;
    D_800E1B9C = 0x8000;
    i = ((D_800EFD64_6 - (s32)D_800CB210) * -0x33333333) >> 5;
    D_800E1B74 = 0;
    if (i == 9) {
        D_800E1B74 = 0x46;
    } else if (i == 0xe) {
        D_800E1B74 = 100;
        D_800E1B9C = 24000;
    } else if (i == 0x13) {
        D_800E1B74 = 0x14;
        D_800E1B9C = -2000;
    }
    i = 0;
    D_800E1B70 = 0;
    func_8004C22C(D_800D11E4);
    D_800E1BC4 = func_800BF79C(D_800E7728, D_800D10CC);
    D_800E1B80 = func_800BF79C(D_800E7728, D_800D0F30);
    D_800E1BEC = func_800BC908(0x200, i, D_800D6000, 0xFE);
    D_800E1B78 = func_800BF79C(D_800E7728, D_800D0E34);

    for (i = 0; i < 3; i++) {
        for (r = D_800C28C0[i]; r->a != 0; r++) {
            r->b = func_800BF79C(D_800E7728, *r->a);
            r->b->p = (u8 *)r->b + 0x14;
            if (r->c != 0) {
                r->d = func_800BF79C(D_800E7728, *r->c);
                r->d->p = (u8 *)r->d + 0x1C;
            }
        }
    }
    for (i = 0; D_800C28CC_2[i].a != 0; i++) {
        D_800C28CC_2[i].b = func_800BF79C(D_800E7728, *D_800C28CC_2[i].a);
        D_800C28CC_2[i].b->p = (u8 *)D_800C28CC_2[i].b + 0x1C;
    }
    func_80069CA0(D_800E1BC4, 0);
}

#define W2(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }
#define W2R(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w1 = (b); _g->words.w0 = (a); }

typedef struct {
    u8 pad[0x14];
    u32 tex;
    u8 pad2[0xA80 - 0x18];
} Plr_3;

extern Plr_3 D_800DC150_8[];

Gfx *func_8000C0E0(Gfx *gdl, s32 x, s32 arg2)
{
    s32 i;
    s32 off;
    Plr_3 *pl;
    Plr_3 *cur;
    s32 *pc;
    s16 *ps;

    func_80002418(gdl, x, 0, 1);
    W2(gdl++, 0xE3001001, 0x8000);
    W2(gdl++, 0xE200001C, 0x442230);
    W2(gdl++, 0xFCFFFFFF, 0xFFFCF87C);
    W2(gdl++, 0xFD100000, D_800E1BC4_2[1]);
    W2(gdl++, 0xE8000000, 0);
    W2(gdl++, 0xF5000100, 0x07000000);
    W2(gdl++, 0xE6000000, 0);
    W2(gdl++, 0xF0000000, 0x073FC000);
    W2(gdl++, 0xE7000000, 0);
    i = 0;
    func_8000B938(D_800E1BEC + D_800E30A8 * 0x100, 0, 0, D_800E1B9C, D_800E1BF0[0], D_800E1BF0[1], D_800E1BF0[2], 0x10, 0);
    W2(gdl++, 0xDA380003, D_800E1BEC + D_800E30A8 * 0x100);
    gdl = func_80013D70(0, D_800E1B80, &D_800E1BC4_2, gdl);
    for (; i < D_800F05A8.n; i++) {
        func_8000B938((s32)&((Mtx *)(D_800E1BEC + D_800E30A8 * 0x100))[i + 1], D_800E1B88[i][0], D_800E1B88[i][1], D_800E1B88[i][2], D_800E1BC8[i][0], D_800E1BC8[i][1], D_800E1BC8[i][2], 0x10, 0);
        W2(gdl++, 0xDA380003, (s32)&((Mtx *)(D_800E1BEC + D_800E30A8 * 0x100))[i + 1]);
        W2(gdl++, 0xFD100000, D_800DC150_8[i].tex);
        W2(gdl++, 0xE8000000, 0);
        W2(gdl++, 0xF5000100, 0x07000000);
        W2(gdl++, 0xE6000000, 0);
        W2(gdl++, 0xF0000000, 0x073FC000);
        W2(gdl++, 0xE7000000, 0);
        gdl = func_80013D70(0, D_800E1B78, &D_800DC150_8[i], gdl);
    }
    return gdl;
}

#define LO9C (cam[1])

typedef struct { s32 f0; s32 len; s32 pad8[2]; void *data; } Clip;
typedef struct { s32 f0; s32 f4; Clip *f8; Clip *fC; } Seg_2;
typedef struct { s16 x, y, z; Rgb c[15]; u8 pad; } Rec34;
typedef struct { s16 x, y, z; u8 c[3]; u8 pad[3]; } Rec12;

typedef struct {
    u8 pad0[4];
    s32 f4, f8, fC;
    u8 pad10[0x134 - 0x10];
    s32 pos[3];
    Mtx3 rot;
} Bike_2;

typedef struct {
    s32 f0, f4, f8;
    s32 fC, f10, f14;
    u8 pad18[0x44 - 0x18];
    u8 f44;
    u8 pad45[0x65C - 0x45];
    Bike_2 *bike;
    u8 pad660[0x7A5 - 0x660];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Player_6;

extern Seg_2 *D_800C28C0_2[];
extern Player_6 D_800E37C0_10[];

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
    Mtx3 m;
    s32 vx, vz;
    u16 o0, o1, o2;
    s32 i, j, k, t, ang, c, s;
    Spot *e;
    Player_6 *p;
    Bike_2 *b;
    Rec34 *r;
    u8 *cp;
    u16 *cam;

    D_800E1BF0[0] = (s32)((f32)(s32)D_800EE498_2[D_800E1B74].x * 1000.0f);
    D_800E1BF0[2] = (s32)((f32)(s32)D_800EE498_2[D_800E1B74].z * 1000.0f);
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
        D_800E37C0_10[i].f7A5 = 0;
        D_800E37C0_10[i].f44 = 1;
        t = D_800E1B7C;
        b = D_800E37C0_10[i].bike;
        k = 0;
        while (t >= D_800C28C0_2[i][k].f8->len) {
            t -= D_800C28C0_2[i][k].f8->len;
            k++;
            if (D_800C28C0_2[i][k].f0 == 0) {
                k--;
                t %= D_800C28C0_2[i][k].f8->len;
                if (D_800E1B70 == 0) {
                    D_800E1B70 = 1;
                    func_8004F410(2);
                }
                break;
            }
        }
        { s32 off = t * sizeof(Rec34); r = (Rec34 *)((u8 *)D_800C28C0_2[i][k].f8->data + off); }
        { Rgb *cc = r->c; for (j = 0; j < 15; j++) {
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
        D_800E37C0_10[i].f0 = D_800E37C0_10[i].fC = D_800E1BF0[0] + vx;
        D_800E37C0_10[i].f4 = D_800E37C0_10[i].f10 = D_800E1BF0[1] + r->y;
        D_800E37C0_10[i].f8 = D_800E37C0_10[i].f14 = D_800E1BF0[2] + vz;
        func_8003C4A4((s32 *)b, 15);
        if (k != 0) {
            k = 1;
            t = D_800E1B7C;
            t -= D_800C28C0_2[i][0].f8->len;
            while (t >= D_800C28C0_2[i][k].fC->len) {
                t -= D_800C28C0_2[i][k].fC->len;
                k++;
                if (D_800C28C0_2[i][k].f4 == 0) {
                    k--;
                    if (i == 0) {
                        t = D_800C28C0_2[i][k].fC->len - 1;
                    } else {
                        t %= D_800C28C0_2[i][k].fC->len;
                    }
                    break;
                }
            }
            { s32 off = t * sizeof(Rec12); r = (Rec34 *)((u8 *)D_800C28C0_2[i][k].fC->data + off); }
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
            q[0] = b->pos[0] + D_800E37C0_10[i].fC;
            q[1] = b->pos[1] + D_800E37C0_10[i].f10;
            q[2] = b->pos[2] + D_800E37C0_10[i].f14;
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

void func_8000CC48(void) {
    func_800BCB04(D_800E1BEC);
    func_8004C3C4();
}

void func_8000CC74(s32 a0, s32 a1, s32 a2, s32 *p4, s32 *p5) {
    s16 *tab = D_800C2C60;
    s32 idx = ((s32)(a0 << 16)) >> 20;
    s32 s1 = tab[idx & 0xFFF];
    s32 m1 = a1 * s1;
    s32 m2 = a2 * s1;
    s32 s2 = tab[(idx - 0x400) & 0xFFF];
    s32 m3 = a1 * s2;
    s32 m4 = a2 * s2;

    *p5 = (m2 + m3) >> 14;
    *p4 = (m1 - m4) >> 14;
}
