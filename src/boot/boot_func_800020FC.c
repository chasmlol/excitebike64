#include "ctx.h"

void boot_func_80001F20(u32 a, u32 b);
void boot_func_80001BB8(u32 a, u32 b);

void boot_func_800020FC(u8 *p0, u32 c0, u32 n0)
{
    u8 *p = p0;
    u32 n = n0;
    u32 c = c0;
    u32 w;
    while (n != 0 && ((u32)p & 3) != 0) {
        boot_func_80001F20((u32)p++, c & 0xFF);
        n--;
    }
    if (n > 3) {
        w = ((c & 0xFF) << 24) | ((c & 0xFF) << 16) | ((c & 0xFF) << 8) | (c & 0xFF);
        do {
            boot_func_80001BB8((u32)p, w);
            n -= 4;
            p += 4;
        } while (n > 3);
    }
    while (n != 0) {
        boot_func_80001F20((u32)p++, c & 0xFF);
        n--;
    }
}
