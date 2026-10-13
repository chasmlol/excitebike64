#include "ctx.h"

typedef struct {
    u8 pad0[0x28];
    u32 flags;
} Part;

typedef struct {
    f32 x, y, z;      /* 0x00 */
    u8 pad0C[0x8];
    f32 f14;          /* 0x14 */
    f32 f18;          /* 0x18 */
    u8 pad1C[0x28];
    s8 f44;           /* 0x44 */
    u8 pad45;
    s16 f46;          /* 0x46 */
    u8 pad48[2];
    s16 f4A;          /* 0x4A */
    u8 pad4C[0xC];
    f32 f58;          /* 0x58 */
    f32 f5C;          /* 0x5C */
    u8 pad60[6];
    u16 f66;          /* 0x66 */
    u8 pad68[4];
} Seg;

typedef struct Bike Bike;

typedef struct {
    s16 id;
    s16 arg;
    s32 (*fn)(Bike *, s32);
} Trig;

typedef struct {
    s16 a, b, c, d;
} Ev;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0x54];
    Trig *f84;
    Ev ev[3];
} Track;

struct Bike {
    s32 x, y, z;      /* 0x000 */
    u8 pad00C[0x38];
    u8 f44;           /* 0x044 */
    u8 pad045[3];
    Vec3f pos;        /* 0x048 */
    f32 f54, f58, f5C;/* 0x054 */
    u8 pad060[0x58];
    f32 fB8;          /* 0x0B8 */
    f32 fBC;
    f32 fC0;          /* 0x0C0 */
    u8 pad0C4[0x30C];
    s8 f3D0;          /* 0x3D0 */
    u8 pad3D1[0x2E];
    u8 f3FF;          /* 0x3FF */
    u8 pad400[0x18];
    s8 f418;          /* 0x418 */
    u8 pad419[0xB];
    f32 f424;         /* 0x424 */
    u8 pad428[0x30];
    s32 f458;         /* 0x458 */
    u8 pad45C[0x88];
    Part *f4E4;       /* 0x4E4 */
    u8 pad4E8[0x14];
    s32 f4FC;         /* 0x4FC */
    u8 pad500[0x88];
    Part *f588;       /* 0x588 */
    u8 pad58C[0x98];
    s16 f624;         /* 0x624 */
    s16 f626;         /* 0x626 */
    s16 seg;          /* 0x628 */
    s16 pad62A;
    f32 f62C;         /* 0x62C */
    f32 f630;         /* 0x630 */
    f32 f634;         /* 0x634 */
    f32 f638;         /* 0x638 */
    f32 f63C;         /* 0x63C */
    u8 pad640[8];
    s32 f648;         /* 0x648 */
    u8 pad64C[0x14];
    s16 f660;         /* 0x660 */
    s16 f662;         /* 0x662 */
    u8 pad664[4];
    s16 f668;         /* 0x668 */
    s16 f66A;         /* 0x66A */
    u8 pad66C[0x20];
    s32 f68C;         /* 0x68C */
    u8 pad690[0x22];
    s16 f6B2;         /* 0x6B2 */
    u8 pad6B4[0x34];
    u8 f6E8;          /* 0x6E8 */
    u8 pad6E9[2];
    u8 f6EB;          /* 0x6EB */
    u8 pad6EC;
    s8 f6ED;          /* 0x6ED */
    u8 f6EE;          /* 0x6EE */
    u8 pad6EF[0x55];
    s32 f744;         /* 0x744 */
    u8 pad748[0x64];
    s16 f7AC;         /* 0x7AC */
};

extern Track *D_800EFD64;
extern Track D_800CB5D0;
extern s32 D_800E8508;
extern s32 D_800EFDBC;
extern u32 D_800F05A8;
extern s16 D_800EE496;
extern f32 D_800EE4A0;
extern Seg *D_800EE498;
extern s32 D_800F05D4[];
extern f32 D_800EE480;
extern f32 D_800EE484;
extern f32 D_800EE48C;
extern f32 D_800E76C8;

void func_800445B0(Bike *b, s32 a);
void func_80046CC0(Bike *b);
s32 func_8002E920(s32 x, s32 y, s32 z);
s32 func_8004A788(Vec3f *pos, s32 start, s32 count, u8 mode);
f32 func_8004A944(Vec3f *pos, s32 idx);
void func_80072D64(s32 id);
s32 func_80050CF4(Bike *p);
void func_8004B494(Bike *p, s32 a1);
u8 func_80048D30(f32 x, s32 idx);
void func_8002AF64(Bike *p);

