#include "ctx.h"

typedef struct {
    u8 w, h, pad, pal;
    u8 *addr;
} Img;

typedef struct {
    u32 n;
    u32 pal;
    Img g[1];
} Font;

typedef struct {
    u32 size;
    u8 pad[0x3C];
    char name[1];
} Entry;

typedef struct {
    s32 mode;
    s32 msg;
    s32 f8;
    s32 pak;
    s32 sel;
    s32 f14;
    s32 scroll;
} PakState;

typedef struct {
    char **lines;
    char *text;
    char *extra;
} Msg;

extern Font *D_800F49F0;
extern Font *D_800F49F4;
extern Font *D_800F4A20;
extern Font *D_800F4A60;
extern PakState D_800F4A28;
extern s32 D_800F4A2C;
extern s32 D_800F4A78;
extern s32 D_800F4A68[];
extern s32 D_800F4AE0[];
extern s32 D_800F4AF0;
extern Entry *D_800F48E0[4][17];
extern Entry D_800CE168;
extern Msg D_800CE068[];
extern char *D_800CDEF0[];
extern u8 D_800C292C;
extern s32 D_800E2004;
extern s32 D_800E2008;
extern s32 D_800F13EC;
extern u8 D_800F47D0[];
extern u32 *D_800CE164;

void func_8000EF8C(char *a0, char *a1);
void func_8000ED30(char *dst, char *src);
s32 func_800C2224(void);
void func_8000EE30(s32 v, char *out);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
void func_800C0ACC(s32 *p, s32 x, s32 y);
Gfx *func_800C1394(Gfx *gdl);
Gfx *func_80088734(Gfx *gdl, u8 *s, s32 p3, s32 p4, s32 x, s32 y);
Gfx *func_8000A690(s32 x, s32 y, Img *img, Gfx *gdl, s32 a4, s32 sx, s32 sy, s32 flags, s32 dw);

static inline Gfx *drawStr(Gfx *gdl, u8 *str, s32 p3, s32 p4, s32 x, s32 y)
{
    s32 i;

    if (p3 == 2) {
        for (i = 0; str[i] != 0; i++) {
            if (str[i] == 0x40) {
                str[i] = 0x7F;
            }
        }
    }
    func_800C1E60(D_800CE164, 0x80, p3, p4, 0);
    func_800C1F2C(D_800CE164, (char *)str);
    func_800C0ACC(D_800CE164, x, y);
    gdl = func_800C1394(gdl);
    return gdl;
}

static inline Gfx *drawCtr(Gfx *gdl, char *str, s32 p3, s32 p4, s32 cx, s32 y)
{
    func_800C1E60(D_800CE164, 0x80, p3, p4, 0);
    func_800C1F2C(D_800CE164, str);
    func_800C0ACC(D_800CE164, cx - (D_800CE164[6] >> 1), y);
    return func_800C1394(gdl);
}

