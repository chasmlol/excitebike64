#include "ctx.h"

extern s32 D_800E37AC;
extern u8 D_800EFD70[];
extern u32 D_800E30B0[];
extern u32 D_800E25B0[];
extern s32 D_800E3710;
extern s16 D_800E3730;

void func_8006F5F8(s32 x, s32 y);
void func_8001102C(s32 param_1);

void func_80017214(void)
{
    s32 cur;
    s32 n;
    u8 *pc;

    if (((D_800E30B0[(s8)D_800EFD70[0x2C8 + D_800E37AC]] & 0x20) != 0) &&
        ((D_800E25B0[(s8)D_800EFD70[0x2C8 + D_800E37AC]] & 0x20) == 0)) {
        D_800E3710 = 1;
        D_800E3730 = 3;
        func_8006F5F8(5, 0);
    } else {
        u32 *t30 = D_800E30B0;
        cur = (s8)D_800EFD70[0x2C8 + D_800E37AC];
        if (((t30[cur] & 0x10) != 0) && ((D_800E25B0[cur] & 0x10) == 0)) {
            pc = D_800EFD70 + cur;
            n = pc[0x50] + 1;
            pc[0x50] = n;
            if (3 < (s8)n) {
                u8 *pd = D_800EFD70 + (s8)D_800EFD70[0x2C8 + D_800E37AC];
                pd[0x50] = pd[0x50] - 4;
            }
            func_8001102C(0);
        }
    }
}
