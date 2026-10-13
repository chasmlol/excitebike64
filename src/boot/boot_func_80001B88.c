#include "ctx.h"

u32 func_80002E04(void);
void func_80002E10(u32 a);

void boot_func_80001B88(u32 a0)
{
    u32 v;

    v = func_80002E04();
    func_80002E10(v | a0);
}
