#include "ctx.h"

extern s32 D_800D5608;
extern u8 *D_800FE870;
extern s32 D_800FE878;
void func_8000ED10(u8 *a0, u8 a1, s32 a2);

void func_800BB920(s32 p)
{
    if (p != 0) {
        D_800D5608 = 1;
        return;
    }
    if (D_800D5608 != 0) {
        func_8000ED10(D_800FE870, 0, D_800FE878);
    }
    D_800D5608 = 0;
}
