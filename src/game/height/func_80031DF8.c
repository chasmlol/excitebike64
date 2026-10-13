#include "ctx.h"

extern s32 D_800E8940[];
extern s32 D_800E8950[];

int func_80031DF8(int x, int y)
{
    int a, b;

    b = y >> 4;
    a = x >> 4;
    a = (a - D_800E8940[0]) * 16 / (D_800E8950[0] - D_800E8940[0]);
    if (a < 0) a = 0;
    if (a > 15) a = 15;
    b = (b - D_800E8940[0]) * 16 / (D_800E8950[1] - D_800E8940[1]);
    if (b < 0) b = 0;
    if (b > 15) b = 15;
    return b * 16 + a;
}
