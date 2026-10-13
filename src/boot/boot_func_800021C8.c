#include "ctx.h"

extern u32 boot_func_80001C74(u8 *);
extern void boot_func_80001F20(u8 *, u32);

void boot_func_800021C8(u8 *dst, u8 *src, s32 n)
{
    s32 cnt;
    u8 buf[8];

    cnt = n - 1;
    while (cnt != -1) {
        boot_func_80001F20(dst++, boot_func_80001C74(src++) & 0xFF);
        cnt--;
    }
}
