#include "ctx.h"

typedef struct Rec {
    s16 cnt;          /* 0x660 */
    s16 f662;         /* 0x662 */
    u8 pad664[4];
    s16 f668;         /* 0x668 */
    s16 f66A;         /* 0x66A */
    s16 f66C;         /* 0x66C */
    u8 pad66E[2];
    s32 f670;         /* 0x670 */
    s32 times[29];    /* 0x674 */
    u8 f6E8;          /* 0x6E8 */
    u8 pad6E9[4];
    s8 f6ED;          /* 0x6ED */
    u8 pad6EE[0x180 - 0x8E];
} Rec;

typedef struct Bike {
    u8 pad000[0x660];
    Rec r;
} Bike;

typedef struct Blk {
    u8 pad00[0x10];
    s32 best;         /* 0x10 */
    u8 pad14[0x1D - 0x14];
    u8 f1D;
    u8 f1E;
    u8 f1F;
    s16 nlaps;        /* 0x20 */
    s16 f22;          /* 0x22 */
    u8 pad24[0x2C - 0x24];
    s32 f2C;          /* 0x2C */
    s16 arr[1];       /* 0x30 */
} Blk;

typedef struct Ctl {
    u32 flags;
    u8 pad04[0x3C - 4];
    s32 n;            /* 0x3C */
} Ctl;

typedef struct G64 {
    u8 pad[0x50];
    u32 f50;
} G64;

extern s32 D_800EFDBC;
extern G64 *D_800EFD64;
extern Ctl D_800EFD70;
extern s32 D_800EFDAC;
extern Bike D_800E37C0[];
extern Blk D_800F05A8;
extern s32 D_800C238C[];
extern s32 D_800C8CE0;
extern s32 D_800C8CE8;
extern s32 D_800C8CD8;
extern s32 D_800E3700;

void func_8004CD78(s32 param_1);
void func_8004CE80(s32 param_1, s32 param_2);
void func_8004ECB4(u32 param_1);
void func_8004D7DC(u32 param_1);
void func_8004E960(u32 p);
u32 func_80050F7C(s32 p0);

void func_800511B8(s32 p)
{
    s32 st = 0;
    Rec *b;
    s32 t;
    s32 i;
    s16 v;

    switch (D_800EFDBC) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 9:
    case 13:
    case 14:
        return;
    }
    b = &D_800E37C0[p].r;
    if (b->cnt < D_800F05A8.nlaps) {
        if (D_800E37C0[p].r.f6E8 == 0 && D_800C8CE8 == 0 && D_800EFDBC != 7 && D_800EFDBC != 5 &&
            !(D_800EFD64->f50 & 2) && (t = b->times[b->cnt]) < D_800F05A8.best &&
            (D_800EFDBC != 8 || p == D_800C8CE0)) {
            D_800F05A8.best = t;
            D_800F05A8.f1D = 0xFF;
            D_800F05A8.f1E = 0xFF;
            D_800F05A8.f1F = (D_800EFDBC == 8) ? 0 : p;
            st = 3;
            D_800EFD64->f50 |= 0x80;
        }
        if (D_800EFD70.flags & 0x81) {
            goto end;
        }
        v = D_800F05A8.arr[b->cnt];
        if (v == -1) {
            D_800F05A8.arr[b->cnt] = ((u16 *)&D_800F05A8.f2C)[1];
        } else {
            b->f670 = v - D_800F05A8.f2C;
            b->f66C = 0x78;
            if (b->f662 == 1) {
                for (i = 0; i < D_800EFD70.n; i++) {
                    if (b->cnt < D_800E37C0[i].r.cnt + D_800E37C0[i].r.f6ED) {
                        D_800E37C0[i].r.f66C = 0x78;
                        D_800E37C0[i].r.f670 = -b->f670;
                        break;
                    }
                }
            }
        }
        b->f66A = 0x78;
        ((volatile Rec *)b)->cnt++;
        if (b->cnt != D_800F05A8.nlaps) {
            if (st != 3) {
                if (D_800EFDBC == 8 && D_800C238C[p] == 0xFF) {
                    st = 1;
                } else if (b->f662 == 0) {
                    st = 2;
                } else if (D_800E37C0[p].r.f6E8 == 0 && b->f670 < -300) {
                    st = 1;
                }
            }
        } else if ((u8)func_80050F7C(p)) {
            if (st != 3) {
                if (b->f668 == 0 && D_800F05A8.f22 >= 2 && st == 0) {
                    st = 4;
                } else if (D_800E37C0[p].r.f6E8 == 0) {
                    st = 5;
                }
            }
        } else if (p < D_800EFDAC) {
            st = 6;
        }
    }
end:
    if (D_800C8CD8 != 0 && D_800E3700 == p) {
        st = 0;
    }
    switch (st) {
    case 1:
        if (D_800C8CD8 == 0 || p != D_800E3700) {
            func_8004CD78(b->cnt - 1);
        }
        break;
    case 2:
        func_8004CE80(p, b->cnt - 1);
        break;
    case 3:
        func_8004ECB4(p);
        break;
    case 4:
        func_8004D7DC(p);
        break;
    case 5:
        func_8004E960(p);
        break;
    case 6:
        break;
    }
}
