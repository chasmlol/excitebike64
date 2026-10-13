#include "ctx.h"

extern u8 D_800D8BA0[];
extern u8 *D_800F0B00;
extern u8 *D_800F0B04;
extern s32 D_800F0B08;

u8 *func_800BC908();
void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_80061DCC(void)
{
    D_800F0B08 = 0;
    D_800F0B00 = func_800BC908(16000, 0, D_800D8BA0, 0x103);
    D_800F0B04 = D_800F0B00;
    func_8000ED10(D_800F0B00, 0, 16000);
}
