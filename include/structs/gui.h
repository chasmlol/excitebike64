/* gui.c: the track editor's dialog kit. gui_init loads the "GuiRes" set and creates the text objects into gGui;
   gui_draw_message draws a one-line box, gui_draw_prompt a question with two choices and gui_draw_menu a menu. */
#ifndef STRUCTS_GUI_H
#define STRUCTS_GUI_H

#include "common.h"
#include "structs/font.h"

typedef struct Gui {
    /* 0x00 */ ResFile *res;             /* the "GuiRes" resource set */
    /* 0x04 */ ImgSet *images;           /* palette, then the boxes and frames of the dialogs and menus */
    /* 0x08 */ ImgSet *cursorImages;     /* palette, then the menu cursor */
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ u8 unk_10[8];
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ u8 unk_1C[8];
    /* 0x24 */ TextObj *menuText[9];     /* gui_draw_menu: the title, then one per item */
    /* 0x48 */ TextObj *unkText[4];
    /* 0x58 */ TextObj *dialogText[3];   /* gui_draw_prompt: the question and the two choices; gui_draw_message: [0] */
} Gui; /* size 0x64 */

/* GuiMenuItem.flags */
#define GUI_ITEM_ENABLED 0x10

typedef struct GuiMenuItem {
    /* 0x00 */ char *text;     /* enabled items starting with a space get the cursorImages picture there */
    /* 0x04 */ u32 flags;      /* GUI_ITEM_ENABLED */
    /* 0x08 */ s16 numValues;  /* 0: no values; else left/right change value */
    /* 0x0A */ s16 value;
    /* 0x0C */ u32 valueMask;  /* bit n: value n can be chosen */
} GuiMenuItem; /* size 0x10 */

/* GuiMenu.flags */
#define GUI_MENU_ACTIVE 1 /* takes input and shows the cursor */

typedef struct GuiMenu {
    /* 0x00 */ char *title;
    /* 0x04 */ s16 numItems;
    /* 0x06 */ s16 cursor;
    /* 0x08 */ GuiMenuItem *items;
    /* 0x0C */ u32 flags;      /* GUI_MENU_ACTIVE */
    /* 0x10 */ s16 choice;     /* the cursor as of the last frame: the chosen item when gui_menu_input returns 1 */
} GuiMenu; /* size 0x14 */

#endif
