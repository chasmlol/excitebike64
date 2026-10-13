#include "ctx.h"

u32 func_80001B50(void);
u32 func_80002828(u32, u32, u32, u32);
u32 func_80001B88(u32);

u32 boot_func_800028E0(void)
{
    u32 a = func_80001B50();
    u32 r = func_80002828(0x101, 0, 0, 0);
    func_80001B88(a);
    return r;
}
