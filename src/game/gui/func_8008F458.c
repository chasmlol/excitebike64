#include "ctx.h"

typedef struct {
    u8 w;
    u8 h;
} Spr;

typedef struct {
    u8 pad0[4];
    s32 *tex;
    u8 pad8[0x58 - 8];
    s32 *txt[3];
} Hud;

typedef struct {
    u8 pad[0x17FE];
    s16 sel;
} Game;

extern Hud *D_800CE200;
extern Game *D_800CEA40;
extern s32 D_800F13F0;

extern Gfx *func_8000A690(s32 x, s32 y, Spr *s, Gfx *gdl, s32 a, s32 b, s32 c, s32 d, s32 e);
extern s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
extern char *func_800C1F2C(s32 *p, char *s);
extern void func_800C0ACC(s32 *a, s32 b, s32 c);

Gfx *func_8008F458(Gfx *gdl, s32 arg1, s32 arg2, char *s0, char *s1, char *s2, s32 f1, s32 f2)
{
    Spr *spr;
    s32 i;
    s32 k;
    s32 a;
    s32 b;
    s32 x;

    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPLoadTLUT_pal256(gdl++, D_800CE200->tex[1]);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);

    spr = (Spr *)((u8 *)D_800CE200->tex + 0x10);
    for (i = 0; i < 3; i++) {
        if (i == 1) {
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            spr = (Spr *)((u8 *)D_800CE200->tex + 8);
        }
        if (1 - D_800CEA40->sel == i - 1) {
            gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
        } else {
            gDPSetPrimColor(gdl++, 0, 0, 180, 180, 180, 255);
        }
        x = 320 - spr->w * 4;
        b = (i - 1) * spr->h + 120;
        gdl = func_8000A690(x, b - (spr->h >> 1), spr, gdl, arg2, 1, 1, i == 0 ? 3 : 6, 0);
    }

    func_800C1E60(D_800CE200->txt[0], 0x40, 5, 2, 0x21);
    func_800C1E60(D_800CE200->txt[1], 0x40, 5, D_800CEA40->sel == 1 ? 7 : 2, f1 == 0 ? 0x20000021 : 0x21);
    func_800C1E60(D_800CE200->txt[2], 0x40, 5, D_800CEA40->sel == 0 ? 7 : 2, f2 == 0 ? 0x20000021 : 0x21);
    func_800C1F2C(D_800CE200->txt[0], s0);
    func_800C1F2C(D_800CE200->txt[1], s1);
    func_800C1F2C(D_800CE200->txt[2], s2);
    func_800C0ACC(D_800CE200->txt[0], 0x140, D_800F13F0 / 2 - spr->h);
    func_800C0ACC(D_800CE200->txt[1], 0x140, D_800F13F0 / 2);
    func_800C0ACC(D_800CE200->txt[2], 0x140, D_800F13F0 / 2 + spr->h);
    return gdl;
}
