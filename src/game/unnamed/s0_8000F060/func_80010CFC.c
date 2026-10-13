#include "ctx.h"

void func_8004F190(void);
void func_800B8C8C(void);

extern s32 D_800C2B5C;
extern s32 D_800D5710;
extern s32 D_800D5730;

void func_80010CFC(void)
{
    if (D_800C2B5C != 0) {
        func_8004F190();
        D_800D5710 = 0;
        D_800C2B5C = 0;
        D_800D5730 = 0;
        func_800B8C8C();
    }
}
