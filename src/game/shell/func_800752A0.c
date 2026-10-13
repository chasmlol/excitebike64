#include "ctx.h"

void func_8001102C(s32 param_1);
void func_800650E4(void);
void func_80064B9C(s32 p1, s32 n);
void func_8007AF38(s32 p1, s32 p2, s32 p3);

typedef struct {
    s32 f0;
    u8 pad4[0x3C - 0x4];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    s32 f4C;
    u8 pad50[0x17C - 0x50];
    s32 f17C;
    u8 pad180[0x2E8 - 0x180];
    s32 f2E8;
    u8 pad2EC[0x3EC - 0x2EC];
    s32 f3EC;
    u8 pad3F0[0x3FC - 0x3F0];
    s32 f3FC;
    s32 f400;
    u8 pad404[0x418 - 0x404];
    u16 f418;
    u8 f41A[6];
    s32 f420[6];
    u8 pad438[0x47C - 0x438];
    s16 f47C;
    u8 pad47E[0x764 - 0x47E];
    u8 f764[6];
    u8 f76A[6];
} GState;

typedef struct {
    u8 n;
    u8 pad[0x23];
} Trk;

typedef struct {
    Trk *trk;
    u8 pad4[0x64 - 4];
    s16 f64;
    u8 pad66[0x6C - 0x66];
    u8 f6C[6];
} Race;

typedef struct {
    u8 pad[0x669];
    u8 f669;
    u8 pad66A[0x68C - 0x66A];
    s32 f68C;
    u8 pad690[0x7E0 - 0x690];
} Bike;

typedef struct {
    u8 pad[0x22];
    u16 f22;
    u8 pad24[0x50 - 0x24];
    u8 f50[6];
} Sel;

extern GState D_800EFD70;
extern Bike D_800E37C0[];
extern Sel D_800F05A8;
extern u8 D_800F05F8;
extern s16 D_800F05CA;
extern Race *D_800CA930;
extern Trk D_800CA9C8[];
extern u8 D_800CD6C0[];
extern u8 D_800CD79C[];
extern s32 D_800F0954;
extern s32 D_800F0A3C;
extern s32 D_800F0950;
extern s32 D_800EFDBC;
extern s32 D_800C8CE8;
extern s32 D_800C8CE4;

typedef struct {
    u8 pad[0x28];
    s32 f28;
} Obj;

void func_800752A0(Obj *arg0, s32 arg1, u32 *arg2)
{
    u32 b;
    Trk *t;
    s32 i;
    s32 d;

    if (++D_800EFD70.f17C == 0x3C) {
        D_800EFD70.f17C = 0;
    }
    if (++D_800EFD70.f3EC == 0x78) {
        D_800EFD70.f3EC = 0;
    }
    b = *arg2;
    if (b & 0x20) {
        func_8001102C(2);
        if (D_800EFD70.f2E8 == 4) {
            D_800EFD70.f2E8 = 0;
            return;
        }
        if (D_800EFD70.f2E8 == 7) {
            D_800EFD70.f2E8 = 4;
            return;
        }
        if (arg0->f28 != 0) {
            func_8007AF38(arg0->f28, 0, 0);
        }
    } else if (b & 0x10) {
        func_8001102C(1);
        if (D_800EFD70.f2E8 == 4) {
            switch (D_800EFD70.f400) {
            case 0:
                D_800EFD70.f0 |= 0x80;
                t = D_800CA930->trk;
                if (D_800CA930->f64 + 1 >= t->n) {
                    if (D_800CA930->f6C[0] == 0 && (t - D_800CA9C8 != 4 || D_800F05F8 == 0)) {
                        D_800EFD70.f0 = (D_800EFD70.f0 & ~0x80) | 0x40000;
                        func_8007AF38(0, 0, 1);
                        D_800C8CE4 = 0;
                        return;
                    }
                } else if (t - D_800CA9C8 != 4 || D_800F05F8 == 0) {
                    func_8007AF38(0, 0, 1);
                    D_800CA930->f64++;
                    func_80064B9C(D_800EFD70.f3C, D_800F05CA - D_800EFD70.f3C);
                    func_800650E4();
                    D_800C8CE4 = 0;
                    return;
                }
                func_8007AF38((s32)D_800CD6C0, 0, 1);
                D_800C8CE4 = 0;
                return;
            case 1:
                D_800EFD70.f418 = D_800F05A8.f22;
                for (i = 0; i < 6; i++) {
                    D_800EFD70.f41A[i] = D_800F05A8.f50[i];
                    D_800EFD70.f420[i] = D_800E37C0[i].f68C;
                    D_800EFD70.f76A[i] = D_800E37C0[i].f669;
                    if (D_800CA930 != 0) {
                        D_800EFD70.f764[i] = D_800CA930->f6C[i];
                    }
                }
                func_8007AF38(0, 0, 1);
                D_800C8CE8 = 1;
                D_800EFD70.f3FC = (s32)arg0;
                D_800EFD70.f0 |= 0x8000;
                return;
            case 2:
                D_800EFD70.f2E8 = 7;
                D_800EFD70.f47C = 0;
                return;
            }
        } else {
            if (D_800EFD70.f2E8 == 7) {
                switch (D_800EFD70.f47C) {
                case 1:
                    func_8007AF38((s32)D_800CD79C, 0, 1);
                    D_800C8CE4 = 0;
                    return;
                case 0:
                    D_800EFD70.f2E8 = 4;
                    return;
                }
            } else {
                D_800EFD70.f400 = 0;
                D_800EFD70.f2E8 = 4;
            }
        }
    } else {
        if (D_800EFD70.f2E8 == 7) {
            if (b & 3) {
                D_800EFD70.f47C ^= 1;
                func_8001102C(0);
            }
        } else if (D_800EFD70.f2E8 == 4) {
            if (b & 0x801) {
                d = D_800F0954 - D_800F0A3C;
                if (D_800EFD70.f4C != 8 ? d >= D_800F0950 : d >= 0x2800) {
                    D_800EFD70.f400 ^= 2;
                } else {
                    if (--D_800EFD70.f400 < 0) {
                        D_800EFD70.f400 += 3;
                    }
                }
                func_8001102C(0);
            }
            if (*arg2 & 0x1002) {
                func_8001102C(0);
                d = D_800F0954 - D_800F0A3C;
                if (D_800EFDBC != 8 ? d >= D_800F0950 : d >= 0x2800) {
                    D_800EFD70.f400 ^= 2;
                } else {
                    if (++D_800EFD70.f400 >= 3) {
                        D_800EFD70.f400 -= 3;
                    }
                }
            }
        }
    }
}
