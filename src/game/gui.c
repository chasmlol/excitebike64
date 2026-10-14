/* gui.c: the track editor's dialog boxes and menus, drawn with the "GuiRes" pictures and the menu fonts. */
#include "game.h"

/* The GUI's picture sets (TexSet), with the pictures as Img */
typedef struct {
    s32 count;
    u16 *pal;
    Img imgs[11];
} GuiImgSet;

#define GUI_IMAGES(set) ((GuiImgSet *)(set))

/* gGui->images */
#define GUI_IMG_CHOICE     0 /* a choice of gui_draw_prompt */
#define GUI_IMG_BOX        1 /* gui_draw_message's box, gui_draw_prompt's question */
#define GUI_IMG_MENU_ITEM  2
#define GUI_IMG_MENU_TITLE 3
#define GUI_IMG_MENU_FRAME 5

/* Opens the "GuiRes" set and creates the text objects of gGui. */
void gui_init(void)
{
    s32 i;
    TextObj *text;
    s32 res;

    gGui = mem_alloc_dbg(sizeof(Gui), 0, sStrGuiFile, 0x3E);
    res_open_set(sStrGuiResSet, &gGui->res);
    gGui->unk_18 = 0;
    gGui->unk_0C = 3;
    res = res_load(gGui->res, sGuiCursorImagesResId);
    gGui->cursorImages = (TexSet *)res;
    res = res_load(gGui->res, sGuiImagesResId);
    gGui->images = (TexSet *)res;
    i = 0;
    do {
        text = font_text_create(NULL, 0x20, FONT_SMALL, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
        gGui->menuText[i] = text;
        i++;
    } while (i < GUI_MENU_TEXTS);
    i = 0;
    do {
        text = font_text_create(NULL, 0x20, FONT_SMALL, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
        gGui->unusedText[i] = text;
        i++;
    } while (i < GUI_UNUSED_TEXTS);
    i = 0;
    do {
        text = font_text_create(NULL, 0x40, FONT_SMALL, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
        gGui->dialogText[i] = text;
        i++;
    } while (i < GUI_DIALOG_TEXTS);
    GUI_IMAGES(gGui->cursorImages)->pal[0] = 0; /* colour 0 transparent */
    font_load_menu_set(gResourceSet);
}

/* Draws a one-line message in a box at the centre of the screen. */
Gfx *gui_draw_message(Gfx *gdl, s32 unused, s32 fb, char *msg)
{
    Gfx *ret;
    Img *box;

    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPLoadTLUT_pal256(gdl++, GUI_IMAGES(gGui->images)->pal);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    box = &GUI_IMAGES(gGui->images)->imgs[GUI_IMG_BOX];
    ret = render_draw_image(320 - box->w * 4, 120 - (box->h >> 1), box, gdl, fb, 1, 1, 3, 0);
    font_text_create(gGui->dialogText[0], 0x40, FONT_MENU, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
    font_text_add(gGui->dialogText[0], msg);
    font_draw_text(gGui->dialogText[0], 320, gScreenHeight / 2);
    return ret;
}

/* Draws a question with two choices under it (gEditor->dialogChoice 1: the first); a choice whose enabled flag is
   0 is greyed. */
Gfx *gui_draw_prompt(Gfx *gdl, s32 unused, s32 fb, char *question, char *choice1, char *choice2, s32 enabled1,
                     s32 enabled2)
{
    Img *box;
    s32 i;
    s32 y;
    s32 x;

    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPLoadTLUT_pal256(gdl++, GUI_IMAGES(gGui->images)->pal);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);

    box = &GUI_IMAGES(gGui->images)->imgs[GUI_IMG_BOX];
    for (i = 0; i < 3; i++) {
        /* 0: the question, 1 and 2: the choices */
        if (i == 1) {
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            box = &GUI_IMAGES(gGui->images)->imgs[GUI_IMG_CHOICE];
        }
        if (1 - gEditor->dialogChoice == i - 1) {
            gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
        } else {
            gDPSetPrimColor(gdl++, 0, 0, 180, 180, 180, 255);
        }
        x = 320 - box->w * 4;
        y = (i - 1) * box->h + 120;
        gdl = render_draw_image(x, y - (box->h >> 1), box, gdl, fb, 1, 1, i == 0 ? 3 : 6, 0);
    }

    font_text_create(gGui->dialogText[0], 0x40, FONT_MENU, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
    font_text_create(gGui->dialogText[1], 0x40, FONT_MENU,
                     gEditor->dialogChoice == 1 ? TEXT_COLOR_GREEN : TEXT_COLOR_LIGHT_GREY,
                     enabled1 == 0 ? TEXT_GREYED | TEXT_CENTER : TEXT_CENTER);
    font_text_create(gGui->dialogText[2], 0x40, FONT_MENU,
                     gEditor->dialogChoice == 0 ? TEXT_COLOR_GREEN : TEXT_COLOR_LIGHT_GREY,
                     enabled2 == 0 ? TEXT_GREYED | TEXT_CENTER : TEXT_CENTER);
    font_text_add(gGui->dialogText[0], question);
    font_text_add(gGui->dialogText[1], choice1);
    font_text_add(gGui->dialogText[2], choice2);
    font_draw_text(gGui->dialogText[0], 320, gScreenHeight / 2 - box->h);
    font_draw_text(gGui->dialogText[1], 320, gScreenHeight / 2);
    font_draw_text(gGui->dialogText[2], 320, gScreenHeight / 2 + box->h);
    return gdl;
}

/* A menu's input from gPadInput: up and down move to the next enabled item, left and right pick from the item's
   values. Returns 1 when A chose the item under the cursor (menu->choice). */
s32 gui_menu_input(GuiMenu *menu)
{
    GuiMenuItem *item;
    s32 i;
    s16 cursor;
    s32 numItems;
    s32 value;
    u32 bit;

    if (menu == NULL) {
        return 0;
    }
    if (menu->flags & GUI_MENU_ACTIVE) {
        item = menu->items + menu->cursor;
        if ((gPadInput & INPUT_A) && !(gPadInputPrev & INPUT_A)) {
            menu->choice = menu->cursor;
            snd_play_menu_sound(MENU_SOUND_SELECT);
            return 1;
        }
        if ((gPadInput & INPUT_DPAD_UP) && !(gPadInputPrev & INPUT_DPAD_UP)) {
            i = menu->cursor;
            for (;;) {
                i = i - 1;
                if (i == menu->cursor) {
                    break;
                }
                if (i < 0) {
                    i = menu->numItems - 1;
                }
                if (menu->items[i].flags & GUI_ITEM_ENABLED) {
                    menu->cursor = i;
                    break;
                }
            }
        }
        if ((gPadInput & INPUT_DPAD_DOWN) && !(gPadInputPrev & INPUT_DPAD_DOWN)) {
            i = menu->cursor;
            i = i + 1;
            cursor = menu->cursor;
            if (i != cursor) {
                numItems = menu->numItems;
                do {
                    if (i >= numItems) {
                        i = 0;
                    }
                    if (menu->items[i].flags & GUI_ITEM_ENABLED) {
                        menu->cursor = i;
                        break;
                    }
                    i = i + 1;
                } while (i != cursor);
            }
        }
        if (item->numValues != 0) {
            if ((gPadInput & INPUT_DPAD_LEFT) && !(gPadInputPrev & INPUT_DPAD_LEFT)) {
                i = item->value;
                for (;;) {
                    i = i - 1;
                    if (i < 0) {
                        i = item->numValues - 1;
                    }
                    value = item->value;
                    if (i == value) {
                        break;
                    }
                    bit = (u32)1 << i;
                    if ((~item->valueMask & bit) == 0) {
                        break;
                    }
                }
                if (item->value != i) {
                    item->value = i;
                    snd_play_menu_sound(MENU_SOUND_MOVE);
                }
            }
            if ((gPadInput & INPUT_DPAD_RIGHT) && !(gPadInputPrev & INPUT_DPAD_RIGHT)) {
                i = item->value;
                for (;;) {
                    i = i + 1;
                    if (i >= item->numValues) {
                        i = 0;
                    }
                    value = item->value;
                    if (i == value) {
                        break;
                    }
                    bit = (u32)1 << i;
                    if ((~item->valueMask & bit) == 0) {
                        break;
                    }
                }
                if (item->value != i) {
                    item->value = i;
                    snd_play_menu_sound(MENU_SOUND_MOVE);
                }
            }
        }
    }
    if (menu->choice != menu->cursor) {
        menu->choice = menu->cursor;
        snd_play_menu_sound(MENU_SOUND_MOVE);
    }
    return 0;
}

/* Draws a menu at (x, y): the frame with the title, then an item bar per item. */
Gfx *gui_draw_menu(s32 fb, Gfx *gdl, GuiMenu *menu, s32 x, s32 y)
{
    GuiImgSet *set;
    Img *bar;
    Img *cursor;
    u8 *text;
    s32 i;
    s32 zero;
    s32 textY;

    if (menu == NULL) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPPipeSync(gdl++);
    set = GUI_IMAGES(gGui->images);
    font_text_create(gGui->menuText[0], 0x20, FONT_TITLE, TEXT_COLOR_LIGHT_GREY, TEXT_CENTER);
    font_text_add(gGui->menuText[0], menu->title);
    font_draw_text(gGui->menuText[0], x + set->imgs[GUI_IMG_MENU_TITLE].w * 4, y + 10);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    for (i = 0; i < menu->numItems; i++) {
        text = (u8 *)menu->items[i].text;
        if (!(menu->items[i].flags & GUI_ITEM_ENABLED)) {
            font_text_create(gGui->menuText[i + 1], 0x20, FONT_MENU, TEXT_COLOR_LIGHT_GREY, TEXT_GREYED | TEXT_CENTER);
        } else {
            font_text_create(gGui->menuText[i + 1], 0x20, FONT_MENU,
                             ((menu->flags & GUI_MENU_ACTIVE) && menu->cursor == i) ? TEXT_COLOR_GREEN
                                                                                    : TEXT_COLOR_LIGHT_GREY,
                             TEXT_CENTER);
        }
        zero = 0;
        bar = &GUI_IMAGES(gGui->images)->imgs[GUI_IMG_MENU_ITEM];
        if ((menu->flags & GUI_MENU_ACTIVE) && menu->cursor == i) {
            gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
        } else {
            gDPSetPrimColor(gdl++, 0, 0, 180, 180, 180, 255);
        }
        if (!(menu->items[i].flags & GUI_ITEM_ENABLED)) {
            while (*text == ' ') {
                text++;
            }
        }
        font_text_add(gGui->menuText[i + 1], (char *)text);
        textY = bar->h * (i + 1) + 11;
        /* the "| zero" and the last argument below (always 0) are as the original computes them */
        font_draw_text(gGui->menuText[i + 1], (x + bar->w * 4) | zero, (y + textY) | zero);
        gdl = render_draw_image(x, y + (i + 1) * bar->h, bar, gdl, fb, 1, 1, 6, ((u32)bar & 1) >> 1);
        if (*text == ' ') {
            /* an enabled item starting with a space: the cursor picture in that space */
            gDPPipeSync(gdl++);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
            gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
            gDPLoadTLUT_pal256(gdl++, GUI_IMAGES(gGui->cursorImages)->pal);
            cursor = &GUI_IMAGES(gGui->cursorImages)->imgs[0];
            gDPLoadTextureBlock(gdl++, cursor->addr, G_IM_FMT_CI, G_IM_SIZ_8b, cursor->w * 8, cursor->h, 0,
                                0, 0, 0, 0, 0, 0);
            gDPPipeSync(gdl++);
            gSPTextureRectangle(gdl++, (x + 11) << 2, (y + (i + 1) * bar->h + 6) << 2,
                                (x + 11 + cursor->w * 8) << 2, (y + (i + 1) * bar->h + 6 + (u8)(cursor->h / 3)) << 2,
                                0, 0, (cursor->h - 1) << 5, 1 << 10, -1 << 10);
            gDPPipeSync(gdl++);
            gDPSetCombineMode(gdl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
        }
    }
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gdl++, GUI_IMAGES(gGui->images)->pal);
    return font_render(render_draw_image(x, y, &GUI_IMAGES(gGui->images)->imgs[GUI_IMG_MENU_FRAME], gdl, fb, 1, 1, 3,
                                         0));
}

/* Moves a menu's cursor off a disabled item, then the chosen item's value off a value that cannot be chosen.
   Returns 0 when no item is enabled or the value was moved, else 1. */
s32 gui_menu_fix_cursor(GuiMenu *menu)
{
    s32 i;
    s32 j;
    GuiMenuItem *item;
    GuiMenuItem *chosen;

    item = menu->items;
    if ((item[menu->choice].flags & GUI_ITEM_ENABLED) == 0) {
        for (i = 0; i < menu->numItems; i++, item++) {
            if (item->flags & GUI_ITEM_ENABLED) {
                menu->choice = i;
                menu->cursor = i;
                break;
            }
        }
        if (i == menu->numItems) {
            return 0;
        }
    }

    chosen = &menu->items[menu->choice];
    if (chosen->numValues == 0) {
        goto ret1;
    }
    if (chosen->valueMask & (1 << chosen->value)) {
        goto ret1;
    }
    for (j = 0; j < chosen->numValues; j++) {
        if (chosen->valueMask & (1 << j)) {
            chosen->value = j;
            break;
        }
    }
    if (j < chosen->numValues) {
        return 0;
    }
ret1:
    return 1;
}

/* Frees gGui's text objects and closes its resource set. */
void gui_free(void)
{
    s32 i;

    if (gGui != NULL) {
        i = 0;
        do {
            mem_free(gGui->menuText[i]);
            i++;
        } while (i < GUI_MENU_TEXTS);
        i = 0;
        do {
            mem_free(gGui->unusedText[i]);
            i++;
        } while (i < GUI_UNUSED_TEXTS);
        i = 0;
        do {
            mem_free(gGui->dialogText[i]);
            i++;
        } while (i < GUI_DIALOG_TEXTS);
        res_close_set(gGui->res);
        mem_free(gGui);
        gGui = NULL;
    }
}
