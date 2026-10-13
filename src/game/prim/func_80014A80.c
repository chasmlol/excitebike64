#include "ctx.h"

extern u8 D_800D614C[];
extern u8 D_800D6150[];
extern s32 D_800E36D0;
extern s32 D_800C8C78;

void func_800BFC40(void *p, void *f);
void func_80014CCC(void);
void func_80014D20(void);

void func_80014A80(void)
{
    func_800BFC40(D_800D614C, func_80014CCC);
    func_800BFC40(D_800D6150, func_80014D20);
    D_800E36D0 = -1;
    D_800C8C78 = 0;
}
