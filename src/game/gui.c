#include "game.h"

void gui_init(void)
{
    s32 i;
    s32 *p;
    s32 r;

    gGui = mem_alloc_dbg(100, 0, D_800DA880, 0x3E);
    res_open_set(D_800DA888, gGui);
    gGui[6] = 0;
    gGui[3] = 3;
    r = res_load((s32 *)gGui[0], D_800D1440);
    gGui[2] = r;
    r = res_load((s32 *)gGui[0], D_800D1408);
    gGui[1] = r;
    i = 0;
    do {
        p = font_text_create(0, 0x20, 2, 2, 0x21);
        { s32 *t = gGui + i; t[9] = (s32)p; }
        i++;
    } while (i < 9);
    i = 0;
    do {
        p = font_text_create(0, 0x20, 2, 2, 0x21);
        { s32 *t = gGui + i; t[18] = (s32)p; }
        i++;
    } while (i < 4);
    i = 0;
    do {
        p = font_text_create(0, 0x40, 2, 2, 0x21);
        { s32 *t = gGui + i; t[22] = (s32)p; }
        i++;
    } while (i < 3);
    *(u16 *)(((s32 *)gGui[2])[1]) = 0;
    font_load_menu_set(D_800E7728);
}

#define PKT(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct {
    u8 pad[4];
    u8 *img;
} Tex_3;

typedef struct {
    u8 pad[4];
    Tex_3 *tex;
    u8 pad2[0x50];
    s32 *obj;
} Res_6;

extern Res_6 *gGui_2;

s32 *gui_draw_message(Gfx *gdl, s32 a1, s32 a2, s32 a3)
{
    s32 *ret;
    u8 *tb;

    PKT(0xE3001001, 0x8000);
    PKT(0xFC119623, 0xFF2FFFFF);
    PKT(0xFD100000, (u32)gGui_2->tex->img);
    PKT(0xE8000000, 0);
    PKT(0xF5000100, 0x07000000);
    PKT(0xE6000000, 0);
    PKT(0xF0000000, 0x073FC000);
    PKT(0xE7000000, 0);
    PKT(0xD7000002, 0x80008000);
    PKT(0xE3000C00, 0);
    PKT(0xE200001C, 0x0C084000);
    tb = (u8 *)gGui_2->tex + 0x10;
    ret = render_draw_image(0x140 - tb[0] * 4, 0x78 - (tb[1] >> 1), tb, gdl, a2, 1, 1, 3, 0);
    font_text_create(gGui_2->obj, 0x40, 5, 2, 0x21);
    font_text_add(gGui_2->obj, a3);
    font_draw_text(gGui_2->obj, 0x140, gScreenHeight / 2);
    return ret;
}

typedef struct {
    u8 w;
    u8 h;
} Spr_3;

typedef struct {
    u8 pad0[4];
    s32 *tex;
    u8 pad8[0x58 - 8];
    s32 *txt[3];
} Hud_5;

typedef struct {
    u8 pad[0x17FE];
    s16 sel;
} Game_34;

extern Hud_5 *gGui_3;
extern Game_34 *gEditor_6;

Gfx *gui_draw_prompt(Gfx *gdl, s32 arg1, s32 arg2, char *s0, char *s1, char *s2, s32 f1, s32 f2)
{
    Spr_3 *spr;
    s32 i;
    s32 k;
    s32 a;
    s32 b;
    s32 x;

    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPLoadTLUT_pal256(gdl++, gGui_3->tex[1]);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);

    spr = (Spr_3 *)((u8 *)gGui_3->tex + 0x10);
    for (i = 0; i < 3; i++) {
        if (i == 1) {
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            spr = (Spr_3 *)((u8 *)gGui_3->tex + 8);
        }
        if (1 - gEditor_6->sel == i - 1) {
            gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
        } else {
            gDPSetPrimColor(gdl++, 0, 0, 180, 180, 180, 255);
        }
        x = 320 - spr->w * 4;
        b = (i - 1) * spr->h + 120;
        gdl = render_draw_image(x, b - (spr->h >> 1), spr, gdl, arg2, 1, 1, i == 0 ? 3 : 6, 0);
    }

    font_text_create(gGui_3->txt[0], 0x40, 5, 2, 0x21);
    font_text_create(gGui_3->txt[1], 0x40, 5, gEditor_6->sel == 1 ? 7 : 2, f1 == 0 ? 0x20000021 : 0x21);
    font_text_create(gGui_3->txt[2], 0x40, 5, gEditor_6->sel == 0 ? 7 : 2, f2 == 0 ? 0x20000021 : 0x21);
    font_text_add(gGui_3->txt[0], s0);
    font_text_add(gGui_3->txt[1], s1);
    font_text_add(gGui_3->txt[2], s2);
    font_draw_text(gGui_3->txt[0], 0x140, gScreenHeight / 2 - spr->h);
    font_draw_text(gGui_3->txt[1], 0x140, gScreenHeight / 2);
    font_draw_text(gGui_3->txt[2], 0x140, gScreenHeight / 2 + spr->h);
    return gdl;
}

