#include "ctx.h"

typedef struct {
    u8 pad0[4];
    u16 *f4;
} Tex;

typedef struct {
    u8 pad0[0xC];
    u16 *src;      /* 0x0C */
    u16 *src2;     /* 0x10 */
    u16 *dst;      /* 0x14 */
    u16 *base;     /* 0x18 */
    u16 pal2[256]; /* 0x1C */
    u8 pad21C[0xA80 - 0x21C];
} Pal;

typedef struct {
    s32 w0;
    u8 pad4[0x38];
    s32 w3C;
    u8 pad40[0x380];
    s32 w3C0;
} Game;

typedef struct {
    u8 pad0[0x654];
    u8 b654;
    u8 pad655[0x6E8 - 0x655];
    u8 b6E8;
    u8 pad6E9[0x7E0 - 0x6E9];
} Player;

typedef struct {
    u8 pad0[0x2C];
    s32 f2C;
    u8 pad30[0xA0 - 0x30];
} Track;

extern s32 D_800C2358;
extern Game D_800EFD70;
extern Pal D_800DC150[];
extern Tex *D_800E1600[][6];
extern s32 D_800CCD64;
extern s32 D_800F14E8;
extern Player D_800E37C0[];
extern s32 D_800E76D0;
extern s32 D_800E1898;
extern s32 D_800C2280;
extern Track *D_800EFD64;
extern Track D_800CB210[];
extern s32 D_800E1A58[];
extern s32 D_800DC0B8[];
extern u8 D_800DC0E0[];
extern s32 D_800E1B50[];
extern s32 D_800E1818[];
extern u8 D_800DC0E8[][4];
extern u8 D_800E1A90[];

void func_8006A9DC(s32 a0, s32 *a1, s32 *a2, s32 *a3);

