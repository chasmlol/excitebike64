#include "ctx.h"

extern void func_8000ECB8(s32) __attribute__((noreturn));

void func_800A3818(void)
{
    *(u8 *)0x80394109 = 1;
    func_8000ECB8(1);
}
