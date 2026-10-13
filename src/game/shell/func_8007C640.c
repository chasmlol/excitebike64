#include "ctx.h"

typedef struct {
    u8 pad0[0x74];
    s32 *f74;
    u8 pad78[0x17C - 0x78];
    s32 f17C;
    u8 pad180[0x3A4 - 0x180];
    struct { u8 b[8]; } *f3A4;
    u8 pad3A8[4];
    u8 *f3AC;
    u8 pad3B0[0x47C - 0x3B0];
    s16 f47C;
    u8 pad47E[2];
    u8 *f480;
    s32 *f484;
} Glob;

extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern Glob D_800EFD70;
extern u8 D_800D9ACC[];
extern u8 D_800D9AD0[];

s32 func_800C2224(void);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
Gfx *func_800C1394(Gfx *gdl);
void func_800C0ACC(s32 *p, s32 a1, s32 a2);
Gfx *func_8000A690(s32 x, s32 y, void *img, Gfx *gdl, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8);

Gfx *func_8007C640(Gfx *gdl, s32 p2, char *str)
{
    u8 *img;
    s32 n;
    s32 y;
    s32 t;

    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, D_800EFD70.f484[1]);
    gdl = func_8000A690(0xA3, 0x58, (u8 *)D_800EFD70.f484 + 8, gdl, p2, 1, 1, 3, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    img = D_800EFD70.f480 + 8;
    gdl = func_8000A690(0x100 - img[0] * 4, 0x78 - (img[1] >> 1), img, gdl, p2, 1, 1, 6, 0);
    func_800C2224();
    func_800C1E60(D_800EFD70.f74, 0x40, 1, 2, 0x21);
    func_800C1F2C(D_800EFD70.f74, str);
    func_800C0ACC(D_800EFD70.f74, 0x105, 0x64);
    func_800C1E60(D_800EFD70.f74, 0x40, 1, D_800EFD70.f47C ? 4 : 2, 0x21);
    func_800C1F2C(D_800EFD70.f74, D_800D9ACC);
    func_800C0ACC(D_800EFD70.f74, 0x108, 0x76);
    func_800C1E60(D_800EFD70.f74, 0x40, 1, D_800EFD70.f47C ? 2 : 4, 0x21);
    func_800C1F2C(D_800EFD70.f74, D_800D9AD0);
    func_800C0ACC(D_800EFD70.f74, 0x103, 0x88);
    gdl = func_800C1394(gdl);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, D_800EFD70.f3AC + 4);
    n = D_800EFD70.f47C;
    return func_8000A690(n * 7 + 0x9D, (t = n * 5, y = 0x82, y - t * 4),
                         &D_800EFD70.f3A4[D_800EFD70.f17C / 2 % 30 + 1], gdl, p2, 1, 1, 3, 0);
}
