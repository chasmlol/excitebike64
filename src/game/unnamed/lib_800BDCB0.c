#include "game.h"

s32 swd_decode_token(s32 *p1, u32 *p2)
{
    u32 r;
    u32 t;
    u8 *q;
    u8 c;


    r = swd_read_bits(1);
    if (r == 0) {
        q = gSwdSrc;
        c = *q;
        *p2 = c;
        gSwdSrc = q + 1;
        *p1 = 1;
        return 1;
    }
    r = swd_read_bits(1);
    if (r == 0) {
        *p1 = 2;
    } else if ((r = swd_read_bits(2)) != 0) {
        *p1 = r + 2;
    } else if ((r = swd_read_bits(4)) != 0) {
        *p1 = r + 5;
    } else {
        r = *gSwdSrc;
        gSwdSrc = gSwdSrc + 1;
        if (r == 0) {
            *p2 = 0;
            *p1 = 0;
            return 0;
        }
        *p1 = r + 0x14;
    }
    r = swd_read_bits(2);
    switch (r) {
    case 0:
        r = swd_read_bits(5);
        *p2 = r + 1;
        break;
    case 1:
        r = swd_read_bits(7);
        *p2 = r + 0x21;
        break;
    case 2:
        r = *gSwdSrc;
        gSwdSrc = gSwdSrc + 1;
        t = swd_read_bits(1);
        r = r * 2 + t + 0xa1;
        *p2 = r;
        break;
    case 3:
        r = *gSwdSrc;
        gSwdSrc = gSwdSrc + 1;
        t = swd_read_bits(2);
        r = r * 4 + t + 0x2a1;
        *p2 = r;
        break;
    default:
        break;
    }
    return *p1;
}

s32 swd_decompress(void)
{
    s32 cnt;
    s32 val;
    u8 *src;
    u8 *dst;
    u8 *p;

    swd_reset_bits();
    for (;;) {
        if (swd_decode_token(&cnt, &val) < 0) {
            return -1;
        }
        if (cnt == 0) {
            break;
        }
        if (cnt == 1) {
            p = gSwdDst;
            *p = (u8)val;
            gSwdDst = p + 1;
        } else {
            src = gSwdDst - val;
            for (cnt = cnt - 1; cnt != -1; cnt = cnt - 1) {
                dst = gSwdDst;
                *dst = *src;
                src = src + 1;
                gSwdDst = dst + 1;
            }
        }
    }
    return 0;
}

void swd_reset_bits(void)
{
    sSwdBitBuf = 1;
}

u32 swd_read_bits(s32 param_1)
{
    u32 r = 0;
    s32 n = param_1;
    n -= 1;

    if (param_1 != 0) {
        do {
            n = n - 1;
            if (sSwdBitBuf_2 == 1) {
                u32 b = *gSwdSrc;
                gSwdSrc = gSwdSrc + 1;
                sSwdBitBuf_2 = b + 0x100;
            }
            r = (r << 1) | (sSwdBitBuf_2 & 1);
            sSwdBitBuf_2 = sSwdBitBuf_2 >> 1;
        } while (n != -1);
    }
    return r;
}
