#include "ctx.h"

typedef struct {
    u8 pad[0x13C];
    u16 *p13C;
} S70;
extern S70 D_800EFD70;

void func_80073664(s32 alpha, u16 *src)
{
    s32 i;
    s32 r, g, b;
    u32 p;

    D_800EFD70.p13C[0] = 0;
    for (i = 1; i < 0x100; i++) {
        if (alpha == 0xFF) {
            b = i;
            g = i;
            r = i;
        } else {
            p = src[i];
            b = (p >> 11) * 8;
            g = (p >> 3) & 0xF8;
            r = (p << 2) & 0xF8;
            b = b + ((i - b) * alpha) / 255;
            g = g + ((i - g) * alpha) / 255;
            r = r + ((i - r) * alpha) / 255;
        }
        D_800EFD70.p13C[i] = ((r >> 3) << 1) | ((g >> 3) << 6) | ((b >> 3) << 11) | 1;
    }
}
