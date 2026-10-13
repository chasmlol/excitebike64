#include "ctx.h"

u32 func_80001B50(void);
void func_80001B88(u32 x);
void func_80002010(void *dst, void *src, u32 n);
s32 func_80002828(s32 a0, s32 a1, u32 a2, s32 a3);

s32 boot_func_80002B78(s32 fd, u8 *buf, u32 size)
{
    u32 s;
    u32 len;
    s32 got;
    s32 total;

    s = func_80001B50();
    total = 0;
    while (size != 0) {
        len = size;
        if (0x8000 < size) {
            len = 0x8000;
        }
        func_80002010((void *)0xB1FF0000, buf, len);
        do {
            got = func_80002828(0x606, fd, len, 0);
        } while (0);
        if (got == 0) {
            func_80001B88(s);
            return total;
        }
        buf += got;
        size -= got;
        total += got;
    }
    func_80001B88(s);
    return total;
}
