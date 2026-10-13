#include "ctx.h"

extern u32 D_800F14F0;
extern u32 D_800F14F8;
void func_800BCB04();

void func_8006B160(void)
{
    if (D_800F14F0 != 0) {
        func_800BCB04(D_800F14F8);
    }
}
