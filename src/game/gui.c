#include "game.h"

/* The GUI's picture sets (TexSet), with the pictures as Img */
typedef struct {
    s32 count;
    u16 *pal;
    Img imgs[11];
} GuiImgSet;

#define GUI_IMAGES(set) ((GuiImgSet *)(set))

void gui_init(void)
{
    s32 i;
    TextObj *p;
    s32 r;

    gGui = mem_alloc_dbg(100, 0, sStrGuiFile, 0x3E);
    res_open_set(sStrGuiResSet, &gGui->res);
    gGui->unk_18 = 0;
    gGui->unk_0C = 3;
    r = res_load(gGui->res, sGuiCursorImagesResId);
    gGui->cursorImages = (TexSet *)r;
    r = res_load(gGui->res, sGuiImagesResId);
    gGui->images = (TexSet *)r;
    i = 0;
    do {
        p = font_text_create(0, 0x20, 2, 2, 0x21);
        gGui->menuText[i] = p;
        i++;
    } while (i < 9);
    i = 0;
    do {
        p = font_text_create(0, 0x20, 2, 2, 0x21);
        gGui->unkText[i] = p;
        i++;
    } while (i < 4);
    i = 0;
    do {
        p = font_text_create(0, 0x40, 2, 2, 0x21);
        gGui->dialogText[i] = p;
        i++;
    } while (i < 3);
    GUI_IMAGES(gGui->cursorImages)->pal[0] = 0;
    font_load_menu_set(gResourceSet);
}

#define PKT(a, b) { Gfx *_g = (Gfx *)(gdl++); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *gui_draw_message(Gfx *gdl, s32 a1, s32 a2, char *msg)
{
    Gfx *ret;
    Img *tb;

    PKT(0xE3001001, 0x8000);
    PKT(0xFC119623, 0xFF2FFFFF);
    PKT(0xFD100000, (u32)GUI_IMAGES(gGui->images)->pal);
    PKT(0xE8000000, 0);
    PKT(0xF5000100, 0x07000000);
    PKT(0xE6000000, 0);
    PKT(0xF0000000, 0x073FC000);
    PKT(0xE7000000, 0);
    PKT(0xD7000002, 0x80008000);
    PKT(0xE3000C00, 0);
    PKT(0xE200001C, 0x0C084000);
    tb = &GUI_IMAGES(gGui->images)->imgs[1];
    ret = render_draw_image(0x140 - tb->w * 4, 0x78 - (tb->h >> 1), tb, gdl, a2, 1, 1, 3, 0);
    font_text_create(gGui->dialogText[0], 0x40, 5, 2, 0x21);
    font_text_add(gGui->dialogText[0], msg);
    font_draw_text(gGui->dialogText[0], 0x140, gScreenHeight / 2);
    return ret;
}

Gfx *gui_draw_prompt(Gfx *gdl, s32 arg1, s32 arg2, char *s0, char *s1, char *s2, s32 f1, s32 f2)
{
    Img *spr;
    s32 i;
    s32 k;
    s32 a;
    s32 b;
    s32 x;

    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPLoadTLUT_pal256(gdl++, GUI_IMAGES(gGui->images)->pal);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetRenderMode(gdl++, 0x0C084000, 0);

    spr = &GUI_IMAGES(gGui->images)->imgs[1];
    for (i = 0; i < 3; i++) {
        if (i == 1) {
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            spr = &GUI_IMAGES(gGui->images)->imgs[0];
        }
        if (1 - gEditor->dialogChoice == i - 1) {
            gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
        } else {
            gDPSetPrimColor(gdl++, 0, 0, 180, 180, 180, 255);
        }
        x = 320 - spr->w * 4;
        b = (i - 1) * spr->h + 120;
        gdl = render_draw_image(x, b - (spr->h >> 1), spr, gdl, arg2, 1, 1, i == 0 ? 3 : 6, 0);
    }

    font_text_create(gGui->dialogText[0], 0x40, 5, 2, 0x21);
    font_text_create(gGui->dialogText[1], 0x40, 5, gEditor->dialogChoice == 1 ? 7 : 2, f1 == 0 ? 0x20000021 : 0x21);
    font_text_create(gGui->dialogText[2], 0x40, 5, gEditor->dialogChoice == 0 ? 7 : 2, f2 == 0 ? 0x20000021 : 0x21);
    font_text_add(gGui->dialogText[0], s0);
    font_text_add(gGui->dialogText[1], s1);
    font_text_add(gGui->dialogText[2], s2);
    font_draw_text(gGui->dialogText[0], 0x140, gScreenHeight / 2 - spr->h);
    font_draw_text(gGui->dialogText[1], 0x140, gScreenHeight / 2);
    font_draw_text(gGui->dialogText[2], 0x140, gScreenHeight / 2 + spr->h);
    return gdl;
}

