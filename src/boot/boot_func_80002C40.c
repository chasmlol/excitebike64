#include "ctx.h"

extern u32 boot_func_80001B50(void);
extern u32 boot_func_80002828(u32 a, u32 b, u32 c, u32 d);
extern u32 boot_func_80001B88(u32 a);

u32 boot_func_80002C40(u32 p1, u32 p2, u32 p3)
{
    u32 u;
    u32 r;

    u = boot_func_80001B50();
    r = boot_func_80002828(0x707, p1, p2, p3);
    boot_func_80001B88(u);
    return r;
}
