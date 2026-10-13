#include "ctx.h"

u32 boot_func_80001C74(u32 a);
u32 boot_func_80001BF0(u32 a);
void boot_func_80001DD0(u32 a, u32 b);

void boot_func_80001E0C(u32 src, u32 dst, u32 cnt)
{
    u32 s = src;
    u32 d = dst;
    u32 n = cnt;
    u32 w;

    while (n != 0 && (s & 3) != 0) {
        boot_func_80001DD0(d++, boot_func_80001C74(s++) & 0xFF);
        n--;
    }
    while (n > 3) {
        w = boot_func_80001BF0(s);
        boot_func_80001DD0(d++, w >> 24);
        boot_func_80001DD0(d++, (w >> 16) & 0xFF);
        boot_func_80001DD0(d++, (w >> 8) & 0xFF);
        boot_func_80001DD0(d++, w & 0xFF);
        s += 4;
        n -= 4;
    }
    while (n != 0) {
        boot_func_80001DD0(d++, boot_func_80001C74(s++) & 0xFF);
        n--;
    }
}
