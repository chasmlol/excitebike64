#include "ctx.h"

typedef struct { u8 w, h; u16 pad; void *addr; } Img;
typedef struct { s32 f0; u16 *pal; Img img[5]; } HudTex;
typedef struct { s32 x, y, w, h; } Rect;
typedef struct {
    u8 pad0[0x648];
    f32 f648;
    u8 pad64C[0x654 - 0x64C];
    u8 f654;
    u8 pad655[0x7E0 - 0x655];
} Player;

extern s32 D_800E1898;
extern s32 D_800C2598;
extern Rect D_800C2494[];
extern Rect D_800E1718[];
extern HudTex *D_800F0524;
extern HudTex *hudA_800F0524[];
extern Player D_800E37C0[];
extern s32 D_800E2038;
extern volatile s32 D_800F13EC;
extern volatile s32 D_800F13F0;

Gfx *func_8000A690(s32 x, s32 y, Img *img, Gfx *gdl, s32 a, s32 b, s32 c, s32 d, s32 e);

#define RP(T, n) ((Rect *)(((D_800C2598 << 4) + (((n) - 1) << 6)) + (u8 *)T))
#define RPN(T, n) ((Rect *)(((((n) - 1) << 6) + (D_800C2598 << 4)) + (u8 *)T))
#define RX(T, n) (RP(T, n)->x + RP(T, n)->w)
#define RY(T, n) (RP(T, n)->y + RP(T, n)->h)
#define R8(c) (((c) >> 8) & 0xF8)
#define G8(c) (((c) >> 3) & 0xF8)
#define B8(c) (((c) << 2) & 0xF8)
#define SL(v,s,w) ((u32)(((u32)(v) & ((0x01 << (w)) - 1)) << (s)))
#define DIM(c, a) (SL(R8(c) >> 2, 24, 8) | SL(G8(c) >> 2, 16, 8) | SL(B8(c) >> 2, 8, 8) | SL(a, 0, 8))
#define GW(g, a, b) { Gfx *_g = (Gfx *)(g); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *func_8005A4C8(Gfx *gdl, s32 a1, s32 a2, s32 pl) {
    s32 fp, n, bar, k;
    u16 col;
    Img *img;
    Rect *r;


    fp = -54;
    n = D_800E1898;
    if (n == 2) fp = -46;
    bar = D_800E37C0[pl].f648 * 38.0f / 12000.0f;
    if (bar >= 39) bar = 38;
    GW(gdl++, 0xE7000000, 0);
    GW(gdl++, 0xE3000C00, 0);
    GW(gdl++, 0xE3001001, 0);
    GW(gdl++, 0xFC119623, 0xFF2FFFFF);
    GW(gdl++, 0xE200001C, 0x00404240);
    GW(gdl++, 0xD9FFFFFE, 0);
    GW(gdl++, 0xD7000002, 0x80008000);
    GW(gdl++, 0xE3000C00, 0);
    GW(gdl++, 0xE3001001, 0);
    GW(gdl++, 0xFC119623, 0xFF2FFFFF);
    GW(gdl++, 0xE200001C, 0x00404240);
    GW(gdl++, 0xFA000000, DIM(D_800F0524->pal[0], 0x8C));
    { Rect *tab = D_800C2494; gdl = func_8000A690(RX(tab, n) - 24, RY(tab, n) + fp, &D_800F0524->img[0], gdl, a2, 1, 1, 6, 0); }
    GW(gdl++, 0xFA000000, DIM(D_800F0524->pal[4], 0xA0));
    gdl = func_8000A690(RX(D_800C2494, D_800E1898) - 24, RY(D_800C2494, D_800E1898) + fp, &D_800F0524->img[4], gdl, a2, 1, 1, 6, 0);
    bar++; GW(gdl++, 0xFA000000, DIM(D_800F0524->pal[3], 0x8C));
    gdl = func_8000A690(RX(D_800C2494, D_800E1898) - 24, RY(D_800C2494, D_800E1898) + fp, &D_800F0524->img[3], gdl, a2, 1, 1, 6, 0);
    for (k = 0; k < 3; k++) {
        switch (k) {
        case 0:
            img = &D_800F0524->img[0];
            col = D_800F0524->pal[0];
            break;
        case 1:
            img = &D_800F0524->img[4];
            col = D_800F0524->pal[4];
            break;
        case 2:
            img = &D_800F0524->img[3];
            col = D_800F0524->pal[3];
            break;
        }
        GW(gdl++, 0xFD680000 | ((img->w * 8 - 1) & 0xFFF), img->addr);
        GW(gdl++, 0xF5680000 | (img->w << 9), 0x07000000);
        GW(gdl++, 0xE6000000, 0);
        GW(gdl++, 0xF4000000, ((((img->w << 5) - 4) & 0xFFF) << 12) | 0x07000000 | (((bar - 1) * 4) & 0xFFF));
        GW(gdl++, 0xE7000000, 0);
        GW(gdl++, 0xF5680000 | (img->w << 9), 0);
        GW(gdl++, 0xF2000000, ((((img->w << 5) - 4) & 0xFFF) << 12) | (((bar - 1) * 4) & 0xFFF));
        GW(gdl++, 0xE7000000, 0);
        GW(gdl++, 0xFA000000, (((col >> 8) & 0xF8) << 24) | (((col >> 3) & 0xF8) << 16) | ((col << 10) & 0xF800) | 0xFF);
        r = RP(D_800E1718, D_800E1898);
        GW(gdl++, 0xE4000000 | ((((r->x + r->w) * 4 + (img->w * 8 - 24) * (D_800F13EC << 2) / 512) & 0xFFF) << 12) |
                      (((r->y + r->h) * 4 + (fp + img->h) * (D_800F13F0 << 2) / 240) & 0xFFF),
           ((((r->x + r->w) * 4 + -24 * (D_800F13EC * 4) / 512) & 0xFFF) << 12) |
               (((r->y + r->h) * 4 + (fp + img->h - bar) * (D_800F13F0 << 2) / 240) & 0xFFF));
        GW(gdl++, 0xE1000000, ((bar - 1) << 5) & 0xFFFF);
        GW(gdl++, 0xF1000000, ((D_800F13EC == 640) ? 0x03330000 : 0x04000000) | ((D_800F13F0 == 240) ? 0xFC00 : 0xFE00));
        GW(gdl++, 0xE7000000, 0);
    }
    if (D_800E37C0[pl].f654) {
        HudTex *h = hudA_800F0524[0]; GW(gdl++, 0xFA000000, (((h->pal[2] >> 8) & 0xF8) << 24) | (((D_800E2038 << 3) & 0x100) ? ((D_800E2038 << 3) & 0xFF) : (0xFF ^ ((D_800E2038 << 3) & 0xFF))));
            { Rect *tab = D_800C2494; gdl = func_8000A690(RX(tab, D_800E1898) - 24, RY(tab, D_800E1898) + fp, &D_800F0524->img[2], gdl, a2, 1, 1, 6, 0); }
    }
    GW(gdl++, 0xFA000000, (((D_800F0524->pal[1] >> 8) & 0xF8) << 24) | (((D_800F0524->pal[1] >> 3) & 0xF8) << 16) | ((D_800F0524->pal[1] << 10) & 0xF800) | 0x82);
    { Rect *tab = D_800C2494; return func_8000A690(RX(tab, D_800E1898) - 24, RY(tab, D_800E1898) + fp, &D_800F0524->img[1], gdl, a2, 1, 1, 6, 0); }
}
