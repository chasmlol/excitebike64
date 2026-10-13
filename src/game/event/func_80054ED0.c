#include "ctx.h"

typedef struct { u8 w; u8 h; u8 pad[2]; void *img; } Img;
typedef struct { s32 unk0; u16 *col; Img img[5]; } Hud;
typedef struct { s32 mode; u8 pad[0x22]; s16 t; } State;
extern State D_800F05A8;
extern s32 D_800C8CE0;
extern Hud *D_800EFD60;
extern s32 D_800E1718[][4][4];
extern s32 D_800E1898;

Gfx *func_80054ED0(Gfx *gdl, s32 a)
{
    f32 scale;
    s32 alpha;
    u16 col;
    Img *img;
    s32 sw, dsdx, th;
    s32 *r;

    if (D_800F05A8.mode == 5 && D_800F05A8.t == 0)
        return gdl;
    if (D_800C8CE0 != 0)
        a = 0;
    switch (D_800F05A8.mode) {
    default:
        return gdl;
    case 1: {
        f32 v;
        col = D_800EFD60->col[4];
        img = &D_800EFD60->img[4];
        scale = D_800F05A8.t + 10;
        scale /= 100.0f;
        scale *= 2.0f;
        if (scale > 1.0f)
            v = 100.0f - (scale - 1.0f) * 100.0f;
        else
            v = scale * 100.0f;
        alpha = v;
        if (alpha < 0)
            alpha = 0;
        scale = 2.0f;
        break;
    }
    case 2: {
        f32 v;
        scale = D_800F05A8.t + 10;
        scale /= 46.0f;
        img = &D_800EFD60->img[2];
        v = scale * 100.0f;
        v += 0.0f;
        col = D_800EFD60->col[2];
        scale *= 3.0f;
        alpha = v;
        break;
    }
    case 3: {
        f32 v;
        scale = D_800F05A8.t + 10;
        scale /= 46.0f;
        img = &D_800EFD60->img[1];
        v = scale * 100.0f;
        v += 0.0f;
        col = D_800EFD60->col[1];
        scale *= 3.0f;
        alpha = v;
        break;
    }
    case 4: {
        f32 v;
        scale = D_800F05A8.t + 10;
        scale /= 46.0f;
        img = &D_800EFD60->img[0];
        v = scale * 100.0f;
        v += 0.0f;
        col = D_800EFD60->col[0];
        scale *= 3.0f;
        alpha = v;
        break;
    }
    case 5:
    case 14:
    case 15:
        scale = D_800F05A8.t + 1;
        scale *= 0.0625f;
        img = &D_800EFD60->img[3];
        col = D_800EFD60->col[3];
        alpha = scale * 120.0f;
        scale = 3.5f - scale * 3.0f;
        break;
    }

    sw = scale * (img->w * 8.0f);
    dsdx = 1024.0f / scale;
    if (D_800F05A8.mode == 1)
        th = scale * img->h * 0.5f;
    else
        th = scale * img->h;

    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPSetRenderMode(gdl++, 0x4041C8, 0);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetTextureImage(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, img->w * 8, img->img);
    gDPSetTile(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, img->w, 0, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0);
    gDPLoadSync(gdl++);
    gDPLoadTile(gdl++, G_TX_LOADTILE, 0, 0, (img->w * 8 - 1) << 2, (img->h - 1) << 2);
    gDPPipeSync(gdl++);
    gDPSetTile(gdl++, G_IM_FMT_IA, G_IM_SIZ_8b, img->w, 0, G_TX_RENDERTILE, 0, 0, 0, 0, 0, 0, 0);
    gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, 0, (img->w * 8 - 1) << 2, (img->h - 1) << 2);
    gDPPipeSync(gdl++);
    gDPSetPrimColor(gdl++, 0, 0, (col >> 8) & 0xF8, (col >> 3) & 0xF8, (col << 2) & 0xF8, alpha);
    {
        u8 *t = (u8 *)D_800E1718;
        r = (s32 *)((a << 4) + ((D_800E1898 - 1) << 6) + t);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, r[0], r[1], r[0] + r[2] - 1, r[1] + r[3] - 1);
    gSPTextureRectangle(gdl++, (r[0] + r[2] / 2 - sw / 2) << 2, (r[1] + r[3] / 2 - th / 2) << 2,
                        (r[0] + r[2] / 2 + sw / 2) << 2, (r[1] + r[3] / 2 + th / 2) << 2,
                        G_TX_RENDERTILE, 0, (img->h - 1) << 5, dsdx,
                        D_800F05A8.mode == 1 ? -(dsdx * 2) : -dsdx);
    gDPPipeSync(gdl++);
    return gdl;
}
