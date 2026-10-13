#include "ctx.h"

extern u8 D_800F1900[];
void func_80010E5C();

void func_80019E78(void)
{
    u8 *s = &D_800F1900[0];
    func_80010E5C(0, s[0x2C6]);
    func_80010E5C(1, s[0x2C5]);
    func_80010E5C(2, s[0x2C7]);
}
