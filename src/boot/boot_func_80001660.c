#include "ctx.h"

extern u32 D_800180F0;

void func_80006CE0(u32 a, u32 b);
void func_80006BB0(u32 a, u32 b);
void func_80006C60(u32 a, u32 b);

void boot_func_80001660(u32 *from, u32 size, u32 *dst)
{
    u32 *src = from;
    u32 i;
    u32 w, op, t, nx;

    D_800180F0 = (u32)dst - (u32)from;
    for (i = 0; i < size; src++, i += 4, dst++) {
        w = *src;
        op = w & 0xFC000000;
        if (op == 0x08000000 || op == 0x0C000000) {
            t = w & 0x3FFFFFF;
            t <<= 2;
            if (t != 0x400) {
                t += D_800180F0;
            }
            t >>= 2;
            t &= 0x3FFFFFF;
            w = op | t;
            *dst = w;
        } else if ((w & 0xFFE00000) == 0x3C000000) {
            nx = src[1];
            op = nx & 0xFC000000;
            if (op == 0x8C000000 || op == 0xAC000000) {
                t = w << 16;
                t += (s16)src[1];
                t += D_800180F0;
                w = (w & 0xFFFF0000) | (u16)(t >> 16);
                if (t & 0x8000) {
                    w += 0x10000;
                }
                *dst++ = w;
                *dst = (src[1] & 0xFFFF0000) | (t & 0xFFFF);
                src++;
                i++;
            } else if (op == 0x34000000) {
                t = w << 16;
                t |= nx & 0xFFFF;
                t += D_800180F0;
                w = (w & 0xFFFF0000) | (u16)(t >> 16);
                *dst++ = w;
                *dst = (src[1] & 0xFFFF0000) | (t & 0xFFFF);
                src++;
                i++;
            } else {
                *dst = w;
            }
        } else {
            *dst = w;
        }
    }
    func_80006CE0(0, 0x800000);
    func_80006BB0(0, 0x800000);
    func_80006C60(0, 0x800000);
}