s32 gui_menu_input(GuiMenu *c) {
    GuiMenuItem *e;
    s32 i;
    s16 cc;
    s32 n;
    s32 ec;
    u32 bit;

    if (c == NULL) {
        return 0;
    }
    if (c->flags & GUI_MENU_ACTIVE) {
        e = c->items + c->cursor;
        if ((gPadInput & 0x10) && !(gPadInputPrev & 0x10)) {
            c->choice = c->cursor;
            snd_play_menu_sound(1);
            return 1;
        }
        if ((gPadInput & 0x800) && !(gPadInputPrev & 0x800)) {
            i = c->cursor;
            for (;;) {
                i = i - 1;
                if (i == c->cursor) {
                    break;
                }
                if (i < 0) {
                    i = c->numItems - 1;
                }
                if (c->items[i].flags & GUI_ITEM_ENABLED) {
                    c->cursor = i;
                    break;
                }
            }
        }
        if ((gPadInput & 0x1000) && !(gPadInputPrev & 0x1000)) {
            i = c->cursor;
            i = i + 1;
            cc = c->cursor;
            if (i != cc) {
                n = c->numItems;
                do {
                    if (i >= n) {
                        i = 0;
                    }
                    if (c->items[i].flags & GUI_ITEM_ENABLED) {
                        c->cursor = i;
                        break;
                    }
                    i = i + 1;
                } while (i != cc);
            }
        }
        if (e->numValues != 0) {
            if ((gPadInput & 0x2000) && !(gPadInputPrev & 0x2000)) {
                i = e->value;
                for (;;) {
                    i = i - 1;
                    if (i < 0) {
                        i = e->numValues - 1;
                    }
                    ec = e->value;
                    if (i == ec) {
                        break;
                    }
                    bit = (u32)1 << i;
                    if ((~e->valueMask & bit) == 0) {
                        break;
                    }
                }
                if (e->value != i) {
                    e->value = i;
                    snd_play_menu_sound(0);
                }
            }
            if ((gPadInput & 0x4000) && !(gPadInputPrev & 0x4000)) {
                i = e->value;
                for (;;) {
                    i = i + 1;
                    if (i >= e->numValues) {
                        i = 0;
                    }
                    ec = e->value;
                    if (i == ec) {
                        break;
                    }
                    bit = (u32)1 << i;
                    if ((~e->valueMask & bit) == 0) {
                        break;
                    }
                }
                if (e->value != i) {
                    e->value = i;
                    snd_play_menu_sound(0);
                }
            }
        }
    }
    if (c->choice != c->cursor) {
        c->choice = c->cursor;
        snd_play_menu_sound(0);
    }
    return 0;
}

Gfx *gui_draw_menu(s32 arg0, Gfx *gdl, GuiMenu *m, s32 x, s32 y)
{
    GuiImgSet *t;
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
    t = GUI_IMAGES(gGui->images);
    font_text_create(gGui->menuText[0], 0x20, 4, 2, 0x21);
    font_text_add(gGui->menuText[0], m->title);
    font_draw_text(gGui->menuText[0], x + t->imgs[3].w * 4, y + 10);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    for (i = 0; i < m->numItems; i++) {
        s = (u8 *)m->items[i].text;
        if (!(m->items[i].flags & GUI_ITEM_ENABLED)) {
            font_text_create(gGui->menuText[i + 1], 0x20, 5, 2, 0x20000021);
        } else {
            font_text_create(gGui->menuText[i + 1], 0x20, 5, ((m->flags & GUI_MENU_ACTIVE) && m->cursor == i) ? 7 : 2, 0x21);
        }
        pad = 0;
        im = &GUI_IMAGES(gGui->images)->imgs[2];
        if ((m->flags & GUI_MENU_ACTIVE) && m->cursor == i) {
            gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
        } else {
            gDPSetPrimColor(gdl++, 0, 0, 180, 180, 180, 255);
        }
        if (!(m->items[i].flags & GUI_ITEM_ENABLED)) {
            while (*s == ' ') {
                s++;
            }
        }
        font_text_add(gGui->menuText[i + 1], (char *)s);
        k = im->h * (i + 1) + 11;
        font_draw_text(gGui->menuText[i + 1], (x + im->w * 4) | pad, (y + k) | pad);
        gdl = render_draw_image(x, y + (i + 1) * im->h, im, gdl, arg0, 1, 1, 6, ((u32)im & 1) >> 1);
        if (*s == ' ') {
            gDPPipeSync(gdl++);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetRenderMode(gdl++, 0x404240, 0);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, GUI_IMAGES(gGui->cursorImages)->pal);
            ti = &GUI_IMAGES(gGui->cursorImages)->imgs[0];
            gDPLoadTextureBlock(gdl++, ti->addr, G_IM_FMT_CI, G_IM_SIZ_8b, ti->w * 8, ti->h, 0,
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
    gDPLoadTLUT_pal256(gdl++, GUI_IMAGES(gGui->images)->pal);
    return font_render(render_draw_image(x, y, &GUI_IMAGES(gGui->images)->imgs[5], gdl, arg0, 1, 1, 3, 0));
}

s32 gui_menu_fix_cursor(GuiMenu *c)
{
    s32 i;
    s32 j;
    GuiMenuItem *s;
    GuiMenuItem *t;

    s = c->items;
    if ((s[c->choice].flags & GUI_ITEM_ENABLED) == 0) {
        for (i = 0; i < c->numItems; i++, s++) {
            if (s->flags & GUI_ITEM_ENABLED) {
                c->choice = i;
                c->cursor = i;
                break;
            }
        }
        if (i == c->numItems) {
            return 0;
        }
    }

    t = &c->items[c->choice];
    if (t->numValues == 0) {
        goto ret1;
    }
    if (t->valueMask & (1 << t->value)) {
        goto ret1;
    }
    for (j = 0; j < t->numValues; j++) {
        if (t->valueMask & (1 << j)) {
            t->value = j;
            break;
        }
    }
    if (j < t->numValues) {
        return 0;
    }
ret1:
    return 1;
}

void gui_free(void)
{
    s32 i;

    if (gGui != NULL) {
        i = 0;
        do {
            mem_free(gGui->menuText[i]);
            i++;
        } while (i < 9);
        i = 0;
        do {
            mem_free(gGui->unkText[i]);
            i++;
        } while (i < 4);
        i = 0;
        do {
            mem_free(gGui->dialogText[i]);
            i++;
        } while (i < 3);
        res_close_set(gGui->res);
        mem_free(gGui);
        gGui = NULL;
    }
}