void func_8004AC98(Bike *b)
{
    s32 oldSeg;
    s32 n;
    s32 i;
    Seg *s;
    Trig *t;
    Ev *e;
    s32 w;
    s32 nx;
    f32 v;
    f32 old;
    f32 sq;
    s32 k;
    s32 cur;

    oldSeg = b->seg;
    if ((b->f4E4->flags & 0x80) && b->f458 <= 0) {
        func_800445B0(b, 0x19);
    }
    if ((b->f588->flags & 0x80) && b->f4FC <= 0) {
        func_800445B0(b, 0x1C);
    }
    if (D_800EFD64->f2C == 1) {
        func_8002E920(b->x, b->y, b->z);
        if (D_800E8508 == 0) {
            func_80046CC0(b);
        }
    }
    if (b->f458 <= 0 && b->f4FC <= 0 && b->f44 == 0) {
        if (!(b->f4E4->flags & 0x100) && !(b->f588->flags & 0x100)) {
            b->f626 = oldSeg;
            b->f624 = 0;
        } else if (D_800EFDBC != 0xE && (b->f4E4->flags & 0x100) && (b->f588->flags & 0x100)) {
            if (++b->f624 > 10) {
                func_80046CC0(b);
            }
        }
    }
    if (D_800F05A8 < 5 && D_800EFDBC != 9) {
        D_800EE4A0 = 0.0f;
        b->seg = D_800EE496;
    } else {
        Vec3f *pos = &b->pos;
        n = func_8004A788(pos, oldSeg, 2, b->f6EB);
        if ((pos->z - D_800EE498[n].z) * D_800EE498[n].f14 - (b->pos.x - D_800EE498[n].x) * D_800EE498[n].f18 < 0.0f) {
            n = D_800EE498[n].f4A;
        }
        b->seg = n;
        if ((s16)n != oldSeg) {
            if (b->f6E8) {
                t = D_800EFD64->f84;
                if (t != NULL) {
                    while (t->id >= 0) {
                        if (t->id != 0 && b->seg == t->id) {
                            b->f6EB = t->fn(b, t->arg) + 1;
                        }
                        t++;
                    }
                } else {
                    b->f6EB = 3;
                }
            }
            for (i = 0; i < 3; i++) {
                if (D_800EFD64->ev[i].a == -1) {
                    break;
                }
                if (D_800EFD64->ev[i].a == b->f660 + b->f6ED
                    && (D_800EFD64->ev[i].b == b->seg || D_800EFD64->ev[i].c == b->seg)) {
                    func_80072D64(D_800EFD64->ev[i].d);
                }
            }
            if (D_800EFDBC == 7) {
                if (b->seg == D_800EE498->f4A) {
                    b->seg = 0;
                }
                n = b->seg;
                if (D_800EE498[n].f44 == 0 && b->f3FF == 0 && b->f44 == 0 && b->f6B2 < n) {
                    if (n < 0x78) {
                        b->f648 = 0;
                        b->f66A = 0x78;
                        b->f6B2 = b->seg;
                    } else {
                        b->f3FF = 1;
                        b->f662 = 0;
                        b->f68C = D_800F05D4[0];
                        func_80050CF4(b);
                        b->f6E8 = 1;
                    }
                }
            }
            func_8004B494(b, oldSeg);
        }
        b->f638 = func_8004A944(&b->pos, b->seg);
        b->f63C = D_800EE4A0;
        if (b->f6E8 == 0) {
            b->f6EE = b->fB8 * D_800EE480 + b->fC0 * D_800EE484 < 0.0f && b->f424 > -0.5f && b->f3D0 != 0xF;
        }
    }
    if (b->f44 == 0) {
        k = b->seg;
        if (b->f63C > 0.5f) {
            k = D_800EE498[k].f46;
        }
        if (D_800EFD64->f2C == 0
            && (D_800EE498[k].f5C < b->f638 - 3.5f || D_800EE498[k].f58 < -(b->f638 + 3.5f))) {
            if (++b->f7AC > 180 && b->f458 <= 0 && b->f4FC <= 0) {
                func_80046CC0(b);
            }
        } else {
            b->f626 = oldSeg;
            b->f7AC = 0;
        }
    }
    {
        w = D_800EE4A0 * 256.0f;
        k = D_800EE498[b->seg].f46;
        cur = D_800EE498[b->seg].f66;
        nx = D_800EE498[k].f66;
    }
    if (nx == 0x8000) {
        nx = 0;
    }
    v = (f32)(s32)(cur * (0x100 - w) + nx * w) * D_800EE48C * 1.1920929e-07f;
    sq = b->f54 * b->f54 + b->f5C * b->f5C;
    old = b->f62C;
    b->f62C = v;
    b->f634 = (v - old) / D_800E76C8;
    if (sq > 1.0f && b->f744 < -15) {
        sq = __builtin_sqrtf(sq);
        if (b->f54 * D_800EE480 + b->f5C * D_800EE484 / sq > 0.7f) {
            b->f418 = func_80048D30(sq, b->seg);
        }
    } else {
        b->f418 = -1;
    }
    if (b->f668 == -1) {
        b->f630 = D_800EE48C * (f32)(b->f6ED + 1 + b->f660) - b->f62C;
    }
    if (b->f6E8 == 0 && b->seg >= 0x13A && b->seg < 0x140 && b->f744 > 30
        && D_800EFD64 == &D_800CB5D0 && b->f58 < 0.0f && b->pos.y < D_800EE498[0x140].y) {
        func_800445B0(b, 0x1C);
        func_8002AF64(b);
    }
}
