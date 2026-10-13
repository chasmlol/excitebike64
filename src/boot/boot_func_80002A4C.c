#include "ctx.h"

u32 boot_func_80001B50(void);
u32 boot_func_80002828(u32 a, u32 b, u32 c, u32 d);
void boot_func_80001B88(u32 a);

u32 boot_func_80002A4C(u32 p)
{
    u32 s1, r;
    s1 = boot_func_80001B50();
    r = boot_func_80002828(0x404, p, 0, 0);
    if ((r & 0x8000) != 0) {
        r = r | 0xffff0000;
    }
    boot_func_80001B88(s1);
    return r;
}
