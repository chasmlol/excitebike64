#include "ctx.h"

extern void func_800BCB04(s32 a);
extern s32 D_800D5608;
extern s32 D_800FE870;

void func_800BB9C0(void)
{
    if (D_800D5608 != 0) {
        func_800BCB04(D_800FE870);
        D_800D5608 = 0;
    }
}
