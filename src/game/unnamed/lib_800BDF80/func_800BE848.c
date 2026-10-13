#include "ctx.h"

typedef struct { u8 pad[0x1C]; u32 size; } FileHdr;
typedef struct { FileHdr *hdr; s32 f4; s32 f8; u32 pos; } Stream;
typedef struct { s32 f0; s32 f4; u8 *data; } Block;

Block *func_800BEA40(Stream *f, u32 blk, s32 partial);
void func_800AEC34(u8 *dst, u8 *src, s32 n);

s32 func_800BE848(u8 *buf, s32 size, s32 count, Stream *f)
{
    s32 n = count * size;
    u32 blk;
    u32 off;
    s32 chunk;
    Block *b;
    u8 *dst;

    if (f->hdr->size < f->pos + n) {
        do { do { do {
            n = f->hdr->size - f->pos;
        } while (0); } while (0); } while (0);
        if (size > 1) {
            do {
                count = n / size;
                n = count * size;
            } while (0);
        }
    }
    dst = buf;
    blk = f->pos >> 13;
    off = f->pos & 0x1FFF;
    while (n != 0) {
        chunk = 0x2000 - off;
        if (n < chunk) {
            do { do { do {
                chunk = n;
            } while (0); } while (0); } while (0);
        }
        b = func_800BEA40(f, blk, chunk != 0x2000);
        if (b == NULL) {
            return 0;
        }
        func_800AEC34(dst, b->data + off, chunk);
        n -= chunk;
        dst += chunk;
        do { do {
            f->pos += chunk;
            off += chunk;
        } while (0); } while (0);
        if (off == 0x2000) {
            off = 0;
            blk++;
        }
    }
    return count;
}
