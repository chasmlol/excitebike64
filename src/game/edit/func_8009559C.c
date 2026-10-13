#include "ctx.h"

typedef struct {
    char pad0[0x16C5];
    u8 name[11];
    s32 sel;
    char pad1[0x8];
    s32 mode;
    char pad2[0x38];
    s32 *font;
} Menu;

typedef struct {
    char pad0[4];
    u8 *img;
} Res;

extern Res *D_800CE200;
extern Menu *D_800CEA40;
extern u8 *D_800CD1AC[];
extern u8 D_800DABE8[];
extern u8 D_800DABF8[];

extern Gfx *func_8000A690(s32 x, s32 y, u8 *img, Gfx *gdl, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8);
extern s32 *func_800C1E60(s32 *p, s32 a, s32 b, s32 c, s32 d);
extern char *func_800C1F2C(s32 *p, u8 *s);
extern void func_800C0ACC(s32 *p, s32 x, s32 y);

#define RAW(g, a, b) { Gfx *_g = (Gfx *)(g); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_8009559C(Gfx *gdl, s32 arg1)
{
    s32 u, j, m, i, k, y0, xo, t, yoff, yy, idx, x, c, s, f;
    u8 *p;
    Menu *mp;
    u8 buf[8];

    RAW(gdl++, 0xE3000C00, 0);
    RAW(gdl++, 0xFA000000, 0xFAFAFAFF);
    RAW(gdl++, 0xFC119623, 0xFF2FFFFF);
    RAW(gdl++, 0xE3001001, 0);
    RAW(gdl++, 0xE200001C, 0x00404240);
    RAW(gdl++, 0xD7000002, 0x80008000);
    p = D_800CE200->img + 0x48;
    gdl = func_8000A690(0xBC, 0x78 - (p[1] >> 1), p, gdl, arg1, 1, 1, 6, 0);
    func_800C1E60(D_800CEA40->font, 0x40, 2, 2, 0x20);
    func_800C1F2C(D_800CEA40->font, D_800DABE8);
    func_800C0ACC(D_800CEA40->font, 0xCC, 0xA7);
    c = -0xF2;
    for (i = 0; i < 5; i++) {
        for (k = 0, idx = i << 3; k < 8; k++) {
            mp = D_800CEA40;
            s = 1;
            if (idx >= 0x26) s = 2;
            f = 2;
            if (idx == mp->sel) f = 7;
            if (mp->mode == 10 && idx < 0x26) func_800C1E60(mp->font, 0x20, s, f, 0x20000021);
            else func_800C1E60(mp->font, 0x20, s, f, 0x21);
            func_800C1F2C(D_800CEA40->font, D_800CD1AC[idx]);
            x = k * 26 - (i * 5 + c);
            func_800C0ACC(D_800CEA40->font, x, i * 17 + 0x48);
            if (idx == D_800CEA40->sel) {
                p = D_800CE200->img + 0x50;
                gdl = func_8000A690(x - p[0] * 4 + 4, i * 17 - (p[1] >> 1) + 0x49, p, gdl, arg1, 1, 1, 6, 0);
                do { } while (0);
                idx++;
            } else {
                idx++;
            }
        }
    }
    for (j = 0; j < 10; j++) {
        if (D_800CEA40->name[j] == 0) break;
        f = 2;
        if (j == D_800CEA40->mode) f = 7;
        func_800C1E60(D_800CEA40->font, 0x20, 2, f, 0x21);
        buf[0] = D_800CEA40->name[j];
        buf[1] = 0;
        func_800C1F2C(D_800CEA40->font, buf);
        func_800C0ACC(D_800CEA40->font, j * 13 + 0x121, 0xA5);
    }
    for (j = 0; j < 10; j++) {
        f = 2;
        if (j == D_800CEA40->mode) f = 7;
        func_800C1E60(D_800CEA40->font, 0x20, 1, f, 0x21);
        func_800C1F2C(D_800CEA40->font, D_800DABF8);
        func_800C0ACC(D_800CEA40->font, j * 13 + 0x120, 0xAB);
    }
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPPipeSync(gdl++);
    return gdl;
}
