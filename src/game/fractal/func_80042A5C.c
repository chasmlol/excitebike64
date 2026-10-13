#include "ctx.h"

typedef struct Field {
    s32 x;
    s32 z;
    s32 f8;
    s32 seed;           /* 0x0C */
    s32 sp;             /* 0x10 */
    u32 stack[64];      /* 0x14 */
    u8 rowE[129];       /* 0x114 */
    u8 colE[129];       /* 0x195 */
    u8 buf[2][128];     /* 0x216 */
    s32 toggle;         /* 0x318 */
    u8 *map;            /* 0x31C */
} Field;

extern s32 D_800EE128;
extern s32 D_800CA414[];

void func_80043B60(Field *f);

#define RND() (D_800EE128 = (D_800EE128 * 0x35DF + 0x3038) % 65531)

void func_80042A5C(Field *f, s32 n)
{
    u32 e;
    s32 row, col, i, v, t;
    u8 *prev, *cur, *p, *next;
    s32 lvl, x, y, size, x2, y2, xm, ym, amp, idx;
    s32 c00, c01, c10, c11;
    s32 cnt;
    u32 u;

    cnt = n;
    D_800EE128 = f->seed;
    while (cnt != 0) {
        if (f->sp == 0) {
            return;
        }
        f->sp--;
        e = f->stack[f->sp];
        if ((e >> 24) == 2) {
            func_80043B60(f);
            continue;
        }
        if ((e >> 24) == 1) {
            e &= 0xFFFFFF;
            row = e >> 7;
            col = e & 0x7F;
            if (row == 0 && col == 0) {
                __builtin_memcpy(f->buf[0], f->map, 128);
                f->toggle = 1;
            }
            t = f->toggle;
            prev = f->buf[1 - t];
            cur = f->buf[t];
            p = f->map + (row << 9);
            if (row == 127) {
                next = f->rowE;
            } else {
                next = p + 0x200;
            }
            prev += col;
            cur += col;
            p += col;
            next += col;
            if (col == 0) {
                __builtin_memcpy(cur, p, 128);
                v = prev[0] * 3;
                v += prev[1];
                v += cur[0] * 6;
                v += cur[1] * 2;
                v += next[0] * 3;
                v += next[1];
                v >>= 4;
                v = (v * v) >> 8;
                if (v < 4) {
                    v = 4;
                }
                v ^= 0x80;
                *p = v;
                p++;
                prev++;
                cur++;
                next++;
                e++;
            }
            for (i = col; i < col + 64; i++) {
                if (i == 127) {
                    break;
                }
                v = prev[-1];
                v += prev[0] * 2;
                v += prev[1];
                v += cur[-1] * 2;
                v += cur[0] * 4;
                v += cur[1] * 2;
                v += next[-1];
                v += next[0] * 2;
                v += next[1];
                v >>= 4;
                v = (v * v) >> 8;
                if (v < 4) {
                    v = 4;
                }
                v ^= 0x80;
                *p = v;
                p++;
                prev++;
                cur++;
                next++;
                e++;
            }
            if (i == 127) {
                v = prev[-1];
                v += prev[0] * 2;
                if (row != 0) {
                    v += f->colE[row - 1];
                } else {
                    v += f->colE[row];
                }
                v += cur[-1] * 2;
                v += cur[0] * 4;
                v += f->colE[row] * 2;
                v += next[-1];
                v += next[0] * 2;
                v += f->colE[row + 1];
                v >>= 4;
                v = (v * v) >> 8;
                if (v < 4) {
                    v = 4;
                }
                v ^= 0x80;
                *p = v;
                e++;
                f->toggle ^= 1;
            }
            if (e < 0x4000) {
                f->stack[f->sp++] = e | 0x1000000;
            }
            cnt--;
        } else {
            row = (e >> 8) & 0xFF;
            u = e >> 16;
            lvl = e & 0xFF;
            size = 1 << lvl;
            x = u & 0xFF;
            y = row << 9;
            x2 = x + size;
            y2 = y + (size << 9);
            xm = x + (size >> 1);
            ym = y + (size << 8);
            c00 = f->map[y | x];
            if (x2 == 128) {
                if (y2 == 0x10000) {
                    c01 = f->colE[y / 512];
                    c10 = f->rowE[x];
                    c11 = f->colE[128];
                } else {
                    c01 = f->colE[y / 512];
                    c10 = f->map[y2 | x];
                    c11 = f->colE[y2 / 512];
                }
            } else {
                if (y2 == 0x10000) {
                    c01 = f->map[y | x2];
                    c10 = f->rowE[x];
                    c11 = f->rowE[x2];
                } else {
                    c01 = f->map[y | x2];
                    c10 = f->map[y2 | x];
                    c11 = f->map[y2 | x2];
                }
            }
            x2 %= 128;
            y2 %= 0x10000;
            amp = D_800CA414[lvl];
            if (f->map[y | xm] == 0) {
                RND();
                v = ((c00 + c01) >> 1) + ((D_800EE128 & 0xFF) - 0x80) * amp / 32;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                f->map[y | xm] = v;
            }
            if (f->map[ym | x] == 0) {
                RND();
                v = ((c00 + c10) >> 1) + ((D_800EE128 & 0xFF) - 0x80) * amp / 32;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                f->map[ym | x] = v;
            }
            if (f->map[ym | x2] == 0) {
                RND();
                v = ((c01 + c11) >> 1) + ((D_800EE128 & 0xFF) - 0x80) * amp / 32;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                f->map[ym | x2] = v;
            }
            if (f->map[y2 | xm] == 0) {
                RND();
                v = ((c10 + c11) >> 1) + ((D_800EE128 & 0xFF) - 0x80) * amp / 32;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                f->map[y2 | xm] = v;
            }
            if (f->map[ym | xm] == 0) {
                RND();
                v = ((c00 + c01) >> 1) + ((D_800EE128 & 0xFF) - 0x80) * amp / 32;
                RND();
                v += ((c00 + c10) >> 1) + ((D_800EE128 & 0xFF) - 0x80) * amp / 32;
                RND();
                v += ((c01 + c11) >> 1) + ((D_800EE128 & 0xFF) - 0x80) * amp / 32;
                RND();
                v += ((c10 + c11) >> 1) + ((D_800EE128 & 0xFF) - 0x80) * amp / 32;
                v >>= 2;
                if (v <= 0) v = 1;
                if (v >= 0x100) v = 0xFF;
                f->map[ym | xm] = v;
            }
            if (size != 2) {
                f->stack[f->sp++] = (xm << 16) | ((ym / 512) << 8) | (lvl - 1);
                f->stack[f->sp++] = (x << 16) | ((ym / 512) << 8) | (lvl - 1);
                f->stack[f->sp++] = (xm << 16) | ((y / 512) << 8) | (lvl - 1);
                f->stack[f->sp++] = (x << 16) | ((y / 512) << 8) | (lvl - 1);
            }
            cnt--;
        }

    }
    f->seed = D_800EE128;
}
