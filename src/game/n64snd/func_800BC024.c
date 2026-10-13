#include "ctx.h"

extern void func_800BB49C(void);
extern void func_800BCB04(s32 a);
extern s32 D_800D562C;
extern s32 D_80100610;
extern s32 D_80100620;

void func_800BC024(void)
{
    D_800D562C = 0;
    func_800BB49C();
    func_800BCB04(D_80100610);
    func_800BCB04(D_80100620);
}