void func_80007D90(s32 pl, s32 k)
{
    s32 r, g, b;
    s32 i, c, v, t;
    s32 fr, fg, fb;
    s32 cr, cg, cb;

    if (D_800C2358 != 0) {
        return;
    }
    if (D_800EFD70.w0 & 0x10) {
        if (D_800EFD70.w3C == 1) {
            D_800DC150[pl].src = D_800E1600[pl][(k + D_800EFD70.w3C0) % 5]->f4;
            D_800DC150[pl].dst = D_800DC150[pl].base + ((k + D_800EFD70.w3C0) % 5) * 0x100;
        } else {
            D_800DC150[pl].src = D_800E1600[pl][k]->f4;
            D_800DC150[pl].dst = D_800DC150[pl].base + k * 0x100;
        }
    }
    i = 0;
    if (D_800CCD64 != 0) {
        do {
            c = D_800DC150[pl].src[i];
            r = c;
            g = (c >> 6) & 0x1F;
            b = (c >> 1) & 0x1F;
            r = (((r >> 11) & 0x1F) + g + b) * 8 / 3;
            func_8006A9DC(r, &r, &g, &b);
            r >>= 3;
            g >>= 3;
            b >>= 3;
            D_800DC150[pl].dst[i] = (r << 11) | (g << 6) | (b << 1) | 1;
            c = D_800DC150[pl].src2[i];
            r = c;
            g = (c >> 6) & 0x1F;
            b = (c >> 1) & 0x1F;
            r = (((r >> 11) & 0x1F) + g + b) * 8 / 3;
            func_8006A9DC(r, &r, &g, &b);
            r >>= 3;
            g >>= 3;
            b >>= 3;
            D_800DC150[pl].pal2[i] = (r << 11) | (g << 6) | (b << 1) | 1;
            i++;
        } while (i < 0x100);
        D_800DC150[pl].pal2[0] &= 0xFFFE;
        return;
    }
    if (D_800F14E8 != 0) {
        do {
            s32 x, y, z;

            x = D_800DC150[pl].src[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x + y + z) / 3;
            if (x > 31) {
                x = 31;
            }
            D_800DC150[pl].dst[i] = (x << 11) | (x << 6) | (x << 1) | 1;
            x = D_800DC150[pl].src2[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x + y + z) / 3;
            if (x > 31) {
                x = 31;
            }
            D_800DC150[pl].pal2[i] = (x << 11) | (x << 6) | (x << 1) | 1;
            i++;
        } while (i < 0x100);
        D_800DC150[pl].pal2[0] &= 0xFFFE;
        D_800E1A90[pl] = 1;
        return;
    }
    if ((D_800E37C0[pl].b654 != 0 && D_800E76D0 % 30 < 15 && D_800E1898 >= 2) ||
        (D_800E37C0[pl].b6E8 == 0 && (D_800C2280 & 0x400) && D_800E76D0 % 32 < 16)) {
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = D_800DC150[pl].src[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x += (31 - x) * 3 / 8;
            y = y * 5 / 8;
            z = z * 5 / 8;
            D_800DC150[pl].dst[i] = (x << 11) | (y << 6) | (z << 1) | 1;
            x = D_800DC150[pl].src2[i];
            x += (31 - x) * 3 / 8;
            y = y * 5 / 8;
            z = z * 5 / 8;
            D_800DC150[pl].pal2[i] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        D_800DC150[pl].pal2[0] &= 0xFFFE;
        D_800E1A90[pl] = 1;
        return;
    }
    if (D_800EFD64->f2C == 2 && D_800E1A58[pl] > 0xC48) {
        fr = D_800DC0E0[0] * 31 / 256;
        fg = D_800DC0E0[1] * 31 / 256;
        fb = D_800DC0E0[2] * 31 / 256;
        t = (D_800E1A58[pl] - 0xC48) * 255 / 5241;
        if (t > 255) {
            t = 255;
        }
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = D_800DC150[pl].src[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x += ((fr - x) * t) >> 8;
            y += ((fg - y) * t) >> 8;
            z += ((fb - z) * t) >> 8;
            D_800DC150[pl].dst[i] = (x << 11) | (y << 6) | (z << 1) | 1;
            x = D_800DC150[pl].src2[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x += ((fr - x) * t) >> 8;
            y += ((fg - y) * t) >> 8;
            z += ((fb - z) * t) >> 8;
            D_800DC150[pl].pal2[i] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        return;
    }
    D_800DC0B8[pl] |= 4;
    if (D_800E1B50[pl] & 0x200) {
        if (D_800E1818[pl * 3] == D_800DC0E8[pl][0] && D_800E1818[pl * 3 + 1] == D_800DC0E8[pl][1] &&
            D_800E1818[pl * 3 + 2] == D_800DC0E8[pl][2] && D_800E1A90[pl] == 0) {
            return;
        }
        D_800E1A90[pl] = 0;
        cr = D_800E1818[pl * 3];
        cg = D_800E1818[pl * 3 + 1];
        cb = D_800E1818[pl * 3 + 2];
        if (D_800EFD64 - D_800CB210 == 15) {
            cr += (255 - cr) / 3;
            cg += (255 - cg) / 3;
            cb += (255 - cb) / 3;
        }
        D_800DC0E8[pl][0] = cr;
        D_800DC0E8[pl][1] = cg;
        D_800DC0E8[pl][2] = cb;
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = D_800DC150[pl].src[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x * cr) >> 8;
            y = (y * cg) >> 8;
            z = (z * cb) >> 8;
            D_800DC150[pl].dst[i] = (x << 11) | (y << 6) | (z << 1) | 1;
        }
        for (i = 0; i < 0x100; i++) {
            s32 x, y, z;

            x = D_800DC150[pl].src2[i];
            y = (x >> 6) & 0x1F;
            z = (x >> 1) & 0x1F;
            x = (x >> 11) & 0x1F;
            x = (x * cr) >> 8;
            y = (y * cg) >> 8;
            z = (z * cb) >> 8;
            D_800DC150[pl].pal2[i] = (x << 11) | (y << 6) | (z << 1) | (i != 0);
        }
    } else {
        __builtin_memcpy(D_800DC150[pl].dst, D_800DC150[pl].src, 0x200);
        __builtin_memcpy(D_800DC150[pl].pal2, D_800DC150[pl].src2, 0x200);
        D_800DC0E8[pl][0] = 0xFF;
        D_800DC0E8[pl][1] = 0xFF;
        D_800DC0E8[pl][2] = 0xFF;
    }
}
