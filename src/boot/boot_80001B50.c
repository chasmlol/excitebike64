#include "boot.h"

u32 sn_disable_ints(void)
{
    u32 v;

    v = sn_get_status_reg();
    sn_set_status_reg(v & 0xFFFFFFFE);
    return v & 1;
}

void sn_restore_ints(u32 a0)
{
    u32 v;

    v = sn_get_status_reg();
    sn_set_status_reg(v | a0);
}

void sn_pi_write_word(s32 *p, s32 v)
{
    while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
    }
    *p = v;
}

u32 sn_pi_read_word(u32 *param_1) {
    while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
    }
    return *param_1;
}

u32 sn_pi_read_byte(u32 addr)
{
    u32 sh = (~addr & 3) << 3;
    u32 v = sn_pi_read_word(addr & ~3u);
    return (v >> sh) & 0xFF;
}

u32 sn_pi_read_byte_alt(u32 a) {
    u32 v;
    u32 sh;

    sh = (~a & 3) << 3;
    v = sn_pi_read_word((void *)(a & ~3));
    return (v >> sh) & 0xFF;
}

void sn_pi_read_bytes(u32 src, u8 *dst, u32 n)
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
    *d = (u8)sn_pi_read_byte(s++);
    d++;
    cnt--;
    if (cnt != 0) {
        goto loop;
    }
words:
    while (cnt >= 4) {
        w = sn_pi_read_word(s);
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
        *d = (u8)sn_pi_read_byte(s++);
        d++;
        cnt--;
    }
}

void sn_pi_read_bytes_wordwise(u32 src, u32 dst, u32 cnt)
{
    u32 s = src;
    u32 d = dst;
    u32 n = cnt;
    u32 w;

    while (n != 0 && (s & 3) != 0) {
        sn_write_byte_wordwise(d++, sn_pi_read_byte(s++) & 0xFF);
        n--;
    }
    while (n > 3) {
        w = sn_pi_read_word(s);
        sn_write_byte_wordwise(d++, w >> 24);
        sn_write_byte_wordwise(d++, (w >> 16) & 0xFF);
        sn_write_byte_wordwise(d++, (w >> 8) & 0xFF);
        sn_write_byte_wordwise(d++, w & 0xFF);
        s += 4;
        n -= 4;
    }
    while (n != 0) {
        sn_write_byte_wordwise(d++, sn_pi_read_byte(s++) & 0xFF);
        n--;
    }
}

void sn_pi_write_byte(u32 param_1, u8 param_2) {
    u32 shift;
    u32 v;

    shift = (~param_1 & 3) << 3;
    param_1 = param_1 & 0xFFFFFFFC;
    v = sn_pi_read_word((void *)param_1);
    sn_pi_write_word((void *)param_1, (v & ~(0xFFU << shift)) | ((u32)param_2 << shift));
}

void sn_pi_write_byte_alt(u32 addr, u8 val) {
    u32 sh;
    u32 v;

    sh = (~addr & 3) << 3;
    addr = addr & 0xFFFFFFFC;
    v = sn_pi_read_word(addr);
    sn_pi_write_word(addr, (v & ~(0xFFu << sh)) | (val << sh));
}

void sn_pi_fill(u8 *p0, u32 c0, u32 n0)
{
    u8 *p = p0;
    u32 n = n0;
    u32 c = c0;
    u32 w;
    while (n != 0 && ((u32)p & 3) != 0) {
        sn_pi_write_byte((u32)p++, c & 0xFF);
        n--;
    }
    if (n > 3) {
        w = ((c & 0xFF) << 24) | ((c & 0xFF) << 16) | ((c & 0xFF) << 8) | (c & 0xFF);
        do {
            sn_pi_write_word((u32)p, w);
            n -= 4;
            p += 4;
        } while (n > 3);
    }
    while (n != 0) {
        sn_pi_write_byte((u32)p++, c & 0xFF);
        n--;
    }
}

void sn_pi_copy_bytes(u8 *dst, u8 *src, s32 n)
{
    s32 cnt;
    u8 buf[8];

    cnt = n - 1;
    while (cnt != -1) {
        sn_pi_write_byte(dst++, sn_pi_read_byte(src++) & 0xFF);
        cnt--;
    }
}

