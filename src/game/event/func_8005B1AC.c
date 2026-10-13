#include "ctx.h"

extern u8 D_800F05A8[];
void func_80064DB8();

void func_8005B1AC(void)
{
    u32 *p = (u32 *)D_800F05A8;
    p[0x58 / 4] = p[0x58 / 4] | 0x10;
    p[0x2C / 4] = 0x1C20;
    func_80064DB8();
}
