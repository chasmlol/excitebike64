#include "ctx.h"

extern s32 func_800BC908(s32, s32, s32 *, s32);
extern s32 D_800C9990;
extern s32 D_800EFD64;
extern s32 D_800CB210[];
extern s32 D_800E1898;
extern s32 D_800E9288[];
extern s32 D_800D6D00[];

void func_8003A780(void)
{
    s32 i;

    D_800C9990 = 0;
    if ((((D_800EFD64 - (s32)D_800CB210) * -0x33333333) >> 5) == 0xb &&
        (i = 0, D_800E1898 == 1)) {
        for (i = 0; i < 2; i++) {
            D_800E9288[i] = func_800BC908(0x1000, 0, D_800D6D00, 0x512);
        }
        D_800C9990 = 1;
    }
}
