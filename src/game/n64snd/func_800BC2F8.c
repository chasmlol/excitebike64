#include "ctx.h"

extern u8 *D_8010061C;
extern s32 D_80100614;
extern s32 D_800D5620;
extern s32 D_800D5638;

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800AD910();

void func_800BC2F8(s32 a0)
{
    if (a0 != 0) {
        D_800D5620 = 1;
        func_8000ED10(D_8010061C, 0, D_80100614 << 1);
        func_800AD910(D_8010061C, D_80100614 << 1);
        D_800D5638 = 3;
    } else {
        D_800D5620 = 0;
    }
}
