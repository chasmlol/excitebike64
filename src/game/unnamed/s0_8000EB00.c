#include "game.h"

/* Small helpers: strings (str_*, named so that GCC does not treat them as its built-ins), mem_fill, number to text,
   isqrt_u32, random_byte and fatal_halt. The boot loader links ten of them again from this C (boot_*). */

/* Stubbed out: prints nothing. */
void debug_printf(s32 fmt, ...)
{
}

/* The integer square root of x, one result bit at a time from the top (the shift-and-subtract method, unrolled). */
u16 isqrt_u32(u32 x)
{
    s32 root;
    s32 trial;
    u32 top;

    top = 0x40000000;
    root = 0;
    if (x >= top) {
        x = x - top;
        root = top;
    }
    trial = root + 0x10000000;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x10000000;
    }
    trial = root + 0x4000000;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x4000000;
    }
    trial = root + 0x1000000;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x1000000;
    }
    trial = root + 0x400000;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x400000;
    }
    trial = root + 0x100000;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x100000;
    }
    trial = root + 0x40000;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x40000;
    }
    trial = root + 0x10000;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x10000;
    }
    trial = root + 0x4000;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x4000;
    }
    trial = root + 0x1000;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x1000;
    }
    trial = root + 0x400;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x400;
    }
    trial = root + 0x100;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x100;
    }
    trial = root + 0x40;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x40;
    }
    trial = root + 0x10;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x10;
    }
    trial = root + 0x4;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x4;
    }
    trial = root + 0x1;
    root = root >> 1;
    if (x >= (u32)trial) {
        x = x - (u32)trial;
        root = root | 0x1;
    }
    return (u16)root;
}

/* Stops the game on an unrecoverable error: break 0x407 (for a debugger), then hangs. The code is not used. */
void fatal_halt(s32 code)
{
    __asm__ volatile("break 0x407");
    while (1) {}
}

/* fatal_halt after the heap report (mem_report, which ignores its arguments). */
void fatal_halt_mem_report(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    mem_report(arg0, arg1, arg2, arg3);
    __asm__ volatile("break 0x407");
    while (1) {
    }
}

/* ASCII case conversion of one character; other characters are returned as they are. */
s32 char_to_upper(s32 c)
{
    if ((u32)(c - 'a') < 26) {
        c = c - ('a' - 'A');
    }
    return c;
}

s32 char_to_lower(s32 c)
{
    if ((u32)(c - 'A') < 26) {
        c = c + ('a' - 'A');
    }
    return c;
}

/* Sets len bytes at dst to value. */
void mem_fill(u8 *dst, u8 value, s32 len)
{
    while (len > 0) {
        *dst = value;
        dst = dst + 1;
        len = len - 1;
    }
}

/* Appends src to the string dst. */
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

/* Copies at most size - 1 characters of src to dst and always ends dst with a 0. */
void str_copy_n(u8 *dst, u8 *src, s32 size)
{
    for (;;) {
        size = size - 1;
        *dst = *src;
        if (size == 0) {
            *dst = 0;
            return;
        }
        dst = dst + 1;
        if (*src == 0) {
            return;
        }
        src = src + 1;
    }
}

/* char_to_upper again, for str_icmp to inline. */
static inline s32 to_upper_inline(s32 c)
{
    if ((u32)(c - 'a') < 26) {
        return c - ('a' - 'A');
    }
    return c;
}

/* Compares two strings ignoring case: -1, 0 or 1. */
s32 str_icmp(u8 *a, u8 *b)
{
    for (;;) {
        s32 ca = *a;
        s32 cb;
        if (to_upper_inline(ca) < to_upper_inline(cb = *b)) {
            return -1;
        }
        if (to_upper_inline(ca) > to_upper_inline(cb)) {
            return 1;
        }
        if (ca == 0) {
            return 0;
        }
        a++;
        b++;
    }
}

/* Empty. */
void critical_enter(void)
{
}

void critical_leave(void)
{
}

/* Writes value in decimal to out, with a '-' when negative. Only the low 28 bits of the magnitude are printed. */
void int_to_str(s32 value, char *out)
{
    s32 n;
    s32 divisor;
    s32 digit;

    if (value < 0) {
        *out = '-';
        out++;
        value = -value;
    }
    n = value & 0xFFFFFFF;
    divisor = 1;
    if (n != 0) {
        do {
            divisor = divisor * 10;
        } while (divisor <= n);
    }
    digit = 0;
    if (n != 0) {
        divisor = divisor / 10;
    }
    for (; 0 < divisor; divisor = divisor / 10) {
        digit = n / divisor;
        *out = digit + '0';
        out++;
        n = n - digit * divisor;
    }
    *out = 0;
}

/* Writes value as eight upper-case hex digits to out. */
void u32_to_hex_str(u32 value, char *out)
{
    s32 i;
    s32 digit;
    for (i = 0; i < 8; i++) {
        digit = (value >> (28 - 4 * i)) & 0xF;
        if (digit >= 10) {
            digit += 7;     /* 'A' - '0' - 10 */
        }
        digit += '0';
        out[i] = digit;
    }
    out[i] = 0;
}

/* Converts a string to upper case in place. */
void str_upper(u8 *s)
{
    u8 c = *s;
    while (c != 0) {
        c = *s;
        if ((u8)(c - 'a') < 26) {
            c -= ('a' - 'A');
        }
        *s = c;
        s++;
        c = *s;
    }
}

/* Copies the string src to dst. */
void str_copy(char *dst, char *src)
{
    while (1) {
        *dst = *src;
        dst = dst + 1;
        if (*src == 0) {
            break;
        }
        src = src + 1;
    }
}

/* The debug log file: stubbed out, it only marks the log as open (sDebugLogFd -1 when closed). */
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

/* A pseudo-random byte from a linear congruential generator modulo the prime 65531 (sRandomSeed). */
u32 random_byte(void)
{
    s32 next;
    s32 seed;

    next = sRandomSeed * 0x35DF + 0x3038;
    seed = next - (next / 0xFFFB) * 0xFFFB;
    sRandomSeed = seed;
    return seed & 0xFF;
}
