#include "ctx.h"

typedef struct { u8 w, h, pad, pal; u8 *addr; } Img;

extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;

Gfx *func_8000A690(s32 x, s32 y, Img *img, Gfx *gdl, s32 a4, s32 sx, s32 sy, s32 flags, s32 dw) {
    s32 step, row, n, sc, srow, sn, clip, wdt;
    s32 m;
    u8 *addr;

    m = 2;
    if (flags & 2) m = 1;
    step = ((flags & 1) ? m << 11 : m << 12) / (img->w * 8);
    if (img->h < step) step = img->h;

    switch (sx) {
    case 2:
        x = x * D_800F13EC / 512 + D_800F13EC / 2;
        break;
    case 3:
        x = x * D_800F13EC / 512 + D_800F13EC;
        break;
    case 1:
        x = x * D_800F13EC / 512;
        break;
    }
    switch (sy) {
    case 2:
        y = y * D_800F13F0 / 240 + D_800F13F0 / 2;
        break;
    case 3:
        y = y * D_800F13F0 / 240 + D_800F13F0;
        break;
    case 1:
        y = y * D_800F13F0 / 240;
        break;
    }
    if (x >= 0) {
        clip = 0;
    } else {
        if (-x < img->w * 8) {
            clip = -x;
            x = 0;
        } else {
            return gdl;
        }
    }
    for (row = 0; row < img->h; row += step) {
        n = step;
        if (img->h - row < step) n = img->h - row;
        if (flags & 0x100) {
            sc = 2;
            srow = row / 2;
            sn = n / 2;
        } else {
            sc = 1;
            srow = row;
            sn = n;
        }
        gDPPipeSync(gdl++);
        if (flags & 2) {
            if (flags & 0x20) addr = img->addr + row * img->w * 8;
            else addr = img->addr + (img->h - row - n) * img->w * 8;
        } else {
            addr = img->addr + (img->h - row - n) * img->w * 4;
        }
        if (flags & 4) {
            if (flags & 2) {
                gDPLoadTextureTile(gdl++, addr, G_IM_FMT_IA, G_IM_SIZ_8b, img->w * 8, 0, clip, 0, img->w * 8 - 1, n - 1,
                                   0, 0, 0, 0, 0, 0, 0);
            } else {
                while (1);
            }
        } else if (flags & 2) {
            gDPLoadTextureTile(gdl++, addr, G_IM_FMT_CI, G_IM_SIZ_8b, img->w * 8, 0, clip, 0, img->w * 8 - 1, n - 1,
                               0, 0, 0, 0, 0, 0, 0);
        } else {
            gDPLoadTextureTile_4b(gdl++, addr, G_IM_FMT_CI, img->w * 8 - clip, 0, clip, 0, img->w * 8 - 1, n - 1,
                                  img->pal & 0xF, 0, 0, 0, 0, 0, 0);
        }
        gDPPipeSync(gdl++);
        if (flags & 0x20) {
            gSPTextureRectangle(gdl++, x * 4, (y + srow) * 4, (x + img->w * 8 - clip) * 4, (y + srow + sn) * 4,
                                0, clip << 5, 0, sc << 10, sc << 10);
        } else if (flags & 0x40) {
            gSPTextureRectangle(gdl++, x * 4, (y + srow) * 4, (x + img->w * 8) * 4, (y + srow + sn) * 4,
                                0, (img->w << 8) - 0x20, (n - 1) << 5, -(sc << 10), -(sc << 10));
        } else {
            if (flags & 0x80) wdt = dw;
            else wdt = img->w * 8;
            if (D_800F13EC == 640) {
                gSPTextureRectangle(gdl++, x * 4, (y + srow * 2) * 4, x * 4 + (wdt - clip) * 5, (y + srow * 2 + sn * 2) * 4,
                                    0, clip << 5, (n - 1) << 5, sc * 0x333, -(sc << 9));
            } else {
                gSPTextureRectangle(gdl++, x * 4, (y + srow) * 4, (x + wdt / sc - clip) * 4, (y + srow + sn) * 4,
                                    0, clip << 5, (n - 1) << 5, sc << 10, -(sc << 10));
            }
        }
        gDPPipeSync(gdl++);
    }
    return gdl;
}
