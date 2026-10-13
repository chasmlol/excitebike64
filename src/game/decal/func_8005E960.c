#include "ctx.h"

extern s32 D_800F0820;
extern s32 D_800F07F8;
extern s32 D_800F0774;
void func_800BCB04(s32 a);

void func_8005E960(void)
{
    func_800BCB04(D_800F0820);
    func_800BCB04(D_800F07F8);
    func_800BCB04(D_800F0774);
}
