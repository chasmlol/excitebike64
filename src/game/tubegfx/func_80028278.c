#include "ctx.h"

extern u32 D_800C9580;
extern u32 D_800C9584;
void func_800BCB04();

void func_80028278(void)
{
    if (D_800C9580 != 0) {
        func_800BCB04(D_800C9580);
        D_800C9580 = 0;
    }
    if (D_800C9584 != 0) {
        func_800BCB04(D_800C9584);
        D_800C9584 = 0;
    }
}
