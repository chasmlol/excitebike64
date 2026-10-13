#include "ctx.h"

u32 boot_func_80001B50(void);
u32 boot_func_80002828(s32 a, s32 b, u32 c, s32 d);
void boot_func_80001CF4(u32 a, u8 *b, u32 c);
void boot_func_80001B88(u32 a);

s32 boot_func_80002AB4(s32 fd, u8 *buf, u32 size)
{
    u32 s;
    u32 len;
    u32 got;
    s32 total;

    s = boot_func_80001B50();
    total = 0;
    while (size != 0) {
        len = size;
        if (0x8000 < size) {
            len = 0x8000;
        }
        do {
            got = boot_func_80002828(0x505, fd, len, 0);
        } while (0);
        if (got == 0) {
            boot_func_80001B88(s);
            return total;
        }
        boot_func_80001CF4(0xb1ff0000, buf, got);
        buf += got;
        size -= got;
        total += got;
    }
    boot_func_80001B88(s);
    return total;
}
