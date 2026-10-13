#include "ctx.h"

extern u32 D_800F5A80;
void func_800BCB04();

void func_800A1A68(void)
{
    u32 *p = &D_800F5A80;
    func_800BCB04(p[0]);
    func_800BCB04(p[1]);
}
