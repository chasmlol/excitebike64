#include "boot.h"

/* Copies a 320x240 RGBA5551 image from `src` to `dst` with each colour scaled by `level` / 256 (alpha dropped).
   Never called. */
void boot_fade_image(u16 *dst, u16 *src, s32 level)
{
    s32 i = 0;
    s32 r, g, b;
    do {
        r = *src; /* the whole pixel until its red is taken */
        g = (r >> 6) & 0x1F;
        b = (r >> 1) & 0x1F;
        r = (u32)r >> 11;
        src++;
        i++;
        r = (r * level) >> 8;
        g = (g * level) >> 8;
        b = (b * level) >> 8;
        *dst++ = (u16)((r << 11) | (g << 6) | (b << 1));
    } while (i < BOOT_SCREEN_PIXELS);
}
