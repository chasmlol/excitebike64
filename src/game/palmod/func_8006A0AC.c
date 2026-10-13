#include "ctx.h"

typedef struct {
    u8 pad[0x2C];
    s32 f2C;
} Track;

typedef struct {
    u8 pad[4];
    s32 f4;
} Obj8;

typedef struct {
    u8 *src;
    s32 f4;
    Obj8 *f8;
    u16 *dst[2];
} PalEnt;

extern s32 D_800EFD64;
extern u8 D_800CB210[];
extern s32 D_800F14C0;
extern PalEnt *D_800F14D0;
extern s32 D_800E30A8;
extern s32 D_800E1898;
extern s32 D_800E3710;
extern s32 D_800F14E8;
extern s32 D_800F14E0;
extern s32 D_800F14DC;
extern s32 D_800CCD68[][10];
extern s32 D_800F14C8[];
extern s32 D_800C2358;
extern s32 D_800E1B70;
extern s32 D_800CCD64;
extern s32 D_800CCD60;
extern s32 D_800E8E9C;

u32 func_8000EFE8(void);

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
int func_8004F270(s32 param_1, s32 param_2, s32 param_3);

void func_8006A0AC(void) {
    s32 i, j, k;
    u16 *dst;
    u8 *src;
    s32 j1;
    s32 r0, g0, b0;
    s32 v;

    for (i = 0; i < D_800F14C0; i++) {
        D_800F14D0[i].f8->f4 = (s32)D_800F14D0[i].dst[D_800E30A8];
    }
    if (((D_800EFD64 - (s32)D_800CB210) * -0x33333333 >> 5) == 11 && D_800E1898 == 1 && D_800E3710 == 0) {
        if (D_800F14E8 != 0) {
            D_800F14E8--;
        }
        if ((s32)func_8000EFE8() % 80 == 0 && D_800F14E0 < 0) {
            D_800F14E0 = 0;
            D_800F14DC = (s32)func_8000EFE8() % 2;
            func_8004F270(0x67, -1, 1);
        }
        if (D_800F14E0 < 0) {
            D_800F14E8 = 0;
        } else {
            s32 n = D_800F14E0;
            D_800F14E0 = n + 1;
            D_800F14E8 = D_800CCD68[D_800F14DC][n];
            if (D_800F14E0 == 10) {
                D_800F14E0 = -1;
            }
        }
        if (D_800E3710 != 0) {
            D_800F14E8 = 0;
        }
        v = D_800F14E8 != 0;
        if (v != D_800F14C8[D_800E30A8]) {
            D_800F14C8[D_800E30A8] = v;
            for (i = 0; i < D_800F14C0; i++) {
                dst = D_800F14D0[i].dst[D_800E30A8];
                src = D_800F14D0[i].src;
                for (k = 0; k < 256; k++) {
                    r0 = *src++;
                    g0 = *src++;
                    b0 = *src++;
                    if (D_800F14E8 != 0) {
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
    } else if (D_800C2358 != 0) {
        if (D_800E1B70 != 0) {
            for (i = 0; i < D_800F14C0; i++) {
                s32 r, g, b, a, dr, dg, db;
                u16 *dst = D_800F14D0[i].dst[D_800E30A8];
                u8 *src = D_800F14D0[i].src;
                for (j1 = 0; j1 < 256; j1++) {
                    r = *src++;
                    g = *src++;
                    b = *src++;
                    a = (r + g + b) / 3;
                    r += (a - r) * D_800E1B70 >> 8;
                    g += (a - g) * D_800E1B70 >> 8;
                    b += (a - b) * D_800E1B70 >> 8;
                    a = (r << 11) | (g << 6) | (b << 1);
                    a |= *dst & 1;
                    *dst = a;
                    dst++;
                }
            }
        }
    } else if (((D_800EFD64 - (s32)D_800CB210) * -0x33333333 >> 5) != 20) {
        if (D_800CCD64 != 0) {
            k = -123;
            if (D_800F14C8[D_800E30A8] != k) {
                D_800F14C8[D_800E30A8] = k;
                for (i = 0; i < D_800F14C0; i++) {
                    dst = D_800F14D0[i].dst[D_800E30A8];
                    src = D_800F14D0[i].src;
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
        } else if (((Track *)D_800EFD64)->f2C == 1) {
            k = (D_800E8E9C - 25) * 3;
            if (k < 0) {
                k = 0;
            }
            if (k > 100) {
                k = 100;
            }
            D_800CCD60 = k;
            if (k != D_800F14C8[D_800E30A8]) {
                D_800F14C8[D_800E30A8] = k;
                for (i = 0; i < D_800F14C0; i++) {
                    dst = D_800F14D0[i].dst[D_800E30A8];
                    src = D_800F14D0[i].src;
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
