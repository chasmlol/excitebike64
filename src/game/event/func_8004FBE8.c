#include "ctx.h"

extern s32 D_800E36F8;
extern volatile s32 D_800E30B0;
extern volatile s32 D_800E25B0;
extern s32 D_800F0618;
extern s32 D_800E374C;
extern u8 *D_800CA930;

void func_8001102C(s32 param_1);
s32 func_800595A4(void);

void func_8004FBE8(void)
{
    if (D_800E36F8 == 5) {
        if (((D_800E30B0 & 0x200) != 0) && ((D_800E25B0 & 0x200) == 0)) {
            if (*(s16 *)(D_800CA930 + 0x64) < 1) {
                return;
            }
            D_800F0618 = 3;
            D_800E36F8 = 6;
            D_800E374C = -1;
            func_8001102C(1);
            return;
        }
        if ((D_800E30B0 & 0x400) == 0) {
            return;
        }
        if ((D_800E25B0 & 0x400) != 0) {
            return;
        }
        if (func_800595A4() == 0) {
            return;
        }
        D_800F0618 = 4;
        D_800E36F8 = 6;
        D_800E374C = -1;
        func_8001102C(1);
    }
}
