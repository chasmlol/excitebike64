#include "ctx.h"

extern s32 D_800F0938;
extern s32 D_800F0A2C;
extern s32 D_800F0A18;
extern s32 D_800F0958[][8];

void func_8005F2D8(void);

s32 func_8005FFB0(s32 *dst)
{
    s32 i;

    if (D_800F0938 == 0) {
        return 1;
    }
    if (D_800F0A2C == 0) {
        func_8005F2D8();
    }
    for (i = 0; i < D_800F0A18; i++) {
        dst[i] = D_800F0958[i][8 - D_800F0A2C];
    }
    D_800F0A2C--;
    D_800F0938--;
    return 0;
}