s32 gui_menu_input(Ctl_22 *c, s32 arg1) {
    Slot *e;
    s32 i;
    s16 cc;
    s32 n;
    s32 ec;
    u32 bit;

    if (c == NULL) {
        return 0;
    }
    if (c->flags & 1) {
        e = c->ents + c->cur;
        if ((gPadInput & 0x10) && !(gPadInputPrev & 0x10)) {
            c->last = c->cur;
            snd_play_menu_sound(1);
            return 1;
        }
        if ((gPadInput & 0x800) && !(gPadInputPrev & 0x800)) {
            i = c->cur;
            for (;;) {
                i = i - 1;
                if (i == c->cur) {
                    break;
                }
                if (i < 0) {
                    i = c->n - 1;
                }
                if (c->ents[i].flags & 0x10) {
                    c->cur = i;
                    break;
                }
            }
        }
        if ((gPadInput & 0x1000) && !(gPadInputPrev & 0x1000)) {
            i = c->cur;
            i = i + 1;
            cc = c->cur;
            if (i != cc) {
                n = c->n;
                do {
                    if (i >= n) {
                        i = 0;
                    }
                    if (c->ents[i].flags & 0x10) {
                        c->cur = i;
                        break;
                    }
                    i = i + 1;
                } while (i != cc);
            }
        }
        if (e->n != 0) {
            if ((gPadInput & 0x2000) && !(gPadInputPrev & 0x2000)) {
                i = e->cur;
                for (;;) {
                    i = i - 1;
                    if (i < 0) {
                        i = e->n - 1;
                    }
                    ec = e->cur;
                    if (i == ec) {
                        break;
                    }
                    bit = (u32)1 << i;
                    if ((~e->mask & bit) == 0) {
                        break;
                    }
                }
                if (e->cur != i) {
                    e->cur = i;
                    snd_play_menu_sound(0);
                }
            }
            if ((gPadInput & 0x4000) && !(gPadInputPrev & 0x4000)) {
                i = e->cur;
                for (;;) {
                    i = i + 1;
                    if (i >= e->n) {
                        i = 0;
                    }
                    ec = e->cur;
                    if (i == ec) {
                        break;
                    }
                    bit = (u32)1 << i;
                    if ((~e->mask & bit) == 0) {
                        break;
                    }
                }
                if (e->cur != i) {
                    e->cur = i;
                    snd_play_menu_sound(0);
                }
            }
        }
    }
    if (c->last != c->cur) {
        c->last = c->cur;
        snd_play_menu_sound(0);
    }
    return 0;
}

typedef struct {
    u8 pad[4];
    u8 *pal;
    TexInfo imgs[6];
} Tex_4;

typedef struct {
    u8 pad[4];
    Tex_4 *t4;
    Tex_4 *t8;
    u8 pad2[0x18];
    s32 *fonts[8];
} Res_7;

typedef struct {
    u8 *str;
    s32 flags;
    s32 pad[2];
} Item_14;

struct Menu_5 {
    u8 *title;
    s16 count;
    s16 sel;
    Item_14 *items;
    s32 flags;
};

extern Res_7 *gGui_4;

Gfx *gui_draw_menu(s32 arg0, Gfx *gdl, Menu_5 *m, s32 x, s32 y)
{
    Tex_4 *t;
    TexInfo *im;
    TexInfo *ti;
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
    t = gGui_4->t4;
    font_text_create(gGui_4->fonts[0], 0x20, 4, 2, 0x21);
    font_text_add(gGui_4->fonts[0], (char *)m->title);
    font_draw_text(gGui_4->fonts[0], x + t->imgs[3].w * 4, y + 10);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    for (i = 0; i < m->count; i++) {
        s = m->items[i].str;
        if (!(m->items[i].flags & 0x10)) {
            font_text_create(gGui_4->fonts[i + 1], 0x20, 5, 2, 0x20000021);
        } else {
            font_text_create(gGui_4->fonts[i + 1], 0x20, 5, ((m->flags & 1) && m->sel == i) ? 7 : 2, 0x21);
        }
        pad = 0;
        im = &gGui_4->t4->imgs[2];
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
        font_text_add(gGui_4->fonts[i + 1], (char *)s);
        k = im->h * (i + 1) + 11;
        font_draw_text(gGui_4->fonts[i + 1], (x + im->w * 4) | pad, (y + k) | pad);
        gdl = render_draw_image(x, y + (i + 1) * im->h, im, gdl, arg0, 1, 1, 6, ((u32)im & 1) >> 1);
        if (*s == ' ') {
            gDPPipeSync(gdl++);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetRenderMode(gdl++, 0x404240, 0);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, gGui_4->t8->pal);
            ti = &gGui_4->t8->imgs[0];
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
    gDPLoadTLUT_pal256(gdl++, gGui_4->t4->pal);
    return font_render(render_draw_image(x, y, &gGui_4->t4->imgs[5], gdl, arg0, 1, 1, 3, 0));
}

s32 gui_menu_fix_cursor(Ctl_2 *c)
{
    s32 i;
    s32 j;
    Slot *s;
    Slot *t;

    s = c->slots;
    if ((s[c->sel].flags & 0x10) == 0) {
        for (i = 0; i < c->count; i++, s++) {
            if (s->flags & 0x10) {
                c->sel = i;
                c->x = i;
                break;
            }
        }
        if (i == c->count) {
            return 0;
        }
    }

    t = &c->slots[c->sel];
    if (t->n == 0) {
        goto ret1;
    }
    if (t->mask & (1 << t->cur)) {
        goto ret1;
    }
    for (j = 0; j < t->n; j++) {
        if (t->mask & (1 << j)) {
            t->cur = j;
            break;
        }
    }
    if (j < t->n) {
        return 0;
    }
ret1:
    return 1;
}

void gui_free(void)
{
    s32 i;

    if (gGui != 0) {
        i = 0;
        do {
            mem_free(*(s32 *)((u8 *)gGui + (i << 2) + 0x24));
            i++;
        } while (i < 9);
        i = 0;
        do {
            mem_free(*(s32 *)((u8 *)gGui + (i << 2) + 0x48));
            i++;
        } while (i < 4);
        i = 0;
        do {
            mem_free(*(s32 *)((u8 *)gGui + (i << 2) + 0x58));
            i++;
        } while (i < 3);
        res_close_set((s32 *)gGui[0]);
        mem_free((s32)gGui);
        gGui = 0;
    }
}
