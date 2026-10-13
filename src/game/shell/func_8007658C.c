#include "ctx.h"

typedef struct {
    u8 n, f1, f2, f3;
    u8 pad4[4];
    u16 f8[7];
    s16 f16[7];
} Track;

typedef struct {
    u8 pad0[0x4C];
    s32 f4C;
    u8 pad50[0x64 - 0x50];
    s32 f64, f68, f6C; u32 f70;
    u8 pad74[0x134 - 0x74];
    s16 f134;
    s16 pad136;
    s16 f138;
    u8 pad13A[0x17C - 0x13A];
    s32 f17C;
    u8 pad180[0x3EC - 0x180];
    s32 f3EC;
    u8 pad3F0[0x410 - 0x3F0];
    s32 f410;
} Menu;

typedef struct {
    u8 pad0[0x2A8];
    u8 f2A8[5];
    u8 pad2AD[0x2BF - 0x2AD];
    u8 f2BF;
    u8 f2C0;
} Save;

typedef struct { u8 pad[0x20]; s32 *f20; u8 pad24[4]; s32 f28; } Arg0;
typedef struct { u8 pad[4]; s32 f4; u16 f8; } Arg1;

extern Menu D_800EFD70;
extern Save D_800F1900;
extern Track D_800CA9C8[];
extern Track **D_800CA930;
extern u8 D_800CAA7C;
extern s32 D_800EFDE0;

void func_8006F5F8(s32 x, s32 y);
void func_8001102C(s32 param_1);
void func_8007AF38(s32 p1, s32 p2, s32 p3);

#define OK (D_800F1900.f2C0 > D_800EFD70.f410 \
    || (D_800F1900.f2C0 == D_800EFD70.f410 && (*a0->f20 == 0 || ((D_800F1900.f2BF >> (*a0->f20 - 1)) & 1))) \
    || (D_800EFD70.f4C == 1 && D_800CA930 != NULL && *D_800CA930 - D_800CA9C8 == 5 \
        && D_800F1900.f2C0 == D_800EFD70.f410 && (D_800F1900.f2BF & 8)))

