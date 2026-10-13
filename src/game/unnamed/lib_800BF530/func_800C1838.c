#include "ctx.h"

typedef struct Node {
    struct Node *next;
    s16 x;
    s16 y;
    u8 w;
    u8 h;
    u8 s;
    u8 t;
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} Node;

extern s32 D_80105378;

#define gDPWord(pkt, a, b)     {         Gfx *_g = (Gfx *)(pkt);         _g->words.w0 = (a);         _g->words.w1 = (b);     }

Gfx *func_800C1838(Gfx *gdl, Node **lists)
{
    s32 base;
    u32 i;
    Node *n;

    base = D_80105378;
    for (i = 0; i < 40; i++) {
        if (lists[i] != NULL) {
            gDPLoadTextureBlock(gdl++, base - i * 0x1000, G_IM_FMT_IA, G_IM_SIZ_8b, 64, 64, 0,
                                G_TX_CLAMP, G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
            n = lists[i];
            do {
                if (n->x >= -63 && n->x < 640) {
                    gDPSetPrimColor(gdl++, 0, 0, n->r, n->g, n->b, n->a);
                    { s32 x = n->x;
                    gDPWord(gdl++, _SHIFTL(0xE4, 24, 8) | _SHIFTL((n->w + x) << 2, 12, 12) | _SHIFTL((n->h + n->y) << 2, 0, 12),
                            _SHIFTL(G_TX_RENDERTILE, 24, 3) | _SHIFTL(n->x << 2, 12, 12) | _SHIFTL(n->y << 2, 0, 12)); }
                    gDPWord(gdl++, _SHIFTL(0xE1, 24, 8), _SHIFTL(n->s << 5, 16, 16) | _SHIFTL(n->t << 5, 0, 16));
                    gDPWord(gdl++, _SHIFTL(0xF1, 24, 8), _SHIFTL(1 << 10, 16, 16) | _SHIFTL(1 << 10, 0, 16));
                }
                n = n->next;
            } while (n != NULL);
        }
    }
    return gdl;
}