void func_80088CF8(s32 arg0, s32 arg1, Gfx **pgdl)
{
    Gfx *gdl;
    s32 c;
    s32 i;
    s32 j;
    s32 k;
    s32 x;
    s32 y;
    s32 col;
    u32 n;
    Entry *e;
    char **lines;
    char **p;
    s32 w;
    s32 h;
    s32 ly;

    gdl = *pgdl;
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    func_800C2224();
    gDPLoadTLUT_pal256(gdl++, D_800F49F0->pal);
    gdl = func_8000A690(0, 0, &D_800F49F0->g[0], gdl, arg1, 1, 1, 3, 0);
    gDPLoadTLUT_pal256(gdl++, D_800F4A60->pal);
    gdl = func_8000A690(0x4E, 0x16, &D_800F4A60->g[0], gdl, arg1, 1, 1, 3, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gdl = func_8000A690(0x4E, 0x13, &D_800F49F4->g[3], gdl, arg1, 1, 1, 6, 0);
    gDPSetCombine(gdl++, 0x11FE23, 0xFFFFF3F9);
    c = 0xFF;
    if (D_800F4A28.msg == 0) {
        c = 0x80;
    }
    gDPSetPrimColor(gdl++, 0, 0, c, c, c, 255);
    gdl = func_8000A690(0x2B, 0x32, &D_800F49F4->g[2], gdl, arg1, 1, 1, 6, 0);
    c ^= 0x7F;
    gDPSetPrimColor(gdl++, 0, 0, c, c, c, 255);
    gdl = func_8000A690(0x28, 0x95, &D_800F49F4->g[1], gdl, arg1, 1, 1, 6, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    for (j = 0; j < 0x200; j += 0x10) {
        gdl = func_8000A690(j, 0xB4, &D_800F49F4->g[0], gdl, arg1, 1, 1, 6, 0);
    }
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gdl++, D_800F4A20->pal);
    gDPSetCombine(gdl++, 0xFF97FF, 0xFF2CFE7F);
    for (i = 0; i < 4; i++) {
        if ((D_800C292C >> i) & 1) {
            c = 0x80;
            if ((D_800E2004 | D_800E2008) & (1 << i)) {
                c = 0xFF;
            }
            gDPSetColor(gdl++, G_SETPRIMCOLOR, c | 0xFFFFFF00);
            gdl = func_8000A690(0x37 + i * 100, 0x9C, &D_800F4A20->g[(u32)(D_800F4A78 / 2) % D_800F4A20->n], gdl,
                                arg1, 1, 1, 3, 0);
        }
    }
    gdl = drawStr(gdl, (u8 *)D_800CDEF0[D_800F4A28.mode], 1, 2, 0x62, 0x1D);
    __builtin_strcpy((char *)D_800F47D0, "Pak  ");
    for (i = 0; i < 4; i++) {
        if ((D_800E2004 | D_800E2008) & (1 << i)) {
            D_800F47D0[4] = i + '1';
            gdl = drawStr(gdl, D_800F47D0, 1, (i == D_800F4A28.pak) ? 4 : 2, 0x55 + i * 100, 0x9F);
        }
    }
    if (D_800E2004 & (1 << D_800F4A28.pak)) {
        for (i = 0; i < D_800F4AE0[D_800F4A28.pak]; i++) {
            c = i - D_800F4A28.scroll;
            if (c >= 0) if (c < 8) {
                e = D_800F48E0[D_800F4A28.pak][i];
                x = 0x58 - c * 5 / 2;
                y = c * 10 + 0x3B;
                func_8000EF8C((char *)D_800F47D0, e->name);
                if (D_800F4A28.msg == 0) {
                    c = 6;
                } else if (i == D_800F4A28.sel) {
                    c = 5;
                } else if (e == &D_800CE168) {
                    c = 7;
                    w = 0;
                } else {
                    c = 2;
                }
                gdl = drawStr(gdl, D_800F47D0, 2, c, x, y);
                if (e != &D_800CE168) {
                    n = e->size;
                    n += 0xFF;
                    n >>= 8;
                    func_8000EE30(n, (char *)D_800F47D0);
                    func_800C1E60(D_800CE164, 0x80, 2, c, 0);
                    func_800C1F2C(D_800CE164, (char *)D_800F47D0);
                    func_800C0ACC(D_800CE164, x + (0x140 - D_800CE164[6]), y);
                    gdl = func_800C1394(gdl);
                    __builtin_strcpy((char *)D_800F47D0, "page");
                    if (n != 1) {
                        func_8000ED30((char *)D_800F47D0, "s");
                    }
                    gdl = drawStr(gdl, D_800F47D0, 2, c, x + 0x14A, y);
                }
            }
        }
    } else if (D_800E2008 & (1 << D_800F4A28.pak)) {
        gdl = drawCtr(gdl, "Damaged or Corrupted Controller Pak!", 2, 6, 0x100, 0x5C);
    }
    if (D_800F4A28.mode == 2 || D_800F4A28.mode == 0) {
        if (D_800E2004 & (1 << D_800F4A28.pak)) {
            func_8000EE30(D_800F4A68[D_800F4A28.pak], (char *)D_800F47D0);
            func_8000ED30((char *)D_800F47D0, " page");
            if (D_800F4A68[D_800F4A28.pak] != 1) {
                func_8000ED30((char *)D_800F47D0, "s");
            }
            func_8000ED30((char *)D_800F47D0, " free");
            gdl = drawCtr(gdl, (char *)D_800F47D0, 1, 2, 0x18B, 0x1E);
        }
    }
    if (D_800CE068[D_800F4A28.msg].lines != NULL) {
        lines = D_800CE068[D_800F4A28.msg].lines;
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
        gDPSetRenderMode(gdl++, 0x404240, 0);
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xC0);
        gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, 0xB4);
        gDPPipeSync(gdl++);
        for (i = 0; lines[i] != NULL; i++) {
        }
        h = i * 14 + 0x50;
        gDPFillRectangle(gdl++, 0, 0x4C, 0x200, h);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPLoadTextureBlock(gdl++, D_800F49F4->g[0].addr, G_IM_FMT_IA, G_IM_SIZ_8b, D_800F49F4->g[0].w * 8,
                            D_800F49F4->g[0].h, 0, G_TX_CLAMP, G_TX_CLAMP, 4, 8, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, 0, 0x114, 0x800, 0x130, 0, 0, 0, 0x400, 0x400);
        gSPTextureRectangle(gdl++, 0, (h - 1) * 4, 0x800, (h + 7) * 4, 0, 0, 0x5C0, 0x400, 0x400);
        gDPPipeSync(gdl++);
        for (i = 0; lines[i] != NULL; i++) {
            gdl = drawCtr(gdl, lines[i], 1, 2, 0x100, 0x53 + i * 14);
        }
        if (D_800CE068[D_800F4A28.msg].extra != NULL) {
            w = (s32)func_80088734(NULL, (u8 *)D_800CE068[D_800F4A28.msg].extra, 1, 4, 0, 0);
            gdl = func_80088734(gdl, (u8 *)D_800CE068[D_800F4A28.msg].extra, 1, 4, 0x100 - w / 2, 0xC6);
        }
    } else if (D_800CE068[D_800F4A28.msg].text != NULL) {
        gdl = drawCtr(gdl, D_800CE068[D_800F4A28.msg].text, 1, 2, 0x100, 0xBE);
        if (D_800CE068[D_800F4A28.msg].extra != NULL) {
            w = (s32)func_80088734(NULL, (u8 *)D_800CE068[D_800F4A28.msg].extra, 1, 4, 0, 0);
            gdl = func_80088734(gdl, (u8 *)D_800CE068[D_800F4A28.msg].extra, 1, 4, 0x100 - w / 2, 0xCE);
        }
    }
    if (D_800F4AF0 != 0) {
        D_800F4AF0--;
    }
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}
