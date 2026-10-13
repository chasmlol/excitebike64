#include "ctx.h"

extern s32 D_800E1898;
extern s32 D_800E76D0;
extern s32 D_800E9294;
extern f32 D_800E9278[];
extern s32 D_800C9998;
extern s32 D_800DC0E8[];
extern s32 D_800E9210;
extern u8 *D_800E9214;

s32 func_8000EFE8(void);
void func_80036374(s32 a, s32 b, f32 *c, s32 *d, s32 *e);

void func_80037290(void)
{
    s32 i;
    s32 z;
    s32 v;
    f32 lf[3];
    s32 li[3];

    if (D_800E1898 < 4) {
        if (D_800E76D0 < D_800E9294) {
            D_800E9294 = -1000;
        }
        if (D_800E76D0 - D_800E9294 < 0x1F) {
            z = 0;
            i = z;
            li[0] = 0;
            li[1] = 0;
            li[2] = 0;
            do {
                i = i + 1;
                v = func_8000EFE8();
                lf[0] = D_800E9278[0] + ((f32)v - 128.0f) * 0.00390625f;
                v = func_8000EFE8();
                lf[1] = D_800E9278[1] + ((f32)v - 128.0f) * 0.0009765625f;
                v = func_8000EFE8();
                lf[2] = D_800E9278[2] + ((f32)v - 128.0f) * 0.00390625f;
                func_80036374(0, z, lf, li, &D_800DC0E8[D_800C9998]);
                *(u16 *)(D_800E9214 + D_800E9210 * 0x24 + 2) = 0x1E;
            } while (i <= 0);
        }
    }
}
