#include "ctx.h"

extern s16 D_800F4FD8[];
extern s32 D_800F4F94;
extern s32 D_800F4F98;
extern s32 D_800F4FE8;
extern s32 D_800F4FEC;

void func_8008EFFC(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4)
{
    s16 *b = D_800F4FD8;

    b[0] = a2 * 2;
    b[2] = 0x1FF;
    b[4] = (a0 + a2 / 2) * 4;
    b[5] = (a1 + a3 / 2) * 4;
    D_800F4F94 = a0;
    D_800F4F98 = a1;
    D_800F4FE8 = a2;
    b[6] = 0x1FF;
    D_800F4FEC = a3;
    b[1] = (a3 + a4) * 2;
}
