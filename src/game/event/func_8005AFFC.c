#include "ctx.h"

extern s32 D_800E36F8;
extern s32 D_800E374C;

s32 func_8005AFFC(void)
{
    if (D_800E36F8 == 5) {
        D_800E36F8 = 6;
        D_800E374C = 0x14;
        return 1;
    }
    return 0;
}
