#include "boot.h"

u32 boot_func_80001B50(void)
{
    u32 v;

    v = func_80002E04();
    func_80002E10(v & 0xFFFFFFFE);
    return v & 1;
}

void boot_func_80001B88(u32 a0)
{
    u32 v;

    v = func_80002E04();
    func_80002E10(v | a0);
}

void boot_func_80001BB8(s32 *p, s32 v)
{
    while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
    }
    *p = v;
}

u32 boot_func_80001BF0(u32 *param_1) {
    while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
    }
    return *param_1;
}

u32 boot_func_80001C74(u32 addr)
{
    u32 sh = (~addr & 3) << 3;
    u32 v = boot_func_80001BF0(addr & ~3u);
    return (v >> sh) & 0xFF;
}

u32 boot_func_80001CB4(u32 a) {
    u32 v;
    u32 sh;

    sh = (~a & 3) << 3;
    v = boot_func_80001BF0((void *)(a & ~3));
    return (v >> sh) & 0xFF;
}

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

void boot_func_80001F20(u32 param_1, u8 param_2) {
    u32 shift;
    u32 v;

    shift = (~param_1 & 3) << 3;
    param_1 = param_1 & 0xFFFFFFFC;
    v = boot_func_80001BF0((void *)param_1);
    boot_func_80001BB8((void *)param_1, (v & ~(0xFFU << shift)) | ((u32)param_2 << shift));
}

void boot_func_80001F98(u32 addr, u8 val) {
    u32 sh;
    u32 v;

    sh = (~addr & 3) << 3;
    addr = addr & 0xFFFFFFFC;
    v = boot_func_80001BF0(addr);
    boot_func_80001BB8(addr, (v & ~(0xFFu << sh)) | (val << sh));
}

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

void boot_func_800023EC(void)
{
    u8 buf[8];
    u32 *src;
    u32 size;
    u32 i;

    src = (u32 *)(boot_func_80001BF0(0xB1FFFFF4) & 0xB1FFFFFC);
    size = boot_func_80001BF0(0xB1FFFFF8) & 0x01FFFFFC;
    boot_func_80001BB8(0xB1FFFFFC, 0);
    while (boot_func_80001BF0(0xB0000010) == 0) {
        boot_func_80001C2C(500);
    }
    for (i = 0; i < size >> 2; i++) {
        boot_func_80001BB8((u32)&((u32 *)((u32)src & 0xB07FFFFF))[i], boot_func_80001BF0((u32)&src[i]));
    }
    boot_func_80001C2C(2000);
    boot_func_80001BB8(0xB1FFFFF4, 0);
}

s32 boot_func_80002808(char *s)
{
    char c;
    char *p;
    c = *s;
    p = s;
    while (p = p + 1, c != 0) {
        c = *p;
    }
    return p - s;
}

void boot_func_80002828(s32 p1, s32 p2, s32 p3, s32 p4)
{
    s32 v;

    boot_func_80001BB8(0xB1FFFFE4, p2);
    boot_func_80001BB8(0xB1FFFFE8, p3);
    boot_func_80001BB8(0xB1FFFFEC, p4);
    boot_func_80001BB8(0xB1FFFFE0, p1);

    do {
        boot_func_80001C2C(1000);
        v = boot_func_80001BF0(0xB1FFFFF0);
        if (v != 0) {
            __asm__ volatile("break 0x400");
        }
        v = boot_func_80001BF0(0xB1FFFFE0);
    } while (v != 0);

    boot_func_80001BF0(0xB1FFFFDC);
}

s32 boot_func_8000292C(u8 *arg0, s32 arg1) {
    s32 mask;
    s32 len;
    s32 ret;

    mask = boot_func_80001B50();
    len = boot_func_80002808(arg0);
    boot_func_80002010((void *)0xB1FF0000, arg0, len);
    ret = boot_func_80002828_3(0x202, len, arg1, 0);
    if (ret & 0x8000) {
        ret |= 0xFFFF0000;
    }
    boot_func_80001B88(mask);
    return ret;
}

s32 boot_func_800029BC(u8 *arg0, s32 arg1) {
    s32 mask;
    s32 len;
    s32 ret;

    mask = boot_func_80001B50();
    len = boot_func_80002808(arg0);
    boot_func_80002010((void *)0xB1FF0000, arg0, len);
    ret = boot_func_80002828_3(0x303, len, arg1, 0);
    if (ret & 0x8000) {
        ret |= 0xFFFF0000;
    }
    boot_func_80001B88(mask);
    return ret;
}

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
            got = boot_func_80002828_4(0x505, fd, len, 0);
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

s32 boot_func_80002B78(s32 fd, u8 *buf, u32 size)
{
    u32 s;
    u32 len;
    s32 got;
    s32 total;

    s = boot_func_80001B50();
    total = 0;
    while (size != 0) {
        len = size;
        if (0x8000 < size) {
            len = 0x8000;
        }
        boot_func_80002010((void *)0xB1FF0000, buf, len);
        do {
            got = boot_func_80002828_5(0x606, fd, len, 0);
        } while (0);
        if (got == 0) {
            boot_func_80001B88(s);
            return total;
        }
        buf += got;
        size -= got;
        total += got;
    }
    boot_func_80001B88(s);
    return total;
}

u32 boot_func_80002C40(u32 p1, u32 p2, u32 p3)
{
    u32 u;
    u32 r;

    u = boot_func_80001B50();
    r = boot_func_80002828_2(0x707, p1, p2, p3);
    boot_func_80001B88_2(u);
    return r;
}
