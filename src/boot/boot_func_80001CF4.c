#include "ctx.h"

u32 boot_func_80001C74(u32 a);
u32 boot_func_80001BF0(u32 a);

void boot_func_80001CF4(u32 src, u8 *dst, u32 n)
{
    u32 w;
    u32 s = src;
    u8 *d = dst;
    u32 cnt = n;

    if (cnt == 0) {
        goto words;
    }
loop:
    if ((s & 3) == 0) {
        goto words;
    }
    *d = (u8)boot_func_80001C74(s++);
    d++;
    cnt--;
    if (cnt != 0) {
        goto loop;
    }
words:
    while (cnt >= 4) {
        w = boot_func_80001BF0(s);
        *d = (u8)(w >> 24);
        d++;
        *d = (u8)(w >> 16);
        d++;
        *d = (u8)(w >> 8);
        d++;
        *d = (u8)w;
        d++;
        s += 4;
        cnt -= 4;
    }
    while (cnt != 0) {
        *d = (u8)boot_func_80001C74(s++);
        d++;
        cnt--;
    }
}
