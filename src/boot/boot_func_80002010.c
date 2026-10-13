#include "ctx.h"

void boot_func_80001F20(u32 a, u32 b);
void boot_func_80001BB8(u32 *a, u32 w);

void boot_func_80002010(u8 *d, u8 *s, u32 n) {
    u8 *p6;
    u8 c;

    while (n != 0 && ((u32)d & 3) != 0) {
        boot_func_80001F20((u32)d++, *s++);
        n = n - 1;
    }
    if (n > 3) {
        do {
            p6 = s + 3;
            boot_func_80001BB8((u32 *)d, ((u32)s[0] << 24) | ((u32)p6[-2] << 16) | ((u32)p6[-1] << 8) | p6[0]);
            d = d + 4;
            s = s + 4;
            n = n - 4;
        } while (n > 3);
    }
    while (n != 0) {
        boot_func_80001F20((u32)d++, *s++);
        n = n - 1;
    }
}
