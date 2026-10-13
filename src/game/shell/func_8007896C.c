#include "ctx.h"

void func_80014AD0(void);
void func_8000ECB8(s32);
void func_8004834C(void);
void func_800BFDD4(s32 *param_1);
void func_800BCB04(void *ptr);
s32 func_800BF530(char *path, void **out);
void func_800B2B00(s32);

typedef struct {
    u32 f0;
    u8 pad4[0x1C - 0x4];
    s32 arr[8];
    s32 f3C;
    s32 f40;
    s32 f44;
    u8 pad48[0x4C - 0x48];
    u32 f4C;
    u8 pad50[0x13C - 0x50];
    void *f13C;
    u8 pad140[0x43C - 0x140];
    u8 f43C[6];
    u8 pad442[0x778 - 0x442];
    s32 f778[6];
    s32 f790[6];
} GState;

typedef struct {
    u8 n;
    u8 pad[0x15];
    s16 f16[7];
} Trk;

typedef struct {
    Trk *trk;
    u8 pad4[0x64 - 4];
    s16 f64;
    u8 pad66[0x6C - 0x66];
    u8 f6C[6];
} Race;

typedef struct {
    u8 pad[0x1E];
    s16 f1E;
} Lvl;

extern GState D_800EFD70;
extern s32 D_800C2280;
extern s32 D_800CD1A8;
extern Lvl *D_800EFD64;
extern Lvl D_800CBF30;
extern void *D_800C9570;
extern Race *D_800CA930;
extern Trk D_800CA9C8[];
extern s32 D_800CCD64;
extern s32 D_800EFDB4;
extern s32 *D_800F47C0;
extern s16 D_800F05CA;
extern char D_800D94A4[];
extern void *D_800E7728;
extern void *D_800F01BC;
extern s32 D_800CA920;

void func_8007896C(void)
{
    s32 i;
    s32 j;
    s32 v;
    s32 k;
    GState *g;

    D_800EFD70.f0 |= 0x4000000;
    D_800C2280 = D_800CD1A8;
    switch (D_800EFD70.f4C) {
    case 8:
    case 9:
        D_800C2280 = D_800CD1A8 & ~0x210;
        break;
    }
    switch (D_800EFD70.f4C) {
    case 0:
    case 1:
    case 8:
        if (!(D_800EFD70.f0 & 0x1000000)) {
            D_800EFD70.f0 &= ~0x4000000;
        }
        break;
    }
    if (D_800EFD64 != &D_800CBF30 && D_800C9570 != 0) {
        func_800BCB04(D_800C9570);
        D_800C9570 = 0;
    }
    if (D_800EFD70.f4C == 1 && D_800CA930 != 0 && D_800CA930->trk - D_800CA9C8 == 4) {
        D_800EFD70.arr[1] = D_800CA930->f64;
        if (D_800EFD70.arr[1] == D_800EFD70.arr[0]) {
            D_800EFD70.arr[1] = 5;
        }
    } else if (D_800EFD70.f4C != 8) {
        v = 0;
        for (i = D_800EFD70.f3C; i < 8; i++) {
            for (j = 0; j < D_800EFD70.f3C; j++) {
                if (v == D_800EFD70.arr[j]) {
                    v++;
                    v %= 8;
                }
            }
            D_800EFD70.arr[i] = v;
            v++;
            v %= 8;
        }
    }
    if (D_800EFD70.f4C != 7 && D_800EFD70.f4C != 2 && D_800CCD64 == 0 && !(D_800EFD70.f0 & 0x40000)) {
        switch (D_800EFD70.f3C) {
        default:
            D_800EFDB4 = 0;
            break;
        case 1:
            D_800EFD70.f44 = 5;
            break;
        case 2:
            D_800EFD70.f44 = D_800EFD70.f3C;
            break;
        }
    } else {
        D_800EFDB4 = 0;
    }
    if (!(D_800EFD70.f0 & 0x8000)) {
        g = &D_800EFD70;
        g->f40 = (g->f4C != 1 || D_800CA930 == 0 || D_800CA930->trk - D_800CA9C8 != 4) ?
            ((g->f4C == 1 && D_800CA930 != 0 && D_800CA930->trk - D_800CA9C8 == 5) ?
             D_800CA930->trk->f16[D_800CA930->f64] : D_800EFD64->f1E) : 1;
    }
    func_800BCB04(D_800EFD70.f13C);
    func_80014AD0();
    func_800BFDD4(D_800F47C0);
    D_800F47C0 = 0;
    func_800B2B00(0xAA);
    D_800EFD70.f0 &= ~1;
    if (D_800EFD70.f0 & 0x40000) {
        D_800EFD70.f3C = D_800F05CA;
        if (D_800EFD70.f3C >= 4) {
            D_800EFD70.f3C = 3;
        }
        for (k = 0; k < 6; k++) {
            D_800EFD70.f778[k] = D_800EFD70.arr[k];
            D_800EFD70.f790[k] = D_800EFD70.f43C[k];
        }
        for (k = 0; k < D_800EFD70.f3C; k++) {
            D_800EFD70.arr[k] = D_800EFD70.f778[D_800CA930->f6C[k]];
            D_800EFD70.f43C[k] = D_800EFD70.f790[D_800CA930->f6C[k]];
        }
    }
    if (func_800BF530(D_800D94A4, &D_800E7728) != 0) {
        func_8000ECB8(1);
    }
    func_800BFDD4(D_800E7728);
    func_800BCB04(D_800F01BC);
    D_800CA920 = 6;
    func_8004834C();
}