void sn_host_copy_rom(void)
{
    u8 buf[8];
    u32 *src;
    u32 size;
    u32 i;

    src = (u32 *)(sn_pi_read_word(0xB1FFFFF4) & 0xB1FFFFFC);
    size = sn_pi_read_word(0xB1FFFFF8) & 0x01FFFFFC;
    sn_pi_write_word(0xB1FFFFFC, 0);
    while (sn_pi_read_word(0xB0000010) == 0) {
        sn_delay(500);
    }
    for (i = 0; i < size >> 2; i++) {
        sn_pi_write_word((u32)&((u32 *)((u32)src & 0xB07FFFFF))[i], sn_pi_read_word((u32)&src[i]));
    }
    sn_delay(2000);
    sn_pi_write_word(0xB1FFFFF4, 0);
}

s32 sn_strlen(char *s)
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

void sn_host_call(s32 p1, s32 p2, s32 p3, s32 p4)
{
    s32 v;

    sn_pi_write_word(0xB1FFFFE4, p2);
    sn_pi_write_word(0xB1FFFFE8, p3);
    sn_pi_write_word(0xB1FFFFEC, p4);
    sn_pi_write_word(0xB1FFFFE0, p1);

    do {
        sn_delay(1000);
        v = sn_pi_read_word(0xB1FFFFF0);
        if (v != 0) {
            __asm__ volatile("break 0x400");
        }
        v = sn_pi_read_word(0xB1FFFFE0);
    } while (v != 0);

    sn_pi_read_word(0xB1FFFFDC);
}

s32 sn_pc_open(u8 *arg0, s32 arg1) {
    s32 mask;
    s32 len;
    s32 ret;

    mask = sn_disable_ints();
    len = sn_strlen(arg0);
    sn_pi_write_bytes((void *)0xB1FF0000, arg0, len);
    ret = sn_host_call_3(0x202, len, arg1, 0);
    if (ret & 0x8000) {
        ret |= 0xFFFF0000;
    }
    sn_restore_ints(mask);
    return ret;
}

s32 sn_pc_creat(u8 *arg0, s32 arg1) {
    s32 mask;
    s32 len;
    s32 ret;

    mask = sn_disable_ints();
    len = sn_strlen(arg0);
    sn_pi_write_bytes((void *)0xB1FF0000, arg0, len);
    ret = sn_host_call_3(0x303, len, arg1, 0);
    if (ret & 0x8000) {
        ret |= 0xFFFF0000;
    }
    sn_restore_ints(mask);
    return ret;
}

s32 sn_pc_read(s32 fd, u8 *buf, u32 size)
{
    u32 s;
    u32 len;
    u32 got;
    s32 total;

    s = sn_disable_ints();
    total = 0;
    while (size != 0) {
        len = size;
        if (0x8000 < size) {
            len = 0x8000;
        }
        do {
            got = sn_host_call_4(0x505, fd, len, 0);
        } while (0);
        if (got == 0) {
            sn_restore_ints(s);
            return total;
        }
        sn_pi_read_bytes(0xb1ff0000, buf, got);
        buf += got;
        size -= got;
        total += got;
    }
    sn_restore_ints(s);
    return total;
}

s32 sn_pc_write(s32 fd, u8 *buf, u32 size)
{
    u32 s;
    u32 len;
    s32 got;
    s32 total;

    s = sn_disable_ints();
    total = 0;
    while (size != 0) {
        len = size;
        if (0x8000 < size) {
            len = 0x8000;
        }
        sn_pi_write_bytes((void *)0xB1FF0000, buf, len);
        do {
            got = sn_host_call_5(0x606, fd, len, 0);
        } while (0);
        if (got == 0) {
            sn_restore_ints(s);
            return total;
        }
        buf += got;
        size -= got;
        total += got;
    }
    sn_restore_ints(s);
    return total;
}

u32 sn_pc_lseek(u32 p1, u32 p2, u32 p3)
{
    u32 u;
    u32 r;

    u = sn_disable_ints();
    r = sn_host_call_2(0x707, p1, p2, p3);
    sn_restore_ints_2(u);
    return r;
}
