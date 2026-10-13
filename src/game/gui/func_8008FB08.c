#include "ctx.h"

typedef struct {
    u8 w;
    u8 h;
    u8 pad[2];
    u8 *img;
} Img;

typedef struct {
    u8 pad[4];
    u8 *pal;
    Img imgs[6];
} Tex;

typedef struct {
    u8 pad[4];
    Tex *t4;
    Tex *t8;
    u8 pad2[0x18];
    s32 *fonts[8];
} Res;

typedef struct {
    u8 *str;
    s32 flags;
    s32 pad[2];
} Item;

typedef struct {
    u8 *title;
    s16 count;
    s16 sel;
    Item *items;
    s32 flags;
} Menu;

extern Res *D_800CE200;
extern s32 *func_800C1E60(s32 *p1, s32 p2, s32 p3, s32 p4, s32 p5);
extern char *func_800C1F2C(s32 *p, char *s);
extern Gfx *func_800C1394(Gfx *gdl);
extern u8 *func_800C0ACC(s32 *p, s32 x, s32 y);
extern Gfx *func_8000A690(s32 x, s32 y, Img *img, Gfx *gdl, s32 a4, s32 a5, s32 a6, s32 a7, s32 a8);

Gfx *func_8008FB08(s32 arg0, Gfx *gdl, Menu *m, s32 x, s32 y)
{
    Tex *t;
    Img *im;
    Img *ti;
    u8 *s;
    s32 i;
    s32 c;
    s32 pad;
    s32 k;

    if (m == NULL) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPSetRenderMode(gdl++, 0x404240, 0);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPPipeSync(gdl++);
    t = D_800CE200->t4;
    func_800C1E60(D_800CE200->fonts[0], 0x20, 4, 2, 0x21);
    func_800C1F2C(D_800CE200->fonts[0], (char *)m->title);
    func_800C0ACC(D_800CE200->fonts[0], x + t->imgs[3].w * 4, y + 10);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    for (i = 0; i < m->count; i++) {
        s = m->items[i].str;
        if (!(m->items[i].flags & 0x10)) {
            func_800C1E60(D_800CE200->fonts[i + 1], 0x20, 5, 2, 0x20000021);
        } else {
            func_800C1E60(D_800CE200->fonts[i + 1], 0x20, 5, ((m->flags & 1) && m->sel == i) ? 7 : 2, 0x21);
        }
        pad = 0;
        im = &D_800CE200->t4->imgs[2];
        if ((m->flags & 1) && m->sel == i) {
            gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
        } else {
            gDPSetPrimColor(gdl++, 0, 0, 180, 180, 180, 255);
        }
        if (!(m->items[i].flags & 0x10)) {
            while (*s == ' ') {
                s++;
            }
        }
        func_800C1F2C(D_800CE200->fonts[i + 1], (char *)s);
        k = im->h * (i + 1) + 11;
        func_800C0ACC(D_800CE200->fonts[i + 1], (x + im->w * 4) | pad, (y + k) | pad);
        gdl = func_8000A690(x, y + (i + 1) * im->h, im, gdl, arg0, 1, 1, 6, ((u32)im & 1) >> 1);
        if (*s == ' ') {
            gDPPipeSync(gdl++);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetRenderMode(gdl++, 0x404240, 0);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, D_800CE200->t8->pal);
            ti = &D_800CE200->t8->imgs[0];
            gDPLoadTextureBlock(gdl++, ti->img, G_IM_FMT_CI, G_IM_SIZ_8b, ti->w * 8, ti->h, 0,
                                0, 0, 0, 0, 0, 0);
            gDPPipeSync(gdl++);
            gSPTextureRectangle(gdl++, (x + 11) << 2, (y + (i + 1) * im->h + 6) << 2,
                                (x + 11 + ti->w * 8) << 2, (y + (i + 1) * im->h + 6 + (u8)(ti->h / 3)) << 2,
                                0, 0, (ti->h - 1) << 5, 1 << 10, -1 << 10);
            gDPPipeSync(gdl++);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
        }
    }
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gdl++, D_800CE200->t4->pal);
    return func_800C1394(func_8000A690(x, y, &D_800CE200->t4->imgs[5], gdl, arg0, 1, 1, 3, 0));
}
