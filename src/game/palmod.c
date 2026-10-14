#include "game.h"

/* Registers a texture set's palette to be rewritten each frame: keeps its colours as 5-bit r, g, b (rotated with
   the Odd Color cheat) and a second copy of the palette for the other frame. */
void palmod_register(TexSet *set, s32 kind)
{
    s32 i;
    s32 k;
    s32 unused;

    if (set != NULL && set->pal != NULL) {
        if (sPalModCount >= sPalModMax) fatal_halt(1);
        sPalModEntries[sPalModCount].kind = kind;
        sPalModEntries[sPalModCount].set = set;
        /* 256 r, g, b triples, then the copy of the palette */
        sPalModEntries[sPalModCount].rgb = mem_alloc_dbg(0x500, 0, sStrPalmodC, 0x8C);
        sPalModEntries[sPalModCount].pal[0] = set->pal;
        sPalModEntries[sPalModCount].pal[1] = (u16 *)(sPalModEntries[sPalModCount].rgb + 0x300);
        __builtin_memcpy(sPalModEntries[sPalModCount].pal[1], sPalModEntries[sPalModCount].pal[0], 0x200);
        k = 0;
        for (i = 0; i < 0x100; i++) {
            if (gCheatFlags & CHEAT_ODD_COLOR) {
                sPalModEntries[sPalModCount].rgb[k++] = (set->pal[i] >> 6) & 0x1F;
                sPalModEntries[sPalModCount].rgb[k++] = (set->pal[i] >> 1) & 0x1F;
                sPalModEntries[sPalModCount].rgb[k++] = set->pal[i] >> 11;
            } else {
                sPalModEntries[sPalModCount].rgb[k++] = set->pal[i] >> 11;
                sPalModEntries[sPalModCount].rgb[k++] = (set->pal[i] >> 6) & 0x1F;
                sPalModEntries[sPalModCount].rgb[k++] = (set->pal[i] >> 1) & 0x1F;
            }
        }
        i = gDesaturation;
        gDesaturation = 0;
        palmod_desaturate_rgb(set->pal, sPalModEntries[sPalModCount].rgb);
        gDesaturation = i;
        __builtin_memcpy(sPalModEntries[sPalModCount].pal[1], set->pal, 0x200);
        sPalModCount++;
    }
}

/* Packs r, g, b into an RGBA5551 colour, keeping the colour's alpha bit. */
static inline void put(u16 *dst, s32 *r, s32 *g, s32 *b) {
    *dst = (*r << 11) | (*g << 6) | (*b << 1) | (*dst & 1);
}

/* palmod_sepia written inline: a grey level (0-255) to a sepia tone. */
static inline void sepia(s32 grey, s32 *r, s32 *g, s32 *b) {
    if (grey > 128) {
        grey -= 128;
        *r = grey * 175 / 128 + 80;
        *g = grey * 185 / 128 + 70;
        *b = grey * 255 / 128;
    } else {
        *r = grey * 80 / 128;
        *g = grey * 70 / 128;
        *b = 0;
    }
}

/* Points each registered set at this frame's palette and rewrites it when the effect changes: lightning on
   Rainforest Run, the podium's desaturation, the credits' sepia, or the sun glare's brightening on outdoor
   tracks. */
