#include "ctx.h"

extern s32 D_800C9800;
void func_800BCB04(s32 a);

void func_80031D24(void)
{
    func_800BCB04(D_800C9800);
    D_800C9800 = 0;
}
