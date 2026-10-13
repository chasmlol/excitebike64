#include "ctx.h"

typedef struct Bike {
    u8 pad0[0x44];
    u8 f44;
    s8 f45;
    u8 pad46[0x342];
    u8 f388[4];
    u8 pad38C[0x44];
    s8 f3D0;
    u8 pad3D1[0x2E];
    u8 f3FF;
    u8 pad400[0x12];
    s16 f412;
    u8 pad414[0x5];
    u8 f419;
    u8 f41A;
    u8 f41B;
    u8 pad41C[0x4];
    f32 f420;
    f32 f424;
    u8 pad428[0x24];
    f32 f44C;
    u8 pad450[0x4];
    u8 f454[4];
    s32 f458;
    u8 pad45C[0x9C];
    u8 f4F8[4];
    s32 f4FC;
    u8 pad500[0xA8];
    u16 f5A8;
    u16 f5AA;
    u8 pad5AC[0x4];
    s32 f5B0;
    u8 pad5B4[0x4];
    s32 f5B8;
    u8 pad5BC[0x13];
    u8 f5CF;
    u8 f5D0;
    u8 pad5D1[0x97];
    s16 f668;
    u8 pad66A[0x26];
    s32 f690;
    u8 pad694[0x54];
    u8 f6E8;
    u8 pad6E9[0x5B];
    s32 f744;
    u8 pad748[0x98];
} Bike;

typedef struct {
    u8 pad[0x58];
    s32 flags;
} Game;

extern Bike D_800E37C0[];
extern s32 D_800E76C4;
extern f32 D_800E76C8;
extern s32 D_800E76D0;
extern s32 D_800E80D4;
extern s32 D_800E84F8;
extern s32 D_800E8958;
extern s32 D_800E8D64;
extern u8 D_800EE30C;
extern s32 D_800EFD70;
extern s32 D_800EFDBC;
extern Game D_800F05A8;

void func_8001BC98(Bike *b);
void func_8001CB80(void *a, void *b);
void func_8001CDE0(Bike *b);
void func_8001D008(Bike *b);
void func_8001D130(Bike *b, s32 n);
void func_8001D640(Bike *b);
void func_8001DD10(Bike *b);
void func_8001EC90(Bike *b);
void func_8001FB40(Bike *b);
void func_8001FC60(Bike *b);
void func_8001FE1C(Bike *b);
void func_80020D38(Bike *b);
void func_80021068(Bike *b);
void func_80021CF8(Bike *b);
void func_80035BAC(s32 i, s32 n);
void func_80036544(Bike *b);
void func_80036DF4(Bike *b);
void func_800445B0(Bike *b, s32 v);
void func_80044B60(Bike *b);
void func_8004AC98(Bike *b);
void func_8005B5F0(Bike *b);
void func_8005B928(Bike *b);
void func_8005C5BC(Bike *b, s32 n);
void func_8006BF04(Bike *b);
void func_8006FB3C(Bike *b);
void func_8006FE84(Bike *b, s32 v);

void func_80021EC8(Bike *b, s32 n)
{
    f32 t;

    D_800E80D4 = 0;
    D_800E84F8 = 0;
    D_800E8D64 = 0;
    D_800E8958 = 0;
    D_800E76C4 = n;
    D_800E76C8 = (f32)D_800E76C4 * b->f44C / 60.0f;
    if (D_800EE30C != 0 && b->f6E8 != 0) {
        D_800E76C8 *= 0.5f;
    }
    func_80021CF8(b);
    func_80036DF4(b);
    if (b->f44 == 0) {
        if (b->f45 > 0) {
            b->f5B0 = 0;
            if (b->f4FC < 10 && b->f458 < 10) {
                func_800445B0(b, b->f45);
            }
        }
        if (b->f45 < 0) {
            func_800445B0(b, -b->f45);
        }
        func_8001D130(b, n);
        func_80020D38(b);
        func_80021068(b);
        if (b->f6E8 != 0) {
            func_8005B5F0(b);
            b->f5CF = 0;
            if (b->f420 > 5.0f) {
                if (b->f41B != 0 || ((b->f5B0 & 0x20000) && D_800E76D0 - b->f5B8 < 30)) {
                    b->f5CF = 2;
                } else if (b->f41A | b->f419) {
                    b->f5CF = 1;
                }
            }
            func_8005B928(b);
        } else {
            func_8005C5BC(b, n);
        }
        if (D_800F05A8.flags & 0x10) {
            b->f412 = 0x3000;
            b->f5B0 |= 0x20;
        }
        if (b->f690 & 0x10) {
            if (b->f424 > 0.0f) {
                b->f5B0 = 0x20;
            } else {
                b->f5B0 = 0;
                b->f5D0 = 0;
                if (b - D_800E37C0 == 0) {
                    D_800F05A8.flags |= 0x2000;
                }
            }
        }
        func_80035BAC(b - D_800E37C0, n);
        func_8001D008(b);
        func_8001CDE0(b);
        func_8001DD10(b);
        func_8006FB3C(b);
        func_8001D640(b);
        func_8001EC90(b);
        if (b->f44 == 0) {
            func_8001FC60(b);
            func_8001FB40(b);
            func_8001CB80(b->f4F8, b->f388);
            func_8001CB80(b->f454, b->f388);
            func_8001BC98(b);
            switch (D_800EFDBC) {
            case 2:
            case 4:
            case 5:
            case 10:
            case 14:
                break;
            case 6:
                func_8006BF04(b);
                break;
            default:
                func_8004AC98(b);
                break;
            }
            switch (b->f3FF) {
            case 0:
                break;
            case 1:
                if (b->f3D0 == -1 && b->f420 < 8.0f && b->f744 < -30) {
                    b->f3FF = 2;
                    func_8006FE84(b, b->f668 + 0x13);
                    if (!(D_800EFD70 & 0x8000)) {
                        b->f5AA = 10;
                    }
                }
                break;
            case 2:
                if (b->f3D0 < 0) {
                    b->f3FF = 3;
                    if (D_800EFDBC == 5) {
                        b->f3FF = 0;
                        b->f6E8 = 1;
                        b->f5AA = b->f5A8;
                    }
                }
                break;
            }
        } else {
            func_80044B60(b);
        }
    } else {
        b->f5B0 = 0;
        func_80035BAC(b - D_800E37C0, n);
        func_80044B60(b);
    }
    func_8001FE1C(b);
    func_80036544(b);
}
