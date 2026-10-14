#include "boot.h"

void boot_fade_image(u16 *dst, u16 *src, s32 k) {
    s32 i = 0;
    s32 p, g, b;
    do {
        p = *src;
        g = (p >> 6) & 0x1F;
        b = (p >> 1) & 0x1F;
        p = (u32)p >> 11;
        src++;
        i++;
        p = (p * k) >> 8;
        g = (g * k) >> 8;
        b = (b * k) >> 8;
        *dst++ = (u16)((p << 11) | (g << 6) | (b << 1));
    } while (i < 0x12C00);
}
