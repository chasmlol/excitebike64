#include "ctx.h"

void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800AD910(u32 a0, u32 a1);

extern s32 D_80100614;
extern u8 *D_8010061C;
extern s32 D_800D5638;

void func_800BC370(void)
{
    func_8000ED10(D_8010061C, 0, D_80100614 << 1);
    func_800AD910((u32)D_8010061C, D_80100614 << 1);
    D_800D5638 = 3;
}
