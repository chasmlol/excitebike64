#include "ctx.h"

extern u32 D_800C29E0;
extern u8 D_800D6154[];
extern u32 func_80013D70(u32 a, u32 b, u32 c, u32 d);
extern u32 func_800BC908(u32 a, u32 b, u8 *c, u32 d);

void func_80014B84(u32 *p1, u32 a1, u32 a2, u32 a3)
{
    u32 r;

    r = func_80013D70(a3 | 1, a1, a2, D_800C29E0);
    r = func_800BC908(r - D_800C29E0, 0, D_800D6154, 0x33B);
    *p1 = r;
    func_80013D70(a3 | 1, a1, a2, r);
}
