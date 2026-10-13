#include "ctx.h"

extern u32 D_800C9990;
extern u32 D_800E9288;
void func_800BCB04();

void func_8003A84C(void)
{
    s32 i;
    u32 *p;
    if (D_800C9990 != 0) {
        i = 0;
        p = &D_800E9288;
        do {
            u32 v = p[0];
            p++;
            i++;
            func_800BCB04(v);
        } while (i < 2);
        D_800C9990 = 0;
    }
}
