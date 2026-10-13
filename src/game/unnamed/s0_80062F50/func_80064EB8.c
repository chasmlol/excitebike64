#include "ctx.h"

typedef struct {
    s16 h0;
    s16 rest[5];
} Ent12;

extern Ent12 D_800F0C30[];
extern Ent12 D_800F0B70[];
extern u8 *D_800F0C10[8];
extern u8 *D_800F0BE0[8];
extern u8 D_800E37C0[];
extern u32 D_800F0BDC;
extern u32 D_800F0C04;

void func_80064EB8(void) {
    s32 i = 0;
    s16 m1 = -1;
    Ent12 *p0 = D_800F0C30;
    Ent12 *p1 = D_800F0B70;
    u8 **p2 = D_800F0C10;
    u8 **p3 = D_800F0BE0;
    u8 *v = D_800E37C0;
    do {
        *p3 = v;
        *p2 = v;
        p1->h0 = m1;
        p0->h0 = m1;
        p0++;
        p1++;
        p2++;
        p3++;
        v[0x7A7] = i;
        i++;
        v += 0x7E0;
    } while (i < 8);
    D_800F0BDC = 0;
    D_800F0C04 = 0;
}
