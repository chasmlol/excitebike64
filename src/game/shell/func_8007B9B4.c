#include "ctx.h"

typedef struct {
    u8 b[8];
} Ent8;

typedef struct {
    u8 pad0[0x3C];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    s32 f4C;
    u8 pad50[0x74 - 0x50];
    s32 *f74[4];
    u8 pad84[0x17C - 0x84];
    s32 f17C;
    u8 pad180[0x390 - 0x180];
    u8 *f390;
    u8 pad394[0x3A4 - 0x394];
    Ent8 *f3A4;
    u8 pad3A8[4];
    u8 *f3AC;
    u8 pad3B0[0x3DC - 0x3B0];
    s32 *f3DC;
    u8 pad3E0[0x404 - 0x3E0];
    u8 *f404;
    u8 pad408[0x410 - 0x408];
    s32 f410;
    u8 pad414[0x74C - 0x414];
    s32 *f74C;
    u8 pad750[0x75C - 0x750];
    s32 f75C;
} Glob;

extern Glob D_800EFD70;
extern s32 D_800F13EC;
extern s32 D_800F13F0;
extern u8 D_800C292D;
extern s32 D_800E2038;
extern u8 D_800F1BC0;
extern char *D_800CDDE8[];
extern char D_800D9AAC[];
extern char D_800D9AC0[];
extern char D_800D9AC4[];

s32 func_800C2224(void);
void func_8000EE30(s32 v, char *out);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
Gfx *func_800C1394(Gfx *gdl);
u8 *func_800C0ACC(s32 *p, s32 x, s32 y);
Gfx *func_8000A690(s32 x, s32 y, void *img, Gfx *gdl, s32 a4, s32 sx, s32 sy, s32 flags, s32 dw);

