#include "ctx.h"

typedef struct {
    s32 f0;
    u32 pal;            /* 0x04 */
    u8 img[1];          /* 0x08 */
} Pic;

typedef struct {
    s32 *font0;         /* 0x00 */
    u8 pad4[0xC];
    s32 *f10;
    s32 *f14;
    s32 *f18;
    s32 *f1C;
    s32 *f20;
    s32 *f24;
    s32 *f28;
    s32 *f2C;
    s32 *f30;
    s32 *f34;
    s32 *f38;
    u8 pad3C[0x128 - 0x3C];
    u8 *f128;           /* 0x128 */
    Pic *f12C[5];       /* 0x12C */
    Pic *f140;          /* 0x140 */
    Pic *f144;          /* 0x144 */
} Res;

typedef struct {
    u8 n;
    u8 pad1[7];
    s16 ids[14];        /* 0x08 */
} List;

typedef struct {
    List *list;
    u8 pad4[0x60];
    s16 sel;            /* 0x64 */
} Sel;

typedef struct {
    u8 pad0[0x3C];
    s32 f3C;
    s32 f40;
    u8 pad44[8];
    s32 f4C;
} Glob;

typedef struct {
    char name[0x1E];
    s16 f1E;
    u8 pad20[0x14];
    s32 f34;
    s32 f38;
    s32 f3C;
    s32 f40;
    char f44[3];
    char f47[3];
    char f4A[3];
    char f4D[3];
} Info;

typedef struct {
    s32 x, y, w, h;
} Rect;

extern Res *D_800C2354;
extern Sel *D_800CA930;
extern List D_800CA9C8[];
extern char D_800CB210[][160];
extern Rect D_800E1718;
extern char D_800D8F58[];
extern char D_800D8F84[];
extern char D_800D8FA4[];
extern s32 D_800E2038;
extern Info *D_800EFD64;
extern Glob D_800EFD70;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;

s32 func_800C2224(void);
void func_8000EE30(s32 v, char *out);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
char *func_800C1F2C(s32 *p, char *s);
Gfx *func_800C1394(Gfx *gdl);
void func_80052F30(s32 param_1, char *param_2);
u8 *func_800C0ACC(s32 *p, s32 x, s32 y);
Gfx *func_8000A690(s32 x, s32 y, void *img, Gfx *gdl, s32 a4, s32 sx, s32 sy, s32 flags, s32 dw);

#define SX(n) (D_800F13EC / 2 - D_800F13EC * (n) / 512)
#define SY(n) (D_800F13F0 * (n) / 240)

