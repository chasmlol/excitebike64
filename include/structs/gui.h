/* The menus of gui.c's kit, as the track editor (edit.c) uses them: a menu is a list of items, each either a plain
   choice or a row of values to pick from (the piece lists). */
#ifndef STRUCTS_GUI_H
#define STRUCTS_GUI_H

#include "common.h"

/* GuiMenuItem.flags */
#define GUI_ITEM_ENABLED 0x10

typedef struct GuiMenuItem {
    /* 0x00 */ char *text;
    /* 0x04 */ u32 flags;           /* GUI_ITEM_* */
    /* 0x08 */ s16 count;           /* values of a row (0: a plain item) */
    /* 0x0A */ s16 value;           /* chosen value */
    /* 0x0C */ u32 mask;            /* values that can be chosen (bit n: value n) */
} GuiMenuItem; /* size 0x10 */

/* GuiMenu.flags */
#define GUI_MENU_ACTIVE 1           /* takes input (cleared while a dialog box is up) */

typedef struct GuiMenu {
    /* 0x00 */ char *title;
    /* 0x04 */ s16 count;
    /* 0x06 */ s16 cursor;          /* set with sel when the editor moves the cursor itself */
    /* 0x08 */ GuiMenuItem *items;
    /* 0x0C */ u32 flags;           /* GUI_MENU_* */
    /* 0x10 */ s16 sel;             /* the chosen item after gui_menu_input */
} GuiMenu; /* size 0x14 */

#endif