Gfx *func_8007B9B4(Gfx *gdl, s32 p2, s32 p3)
{
    s32 dx, dy, dz;
    s32 i;
    u8 *img;
    char buf[32];
    char num[2];

    if (p3 != 1 || D_800C292D < 2 || D_800EFD70.f4C != 0) {
        dx = 0;
        if (p3 == -1) {
            dx = 7;
        }
    } else {
        dx = -0x18;
    }
    dy = 0;
    if (p3 == -1) {
        dy = -0xC;
    }
    dz = 0;
    if (p3 == 1 && D_800C292D >= 2 && D_800EFD70.f4C == 0) {
        dz = -0x17;
    }
    func_800C2224();
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
    gDPFillRectangle(gdl++, 0, 0, D_800F13EC - 1, D_800F13F0 - 1);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPLoadTLUT_pal256(gdl++, D_800EFD70.f3DC[1]);
    for (i = (p3 == -1); i < 4; i++) {
        if (p3 == 1 && D_800C292D >= 2 && D_800EFD70.f4C == 0) {
            if (i == p3) {
                img = (u8 *)D_800EFD70.f3DC + 0x10;
            } else {
                img = (u8 *)D_800EFD70.f3DC + 0x18;
            }
        } else {
            img = (u8 *)D_800EFD70.f3DC + 8;
        }
        gdl = func_8000A690(0xC0 - i * 7 + dx, i * 25 + 0x46 + dy, img, gdl, p2, 1, 1, 3, 0);
        if (p3 == 1) {
            if (i >= D_800C292D) {
                func_800C1E60(D_800EFD70.f74[i], 0x40, 1, 2, 0x20000021);
                func_800C1F2C(D_800EFD70.f74[i], D_800D9AAC);
                func_800C0ACC(D_800EFD70.f74[i], D_800F13EC / 2 + 17 - i * 7 + dx,
                              D_800F13F0 / 2 - 39 + i * 25 + dy);
            } else {
                if (i == p3 && D_800EFD70.f4C == 0) {
                    func_800C1E60(D_800EFD70.f74[i], 0x40, 1, (D_800EFD70.f3C == 2) ? 4 : 2, 0x21);
                    func_800C1F2C(D_800EFD70.f74[i], D_800D9AC0);
                    func_800C0ACC(D_800EFD70.f74[i], D_800F13EC / 2 + (dx + 10) + 0x50 + dz,
                                  D_800F13F0 / 2 + (dy - 14));
                    func_800C1E60(D_800EFD70.f74[i], 0x40, 1, (D_800EFD70.f3C == 2) ? 6 : 2, 0x21);
                    func_8000EE30(D_800EFD70.f75C, buf);
                    func_800C1F2C(D_800EFD70.f74[i], buf);
                    func_800C0ACC(D_800EFD70.f74[i], D_800F13EC / 2 + (dx + 10) + 0x7A + dz,
                                  D_800F13F0 / 2 + (dy - 14));
                }
                if (((D_800EFD70.f4C == 2 || D_800EFD70.f4C == 3) && !(i & 1)) ||
                    (D_800EFD70.f4C == 7 && i >= 2) || (D_800EFD70.f4C == 10 && i >= 2)) {
                    func_800C1E60(D_800EFD70.f74[i], 0x40, 1, (i == D_800EFD70.f3C - 1) ? 4 : 2, 0x20000021);
                } else {
                    func_800C1E60(D_800EFD70.f74[i], 0x40, 1, (i == D_800EFD70.f3C - 1) ? 4 : 2, 0x21);
                }
                num[0] = i + '1';
                num[1] = 0;
                func_800C1F2C(D_800EFD70.f74[i], num);
                func_800C1F2C(D_800EFD70.f74[i], D_800D9AC4);
                func_800C0ACC(D_800EFD70.f74[i], D_800F13EC / 2 + 17 - i * 7 + dx + dz,
                              D_800F13F0 / 2 - 39 + i * 25 + dy);
            }
        } else {
            func_800C1E60(D_800EFD70.f74[i], 0x40, 1, (i == D_800EFD70.f410) ? 4 : 2,
                          (i >= D_800F1BC0 + 1) ? 0x20000021 : 0x21);
            func_800C1F2C(D_800EFD70.f74[i], D_800CDDE8[i]);
            func_800C0ACC(D_800EFD70.f74[i], D_800F13EC / 2 + 17 - i * 7 + dx,
                          D_800F13F0 / 2 - 39 + i * 25 + dy);
        }
    }
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    if (p3 == -1) {
        img = D_800EFD70.f404 + 0x10;
    } else if (p3 == 1 && D_800C292D >= 2 && D_800EFD70.f4C == 0) {
        img = D_800EFD70.f404 + 8;
    } else {
        img = D_800EFD70.f390 + 0x18;
    }
    gdl = func_8000A690((D_800F13EC - img[0] * 8) / 2, (D_800F13F0 - img[1]) / 2, img, gdl, p2, 1, 1, 6, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, D_800EFD70.f3AC + 4);
    img = (u8 *)&D_800EFD70.f3A4[D_800EFD70.f17C / 2 % 30 + 1];
    if (p3 == 1) {
        gdl = func_8000A690(0xA7 - (D_800EFD70.f3C - 1) * 7 + dx, (D_800EFD70.f3C - 1) * 25 + 0x49 + dy, img,
                            gdl, p2, 1, 1, 3, 0);
    } else {
        gdl = func_8000A690(0xA7 - D_800EFD70.f410 * 7 + dx, D_800EFD70.f410 * 25 + 0x49 + dy, img,
                            gdl, p2, 1, 1, 3, 0);
    }
    if (p3 == 1 && D_800C292D >= 2 && D_800EFD70.f4C == 0 && D_800EFD70.f3C == 2 && (D_800E2038 & 0x20)) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        gDPLoadTLUT_pal256(gdl++, D_800EFD70.f74C[1]);
        gdl = func_8000A690(D_800F13EC / 2 + 10 + dx + 0x60 + dz, D_800F13F0 / 2 - 18 + dy, (u8 *)D_800EFD70.f74C + 0x18,
                            gdl, p2, 1, 1, 3, 0);
        gdl = func_8000A690(D_800F13EC / 2 + 10 + dx + 0x84 + dz, D_800F13F0 / 2 - 18 + dy, (u8 *)D_800EFD70.f74C + 0x20,
                            gdl, p2, 1, 1, 3, 0);
    }
    return func_800C1394(gdl);
}
