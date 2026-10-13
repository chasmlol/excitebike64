#include "ctx.h"

extern s32 D_800F4F94;
extern s32 D_800F4F98;
extern s32 D_800F4FE8;
extern s32 D_800F4FEC;
extern s16 D_800F4FD8[];

void func_8008EF84(s32 a0, s32 a1, s32 a2, s32 a3) {
    s16 *q = D_800F4FD8;
    s32 k = 0x1FF;

    q[0] = a2 << 1;
    q[1] = a3 << 1;
    q[2] = k;
    q[4] = (a0 + a2 / 2) * 4;
    q[5] = (a1 + a3 / 2) * 4;
    D_800F4F94 = a0;
    D_800F4F98 = a1;
    D_800F4FE8 = a2;
    q[6] = k;
    D_800F4FEC = a3;
}
