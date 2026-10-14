#include "game.h"

void debug_printf(s32 a, ...)
{
}

u16 isqrt_u32(u32 r)
{
    s32 v;
    s32 t;
    u32 k;

    k = 0x40000000;
    v = 0;
    if (r >= k) {
        r = r - k;
        v = k;
    }
    t = v + 0x10000000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x10000000;
    }
    t = v + 0x4000000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x4000000;
    }
    t = v + 0x1000000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x1000000;
    }
    t = v + 0x400000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x400000;
    }
    t = v + 0x100000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x100000;
    }
    t = v + 0x40000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x40000;
    }
    t = v + 0x10000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x10000;
    }
    t = v + 0x4000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x4000;
    }
    t = v + 0x1000;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x1000;
    }
    t = v + 0x400;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x400;
    }
    t = v + 0x100;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x100;
    }
    t = v + 0x40;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x40;
    }
    t = v + 0x10;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x10;
    }
    t = v + 0x4;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x4;
    }
    t = v + 0x1;
    v = v >> 1;
    if (r >= (u32)t) {
        r = r - (u32)t;
        v = v | 0x1;
    }
    return (u16)v;
}

void fatal_halt(s32 arg0)
{
    __asm__ volatile("break 0x407");
    while (1) {}
}

void fatal_halt_mem_report(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    mem_report(arg0, arg1, arg2, arg3);
    __asm__ volatile("break 0x407");
    while (1) {
    }
}

s32 char_to_upper(s32 a0)
{
    if ((u32)(a0 - 0x61) < 0x1A) {
        a0 = a0 - 0x20;
    }
    return a0;
}

s32 char_to_lower(s32 a0)
{
    if ((u32)(a0 - 0x41) < 0x1A) {
        a0 = a0 + 0x20;
    }
    return a0;
}

void mem_fill(u8 *a0, u8 a1, s32 a2)
{
    while (a2 > 0) {
        *a0 = a1;
        a0 = a0 + 1;
        a2 = a2 - 1;
    }
}

void str_cat(char *dst, char *src)
{
    while (*dst != '\0') {
        dst++;
    }
loop:
    *dst = *src;
    dst++;
    if (*src == '\0') {
        goto done;
    }
    src++;
    goto loop;
done:
    return;
}

void str_copy_n(u8 *d, u8 *s, s32 n)
{
    for (;;) {
        n = n - 1;
        *d = *s;
        if (n == 0) {
            *d = 0;
            return;
        }
        d = d + 1;
        if (*s == 0) {
            return;
        }
        s = s + 1;
    }
}

static inline s32 up(s32 c)
{
    if ((u32)(c - 0x61) < 0x1A) {
        return c - 0x20;
    }
    return c;
}

s32 str_icmp(u8 *param_1, u8 *param_2)
{
    for (;;) {
        s32 c1 = *param_1;
        s32 c2;
        if (up(c1) < up(c2 = *param_2)) {
            return -1;
        }
        if (up(c1) > up(c2)) {
            return 1;
        }
        if (c1 == 0) {
            return 0;
        }
        param_1++;
        param_2++;
    }
}

void critical_enter(void)
{
}

void critical_leave(void)
{
}

void int_to_str(s32 v, char *out)
{
    s32 u;
    s32 d;
    s32 q;

    if (v < 0) {
        *out = '-';
        out++;
        v = -v;
    }
    u = v & 0xFFFFFFF;
    d = 1;
    if (u != 0) {
        do {
            d = d * 10;
        } while (d <= u);
    }
    q = 0;
    if (u != 0) {
        d = d / 10;
    }
    for (; 0 < d; d = d / 10) {
        q = u / d;
        *out = q + '0';
        out++;
        u = u - q * d;
    }
    *out = 0;
}

void u32_to_hex_str(u32 param_1, char *param_2) {
    s32 i;
    s32 d;
    for (i = 0; i < 8; i++) {
        d = (param_1 >> (28 - 4 * i)) & 0xF;
        if (d >= 10) {
            d += 7;
        }
        d += '0';
        param_2[i] = d;
    }
    param_2[i] = 0;
}

void str_upper(u8 *s)
{
    u8 c = *s;
    while (c != 0) {
        c = *s;
        if ((u8)(c - 0x61) < 0x1A) {
            c -= 0x20;
        }
        *s = c;
        s++;
        c = *s;
    }
}

void str_copy(char *a0, char *a1)
{
    while (1) {
        *a0 = *a1;
        a0 = a0 + 1;
        if (*a1 == 0) {
            break;
        }
        a1 = a1 + 1;
    }
}

void debug_log()
{
    if (sDebugLogFd == -1) {
        sDebugLogFd = 0;
    }
}

void debug_log_close(void)
{
    if (sDebugLogFd >= 0) {
        sDebugLogFd = -1;
    }
}

u32 random_byte(void) {
    s32 x;
    s32 v;

    x = sRandomSeed * 0x35DF + 0x3038;
    v = x - (x / 0xFFFB) * 0xFFFB;
    sRandomSeed = v;
    return v & 0xFF;
}
