#include "ctx.h"

typedef struct {
    s32 f0[2];
    s32 f8;
    s32 fC[5];
    s32 f20;
    s32 f24;
    s32 f28;
    s32 f2C[4];
    s32 f3C;
    s32 f40[4];
} Rec;

extern s32 D_80000300;
extern Rec D_800D3210;
extern Rec D_800D3170;
extern Rec D_800D3350;
extern Rec D_800D32B0;
extern Rec D_800F0CB0[];
extern s32 D_800CCCAC[];
extern void func_8000E9B8(void);

void func_800656B4(s32 p)
{
    Rec *s;
    Rec *b;
    s32 off;
    Rec *base;

    if (D_80000300 == 1) {
        if (p == 2) {
            s = &D_800D3210;
        } else {
            s = &D_800D3170;
        }
    } else if (D_80000300 == 2) {
        if (p == 2) {
            s = &D_800D3350;
        } else {
            s = &D_800D32B0;
        }
    } else {
        func_8000E9B8();
    }
    D_800F0CB0[p] = *s;
    off = p * 0x50;
    base = D_800F0CB0;
    if (p != 2) {
        b = (Rec *)(off + (s32)base);
        b->f8 = D_800CCCAC[p];
        b->f20 = (D_800CCCAC[p] << 9) / 320;
        b->f28 = D_800CCCAC[p] << 1;
        b->f3C = D_800CCCAC[p] << 1;
    }
}
