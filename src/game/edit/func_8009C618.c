#include "ctx.h"

typedef struct {
    u16 x0;
    u16 x2;
    u8 pad4[0x2C];
    void *x30;
    void *x34;
    u8 pad38[8];
} Rec;

typedef struct {
    u8 pad[0x16F8];
    s32 x16F8;
    s32 x16FC;
    u16 x1700;
    u16 x1702;
} Ctl;

extern Rec D_800CE280[];
extern Ctl *D_800CEA40;
extern u8 D_800DAB1C[];
extern u16 D_800F55B8[][15];

void *func_800BC908(s32 a0, s32 a1, void *a2, s32 a3);
void func_800A5484(void *a0, void *a1, s32 a2);
void func_800A4FF0(void *a0, void *a1, void *a2, s32 a3);

void func_8009C618(void)
{
    s32 i, j;
    s32 *p1 = D_800CE280[0x1C].x30;
    s32 *p2 = D_800CE280[0x1C].x34;

    D_800CEA40->x16F8 = 1;
    D_800CEA40->x16FC = 0;
    D_800CEA40->x1702 = 8;
    for (i = 0; i < 8; i++) {
        D_800CE280[i + 0x12].x2 = 4;
        D_800CE280[i + 0x12].x30 = func_800BC908((*p1 << 8) | 4, 0, D_800DAB1C, 0x1D73);
        D_800CE280[i + 0x12].x34 = func_800BC908((*p2 << 8) | 4, 0, D_800DAB1C, 0x1D74);
        func_800A5484(D_800CE280[i + 0x12].x30, D_800CE280[i + 0x12].x34, 8);
        for (j = 0; j < 15; j++) {
            D_800F55B8[i][j] = 0;
        }
        func_800A4FF0(D_800CE280[i + 0x12].x30, D_800CE280[i + 0x12].x34, D_800F55B8[i], 8);
    }
    {
        Ctl *ea;
        u8 *rb;
        volatile Rec *r;
        u8 *tb;
        i = 0;
        tb = (u8 *)D_800F55B8;
        if (tb) j = 3; else j = 4;
        ea = D_800CEA40;
        rb = (u8 *)D_800CE280;
        r = (volatile Rec *)(rb + ((ea->x16FC + 0x12) << 6));
        r->x2;
        if (r) j = 1; else j = 2;
        for (; i < 15; i++) {
            *(u16 *)(tb + (ea->x16FC * 15 + i) * 2) = 0;
        }
    }
}
