#include "game.h"

typedef struct Rec_40 {
    u8 *buf;
    s32 val;
    Obj_51 *obj;
    u16 *src;
    u8 *dst;
} Rec_40;

extern Rec_40 *sPalModEntries;

void palmod_register(Obj_51 *a, s32 b)
{
    s32 i;
    s32 k;
    s32 save;

    if (a != NULL && a->src != NULL) {
        if (sPalModCount >= sPalModMax) fatal_halt(1);
        sPalModEntries[sPalModCount].val = b;
        sPalModEntries[sPalModCount].obj = a;
        sPalModEntries[sPalModCount].buf = mem_alloc_dbg(0x500, 0, D_800D8E10, 0x8C);
        sPalModEntries[sPalModCount].src = a->src;
        sPalModEntries[sPalModCount].dst = sPalModEntries[sPalModCount].buf + 0x300;
        __builtin_memcpy(sPalModEntries[sPalModCount].dst, sPalModEntries[sPalModCount].src, 0x200);
        k = 0;
        for (i = 0; i < 0x100; i++) {
            if (gCheatFlags & 0x80) {
                sPalModEntries[sPalModCount].buf[k++] = (a->src[i] >> 6) & 0x1F;
                sPalModEntries[sPalModCount].buf[k++] = (a->src[i] >> 1) & 0x1F;
                sPalModEntries[sPalModCount].buf[k++] = a->src[i] >> 11;
            } else {
                sPalModEntries[sPalModCount].buf[k++] = a->src[i] >> 11;
                sPalModEntries[sPalModCount].buf[k++] = (a->src[i] >> 6) & 0x1F;
                sPalModEntries[sPalModCount].buf[k++] = (a->src[i] >> 1) & 0x1F;
            }
        }
        i = gDesaturation;
        gDesaturation = 0;
        palmod_desaturate_rgb(a->src, sPalModEntries[sPalModCount].buf);
        gDesaturation = i;
        __builtin_memcpy(sPalModEntries[sPalModCount].dst, a->src, 0x200);
        sPalModCount++;
    }
}

typedef struct {
    u8 *src;
    s32 f4;
    Trk_2 *f8;
    u16 *dst[2];
} PalEnt_2;

extern PalEnt_2 *sPalModEntries_2;

static inline void put(u16 *d, s32 *r, s32 *g, s32 *b) {
    *d = (*r << 11) | (*g << 6) | (*b << 1) | (*d & 1);
}

static inline void sepia(s32 v, s32 *r, s32 *g, s32 *b) {
    if (v > 128) {
        v -= 128;
        *r = v * 175 / 128 + 80;
        *g = v * 185 / 128 + 70;
        *b = v * 255 / 128;
    } else {
        *r = v * 80 / 128;
        *g = v * 70 / 128;
        *b = 0;
    }
}

