#include "ctx.h"

typedef struct Spr {
    struct Spr *next;
    s16 x;
    s16 y;
    u8 w, h, s, t;
    u8 r, g, b, a;
} Spr;

extern s32 D_800D576C;
extern u8 *D_80105378;
extern s32 D_800D5758;
extern s32 D_800D575C;
extern s32 D_800D5760;
extern s32 D_800D5764;

Gfx *func_800C14F4(Gfx *gdl, Spr **list)
{
    u32 i;
    Spr *s;
    s32 sy;

    for (i = 0; i < 40; i++) {
        if (list[i] != NULL) {
            gDPLoadTextureBlock(gdl++, D_80105378 - i * 0x1000, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
            sy = D_800D575C;
            s = list[i];
            do {
                if ((u16)(s->x + 63) < 703) {
                    gDPSetPrimColor(gdl++, 0, 0, s->r, s->g, s->b, s->a);
                    if (D_800D576C == 0) {
                        gSPTextureRectangle(gdl++,
                            (s->x + (((0x10000 - D_800D5758) * s->w) >> 17)) << 2,
                            (s->y + (((0x10000 - sy) * s->h) >> 17)) << 2,
                            (s->x + (((D_800D5758 + 0x10000) * s->w) >> 17)) << 2,
                            (s->y + (((sy + 0x10000) * s->h) >> 17)) << 2,
                            G_TX_RENDERTILE, s->s << 5, s->t << 5, D_800D5760, D_800D5764);
                    } else {
                        gSPTextureRectangle(gdl++,
                            s->x << 2,
                            s->y << 2,
                            (s->x + ((D_800D5758 * s->w) >> 16)) << 2,
                            (s->y + ((sy * s->h) >> 16)) << 2,
                            G_TX_RENDERTILE, s->s << 5, s->t << 5, D_800D5760, D_800D5764);
                    }
                }
                s = s->next;
            } while (s != NULL);
        }
    }
    return gdl;
}
