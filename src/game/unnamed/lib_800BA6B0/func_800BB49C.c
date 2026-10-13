#include "ctx.h"

extern s32 D_800FE8D0;
extern s32 D_800FE8D4;
extern s32 D_800D5604;

void func_800BCB04(s32 a);
void func_800BFDD4(s32 a);

void func_800BB49C(void)
{
    func_800BCB04(D_800FE8D0);
    func_800BCB04(D_800FE8D4);
    if (D_800D5604 != 0) {
        func_800BFDD4(D_800D5604);
    }
    D_800D5604 = 0;
}
