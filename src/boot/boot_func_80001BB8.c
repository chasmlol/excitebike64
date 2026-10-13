#include "ctx.h"

void boot_func_80001BB8(s32 *p, s32 v)
{
    while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
    }
    *p = v;
}
