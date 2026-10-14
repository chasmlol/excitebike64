#include "game.h"

/* SWD decompression (John Brandwood's SWD format, the same as the boot loader's SwdExpand) from gSwdSrc to gSwdDst:
   a stream of tokens whose flags and short numbers come from a bit buffer refilled one byte at a time from the
   source, between the bytes the tokens take whole. */

/* Decodes one token. A literal sets *len to 1 and *value to its byte. A match sets *len to its length (2-275) and
   *value to how far back it copies from (1-0x6A0); the end of the data sets both to 0. Returns *len. */
s32 swd_decode_token(s32 *len, u32 *value)
{
    u32 bits;
    u32 low;
    u8 *src;
    u8 byte;


    bits = swd_read_bits(1);
    if (bits == 0) {
        src = gSwdSrc;
        byte = *src;
        *value = byte;
        gSwdSrc = src + 1;
        *len = 1;
        return 1;
    }
    /* The length: 0 for 2; then 2 bits for 3-5; then 4 bits for 6-20; then a byte, 0 for the end, else 21-275. */
    bits = swd_read_bits(1);
    if (bits == 0) {
        *len = 2;
    } else if ((bits = swd_read_bits(2)) != 0) {
        *len = bits + 2;
    } else if ((bits = swd_read_bits(4)) != 0) {
        *len = bits + 5;
    } else {
        bits = *gSwdSrc;
        gSwdSrc = gSwdSrc + 1;
        if (bits == 0) {
            *value = 0;
            *len = 0;
            return 0;
        }
        *len = bits + 0x14;
    }
    /* The distance: 2 bits select 5 bits (1-0x20), 7 bits (0x21-0xA0), a byte and 1 bit (0xA1-0x2A0) or a byte and
       2 bits (0x2A1-0x6A0). */
    bits = swd_read_bits(2);
    switch (bits) {
    case 0:
        bits = swd_read_bits(5);
        *value = bits + 1;
        break;
    case 1:
        bits = swd_read_bits(7);
        *value = bits + 0x21;
        break;
    case 2:
        bits = *gSwdSrc;
        gSwdSrc = gSwdSrc + 1;
        low = swd_read_bits(1);
        bits = bits * 2 + low + 0xa1;
        *value = bits;
        break;
    case 3:
        bits = *gSwdSrc;
        gSwdSrc = gSwdSrc + 1;
        low = swd_read_bits(2);
        bits = bits * 4 + low + 0x2a1;
        *value = bits;
        break;
    default:
        break;
    }
    return *len;
}

/* Unpacks gSwdSrc to gSwdDst until the end token. Returns 0 (-1 for a negative token length, which cannot
   happen). */
s32 swd_decompress(void)
{
    s32 len;
    s32 value;
    u8 *src;
    u8 *dst;
    u8 *out;

    swd_reset_bits();
    for (;;) {
        if (swd_decode_token(&len, &value) < 0) {
            return -1;
        }
        if (len == 0) {
            break;
        }
        if (len == 1) {
            out = gSwdDst;
            *out = (u8)value;
            gSwdDst = out + 1;
        } else {
            /* A byte at a time: the copy may overlap what it writes. */
            src = gSwdDst - value;
            for (len = len - 1; len != -1; len = len - 1) {
                dst = gSwdDst;
                *dst = *src;
                src = src + 1;
                gSwdDst = dst + 1;
            }
        }
    }
    return 0;
}

/* Empties the bit buffer: 1 is the marker bit alone. */
void swd_reset_bits(void)
{
    sSwdBitBuf = 1;
}

/* Reads `count` bits, the first one read the highest. The buffer holds the bits of one source byte, lowest first,
   above them a marker bit (0x100); when only the marker is left the next byte is loaded. */
u32 swd_read_bits(s32 count)
{
    u32 bits = 0;
    s32 left = count;
    left -= 1;

    if (count != 0) {
        do {
            left = left - 1;
            if (sSwdBitBuf_2 == 1) {
                u32 byte = *gSwdSrc;
                gSwdSrc = gSwdSrc + 1;
                sSwdBitBuf_2 = byte + 0x100;
            }
            bits = (bits << 1) | (sSwdBitBuf_2 & 1);
            sSwdBitBuf_2 = sSwdBitBuf_2 >> 1;
        } while (left != -1);
    }
    return bits;
}
