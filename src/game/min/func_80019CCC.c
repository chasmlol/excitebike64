#include "ctx.h"

typedef struct {
    s32 flags;
    s32 pad[14];
    s32 f3C;
    s32 f40;
    s32 f44;
} Ctl;

extern s32 D_800C8CE4;
extern s32 D_800C8CD8;
extern s32 D_800E3748;
extern s32 D_800E3700;
extern s32 D_800E370C;
extern Ctl D_800EFD70;

void func_8005EA20(s32 a, s32 b);
void func_8005FEE0(s32 a);

void func_80019CCC(s32 p)
{
    if (p != 0) {
        Ctl *c;
        D_800E3700 ^= 1;
        c = &D_800EFD70;
        c->f3C = 1;
        c->f44 = 0;
        func_8005EA20(D_800C8CE4, D_800E3748);
        D_800E370C = 2;
        c->f3C = 2;
        D_800E3700 ^= 1;
    } else {
        Ctl *c = &D_800EFD70;
        c->f3C = 1;
        c->f44 = 0;
        D_800E370C = 1;
        D_800E3700 = 0;
        func_8005EA20(D_800C8CE4, D_800E3748);
        D_800E3700 = 1;
    }
    if (D_800C8CD8 != 0) {
        Ctl *c = &D_800EFD70;
        if (c->flags & 0x80000) {
            c->f3C = 1;
            func_8005FEE0(D_800C8CE4 + 0x6400);
            c->f3C = 2;
        }
    }
}

