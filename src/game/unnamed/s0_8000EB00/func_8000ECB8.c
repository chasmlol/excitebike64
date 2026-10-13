#include "ctx.h"

void func_8000ECB8(void)
{
    __asm__ volatile("break 0x407");
    while (1) {}
}