void palmod_update(void) {
    s32 i, j, k;
    u16 *dst;
    u8 *src;
    s32 col;
    s32 red, green, blue;
    s32 flashing;

    for (i = 0; i < sPalModCount; i++) {
        sPalModEntries[i].set->pal = sPalModEntries[i].pal[gGfxFrameIndex];
    }
    /* (gCurTrack_6 - gTracks) / 0xA0, the track index, as a multiplication */
    if (((gCurTrack_6 - (s32)gTracks) * -0x33333333 >> 5) == TRACK_RAINFOREST_RUN && gNumScreens == 1 &&
        gPauseMenu == 0) {
        if (gLightningFlash != 0) {
            gLightningFlash--;
        }
        if ((s32)random_byte() % 80 == 0 && sLightningFrame < 0) {
            sLightningFrame = 0;
            sLightningPattern = (s32)random_byte() % 2;
            tunes_play_speech(0x67, -1, 1); /* the thunder, presumably */
        }
        if (sLightningFrame < 0) {
            gLightningFlash = 0;
        } else {
            s32 n = sLightningFrame;
            sLightningFrame = n + 1;
            gLightningFlash = sLightningPatterns[sLightningPattern][n];
            if (sLightningFrame == 10) {
                sLightningFrame = -1;
            }
        }
        if (gPauseMenu != 0) {
            gLightningFlash = 0;
        }
        flashing = gLightningFlash != 0;
        if (flashing != sPalModState[gGfxFrameIndex]) {
            sPalModState[gGfxFrameIndex] = flashing;
            for (i = 0; i < sPalModCount; i++) {
                dst = sPalModEntries[i].pal[gGfxFrameIndex];
                src = sPalModEntries[i].rgb;
                for (k = 0; k < 256; k++) {
                    red = *src++;
                    green = *src++;
                    blue = *src++;
                    if (gLightningFlash != 0) {
                        red = (red + green + blue) / 3;
                        red *= 3;
                        if (red >= 32) {
                            red = 31;
                        }
                        blue = red;
                        green = red;
                    }
                    j = (red << 11) | (green << 6) | (blue << 1);
                    j |= *dst & 1;
                    *dst = j;
                    dst++;
                }
            }
        }
    } else if (gPodiumMode != 0) {
        if (gDesaturation != 0) {
            for (i = 0; i < sPalModCount; i++) {
                s32 r, g, b, a, dr, dg, db;
                u16 *dst = sPalModEntries[i].pal[gGfxFrameIndex];
                u8 *src = sPalModEntries[i].rgb;
                for (col = 0; col < 256; col++) {
                    r = *src++;
                    g = *src++;
                    b = *src++;
                    a = (r + g + b) / 3;
                    r += (a - r) * gDesaturation >> 8;
                    g += (a - g) * gDesaturation >> 8;
                    b += (a - b) * gDesaturation >> 8;
                    a = (r << 11) | (g << 6) | (b << 1);
                    a |= *dst & 1;
                    *dst = a;
                    dst++;
                }
            }
        }
    } else if (((gCurTrack_6 - (s32)gTracks) * -0x33333333 >> 5) != TRACK_EXCITE_3D) {
        if (gCreditsMode != 0) {
            k = -123; /* the state value of the sepia palettes */
            if (sPalModState[gGfxFrameIndex] != k) {
                sPalModState[gGfxFrameIndex] = k;
                for (i = 0; i < sPalModCount; i++) {
                    dst = sPalModEntries[i].pal[gGfxFrameIndex];
                    src = sPalModEntries[i].rgb;
                    for (j = 0; j < 256; j++) {
                        red = *src++;
                        green = *src++;
                        blue = *src++;
                        red = (red + green + blue) / 3;
                        sepia(red, &red, &green, &blue);
                        put(dst, &red, &green, &blue);
                        dst++;
                    }
                }
            }
        } else if (((Cfg *)gCurTrack_6)->f2C == TRACK_ENV_OUTDOOR) {
            k = (gSunGlare - 25) * 3;
            if (k < 0) {
                k = 0;
            }
            if (k > 100) {
                k = 100;
            }
            gPalBrightness = k;
            if (k != sPalModState[gGfxFrameIndex]) {
                sPalModState[gGfxFrameIndex] = k;
                for (i = 0; i < sPalModCount; i++) {
                    dst = sPalModEntries[i].pal[gGfxFrameIndex];
                    src = sPalModEntries[i].rgb;
                    for (j = 0; j < 256; j++) {
                        red = *src++;
                        green = *src++;
                        blue = *src++;
                        red = red + (31 - red) * k / 256;
                        green = green + (31 - green) * k / 256;
                        blue = blue + (31 - blue) * k / 256;
                        put(dst, &red, &green, &blue);
                        dst++;
                    }
                }
            }
        }
    }
}