void func_8007658C(Arg0 *a0, Arg1 *a1, s32 *pad) {
    s32 k, j, i;

    if (++D_800EFD70.f17C == 60) D_800EFD70.f17C = 0;
    if (++D_800EFD70.f3EC == 64) D_800EFD70.f3EC = 0;
    switch (D_800EFD70.f70) {
    case 0:
        if (++D_800EFD70.f64 == 60) {
            if (++D_800EFD70.f68 >= D_800CA9C8[*a0->f20].n) D_800EFD70.f68 = 0;
            D_800EFD70.f64 = 0;
        }
        if (*a0->f20 == 5 && D_800CAA7C != 0) {
            if (*pad & 1) {
                s32 v = D_800EFD70.f6C;
                D_800EFD70.f6C = v - 1;
                if (v - 1 < 0) D_800EFD70.f6C = v + 1;
            }
            if (*pad & 2) {
                s32 v = D_800EFD70.f6C;
                if (++D_800EFD70.f6C >= 2) D_800EFD70.f6C -= 2;
            }
        }
        if (D_800EFD70.f410 != 0 && (*pad & 0xC)) {
            if (*pad & 8) {
                s32 *p = a0->f20;
                s32 v = *p;
                switch (D_800EFD70.f410) {
                case 1:
                    if (v <= 0) *p = v + 1;
                    break;
                case 2:
                    if (v < 2) *p = v + 1;
                    break;
                case 3:
                    if (v < 5) *p = v + 1;
                    break;
                }
                if (v != *a0->f20) {
                    func_8001102C(0);
                    D_800EFD70.f64 = 0;
                    D_800EFD70.f68 = 0;
                }
            } else if (*a0->f20 > 0) {
                func_8001102C(0);
                (*a0->f20)--;
                D_800EFD70.f64 = 0;
                D_800EFD70.f68 = 0;
            }
        }
        if ((*pad & 0x50) && !OK) {
            func_8001102C(3);
        } else if ((*pad & 0x50) && OK) {
            if (*a0->f20 == 5 && (D_800EFD70.f6C == 1 || D_800CAA7C == 0)) {
                D_800EFD70.f70 = 1;
                D_800EFD70.f6C = 0;
            } else if (!(a1->f8 & 1)) {
                func_8007AF38(a1->f4, 1, 1);
                func_8001102C(1);
            } else {
                func_8001102C(3);
            }
        } else if ((*pad & 0x20) && a0->f28 != 0) {
            func_8007AF38(a0->f28, 0, 0);
            func_8001102C(2);
        }
        break;
    case 1:
        if ((*pad & 1) && D_800EFD70.f6C != 0) D_800EFD70.f6C--;
        if (D_800EFD70.f6C < D_800CA9C8[*a0->f20].n) {
            if (*pad & 4) {
                if (D_800CA9C8[*a0->f20].f16[D_800EFD70.f6C] >= 2) D_800CA9C8[*a0->f20].f16[D_800EFD70.f6C]--;
            }
            if (*pad & 8) {
                if (D_800CA9C8[*a0->f20].f16[D_800EFD70.f6C] < 6) D_800CA9C8[*a0->f20].f16[D_800EFD70.f6C]++;
            }
        }
        if ((*pad & 2) && D_800EFD70.f6C < D_800CA9C8[*a0->f20].n) {
            s32 v = D_800EFD70.f6C + 1;
            if (v < 5) D_800EFD70.f6C = v;
        }
        k = *pad;
        if (k & 0x50) {
            u16 w;
            D_800EFD70.f70 = 2;
            if (D_800EFD70.f6C == D_800CA9C8[*a0->f20].n) {
                D_800EFD70.f134 = 0;
                D_800CA9C8[*a0->f20].f1++;
                D_800CA9C8[*a0->f20].f2++;
                D_800CA9C8[*a0->f20].f3++;
                D_800CA9C8[*a0->f20].n++;
                D_800EFD70.f138 = -1;
            } else {
                w = D_800CA9C8[*a0->f20].f8[D_800EFD70.f6C];
                D_800EFD70.f134 = w;
                D_800EFD70.f138 = w;
            }
            D_800CA9C8[*a0->f20].f8[D_800EFD70.f6C] = D_800EFD70.f134;
        } else if (k & 0x20) {
            D_800EFD70.f70 = 0;
            D_800EFD70.f6C = 0;
            for (i = 0; i < 5; i++) {
                if (i < D_800CA9C8[5].n) D_800F1900.f2A8[i] = D_800CA9C8[5].f8[i] & 0x1F;
                else D_800F1900.f2A8[i] = 0x3F;
                D_800F1900.f2A8[i] |= (D_800CA9C8[5].f16[i] & 7) << 5;
            }
            func_8006F5F8(1, 0);
        } else {
            if (D_800EFD70.f6C != D_800CA9C8[*a0->f20].n && (k & 0x20000)) {
                for (j = D_800EFD70.f6C; j < D_800CA9C8[*a0->f20].n - 1; j++) {
                    D_800CA9C8[*a0->f20].f8[j] = D_800CA9C8[*a0->f20].f8[j + 1];
                }
                { u8 m = --D_800CA9C8[*a0->f20].n;
                D_800CA9C8[*a0->f20].f8[m] = 0; }
            }
        }
        break;
    case 2:
        if (*pad & 4) {
            u16 v = D_800EFD70.f134;
            D_800EFD70.f134 = v - 1;
            if ((s16)v % 5 == 0) D_800EFD70.f134 = v + 4;
        }
        if (*pad & 8) {
            u16 v = D_800EFD70.f134;
            D_800EFD70.f134 = v + 1;
            if ((s16)(v + 1) % 5 == 0) D_800EFD70.f134 = v - 4;
        }
        if (*pad & 1) {
            D_800EFD70.f134 -= 5;
            if (D_800EFD70.f134 < 0) D_800EFD70.f134 += 20;
        }
        if (*pad & 2) {
            u16 v = D_800EFD70.f134;
            D_800EFD70.f134 = v + 5;
            if (D_800EFD70.f134 >= 20) D_800EFD70.f134 = v - 15;
        }
        k = *pad;
        if (k & 0x50) {
            D_800EFDE0 = 1;
        } else if (k & 0x20) {
            D_800EFD70.f70 = 1;
            if (D_800EFD70.f138 == -1) D_800CA9C8[*a0->f20].n--;
            else D_800EFD70.f134 = D_800EFD70.f138;
        }
        D_800CA9C8[*a0->f20].f8[D_800EFD70.f6C] = D_800EFD70.f134;
        break;
    }
    *D_800CA930 = &D_800CA9C8[*a0->f20];
}
