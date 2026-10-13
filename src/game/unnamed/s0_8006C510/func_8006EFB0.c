#include "ctx.h"

typedef struct {
    u32 w[7];
} Rec;

typedef struct {
    u8 pad0[0x50];
    u32 flags;
    u8 pad54[0xA0 - 0x54];
} Rider;

typedef struct {
    u32 magic;
    u32 sum;
    Rec recs[24];
    u8 name[0x17];
    u8 f2BF;
    u8 f2C0;
    u8 f2C1;
    u8 pad2C2[2];
    u8 f2C4;
    u8 f2C5;
    u8 f2C6;
    u8 f2C7;
    u8 f2C8;
    u8 pad2C9[7];
    u32 f2D0;
    u8 f2D4[0x804 - 0x2D4];
} Save;

extern Save D_800F1900;
extern Rec D_800F0200[];
extern Rider D_800CB210[];

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800A1B0C(u8 *p);

void func_8006EFB0(void)
{
    Rec *pd;
    Rec *ps;
    Rider *pc;
    u8 *q;
    u8 c;
    u32 sum;
    u32 *p;
    s32 i;

    func_8000ED10((u8 *)&D_800F1900, 0, sizeof(Save));
    func_800A1B0C(D_800F1900.f2D4);
    i = 0;
    pc = D_800CB210;
    pd = D_800F1900.recs;
    ps = D_800F0200;
    do {
        i++;
        if (pc->flags & 0x200) {
            *pd = *ps;
            pd++;
            ps++;
        }
        pc++;
    } while (i < 27);
    c = '?';
    D_800F1900.f2C0 = 1;
    D_800F1900.f2BF = 0;
    i = 4;
    q = (u8 *)&D_800F1900 + 4;
    do {
        q[0x2A8] = c;
        i--;
        q--;
    } while (i >= 0);
    sum = 0;
    D_800F1900.f2C5 = 0x8C;
    D_800F1900.f2C6 = 200;
    D_800F1900.f2C7 = 0xA0;
    D_800F1900.f2C4 = 1;
    D_800F1900.f2D0 = 1;
    D_800F1900.f2BF = 0x1F;
    D_800F1900.f2C1 = 7;
    D_800F1900.f2C8 = 0;
    D_800F1900.f2C0 = 3;
    D_800F1900.magic = 0x5748FB3A;
    p = (u32 *)&D_800F1900;
    for (i = 0; i < 0x200; i++) {
        if (i != 1) {
            sum += p[i];
        }
    }
    sum ^= 0xFCB1121D;
    p[1] = sum;
}
