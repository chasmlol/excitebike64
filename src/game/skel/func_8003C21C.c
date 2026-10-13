#include "ctx.h"

extern u8 D_800D6DF4[];
extern void *D_800E9628;
extern void *D_800E92D0;
extern u32 D_800E92DC;

void *func_800BC908(u32 a0, u32 a1, u8 *a2, u32 a3);
void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_8003C21C(void)
{
    u32 x;

    D_800E9628 = func_800BC908(0x9000, 0, D_800D6DF4, 0x1F6);
    D_800E92D0 = func_800BC908(0x4040, 0, D_800D6DF4, 0x1F9);
    func_8000ED10((u8 *)D_800E92D0, 0, 0x4040);
    x = (u32)D_800E92D0;
    D_800E92DC = x;
    while ((x & 0x3F) != 0) {
        D_800E92DC = D_800E92DC + 0x10;
        x = D_800E92DC;
    }
}
