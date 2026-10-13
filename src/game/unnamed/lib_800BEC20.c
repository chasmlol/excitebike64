#include "game.h"

typedef struct Slot_18 {
    s32 w0, w1, w2, w3, w4, w5, w6, w7;
    u8 w8[4];
    s32 w9, w10, w11, w12, w13, w14, w15, w16, w17, w18;
} Slot_18;

s32 func_800BEC20(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4)
{
    s32 i;
    s32 off;
    s32 t;
    Slot_18 *base;
    Slot_18 *p;
    Slot_18 *e;

    func_800BED20();
    if (D_800D5710 != 0) {
        D_800D5710();
    }
    i = 0;
    base = D_80100720;
    p = base;
    for (; i < 8; i++) {
        if (p->w0 == 0) {
            break;
        }
        p++;
    }
    if (i == 8) {
        return -2;
    }
    off = i * 76;
    e = (Slot_18 *)(off + (s32)base);
    e->w15 = a3;
    e->w16 = a2;
    e->w17 = a4;
    e->w14 = a0;
    e->w18 = a1;
    e->w8[2] = 0;
    t = (s32)&base->w1;
    e->w9 = off + t;
    e->w0 = 2;
    e->w10 = e->w15;
    e->w11 = e->w16;
    e->w12 = e->w17;
    return i;
}

typedef struct {
    s32 state;
    u8 f04[0x1C];
    u8 f20[2];
    u8 flag22;
    u8 pad23;
    void *p24;
    s32 f28, f2C, f30, f34;
    s32 f38, f3C, f40, f44, f48;
} Ent_107;

typedef void (*FnPtr)(s32, s32, s32);

extern Ent_107 D_80100720_2[];
extern FnPtr D_800D5714;

void func_800BED20(void)
{
    s32 i;

    D_80100704 = 0;
    D_801009A0 = 0;
    for (i = 0; i < 8; i++) {
        if (D_80100720_2[i].state == 1) {
            if (func_800B0610(D_80100720_2[i].f04, 0, 0) == 0) {
                D_80100720_2[i].state = 3;
            } else {
                D_80100704++;
                return;
            }
        } else if (D_80100720_2[i].state == 2) {
            if (D_800D5714 == 0) {
                D_80100720_2[i].p24 = D_80100720_2[i].f04;
                D_801009A0++;
                D_80100720_2[i].flag22 = 0;
                D_80100720_2[i].f28 = D_80100720_2[i].f3C;
                D_80100720_2[i].f2C = D_80100720_2[i].f40;
                D_80100720_2[i].f30 = D_80100720_2[i].f44;
                func_8000EA24();
                if (func_800BF040(D_80100720_2[i].f38, D_80100720_2[i].f20, D_80100720_2[i].f48) == 0) {
                    D_80100720_2[i].state = 1;
                    return;
                }
            } else {
                D_800D5714(D_80100720_2[i].f40, D_80100720_2[i].f3C, D_80100720_2[i].f44);
                D_80100720_2[i].state = 3;
            }
        }
    }
}

s32 func_800BEE78(void)
{
    s32 i;
    u8 *b;
    s32 *z;
    u8 *q;
    u8 *p;

    func_800AC920(0x96, D_80100708, D_80100980, 8);
    i = 0;
    b = D_80100724;
    z = (s32 *)(b - 4);
    q = b + 0x18;
    p = b;
    do {
        func_800B05E0_4(p, q, 1);
        *z = 0;
        z = (s32 *)((u8 *)z + 0x4C);
        q += 0x4C;
        i++;
        p += 0x4C;
    } while (i < 8);
    D_801009A4 = func_800BF100();
    D_80100700 = (s32)func_800BF3C0();
    return 0;
}

s32 func_800BEF2C(s32 a0) {
    if (D_800D5710 != NULL) {
        D_800D5710();
    }
    func_800BED20();
    return D_80100720[a0 * 19];
}

void func_800BEF8C(s32 a0)
{
    D_80100720[a0 * 0x13] = 0;
}

typedef struct {
    s32 state;
    u8 pad[0x48];
} Ent_108;

extern Ent_108 D_80100720_3[];

void func_800BEFB4(s32 x) {
    do {
        if (D_800D5710 != NULL) {
            D_800D5710();
        }
        func_800BED20();
    } while (D_80100720_3[x].state != 3 && D_80100720_3[x].state != 0);
    D_80100720_3[x].state = 0;
}