/* Resets the palette effects; the podium ceremony registers up to 16 sets, a race 8. */
void palmod_init(s32 podium)
{
    gPalBrightness = 0;
    sPalModCount = 0;
    sPalModState[0] = -1;
    sPalModState[1] = -1;
    sLightningFrame = -1;
    if (podium != 0) {
        sPalModMax = 0x10;
    } else {
        sPalModMax = 8;
    }
    sPalModEntries = mem_alloc_dbg(sPalModMax * sizeof(PalMod), 0, sStrPalmodC, 0x74);
}

/* Frees the registered sets' colour copies. */
void palmod_free(void)
{
    s32 i;

    for (i = 0; i < sPalModCount; i++) {
        mem_free(sPalModEntries[i].rgb);
    }
    D_800F14D4 = 0;
    D_800F14E4 = 0;
    sPalModCount = 0;
    mem_free(sPalModEntries);
}

/* A grey level (0-255) to the credits' sepia: dark browns up to 128, then towards white. */
void palmod_sepia(s32 grey, s32 *r, s32 *g, s32 *b)
{
    s32 t;
    s32 above;

    if (grey > 0x80) {
        above = grey - 0x80;
        t = above * 0xAF;
        if (t < 0) {
            t = t + 0x7F;
        }
        *r = (t >> 7) + 0x50;
        t = above * 0xB9;
        if (t < 0) {
            t = t + 0x7F;
        }
        *g = (t >> 7) + 0x46;
        t = above * 0xFF;
        if (t < 0) {
            t = t + 0x7F;
        }
        *b = t >> 7;
        return;
    }
    t = grey * 0x50;
    if (t < 0) {
        t = t + 0x7F;
    }
    *r = t >> 7;
    t = grey * 0x46;
    if (t < 0) {
        t = t + 0x7F;
    }
    *g = t >> 7;
    *b = 0;
}

/* Writes 256 r, g, b triples (5 bits each) into a palette, moved towards their grey by gDesaturation / 256. */
void palmod_desaturate_rgb(u16 *dst, u8 *src)
{
    s32 i;
    s32 r, g, b, grey;

    for (i = 0; i < 0x100; i++) {
        r = *src++;
        g = *src++;
        b = *src++;
        grey = (r + g + b) / 3;
        r += ((grey - r) * gDesaturation) >> 8;
        g += ((grey - g) * gDesaturation) >> 8;
        b += ((grey - b) * gDesaturation) >> 8;
        grey = (r << 11) | (g << 6) | (b << 1);
        grey |= *dst & 1;
        *dst = grey;
        dst++;
    }
}

/* Writes a 256-colour palette into another, moved towards grey by gDesaturation / 256. */
void palmod_desaturate_pal(u16 *dst, u16 *src)
{
    s32 i;
    u32 r;
    s32 g, b, grey;

    for (i = 0; i < 0x100; i++) {
        r = *src;
        g = (r >> 3) & 0xF8;
        b = (r << 2) & 0xF8;
        r = (r >> 8) & 8; /* an original bug: keeps one bit of red (0xF8 was meant) */
        grey = (s32)(r + g + b) / 3;
        r += ((grey - (s32)r) * gDesaturation) >> 8;
        r = (s32)r >> 3;
        g += ((grey - g) * gDesaturation) >> 8;
        g >>= 3;
        b += ((grey - b) * gDesaturation) >> 8;
        b >>= 3;
        grey = (r << 11) | (g << 6) | (b << 1);
        grey |= *dst & 1;
        *dst = grey;
        dst++;
        src++;
    }
}
