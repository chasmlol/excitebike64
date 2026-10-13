#include "ctx.h"

u32 func_80002E04(void);
void func_80002E10(u32 a);

u32 boot_func_80001B50(void)
{
    u32 v;

    v = func_80002E04();
    func_80002E10(v & 0xFFFFFFFE);
    return v & 1;
}