Gfx *func_8006D664(Gfx *gdl, s32 arg1, s32 arg2)
{
    char buf[0x40];
    s32 i;
    s32 c;
    s32 sel;
    s32 x;
    s32 y;
    u8 *p;
    Glob *g;

    func_800C2224();
    gDPPipeSync(gdl++);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gDPLoadTLUT_pal256(gdl++, D_800C2354->f140->pal);
    gdl = func_8000A690(D_800E1718.x + 8, D_800E1718.y + 3, D_800C2354->f140->img, gdl, arg2, 1, 1, 3, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    if (D_800EFD70.f4C == 1) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        for (i = 0; i < D_800CA930->list->n; i++) {
            gDPLoadTLUT_pal256(gdl++, D_800C2354->f12C[i]->pal);
            p = D_800C2354->f12C[i]->img;
            if (i != D_800CA930->sel || (D_800E2038 & 0x20)) {
                func_800C1E60(D_800C2354->font0, 0x40, 2, (i == D_800CA930->sel) ? 7 : 2, (i != D_800CA930->sel) ? 0x20000021 : 0x21);
                func_800C1F2C(D_800C2354->font0, D_800CB210[D_800CA930->list->ids[i]]);
                func_800C0ACC(D_800C2354->font0, D_800E1718.x + D_800E1718.w - 0x69,
                              D_800E1718.y + 0x15 + D_800C2354->f128[0x21] * i);
            }
            gdl = func_8000A690(D_800E1718.x + D_800E1718.w - p[0] * 8 - 8,
                                D_800E1718.y + 0xB + D_800C2354->f128[0x21] * i, p, gdl, arg2, 1, 1, 3, 0);
        }
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        p = D_800C2354->f128 + 0x20;
        for (i = 0; i < D_800CA930->list->n; i++) {
            gdl = func_8000A690(D_800E1718.x + D_800E1718.w - p[0] * 8, D_800E1718.y + 6 + p[1] * i,
                                p, gdl, arg2, 1, 1, 6, 0);
        }
        p = D_800C2354->f128 + 0x18;
    } else {
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        p = D_800C2354->f128 + 0x18;
    }
    if (D_800E2038 & 0x20) {
        y = 0xAA;
        if (D_800EFD70.f4C == 1) y = 0x9E;
        func_800C1E60(D_800C2354->f10, 0x40, 2, 4, 0x21);
        func_800C1F2C(D_800C2354->f10, "Press the A Button to Race");
        func_800C0ACC(D_800C2354->f10, D_800F13EC / 2 - ((D_800EFD70.f4C == 1) ? 0x52 : 0), y);
    }
    if (D_800EFD70.f4C == 1) {
        func_800C1E60(D_800C2354->f10, 0x40, 2, 7, 0x21);
        if (D_800CA930->list - D_800CA9C8 == 4) {
            if (D_800CA930->sel == D_800CA930->list->n - 1) {
                func_800C1F2C(D_800C2354->f10, D_800D8F58);
            } else {
                func_800C1F2C(D_800C2354->f10, D_800D8F84);
            }
        } else {
            func_800C1F2C(D_800C2354->f10, D_800D8FA4);
        }
        func_800C0ACC(D_800C2354->f10, D_800F13EC / 2 - 0x52, 0xAA);
    }
    gdl = func_8000A690(D_800E1718.x, D_800E1718.y, p, gdl, arg2, 1, 1, 6, 0);
    p = D_800C2354->f128 + 8;
    gdl = func_8000A690(D_800E1718.x, D_800E1718.y + D_800E1718.h - p[1], p, gdl, arg2, 1, 1, 6, 0);
    if (D_800EFD70.f4C == 0 && D_800EFD70.f3C >= 2 && (D_800E2038 & 0x20)) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        gDPLoadTLUT_pal256(gdl++, D_800C2354->f144->pal);
        if (D_800EFD70.f40 >= 2) {
            gdl = func_8000A690(D_800E1718.x + D_800E1718.w - 0x4B, D_800E1718.y + D_800E1718.h - 0xE,
                                D_800C2354->f144->img, gdl, arg2, 1, 1, 3, 0);
        }
        if (D_800EFD70.f40 < 6) {
            gdl = func_8000A690(D_800E1718.x + D_800E1718.w - 0x28, D_800E1718.y + D_800E1718.h - 0xE,
                                D_800C2354->f144->img + 8, gdl, arg2, 1, 1, 3, 0);
        }
    }
    func_800C1E60(D_800C2354->f10, 0x40, 1, 4, 0x20);
    func_800C1F2C(D_800C2354->f10, "Best Times:");
    func_800C0ACC(D_800C2354->f10, SX(221), SY(197));
    func_800C1E60(D_800C2354->f14, 0x40, 1, 4, 0x20);
    func_800C1F2C(D_800C2354->f14, "Best Lap:");
    func_800C0ACC(D_800C2354->f14, SX(-7), SY(197));
    g = &D_800EFD70;
    func_800C1E60(D_800C2354->f14, 0x40, 1, 4, 0x20);
    func_800C1F2C(D_800C2354->f14, "Number of Laps:");
    func_800C0ACC(D_800C2354->f14, SX(-2), SY(214));
    func_800C1E60(D_800C2354->f14, 0x40, 1, 2, (g->f4C == 0 && g->f3C >= 2) ? 0x21 : 0x22);
    func_8000EE30(D_800EFD70.f40, buf);
    func_800C1F2C(D_800C2354->f14, buf);
    func_800C0ACC(D_800C2354->f14, SX(-217) - ((D_800EFD70.f4C == 0 && D_800EFD70.f3C >= 2) ? 0x1E : 0), SY(214));
    func_800C1E60(D_800C2354->f1C, 0x40, 1, 2, 0x20);
    if (D_800EFD70.f40 != D_800EFD64->f1E) {
        func_800C1E60(D_800C2354->f18, 0x40, 2, 2, 0x21);
        func_800C1E60(D_800C2354->f28, 0x40, 2, 2, 0x21);
        func_800C1E60(D_800C2354->f2C, 0x40, 2, 2, 0x21);
        func_800C1F2C(D_800C2354->f18, "Unavailable");
        func_800C0ACC(D_800C2354->f18, SX(61), SY(196));
        func_800C1F2C(D_800C2354->f28, "Unavailable");
        func_800C0ACC(D_800C2354->f28, SX(64), SY(205));
        func_800C1F2C(D_800C2354->f2C, "Unavailable");
        func_800C0ACC(D_800C2354->f2C, SX(66), SY(214));
    } else {
        func_800C1E60(D_800C2354->f18, 0x40, 2, 2, 0x20);
        func_800C1E60(D_800C2354->f28, 0x40, 2, 2, 0x20);
        func_800C1E60(D_800C2354->f2C, 0x40, 2, 2, 0x20);
        func_80052F30(D_800EFD64->f34, buf);
        func_800C1F2C(D_800C2354->f18, buf);
        func_800C0ACC(D_800C2354->f18, SX(116), SY(196));
        func_80052F30(D_800EFD64->f38, buf);
        func_800C1F2C(D_800C2354->f28, buf);
        func_800C0ACC(D_800C2354->f28, SX(119), SY(205));
        func_80052F30(D_800EFD64->f3C, buf);
        func_800C1F2C(D_800C2354->f2C, buf);
        func_800C0ACC(D_800C2354->f2C, SX(121), SY(214));
    }
    func_80052F30(D_800EFD64->f40, buf);
    func_800C1F2C(D_800C2354->f1C, buf);
    func_800C0ACC(D_800C2354->f1C, D_800F13EC / 2 + D_800F13EC * 111 / 512, SY(197));
    if (D_800EFD70.f40 == D_800EFD64->f1E) {
        func_800C1E60(D_800C2354->f20, 0x40, 2, 2, 0x22);
        __builtin_memcpy(buf, D_800EFD64->f44, 3);
        buf[3] = 0;
        func_800C1F2C(D_800C2354->f20, buf);
        func_800C0ACC(D_800C2354->f20, SX(7), SY(196));
        func_800C1E60(D_800C2354->f30, 0x40, 2, 2, 0x22);
        __builtin_memcpy(buf, D_800EFD64->f47, 3);
        buf[3] = 0;
        func_800C1F2C(D_800C2354->f30, buf);
        func_800C0ACC(D_800C2354->f30, SX(10), SY(205));
        func_800C1E60(D_800C2354->f34, 0x40, 2, 2, 0x22);
        __builtin_memcpy(buf, D_800EFD64->f4A, 3);
        buf[3] = 0;
        func_800C1F2C(D_800C2354->f34, buf);
        func_800C0ACC(D_800C2354->f34, SX(12), SY(214));
    }
    func_800C1E60(D_800C2354->f24, 0x40, 1, 2, 0x22);
    __builtin_memcpy(buf, D_800EFD64->f4D, 3);
    buf[3] = 0;
    func_800C1F2C(D_800C2354->f24, buf);
    func_800C0ACC(D_800C2354->f24, SX(-220), SY(197));
    func_800C1E60(D_800C2354->f38, 0x40, 4, 2, 0x20);
    func_800C1F2C(D_800C2354->f38, D_800EFD64->name);
    func_800C0ACC(D_800C2354->f38, SX(215), SY(29));
    return func_800C1394(gdl);
}
