#include "ctx.h"

typedef struct {
    u8 pad0[0x4C];
    s32 mode;
    u8 pad50[0x74 - 0x50];
    s32 *f74[3];
    u8 pad80[0x17C - 0x80];
    s32 f17C;
    u8 pad180[0x3A4 - 0x180];
    struct { u8 b[8]; } *f3A4;
    u8 pad3A8[4];
    u8 *f3AC;
    u8 pad3B0[0x3DC - 0x3B0];
    s32 *f3DC;
    u8 pad3E0[0x404 - 0x3E0];
    u8 *f404;
} Glob;

extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern Glob D_800EFD70;
extern s32 D_800F0950;
extern s32 D_800F0954;
extern s32 D_800F0A3C;

s32 func_800C2224(void);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
void func_800C0ACC(s32 *p, s32 a1, s32 a2);
Gfx *func_8000A690(s32 x, s32 y, void *img, Gfx *gdl, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8);

Gfx *func_8007B39C(Gfx *gdl, s32 p2, s32 sel, char **strs)
{
    s32 flag;
    s32 i;
    u8 *img;
    s32 x, y, a, c;

    flag = 0;
    if (D_800EFD70.mode == 8 || D_800F0954 - D_800F0A3C >= D_800F0950) {
        flag = 1;
    }
    func_800C2224();
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPLoadTLUT_pal256(gdl++, D_800EFD70.f3DC[1]);
    i = 0;
    x = -25;
    y = -13;
    c = 0;
    for (; i < 3; i++) {
        gdl = func_8000A690(0xC0 - c, i * 25 + 0x52, (u8 *)D_800EFD70.f3DC + 8, gdl, p2, 1, 1, 3, 0);
        func_800C1E60(D_800EFD70.f74[i], 0x40, 1, (sel == i) ? 4 : 2, (i == 1 && flag) ? 0x20000021 : 0x21);
        func_800C1F2C(D_800EFD70.f74[i], strs[i]);
        func_800C0ACC(D_800EFD70.f74[i], D_800F13EC / 2 - y, D_800F13F0 / 2 + x);
        x += 25;
        y += 7;
        c += 7;
    }
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    img = D_800EFD70.f404 + 0x10;
    gdl = func_8000A690((D_800F13EC - img[0] * 8) / 2, (D_800F13F0 - img[1]) / 2, img, gdl, p2, 1, 1, 6, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, D_800EFD70.f3AC + 4);
    return func_8000A690(0xA1 - (sel - 1) * 7, sel * 25 + 0x56,
                         &D_800EFD70.f3A4[D_800EFD70.f17C / 2 % 30 + 1], gdl, p2, 1, 1, 3, 0);
}