void palmod_update(void) {
    s32 i, j, k;
    u16 *dst;
    u8 *src;
    s32 j1;
    s32 r0, g0, b0;
    s32 v;

    for (i = 0; i < sPalModCount; i++) {
        sPalModEntries_2[i].f8->f4 = (s32)sPalModEntries_2[i].dst[gGfxFrameIndex];
    }
    if (((gCurTrack_6 - (s32)gTracks) * -0x33333333 >> 5) == 11 && gNumScreens == 1 && gPauseMenu == 0) {
        if (gLightningFlash != 0) {
            gLightningFlash--;
        }
        if ((s32)random_byte() % 80 == 0 && sLightningFrame < 0) {
            sLightningFrame = 0;
            sLightningPattern = (s32)random_byte() % 2;
            tunes_play_speech(0x67, -1, 1);
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
        v = gLightningFlash != 0;
        if (v != sPalModState[gGfxFrameIndex]) {
            sPalModState[gGfxFrameIndex] = v;
            for (i = 0; i < sPalModCount; i++) {
                dst = sPalModEntries_2[i].dst[gGfxFrameIndex];
                src = sPalModEntries_2[i].src;
                for (k = 0; k < 256; k++) {
                    r0 = *src++;
                    g0 = *src++;
                    b0 = *src++;
                    if (gLightningFlash != 0) {
                        r0 = (r0 + g0 + b0) / 3;
                        r0 *= 3;
                        if (r0 >= 32) {
                            r0 = 31;
                        }
                        b0 = r0;
                        g0 = r0;
                    }
                    j = (r0 << 11) | (g0 << 6) | (b0 << 1);
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
                u16 *dst = sPalModEntries_2[i].dst[gGfxFrameIndex];
                u8 *src = sPalModEntries_2[i].src;
                for (j1 = 0; j1 < 256; j1++) {
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
    } else if (((gCurTrack_6 - (s32)gTracks) * -0x33333333 >> 5) != 20) {
        if (gCreditsMode != 0) {
            k = -123;
            if (sPalModState[gGfxFrameIndex] != k) {
                sPalModState[gGfxFrameIndex] = k;
                for (i = 0; i < sPalModCount; i++) {
                    dst = sPalModEntries_2[i].dst[gGfxFrameIndex];
                    src = sPalModEntries_2[i].src;
                    for (j = 0; j < 256; j++) {
                        r0 = *src++;
                        g0 = *src++;
                        b0 = *src++;
                        r0 = (r0 + g0 + b0) / 3;
                        sepia(r0, &r0, &g0, &b0);
                        put(dst, &r0, &g0, &b0);
                        dst++;
                    }
                }
            }
        } else if (((Cfg *)gCurTrack_6)->f2C == 1) {
            k = (gSunGlare - 25) * 3;
            if (k < 0) {
                k = 0;
            }
            if (k > 100) {
                k = 100;
            }
            D_800CCD60 = k;
            if (k != sPalModState[gGfxFrameIndex]) {
                sPalModState[gGfxFrameIndex] = k;
                for (i = 0; i < sPalModCount; i++) {
                    dst = sPalModEntries_2[i].dst[gGfxFrameIndex];
                    src = sPalModEntries_2[i].src;
                    for (j = 0; j < 256; j++) {
                        r0 = *src++;
                        g0 = *src++;
                        b0 = *src++;
                        r0 = r0 + (31 - r0) * k / 256;
                        g0 = g0 + (31 - g0) * k / 256;
                        b0 = b0 + (31 - b0) * k / 256;
                        put(dst, &r0, &g0, &b0);
                        dst++;
                    }
                }
            }
        }
    }
}

void palmod_init(s32 p)
{
    D_800CCD60 = 0;
    sPalModCount = 0;
    sPalModState[0] = -1;
    sPalModState[1] = -1;
    sLightningFrame = -1;
    if (p != 0) {
        sPalModMax = 0x10;
    } else {
        sPalModMax = 8;
    }
    sPalModEntries = (s32)mem_alloc_dbg(sPalModMax * 0x14, 0, D_800D8E10, 0x74);
}

typedef struct {
    s32 w0, w1, w2, w3, w4;
} Ent_70;

extern Ent_70 *sPalModEntries_3;

void palmod_free(void)
{
    s32 i;

    for (i = 0; i < sPalModCount; i++) {
        mem_free(sPalModEntries_3[i].w0);
    }
    D_800F14D4 = 0;
    D_800F14E4 = 0;
    sPalModCount = 0;
    mem_free((s32)sPalModEntries_3);
}

void palmod_sepia(s32 a0, s32 *a1, s32 *a2, s32 *a3)
{
    s32 t;
    s32 d;

    if (a0 > 0x80) {
        d = a0 - 0x80;
        t = d * 0xAF;
        if (t < 0) {
            t = t + 0x7F;
        }
        *a1 = (t >> 7) + 0x50;
        t = d * 0xB9;
        if (t < 0) {
            t = t + 0x7F;
        }
        *a2 = (t >> 7) + 0x46;
        t = d * 0xFF;
        if (t < 0) {
            t = t + 0x7F;
        }
        *a3 = t >> 7;
        return;
    }
    t = a0 * 0x50;
    if (t < 0) {
        t = t + 0x7F;
    }
    *a1 = t >> 7;
    t = a0 * 0x46;
    if (t < 0) {
        t = t + 0x7F;
    }
    *a2 = t >> 7;
    *a3 = 0;
}

void palmod_desaturate_rgb(u16 *dst, u8 *src)
{
    s32 i;
    s32 r, g, b, c;

    for (i = 0; i < 0x100; i++) {
        r = *src++;
        g = *src++;
        b = *src++;
        c = (r + g + b) / 3;
        r += ((c - r) * gDesaturation) >> 8;
        g += ((c - g) * gDesaturation) >> 8;
        b += ((c - b) * gDesaturation) >> 8;
        c = (r << 11) | (g << 6) | (b << 1);
        c |= *dst & 1;
        *dst = c;
        dst++;
    }
}

void palmod_desaturate_pal(u16 *dst, u16 *src)
{
    s32 i;
    u32 v;
    s32 g, b, avg;

    for (i = 0; i < 0x100; i++) {
        v = *src;
        g = (v >> 3) & 0xF8;
        b = (v << 2) & 0xF8;
        v = (v >> 8) & 8;
        avg = (s32)(v + g + b) / 3;
        v += ((avg - (s32)v) * gDesaturation) >> 8;
        v = (s32)v >> 3;
        g += ((avg - g) * gDesaturation) >> 8;
        g >>= 3;
        b += ((avg - b) * gDesaturation) >> 8;
        b >>= 3;
        avg = (v << 11) | (g << 6) | (b << 1);
        avg |= *dst & 1;
        *dst = avg;
        dst++;
        src++;
    }
}
