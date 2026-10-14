/* pak.c: the Controller Pak screen (list, load, save and erase notes, repair a damaged pak), the Pfs calls behind
   it, and the probing of what is plugged into the controllers. */
#include "game.h"

/* a note's name and extension, copied as blocks */
typedef struct { char c[16]; } Name16;
typedef struct { char c[4]; } Ext4;

/* The screen's picture sets (resources of gGuiRes) */
typedef struct {
    u32 count;
    u32 pal;
    Img imgs[1];
} PakImgSet;

extern PakImgSet *sPakButtonIcons; /* the icons of the buttons in the button hints: A, B, ... */

#define RUMBLE_STOP 0 /* __osMotorAccess: stop the motor (MOTOR_STOP of later libultra headers) */
#define PAK_WAIT_FRAMES 30 /* a working state's message stays up this long before the work starts */
#define PAK_PAGES(bytes) (((bytes) + 0xFF) >> 8)

/* Builds the list of the notes the screen shows of a pak (sPakNoteList), labelled by the describe callback;
   withNewNote: first the "create new note" entry. Loading hides the notes of other games and of the other kind. */
void pak_build_note_list(s32 pak, s32 withNewNote)
{
    s32 i;
    s32 status;
    s32 slot;
    PakNote *note;

    sPakNoteCount[pak] = 0;
    if (withNewNote != 0) {
        sPakNoteList[pak][0] = &sPakNewNoteEntry;
        sPakNoteCount[pak] = 1;
    }
    for (i = 0; i < PAK_MAX_NOTES; i++) {
        note = &sPakNoteTables[pak][i];
        if (note->fileNo != -1) {
            if (note->gameCode != PAK_GAME_CODE) {
                status = PAK_NOTE_FOREIGN;
            } else {
                status = gPakScreen.describe(note->header, sPakStrBuf, gPakScreen.mode == PAK_MODE_MANAGER);
                if (status == PAK_NOTE_HIDDEN) {
                    if (gPakScreen.mode == PAK_MODE_LOAD) {
                        continue;
                    }
                    status = PAK_NOTE_FOREIGN;
                }
            }
            if (status != PAK_NOTE_FOREIGN || gPakScreen.mode != PAK_MODE_LOAD) {
                mem_fill((u8 *)note->label, 0, sizeof(note->label));
                *(Name16 *)note->label = *(Name16 *)note->name;
                if (note->ext[0] != 0) {
                    str_cat(note->label, sStrPakExtDot);
                    *(Ext4 *)&note->label[strlen(note->label)] = *(Ext4 *)note->ext;
                }
                if (status != PAK_NOTE_FOREIGN) {
                    str_cat(note->label, sStrPakLabelSep);
                    str_cat(note->label, sPakStrBuf);
                }
                note->status = status;
                slot = pak * 17; slot += sPakNoteCount[pak];
                sPakNoteList[0][slot] = note;   /* sPakNoteList[pak][sPakNoteCount[pak]] */
                sPakNoteCount[pak]++;
            }
        }
    }
}

/* Draws a text; in FONT_SMALL its '@'s become character 0x7F (in place). */
static inline Gfx *draw_text(Gfx *gdl, u8 *str, s32 font, s32 color, s32 x, s32 y)
{
    s32 i;

    if (font == FONT_SMALL) {
        for (i = 0; str[i] != 0; i++) {
            if (str[i] == '@') {
                str[i] = 0x7F;
            }
        }
    }
    font_text_create(sPakText, 0x80, font, color, 0);
    font_text_add(sPakText, (char *)str);
    font_draw_text(sPakText, x, y);
    gdl = font_render(gdl);
    return gdl;
}

/* Draws a button hint ("@A OK    @B Cancel": @X is the icon of button X) from (x, y). With gdl NULL it only
   measures, and returns the x where the text ends. */
Gfx *pak_draw_button_text(Gfx *gdl, u8 *str, s32 font, s32 color, s32 x, s32 y)
{
    s32 len = 0;
    s32 button;
    s32 i;

    if (*str != 0) {
        i = 0;
        do {
            if (str[i] != '@') {
                sPakStrBuf[len] = str[i];
                i++;
                len++;
            } else {
                /* the text so far, then the icon */
                sPakStrBuf[len] = 0;
                if (len != 0) {
                    if (gdl != NULL) {
                        gdl = draw_text(gdl, sPakStrBuf, font, color, x, y);
                    } else {
                        font_text_create(sPakText, 0x80, font, color, 0);
                        font_text_add(sPakText, (char *)sPakStrBuf);
                    }
                    x += sPakText->w;
                }
                i++;
                len = 0;
                button = str[i] - 'A';
                if (gdl != NULL) {
                    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                    gDPLoadTLUT_pal256(gdl++, sPakButtonIcons->pal);
                    {
                        Gfx *g = gdl++;
                        g->words.w0 = 0xFCFFFFFF; /* gDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA) */
                        g->words.w1 = 0xFFFCF279;
                    }
                    gdl = render_draw_image(x, y - 4, &sPakButtonIcons->imgs[button], gdl, gGfxFrameIndex, 1, 1, 3, 0);
                }
                i++;
                x += sPakButtonIcons->imgs[button].w * 8;
            }
        } while (str[i] != 0);
    }
    sPakStrBuf[len] = 0;
    if (len != 0) {
        if (gdl != NULL) {
            gdl = draw_text(gdl, sPakStrBuf, font, color, x, y);
        } else {
            font_text_create(sPakText, 0x80, font, color, 0);
            font_text_add(sPakText, (char *)sPakStrBuf);
        }
        x += sPakText->w;
    }
    if (gdl != NULL) {
        return gdl;
    }
    return (Gfx *)x;
}

/* Starts the screen: the states, the notes of each pak, the viewport and the pictures. gPakScreen's mode, data,
   pages and callbacks are set by the front end first. */
void pak_screen_init(void)
{
    s32 *res;
    s32 scaleW, scaleH, transW, transH, leftW, rightW, bottomH, topH;
    f32 left, right, bottom, top;

    sPakRepairDeclined = 0;
    sPakPlugMask = 0;
    sPakValidMask = 0;
    sPakFrameCounter = -1;
    sPakOverwriteExt = -1;
    gPakScreenDone = 0;
    D_800F483C = -1;
    sPakWaitTimer = 0;
    gPakScreen.state = PAK_STATE_SELECT_PAK;
    gPakScreen.subState = PAK_STEP_START;
    gPakScreen.pak = 0;
    gPakScreen.sel = 0;
    gPakScreen.unk_14 = 0;
    gPakScreen.scroll = 0;
    res = &sPakGuiRes;
    *res = gGuiRes;
    pak_alloc_note_tables();
    /* volatile: each read of the screen size is a load of its own */
    scaleW = *(volatile s32 *)&gScreenWidth;
    scaleH = *(volatile s32 *)&gScreenHeight;
    transW = *(volatile s32 *)&gScreenWidth;
    transH = *(volatile s32 *)&gScreenHeight;
    leftW = *(volatile s32 *)&gScreenWidth;
    left = (f32)-(leftW / 2);
    rightW = *(volatile s32 *)&gScreenWidth;
    right = (f32)(rightW / 2);
    bottomH = *(volatile s32 *)&gScreenHeight;
    bottom = (f32)-(bottomH / 2);
    topH = *(volatile s32 *)&gScreenHeight;
    top = (f32)(topH / 2);
    sPakViewport[0] = (s16)(scaleW << 1);
    sPakViewport[1] = (s16)(scaleH << 1);
    sPakViewport[4] = (s16)(transW << 1);
    sPakViewport[5] = (s16)(transH << 1);
    guOrtho((s32 *)sPakOrthoMtx, left, right, bottom, top, 50.0f, 16384.0f, 1.0f);
    sPakIconTex = res_load(*res, gControllerIconResId);
    sPakBgTex = res_load(*res, gMenuBackgroundResId);
    sPakUiTex = res_load(*res, sPakUiTexResId);
    sPakLogoTex = res_load(*res, gTitleBarResId);
    sPakButtonIcons = res_load(*res, sPakButtonIconsResId);
    sPakText = font_text_create(NULL, 0x80, FONT_MAIN, TEXT_COLOR_YELLOW, 0);
}

extern PakImgSet *sPakBgTex_2;
extern PakImgSet *sPakUiTex_2;   /* [0] the frame's edge, [1] [2] the list's panels, [3] the title bar's */
extern PakImgSet *sPakIconTex_2; /* the controller icon, animated */
extern PakImgSet *sPakLogoTex_2;

/* draw_text again: the screen's drawing has its own copies */
static inline Gfx *draw_text_2(Gfx *gdl, u8 *str, s32 font, s32 color, s32 x, s32 y)
{
    s32 i;

    if (font == FONT_SMALL) {
        for (i = 0; str[i] != 0; i++) {
            if (str[i] == '@') {
                str[i] = 0x7F;
            }
        }
    }
    font_text_create(sPakText, 0x80, font, color, 0);
    font_text_add(sPakText, (char *)str);
    font_draw_text(sPakText, x, y);
    gdl = font_render(gdl);
    return gdl;
}

/* Draws a text centred on centerX. */
static inline Gfx *draw_text_centered(Gfx *gdl, char *str, s32 font, s32 color, s32 centerX, s32 y)
{
    font_text_create(sPakText, 0x80, font, color, 0);
    font_text_add(sPakText, str);
    font_draw_text(sPakText, centerX - (sPakText->w >> 1), y);
    return font_render(gdl);
}

/* Draws the screen: the paks plugged in, the selected pak's notes, and the state's message box or line with its
   button hints. */
void pak_screen_draw(s32 unused, s32 fb, Gfx **pgdl)
{
    Gfx *gdl;
    s32 c;
    s32 i;
    s32 j;
    s32 x;
    s32 y;
    u32 pages;
    PakNote *note;
    char **lines;
    s32 w;
    s32 boxBottom;

    gdl = *pgdl;
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    font_reset_queue();
    gDPLoadTLUT_pal256(gdl++, sPakBgTex_2->pal);
    gdl = render_draw_image(0, 0, &sPakBgTex_2->imgs[0], gdl, fb, 1, 1, 3, 0);
    gDPLoadTLUT_pal256(gdl++, sPakLogoTex_2->pal);
    gdl = render_draw_image(0x4E, 0x16, &sPakLogoTex_2->imgs[0], gdl, fb, 1, 1, 3, 0);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gdl = render_draw_image(0x4E, 0x13, &sPakUiTex_2->imgs[3], gdl, fb, 1, 1, 6, 0);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    /* the list's panel is dimmed while a pak is chosen, the paks' panel otherwise */
    c = 0xFF;
    if (gPakScreen.state == PAK_STATE_SELECT_PAK) {
        c = 0x80;
    }
    gDPSetPrimColor(gdl++, 0, 0, c, c, c, 255);
    gdl = render_draw_image(0x2B, 0x32, &sPakUiTex_2->imgs[2], gdl, fb, 1, 1, 6, 0);
    c ^= 0x7F;
    gDPSetPrimColor(gdl++, 0, 0, c, c, c, 255);
    gdl = render_draw_image(0x28, 0x95, &sPakUiTex_2->imgs[1], gdl, fb, 1, 1, 6, 0);
    gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    for (j = 0; j < 0x200; j += 0x10) {
        gdl = render_draw_image(j, 0xB4, &sPakUiTex_2->imgs[0], gdl, fb, 1, 1, 6, 0);
    }
    /* a controller icon per controller, dim without a pak */
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPLoadTLUT_pal256(gdl++, sPakIconTex_2->pal);
    gDPSetCombineLERP(gdl++, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0);
    for (i = 0; i < MAXCONTROLLERS; i++) {
        if ((gContMask >> i) & 1) {
            c = 0x80;
            if ((sPakValidMask | sPakBadMask) & (1 << i)) {
                c = 0xFF;
            }
            gDPSetColor(gdl++, G_SETPRIMCOLOR, c | 0xFFFFFF00);
            gdl = render_draw_image(0x37 + i * 100, 0x9C,
                                    &sPakIconTex_2->imgs[(u32)(sPakFrameCounter / 2) % sPakIconTex_2->count], gdl,
                                    fb, 1, 1, 3, 0);
        }
    }
    gdl = draw_text_2(gdl, (u8 *)sPakTitles[gPakScreen.mode], FONT_MAIN, TEXT_COLOR_LIGHT_GREY, 0x62, 0x1D);
    __builtin_strcpy((char *)sPakStrBuf, "Pak  ");
    for (i = 0; i < MAXCONTROLLERS; i++) {
        if ((sPakValidMask | sPakBadMask) & (1 << i)) {
            sPakStrBuf[4] = i + '1';
            gdl = draw_text_2(gdl, sPakStrBuf, FONT_MAIN,
                              (i == gPakScreen.pak) ? TEXT_COLOR_YELLOW : TEXT_COLOR_LIGHT_GREY, 0x55 + i * 100, 0x9F);
        }
    }
    if (sPakValidMask & (1 << gPakScreen.pak)) {
        /* the notes: label, and size in pages */
        for (i = 0; i < sPakNoteCount[gPakScreen.pak]; i++) {
            c = i - gPakScreen.scroll;
            if (c >= 0) if (c < PAK_LIST_ROWS) {
                note = sPakNoteList[gPakScreen.pak][i];
                x = 0x58 - c * 5 / 2;
                y = c * 10 + 0x3B;
                str_copy((char *)sPakStrBuf, note->label);
                if (gPakScreen.state == PAK_STATE_SELECT_PAK) {
                    c = TEXT_COLOR_CYAN;
                } else if (i == gPakScreen.sel) {
                    c = TEXT_COLOR_BRIGHT_YELLOW;
                } else if (note == &sPakNewNoteEntry) {
                    c = TEXT_COLOR_GREEN;
                    w = 0;
                } else {
                    c = TEXT_COLOR_LIGHT_GREY;
                }
                gdl = draw_text_2(gdl, sPakStrBuf, FONT_SMALL, c, x, y);
                if (note != &sPakNewNoteEntry) {
                    pages = note->size;
                    pages += 0xFF;
                    pages >>= 8;
                    int_to_str(pages, (char *)sPakStrBuf);
                    font_text_create(sPakText, 0x80, FONT_SMALL, c, 0);
                    font_text_add(sPakText, (char *)sPakStrBuf);
                    font_draw_text(sPakText, x + (0x140 - sPakText->w), y);
                    gdl = font_render(gdl);
                    __builtin_strcpy((char *)sPakStrBuf, "page");
                    if (pages != 1) {
                        str_cat((char *)sPakStrBuf, "s");
                    }
                    gdl = draw_text_2(gdl, sPakStrBuf, FONT_SMALL, c, x + 0x14A, y);
                }
            }
        }
    } else if (sPakBadMask & (1 << gPakScreen.pak)) {
        gdl = draw_text_centered(gdl, "Damaged or Corrupted Controller Pak!", FONT_SMALL, TEXT_COLOR_CYAN, 0x100, 0x5C);
    }
    if (gPakScreen.mode == PAK_MODE_SAVE || gPakScreen.mode == PAK_MODE_MANAGER) {
        if (sPakValidMask & (1 << gPakScreen.pak)) {
            int_to_str(sPakFreePages[gPakScreen.pak], (char *)sPakStrBuf);
            str_cat((char *)sPakStrBuf, " page");
            if (sPakFreePages[gPakScreen.pak] != 1) {
                str_cat((char *)sPakStrBuf, "s");
            }
            str_cat((char *)sPakStrBuf, " free");
            gdl = draw_text_centered(gdl, (char *)sPakStrBuf, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, 0x18B, 0x1E);
        }
    }
    if (sPakMessages[gPakScreen.state].lines != NULL) {
        /* the message box: darken the screen, a band for the lines with edges top and bottom */
        lines = sPakMessages[gPakScreen.state].lines;
        gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_NOOP2);
        gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xC0);
        gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, 0xB4);
        gDPPipeSync(gdl++);
        for (i = 0; lines[i] != NULL; i++) {
        }
        boxBottom = i * 14 + 0x50;
        gDPFillRectangle(gdl++, 0, 0x4C, 0x200, boxBottom);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPLoadTextureBlock(gdl++, sPakUiTex_2->imgs[0].addr, G_IM_FMT_IA, G_IM_SIZ_8b, sPakUiTex_2->imgs[0].w * 8,
                            sPakUiTex_2->imgs[0].h, 0, G_TX_CLAMP, G_TX_CLAMP, 4, 8, G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(gdl++, 0, 0x114, 0x800, 0x130, 0, 0, 0, 0x400, 0x400);
        gSPTextureRectangle(gdl++, 0, (boxBottom - 1) * 4, 0x800, (boxBottom + 7) * 4, 0, 0, 0x5C0, 0x400, 0x400);
        gDPPipeSync(gdl++);
        for (i = 0; lines[i] != NULL; i++) {
            gdl = draw_text_centered(gdl, lines[i], FONT_MAIN, TEXT_COLOR_LIGHT_GREY, 0x100, 0x53 + i * 14);
        }
        if (sPakMessages[gPakScreen.state].buttons != NULL) {
            w = (s32)pak_draw_button_text(NULL, (u8 *)sPakMessages[gPakScreen.state].buttons, FONT_MAIN,
                                          TEXT_COLOR_YELLOW, 0, 0);
            gdl = pak_draw_button_text(gdl, (u8 *)sPakMessages[gPakScreen.state].buttons, FONT_MAIN,
                                       TEXT_COLOR_YELLOW, 0x100 - w / 2, 0xC6);
        }
    } else if (sPakMessages[gPakScreen.state].text != NULL) {
        gdl = draw_text_centered(gdl, sPakMessages[gPakScreen.state].text, FONT_MAIN, TEXT_COLOR_LIGHT_GREY, 0x100,
                                 0xBE);
        if (sPakMessages[gPakScreen.state].buttons != NULL) {
            w = (s32)pak_draw_button_text(NULL, (u8 *)sPakMessages[gPakScreen.state].buttons, FONT_MAIN,
                                          TEXT_COLOR_YELLOW, 0, 0);
            gdl = pak_draw_button_text(gdl, (u8 *)sPakMessages[gPakScreen.state].buttons, FONT_MAIN,
                                       TEXT_COLOR_YELLOW, 0x100 - w / 2, 0xCE);
        }
    }
    if (sPakWaitTimer != 0) {
        sPakWaitTimer--;
    }
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}

/* Reads a pak's notes and free pages again (3 tries each; failing marks the pak bad) and rebuilds its list. */
void pak_refresh(s32 pak)
{
    s32 tries;
    s32 bytes;

    tries = 3;
    do {
        if (pak_read_note_states(pak) == 0) {
            break;
        }
        tries = tries - 1;
    } while (tries != 0);
    if (tries == 0) {
        sPakBadMask = sPakBadMask | (1 << pak);
        sPakValidMask = sPakValidMask & ~(1 << pak);
    }
    tries = 3;
    do {
        bytes = pak_get_free_bytes(pak);
        if (bytes < 0) {
            bytes = bytes + 0xFF;
        }
        sPakFreePages[pak] = bytes >> 8;
    } while ((bytes >> 8) < 0 && (tries = tries - 1) != 0);
    if (tries == 0) {
        sPakBadMask = sPakBadMask | (1 << pak);
        sPakValidMask = sPakValidMask & ~(1 << pak);
    }
    pak_build_note_list(pak, 0);
}

/* 16 entries of a pak's note list, copied one down at once */
typedef struct {
    PakNote *e[16];
} PakNoteRow;

/* Goes to a state, with a menu sound. */
static inline void set_state(s32 state, s32 sound)
{
    gPakScreen.state = state;
    gPakScreen.subState = PAK_STEP_START;
    sPakSound = sound;
}

/* The menu sound to play at the end of the frame. */
static inline void set_sound(s32 sound)
{
    sPakSound = sound;
}

/* Goes to a state without a sound. */
static inline void set_state_silent(s32 state)
{
    gPakScreen.state = state;
    gPakScreen.subState = PAK_STEP_START;
}

/* Takes the "create new note" entry off the head of a pak's list. */
static inline void drop_new_note_entry(s32 pak)
{
    if (sPakNoteList[pak][0] == &sPakNewNoteEntry && sPakNoteCount[pak] > 0) {
        *(PakNoteRow *)&sPakNoteList[pak][0] = *(PakNoteRow *)&sPakNoteList[pak][1];
        sPakNoteCount[pak]--;
    }
}

/* The screen's frame: watches the paks being plugged and unplugged, then runs the state's input and work. */
void pak_screen_update(Pad16 *pads)
{
    u8 plugged[8];
    u32 buttons;
    s32 i;
    s32 n;
    s32 j;
    s32 count;
    PakNote *note;

    if (gPakScreenDone != 0) {
        return;
    }
    sPakSound = -1;
    sPakMessages[PAK_STATE_SELECT_NOTE].text = sPakModePrompts[gPakScreen.mode];
    if (gPakScreen.mode == PAK_MODE_MANAGER) {
        sPakMessages[PAK_STATE_NO_NOTES].lines = sPakEmptyLines;
    } else if (gGame.pakScreenMode == PAK_SCREEN_LOAD_GHOST) {
        sPakMessages[PAK_STATE_NO_NOTES].lines = sPakNoGhostLines;
    } else {
        sPakMessages[PAK_STATE_NO_NOTES].lines = sPakNoTrackLines;
    }
    if (gPakScreen.mode == PAK_MODE_SAVE) {
        sPakMessages[PAK_STATE_SELECT_PAK].buttons = sPakButtonsSelectNoSave;
    } else {
        sPakMessages[PAK_STATE_SELECT_PAK].buttons = sPakButtonsSelectCancel;
    }
    /* the buttons pressed this frame, the D-pad as the stick */
    buttons = gPadInput;
    if (buttons & INPUT_D_LEFT) {
        buttons |= INPUT_STICK_LEFT;
    }
    if (buttons & INPUT_D_RIGHT) {
        buttons |= INPUT_STICK_RIGHT;
    }
    if (buttons & INPUT_D_UP) {
        buttons |= INPUT_STICK_UP;
    }
    if (buttons & INPUT_D_DOWN) {
        buttons |= INPUT_STICK_DOWN;
    }
    i = sPakPrevButtons;
    sPakPrevButtons = buttons;
    buttons &= ~i;
    /* every 15 frames: probe the controllers whose pak was plugged in or out */
    sPakFrameCounter++;
    if (sPakFrameCounter % 15 == 0 && pak_get_plug_mask_2(plugged) == 0 && plugged[0] != sPakPlugMask) {
        for (i = 0; i < MAXCONTROLLERS; i++) {
            if (((plugged[0] ^ sPakPlugMask) >> i) & 1) {
                sPakRepairDeclined &= ~(1 << i);
                pak_probe_port(i);
                if (sPakValidMask & (1 << i)) {
                    pak_refresh(i);
                } else {
                    mem_fill((u8 *)sPakNoteTables[i], 0, PAK_MAX_NOTES * sizeof(PakNote));
                }
                if ((sPakBadMask & (1 << i))
                    && (gPakScreen.state == PAK_STATE_SELECT_PAK || gPakScreen.state == PAK_STATE_NO_PAK)) {
                    gPakScreen.pak = i;
                    set_state(PAK_STATE_DAMAGED, MENU_SOUND_ERROR);
                }
            }
        }
        sPakPlugMask = plugged[0];
    }
    sPakBadMask &= gContMask;
    sPakValidMask &= gContMask;
    gRumblePakMask &= gContMask;
    sPakRepairDeclined &= sPakBadMask;
    if (gPakScreen.state == PAK_STATE_SELECT_PAK || gPakScreen.state == PAK_STATE_NO_PAK) {
        /* offer to repair a bad pak, unless declined */
        for (i = 0; i < MAXCONTROLLERS; i++) {
            n = (gPakScreen.pak + i) % 4;
            if ((sPakBadMask & (1 << n)) && !((sPakRepairDeclined >> n) & 1)) {
                gPakScreen.pak = n;
                set_state(PAK_STATE_DAMAGED, MENU_SOUND_ERROR);
            }
        }
    }
    if (gPakScreen.state == PAK_STATE_SELECT_NOTE && sPakNoteCount[gPakScreen.pak] == 0) {
        gPakScreen.state = PAK_STATE_SELECT_PAK;
        gPakScreen.subState = PAK_STEP_START;
    }
    if ((gPakScreen.state < PAK_STATE_NO_PAK || gPakScreen.state > PAK_STATE_LOADED)
        && gPakScreen.state != PAK_STATE_RUMBLE_NOTICE) {
        if ((sPakValidMask | sPakBadMask) == 0 && gPakScreen.state != PAK_STATE_NO_PAK) {
            gPakScreen.state = PAK_STATE_NO_PAK;
            gPakScreen.subState = PAK_STEP_START;
        } else if (!((sPakValidMask | sPakBadMask) & (1 << gPakScreen.pak)) && gPakScreen.state != PAK_STATE_SELECT_PAK) {
            gPakScreen.state = PAK_STATE_SELECT_PAK;
            gPakScreen.subState = PAK_STEP_START;
        }
    }
    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (!((sPakBadMask | sPakValidMask) & (1 << gPakScreen.pak))) {
            gPakScreen.pak = (gPakScreen.pak + 1) % 4;
        }
    }
    if (sPakScreenSaved.state != -1 && gPakScreen.state == PAK_STATE_SELECT_PAK && gPakScreen.mode == PAK_MODE_MANAGER) {
        /* back from the manager opened by a full pak: the save screen again */
        gPakScreen = sPakScreenSaved;
        sPakScreenSaved.state = -1;
        gPakScreen.state = PAK_STATE_SELECT_PAK;
        gPakScreen.subState = PAK_STEP_START;
    }
    switch (gPakScreen.state) {
    case PAK_STATE_SELECT_PAK:
        drop_new_note_entry(gPakScreen.pak);
        gPakScreen.sel = 0;
        gPakScreen.scroll = 0;
        if (buttons & INPUT_STICK_LEFT) {
            for (i = gPakScreen.pak - 1; i >= 0; i--) {
                if ((sPakValidMask | sPakBadMask) & (1 << i)) {
                    break;
                }
            }
            if (i < 0) {
                sPakSound = MENU_SOUND_ERROR;
            } else {
                gPakScreen.pak = i;
                sPakSound = MENU_SOUND_MOVE;
            }
        } else if (buttons & INPUT_STICK_RIGHT) {
            for (i = gPakScreen.pak + 1; i < MAXCONTROLLERS; i++) {
                if ((sPakValidMask | sPakBadMask) & (1 << i)) {
                    break;
                }
            }
            if (i >= MAXCONTROLLERS) {
                sPakSound = MENU_SOUND_ERROR;
            } else {
                gPakScreen.pak = i;
                sPakSound = MENU_SOUND_MOVE;
            }
        }
        if (buttons & INPUT_B) {
            if (gPakScreen.mode == PAK_MODE_LOAD) {
                set_state(PAK_STATE_CANCEL_LOAD, MENU_SOUND_BACK);
            } else if (gPakScreen.mode == PAK_MODE_SAVE) {
                set_state(PAK_STATE_CANCEL_SAVE, MENU_SOUND_BACK);
            } else {
                gPakScreenAborted = 0;
                if (sPakSkipRumblePrompt != 0) {
                    gPakScreenDone = 1;
                    sPakSound = MENU_SOUND_SELECT;
                    sPakSkipRumblePrompt = 0;
                    break;
                }
                set_state(PAK_STATE_RUMBLE_NOTICE, MENU_SOUND_BACK);
            }
        } else if (buttons & INPUT_A) {
            if (sPakValidMask & (1 << gPakScreen.pak)) {
                n = 0;
                for (i = 0; i < PAK_MAX_NOTES; i++) {
                    if (sPakNoteTables[gPakScreen.pak][i].fileNo != -1) {
                        n++;
                    }
                }
                gPakScreen.sel = 0;
                if (gPakScreen.mode == PAK_MODE_SAVE) {
                    if (n == PAK_MAX_NOTES) {
                        set_state(PAK_STATE_PAK_FULL, MENU_SOUND_SELECT);
                    } else {
                        pak_build_note_list(gPakScreen.pak, 1);
                        set_state(PAK_STATE_SELECT_NOTE, MENU_SOUND_SELECT);
                    }
                } else if (gPakScreen.mode == PAK_MODE_MANAGER && n == 0) {
                    set_state(PAK_STATE_NO_NOTES, MENU_SOUND_ERROR);
                } else if (gPakScreen.mode == PAK_MODE_LOAD && sPakNoteCount[gPakScreen.pak] == 0) {
                    set_state(PAK_STATE_NO_NOTES, MENU_SOUND_ERROR);
                } else {
                    set_state(PAK_STATE_SELECT_NOTE, MENU_SOUND_SELECT);
                }
            } else if (sPakBadMask & (1 << gPakScreen.pak)) {
                set_state(PAK_STATE_DAMAGED, MENU_SOUND_ERROR);
            }
        }
        break;
    case PAK_STATE_SELECT_NOTE:
        if (gPakScreen.sel >= sPakNoteCount[gPakScreen.pak]) {
            gPakScreen.sel = sPakNoteCount[gPakScreen.pak] - 1;
        }
        if (gPakScreen.sel < 0) {
            gPakScreen.sel = 0;
        }
        /* move to the next note that can be chosen (saving: only over the game's own notes) */
        j = gPakScreen.sel;
        n = 0;
        if (buttons & INPUT_STICK_DOWN) {
            n = 1;
        }
        i = j;
        if (buttons & INPUT_STICK_UP) {
            n = -1;
        }
        j += n;
        count = sPakNoteCount[gPakScreen.pak];
        do {
            if (j < 0) {
                if (n != 0) {
                    j = i;
                    goto done;
                }
                j = sPakNoteCount[gPakScreen.pak] - 1;
            }
            if (j >= sPakNoteCount[gPakScreen.pak]) {
                if (n == 0) {
                    j = 0;
                } else {
                    j = i;
                    goto done;
                }
            }
            if (gPakScreen.mode != PAK_MODE_SAVE || sPakNoteList[gPakScreen.pak][j] == &sPakNewNoteEntry ||
                sPakNoteList[gPakScreen.pak][j]->gameCode == PAK_GAME_CODE) {
                goto done;
            }
            j += n;
        } while (n != 0 || (n = 1, --count != 0));
    done:
        gPakScreen.sel = j;
        if (j < gPakScreen.scroll) {
            gPakScreen.scroll = j;
        }
        if (gPakScreen.scroll < j - (PAK_LIST_ROWS - 1)) {
            gPakScreen.scroll = j - (PAK_LIST_ROWS - 1);
        }
        if (sPakNoteCount[gPakScreen.pak] - PAK_LIST_ROWS < gPakScreen.scroll) {
            gPakScreen.scroll = sPakNoteCount[gPakScreen.pak] - PAK_LIST_ROWS;
        }
        if (gPakScreen.scroll < 0) {
            gPakScreen.scroll = 0;
        }
        if (buttons & (INPUT_STICK_UP | INPUT_STICK_DOWN)) {
            if (i == j) {
                sPakSound = MENU_SOUND_ERROR;
            } else {
                sPakSound = MENU_SOUND_MOVE;
            }
        }
        if (buttons & INPUT_B) {
            set_state(PAK_STATE_SELECT_PAK, MENU_SOUND_BACK);
        }
        if (buttons & INPUT_A) {
            if (gPakScreen.mode == PAK_MODE_MANAGER) {
                set_state(PAK_STATE_CONFIRM_ERASE, MENU_SOUND_SELECT);
            } else if (gPakScreen.mode == PAK_MODE_SAVE) {
                note = sPakNoteList[gPakScreen.pak][gPakScreen.sel];
                if (note == &sPakNewNoteEntry) {
                    if (sPakFreePages[gPakScreen.pak] < gPakScreen.pages) {
                        set_state(PAK_STATE_PAK_FULL, MENU_SOUND_SELECT);
                        break;
                    }
                } else {
                    j = PAK_PAGES(note->size);
                    if (sPakFreePages[gPakScreen.pak] + j < gPakScreen.pages) {
                        set_state(PAK_STATE_PAK_FULL, MENU_SOUND_SELECT);
                        break;
                    }
                }
                if (note != &sPakNewNoteEntry) {
                    set_state(PAK_STATE_CONFIRM_OVERWRITE, MENU_SOUND_SELECT);
                } else {
                    set_state(PAK_STATE_SAVING, MENU_SOUND_SELECT);
                }
            } else {
                set_state(PAK_STATE_LOADING, MENU_SOUND_SELECT);
            }
        }
        break;
    case PAK_STATE_CONFIRM_ERASE:
        if (buttons & INPUT_B) {
            set_state(PAK_STATE_SELECT_NOTE, MENU_SOUND_BACK);
        }
        if (buttons & INPUT_A) {
            set_state(PAK_STATE_ERASING, MENU_SOUND_SELECT);
        }
        break;
    case PAK_STATE_ERASING:
        switch (gPakScreen.subState) {
        case PAK_STEP_START:
            sPakWaitTimer = PAK_WAIT_FRAMES;
            gPakScreen.subState = PAK_STEP_WAIT;
            break;
        case PAK_STEP_WAIT:
            if (sPakWaitTimer == 0) {
                gPakScreen.subState = PAK_STEP_RUN;
            }
            break;
        case PAK_STEP_RUN:
            if (pak_delete_note(gPakScreen.pak, sPakNoteList[gPakScreen.pak][gPakScreen.sel]->fileNo) != 0) {
                set_state(PAK_STATE_ERROR, MENU_SOUND_ERROR);
            } else if (gPakScreen.mode != PAK_MODE_MANAGER) {
                pak_refresh(gPakScreen.pak);
                set_state(PAK_STATE_SELECT_PAK, MENU_SOUND_SELECT);
            } else {
                pak_refresh(gPakScreen.pak);
                if (sPakNoteList[gPakScreen.pak] == NULL) {
                    set_state(PAK_STATE_SELECT_PAK, MENU_SOUND_SELECT);
                } else {
                    set_state(PAK_STATE_SELECT_NOTE, MENU_SOUND_SELECT);
                }
            }
            break;
        }
        break;
    case PAK_STATE_DAMAGED:
        sPakDamagedLines[0][0x11] = gPakScreen.pak + '1'; /* "Controller Pak #0 is damaged ...": the pak's number */
        if (buttons & INPUT_A) {
            set_state(PAK_STATE_REPAIRING, MENU_SOUND_SELECT);
        }
        if (buttons & INPUT_B) {
            set_sound(MENU_SOUND_BACK);
            set_state_silent(PAK_STATE_SELECT_PAK);
            sPakRepairDeclined |= 1 << gPakScreen.pak;
        }
        break;
    case PAK_STATE_NO_NOTES:
        if (buttons & INPUT_A) {
            set_state(PAK_STATE_SELECT_PAK, MENU_SOUND_SELECT);
        }
        break;
    case PAK_STATE_REPAIRING:
        switch (gPakScreen.subState) {
        case PAK_STEP_START:
            sPakWaitTimer = PAK_WAIT_FRAMES;
            gPakScreen.subState = PAK_STEP_WAIT;
            break;
        case PAK_STEP_WAIT:
            if (sPakWaitTimer == 0) {
                gPakScreen.subState = PAK_STEP_RUN;
            }
            break;
        case PAK_STEP_RUN:
            if (pak_repair(gPakScreen.pak) != 0) {
                set_state(PAK_STATE_REPAIR_FAILED, MENU_SOUND_ERROR);
            } else {
                gPakScreen.state = PAK_STATE_REPAIRED;
                sPakSound = MENU_SOUND_SELECT;
                gPakScreen.subState = PAK_STEP_START;
                sPakPlugMask &= ~(1 << gPakScreen.pak); /* probe it again */
            }
            break;
        }
        break;
    case PAK_STATE_REPAIR_FAILED:
        if (buttons & INPUT_A) {
            set_state(PAK_STATE_REPAIRING, MENU_SOUND_SELECT);
        }
        if (buttons & INPUT_B) {
            set_state(PAK_STATE_SELECT_PAK, MENU_SOUND_SELECT);
            sPakRepairDeclined |= 1 << gPakScreen.pak;
        }
        break;
    case PAK_STATE_REPAIRED:
        if (buttons & INPUT_A) {
            set_state(PAK_STATE_SELECT_PAK, MENU_SOUND_SELECT);
        }
        break;
    case PAK_STATE_CONFIRM_OVERWRITE:
        if (buttons & INPUT_B) {
            set_state(PAK_STATE_SELECT_NOTE, MENU_SOUND_BACK);
        }
        if (buttons & INPUT_A) {
            /* erase the note first; the new one takes its extension if it was the game's */
            note = sPakNoteList[gPakScreen.pak][gPakScreen.sel];
            if (pak_delete_note(gPakScreen.pak, note->fileNo) != 0) {
                set_state(PAK_STATE_ERROR, MENU_SOUND_ERROR);
            } else {
                if (note->gameCode == PAK_GAME_CODE) {
                    sPakOverwriteExt = note->ext[0];
                } else {
                    sPakOverwriteExt = -1;
                }
                set_state(PAK_STATE_SAVING, MENU_SOUND_SELECT);
            }
        }
        break;
    case PAK_STATE_ERROR:
        if (buttons & INPUT_A) {
            set_state(PAK_STATE_SELECT_NOTE, MENU_SOUND_SELECT);
            sPakPlugMask = 0;
            sPakValidMask = 0;
            sPakFrameCounter = -1;
        }
        break;
    case PAK_STATE_NO_PAK:
        if (sPakBadMask | sPakValidMask) {
            gPakScreen.state = PAK_STATE_SELECT_PAK;
            gPakScreen.subState = PAK_STEP_START;
        } else if (buttons & INPUT_B) {
            gPakScreenAborted = 1;
            gPakScreenDone = 1;
            sPakSound = MENU_SOUND_BACK;
        }
        break;
    case PAK_STATE_LOADING:
        switch (gPakScreen.subState) {
        case PAK_STEP_START:
            sPakWaitTimer = PAK_WAIT_FRAMES;
            gPakScreen.subState = PAK_STEP_WAIT;
            break;
        case PAK_STEP_WAIT:
            if (sPakWaitTimer == 0) {
                gPakScreen.subState = PAK_STEP_RUN;
            }
            break;
        case PAK_STEP_RUN:
            note = sPakNoteList[gPakScreen.pak][gPakScreen.sel];
            if (gPakScreen.pages < PAK_PAGES(note->size)) {
                set_state(PAK_STATE_INVALID_NOTE, MENU_SOUND_ERROR);
                return;
            }
            mem_fill(gPakScreen.data, 0, gPakScreen.pages << 8);
            if (pak_read_note(gPakScreen.pak, note->fileNo, 0, (note->size + 0xFF) & ~0xFF, gPakScreen.data) != 0) {
                set_state(PAK_STATE_ERROR, MENU_SOUND_ERROR);
                return;
            }
            if (gPakScreen.validate != NULL
                && gPakScreen.validate(gPakScreen.data, (note->size + 0xFF) & ~0xFF) != 0) {
                set_state(PAK_STATE_INVALID_NOTE, MENU_SOUND_ERROR);
                return;
            }
            set_state_silent(PAK_STATE_LOADED);
            break;
        }
        break;
    case PAK_STATE_SAVING:
        switch (gPakScreen.subState) {
        case PAK_STEP_START:
            sPakWaitTimer = PAK_WAIT_FRAMES;
            gPakScreen.subState = PAK_STEP_WAIT;
            break;
        case PAK_STEP_WAIT:
            if (sPakWaitTimer == 0) {
                gPakScreen.subState = PAK_STEP_RUN;
            }
            break;
        case PAK_STEP_RUN:
            /* the extension: the overwritten note's, or the first letter no note of the game's has */
            if (sPakOverwriteExt == -1) {
                n = 'A';
                for (;;) {
                    for (i = 0; i < sPakNoteCount[gPakScreen.pak]; i++) {
                        note = sPakNoteList[gPakScreen.pak][i];
                        if (note->gameCode == PAK_GAME_CODE && note->ext[0] == n) {
                            break;
                        }
                    }
                    if (i == sPakNoteCount[gPakScreen.pak]) {
                        break;
                    }
                    if (++n > 'Z') {
                        break;
                    }
                }
            } else {
                n = sPakOverwriteExt;
            }
            sPakOverwriteExt = -1;
            mem_fill(sPakStrBuf, 0, 4);
            sPakStrBuf[0] = n;
            /* create, write and read back to compare; the errors fall through to the next steps (original) */
            j = pak_create_note(gPakScreen.pak, sPakGameName, sPakStrBuf, PAK_COMPANY_CODE, PAK_GAME_CODE,
                                gPakScreen.pages << 8);
            if (j < 0) {
                set_state(PAK_STATE_ERROR, MENU_SOUND_ERROR);
            }
            if (pak_write_note(gPakScreen.pak, j, 0, gPakScreen.pages << 8, gPakScreen.data) != 0) {
                pak_delete_note(gPakScreen.pak, j);
                set_state(PAK_STATE_ERROR, MENU_SOUND_ERROR);
            }
            sPakVerifyBuf = mem_alloc_dbg(gPakScreen.pages << 8, 0, "pak.c", 0x5E6);
            n = pak_read_note(gPakScreen.pak, j, 0, gPakScreen.pages << 8, sPakVerifyBuf);
            if (n == 0) {
                for (i = 0; i < gPakScreen.pages << 8; i++) {
                    if (sPakVerifyBuf[i] != gPakScreen.data[i]) {
                        n = 1;
                    }
                }
            }
            mem_free(sPakVerifyBuf);
            if (n != 0) {
                pak_delete_note(gPakScreen.pak, j);
                set_state(PAK_STATE_ERROR, MENU_SOUND_ERROR);
            }
            pak_refresh(gPakScreen.pak);
            gPakScreen.state = PAK_STATE_SAVED;
            gPakScreen.subState = PAK_STEP_START;
            break;
        }
        break;
    case PAK_STATE_INVALID_NOTE:
        if (buttons & INPUT_A) {
            set_state(PAK_STATE_CONFIRM_ERASE, MENU_SOUND_SELECT);
        } else if (buttons & INPUT_B) {
            set_state(PAK_STATE_SELECT_PAK, MENU_SOUND_SELECT);
            sPakPlugMask = 0;
            sPakValidMask = 0;
            sPakFrameCounter = -1;
        }
        break;
    case PAK_STATE_PAK_FULL:
        if (buttons & INPUT_B) {
            set_state(PAK_STATE_SELECT_PAK, MENU_SOUND_BACK);
        } else if (buttons & INPUT_A) {
            /* the manager, until it is left: then back to saving */
            sPakScreenSaved = gPakScreen;
            gPakScreen.mode = PAK_MODE_MANAGER;
            pak_build_note_list(gPakScreen.pak, 0);
            drop_new_note_entry(gPakScreen.pak);
            set_state(PAK_STATE_SELECT_NOTE, MENU_SOUND_SELECT);
        }
        break;
    case PAK_STATE_CANCEL_SAVE:
        gPakScreenAborted = 1;
        if (buttons & INPUT_A) {
            set_state(PAK_STATE_RUMBLE_NOTICE, MENU_SOUND_SELECT);
        } else if (buttons & INPUT_B) {
            set_state(PAK_STATE_SELECT_PAK, MENU_SOUND_BACK);
        }
        break;
    case PAK_STATE_CANCEL_LOAD:
        gPakScreenAborted = 1;
        if (buttons & INPUT_A) {
            set_state(PAK_STATE_RUMBLE_NOTICE, MENU_SOUND_SELECT);
        } else if (buttons & INPUT_B) {
            set_state(PAK_STATE_SELECT_PAK, MENU_SOUND_BACK);
        }
        break;
    case PAK_STATE_SAVED:
    case PAK_STATE_LOADED:
        gPakScreenAborted = 0;
    case PAK_STATE_RUMBLE_NOTICE:
        if (buttons & INPUT_A) {
            gPakScreenDone = 1;
            sPakSound = MENU_SOUND_SELECT;
        }
        break;
    }
    if (gPakScreenDone != 0 && sPakScreenSaved.state != -1) {
        /* leaving the manager opened from the save screen: back to it */
        gPakScreen = sPakScreenSaved;
        sPakScreenSaved.state = -1;
        gPakScreen.state = PAK_STATE_SELECT_PAK;
        gPakScreen.subState = PAK_STEP_START;
        gPakScreenDone = 0;
    }
    if (sPakSound != -1) {
        snd_play_menu_sound(sPakSound);
    }
    if (gPakScreenDone != 0) {
        pak_probe_all();
        if (gPakScreenDone != 0) {
            sPakSkipRumblePrompt = 0;
        }
    }
}

/* Unloads the screen's pictures and text and frees the note tables. */
void pak_screen_exit(void)
{
    res_unload((u32 *)(sPakGuiRes_2[1] + gControllerIconResId * 4));
    res_unload((u32 *)(sPakGuiRes_2[1] + gMenuBackgroundResId * 4));
    res_unload((u32 *)(sPakGuiRes_2[1] + sPakUiTexResId * 4));
    res_unload((u32 *)(sPakGuiRes_2[1] + gTitleBarResId * 4));
    res_unload((u32 *)(sPakGuiRes_2[1] + sPakButtonIconsResId * 4));
    font_text_destroy(sPakText);
    mem_free(sPakText);
    pak_free_note_tables();
    pak_probe_all();
}

/* a note's first 0x20 bytes, copied whole */
typedef struct { u32 w[8]; } Hdr_10;

/* Reads the state of each note of a pak into its table (and the first bytes of the game's notes). Returns 0, -1
   without a valid pak, -2 on an error. */
s32 pak_read_note_states(u32 pak)
{
    Hdr_10 header;
    s32 fileNo, slot, ret;

    if (!(sPakValidMask & (1 << pak))) {
        return -1;
    }
    cont_finish_read();
    for (fileNo = 0; fileNo < PAK_MAX_NOTES; fileNo++) {
        sPakNoteTables[pak][fileNo].fileNo = -1;
    }
    slot = 0;
    fileNo = 0;
    do {
        ret = osPfsFileState(&sPakFs[pak], fileNo, (OSPfsState *)&sPakNoteTables[pak][slot]);
        if (ret == PFS_ERR_INVALID) {
            /* no note there */
            mem_fill((u8 *)&sPakNoteTables[pak][slot], 0, sizeof(PakNote));
            sPakNoteTables[pak][slot].fileNo = -1;
        } else if (ret == 0) {
            sPakNoteTables[pak][slot].fileNo = fileNo;
            pak_pfs_to_ascii(sPakNoteTables[pak][slot].name, PFS_FILE_NAME_LEN);
            pak_pfs_to_ascii(sPakNoteTables[pak][slot].ext, PFS_FILE_EXT_LEN);
            if (sPakNoteTables[pak][slot].gameCode == PAK_GAME_CODE) {
                if (pak_read_note(pak, fileNo, 0, sizeof(header), (u8 *)&header) != 0) {
                    sPakNoteTables[pak][slot].gameCode = 0;
                }
                *(Hdr_10 *)sPakNoteTables[pak][slot].header = header;
            }
            slot++;
        } else {
            return -2;
        }
        fileNo++;
    } while (fileNo < PAK_MAX_NOTES);
    return 0;
}

/* Queries the controllers, then finds what is plugged into controller chan: a Controller Pak (valid or bad), a
   Rumble Pak or a Transfer Pak. */
void pak_probe_port(s32 chan)
{
    u32 bit = 1 << chan;
    s32 msg[2];
    s32 i;

    if (gContReadPending != 0) {
        cont_finish_read();
    }
    osContStartQuery(&gSiMesgQueue);
    osRecvMesg(&gSiMesgQueue, msg, OS_MESG_BLOCK);
    osContGetQuery(gContStatus);
    gContMask = 0;
    gContCount = 0;
    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (gContStatus[i].errno == 0) {
            if (gContMask == 0) {
                gContFirst = i;
            }
            gContMask |= 1 << i;
            gContCount++;
        }
    }
    if ((gContMask & bit) == 0) {
        return;
    }
    i = chan;
    sPakValidMask &= ~bit;
    gRumblePakMask &= ~bit;
    sPakBadMask &= ~bit;
    switch (osPfsInitPak(&gSiMesgQueue, &sPakFs[i], i)) {
    case 0:
        sPakValidMask |= bit;
        break;
    case PFS_ERR_NOPACK: case PFS_ERR_NEW_PACK: case PFS_ERR_INCONSISTENT: case PFS_ERR_CONTRFAIL:
    case PFS_ERR_INVALID: case PFS_ERR_BAD_DATA: case PFS_DATA_FULL: case PFS_DIR_FULL: case PFS_ERR_EXIST:
        break;
    case PFS_ERR_DEVICE:
        if (osMotorInit(&gSiMesgQueue, &gRumblePfs[i], i) != 0) {
            if (osGbpakInit(&gSiMesgQueue, &sGbPakFs, i) != 0) {
                sPakBadMask |= 1 << i;
            }
        } else {
            __osMotorAccess(&gRumblePfs[i], RUMBLE_STOP);
            gRumblePakMask |= bit;
        }
        break;
    case PFS_ERR_ID_FATAL:
        if (osMotorInit(&gSiMesgQueue, &gRumblePfs[i], i) == 0) {
            __osMotorAccess(&gRumblePfs[i], RUMBLE_STOP);
            gRumblePakMask |= bit;
        } else {
            sPakBadMask |= bit;
        }
        break;
    }
}

/* Queries the controllers and finds what is plugged into each (as pak_probe_port), and the EEPROM type. */
void pak_probe_all(void)
{
    u32 msg[2];
    s32 i;

    if (gContReadPending != 0) {
        cont_finish_read();
    }
    osContStartQuery(&gSiMesgQueue);
    osRecvMesg(&gSiMesgQueue, msg, OS_MESG_BLOCK);
    osContGetQuery(gContStatus);
    gContMask = 0;
    gContCount = 0;
    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (gContStatus[i].errno == 0) {
            if (gContMask == 0) {
                gContFirst = i;
            }
            gContMask |= 1 << i;
            gContCount++;
        }
    }
    gRumbleActiveMask = 0;
    sPakValidMask = 0;
    gRumblePakMask = 0;
    sPakBadMask = 0;
    for (i = 0; i < MAXCONTROLLERS; i++) {
        gRumbleTimers[i] = 0;
        if ((gContMask >> i) & 1) {
            switch (osPfsInitPak(&gSiMesgQueue, &sPakFs[i], i)) {
            case 0:
                sPakValidMask |= 1 << i;
                break;
            case PFS_ERR_NOPACK:
            case PFS_ERR_NEW_PACK:
            case PFS_ERR_INCONSISTENT:
            case PFS_ERR_CONTRFAIL:
            case PFS_ERR_INVALID:
            case PFS_ERR_BAD_DATA:
            case PFS_DATA_FULL:
            case PFS_DIR_FULL:
            case PFS_ERR_EXIST:
                break;
            case PFS_ERR_DEVICE:
                if (osMotorInit(&gSiMesgQueue, &gRumblePfs[i], i) == 0) {
                    __osMotorAccess(&gRumblePfs[i], RUMBLE_STOP);
                    gRumblePakMask |= 1 << i;
                } else if (osGbpakInit(&gSiMesgQueue, &sGbPakFs, i) != 0) {
                    sPakBadMask |= 1 << i;
                }
                break;
            case PFS_ERR_ID_FATAL:
                if (osMotorInit(&gSiMesgQueue, &gRumblePfs[i], i) == 0) {
                    __osMotorAccess(&gRumblePfs[i], RUMBLE_STOP);
                    gRumblePakMask |= 1 << i;
                } else {
                    sPakBadMask |= 1 << i;
                }
                break;
            }
        }
    }
    sEepromType = osEepromProbe(&gSiMesgQueue);
}

/* Queries which controllers are connected: gContMask, gContCount, gContFirst. */
void pak_query_controllers(void)
{
    s32 msg[2];
    s32 i;
    u8 *status;

    if (gContReadPending != 0) {
        cont_finish_read();
    }
    osContStartQuery(&gSiMesgQueue);
    osRecvMesg(&gSiMesgQueue, msg, OS_MESG_BLOCK);
    osContGetQuery(gContStatus);
    i = 0;
    gContMask = 0;
    gContCount = 0;
    status = (u8 *)gContStatus;
    do {
        if (status[3] == 0) { /* OSContStatus.errno */
            if (gContMask == 0) {
                gContFirst = i;
            }
            gContMask = gContMask | (1 << i);
            gContCount = gContCount + 1;
        }
        i++;
        status += 4;
    } while (i < MAXCONTROLLERS);
}

/* Probes the controllers and fills gPakFreePages: the free pages of each Controller Pak, -1 when its 16 notes
   are used, -2 without a pak. */
void pak_update_free_pages(void)
{
    s32 i;
    s32 maxFiles;
    s32 filesUsed;
    s32 freeBytes;
    s32 bytes;

    pak_probe_all();
    for (i = 0; i < MAXCONTROLLERS; i++) {
        gPakFreePages[i] = -2;
        if ((sPakValidMask & (1 << i)) && osPfsNumFiles(&sPakFs[i], &maxFiles, &filesUsed) == 0) {
            if (!(sPakValidMask & (1 << i))) {
                bytes = -1;
            } else {
                cont_finish_read();
                if (osPfsFreeBlocks(&sPakFs[i], &freeBytes) != 0) {
                    bytes = -2;
                } else {
                    bytes = freeBytes;
                }
            }
            bytes &= ~0xFF;
            if (filesUsed == PAK_MAX_NOTES) {
                gPakFreePages[i] = -1;
            } else {
                gPakFreePages[i] = bytes / 256;
            }
        }
    }
}

/* The free bytes of a pak, or -1 without a valid pak, -2 on an error. */
int pak_get_free_bytes(u32 pak)
{
    int bytes[2];

    if ((sPakValidMask & (1 << pak)) == 0) {
        return -1;
    }
    cont_finish_read();
    if (osPfsFreeBlocks(&sPakFs[pak], bytes) != 0) {
        return -2;
    }
    return bytes[0];
}

/* Creates a note (name and extension in ASCII). Returns its file number, -1 without a valid pak, or minus the
   Pfs error. */
s32 pak_create_note(u32 pak, char *name, char *ext, s32 companyCode, s32 gameCode, s32 size)
{
    s32 ret;
    u8 pfsName[PFS_FILE_NAME_LEN];
    u8 pfsExt[8];
    s32 fileNo[2];

    if ((sPakValidMask & (1 << pak)) == 0) {
        ret = -1;
    } else {
        cont_finish_read();
        mem_fill(pfsExt, 0, PFS_FILE_EXT_LEN);
        str_copy(pfsExt, ext);
        mem_fill(pfsName, 0, PFS_FILE_NAME_LEN);
        str_copy(pfsName, name);
        pak_ascii_to_pfs(pfsName, PFS_FILE_NAME_LEN);
        pak_ascii_to_pfs(pfsExt, PFS_FILE_EXT_LEN);
        ret = osPfsAllocateFile(&sPakFs[pak], (u16)companyCode, gameCode, pfsName, pfsExt, size, fileNo);
        if (ret == 0) {
            ret = fileNo[0];
        } else {
            ret = -ret;
        }
    }
    return ret;
}

/* Deletes a pak's note. Returns 0, -1 without a valid pak, 100 minus the error of reading the note's state, or
   minus the error of deleting it. */
s32 pak_delete_note(s32 channel, s32 file_no)
{
    volatile s32 pad[6];
    OSPfsState state;
    s32 ret;
    s32 r2;

    if (!(sPakValidMask & (1 << channel))) {
        return -1;
    }
    cont_finish_read();
    ret = osPfsFileState(&sPakFs[channel], file_no, &state);
    if (ret != 0) {
        return 100 - ret;
    }
    r2 = osPfsDeleteFile(&sPakFs[channel], state.company_code, state.game_code, state.game_name, state.ext_name);
    if (r2 != 0) {
        return -r2;
    }
    return 0;
}

/* Reads size bytes of a note from offset. Returns 0, -1 without a valid pak, or minus the Pfs error. */
s32 pak_read_note(u32 pak, s32 fileNo, u32 offset, u32 size, u8 *buf)
{
    s32 ret;

    if ((sPakValidMask & (1 << pak)) == 0) {
        return -1;
    }
    cont_finish_read();
    ret = osPfsReadWriteFile(&sPakFs[pak], fileNo, PFS_READ, offset, size, buf);
    if (ret == 0) {
        return 0;
    }
    return -ret;
}

/* Writes size bytes of a note at offset. Returns 0, -1 without a valid pak, or minus the Pfs error. */
s32 pak_write_note(u32 pak, s32 fileNo, u32 offset, u32 size, u8 *buf)
{
    s32 ret;

    if ((sPakValidMask & (1u << pak)) == 0) {
        return -1;
    }
    cont_finish_read();
    ret = osPfsReadWriteFile(&sPakFs[pak], fileNo, PFS_WRITE, offset, size, buf);
    if (ret != 0) {
        return -ret;
    }
    return 0;
}

/* Reads count EEPROM blocks from block first. Each block is read again until it holds something other than the
   pattern it was filled with before the read (0x55, then 0x56, ...). Not called in this build. */
void pak_eeprom_read(s32 first, u8 *buf, s32 count)
{
    s32 numRead;
    s32 pattern;
    u32 i;
    u32 j;
    u8 *block;

    pattern = 0x55;
    cont_finish_read();
    mem_fill_4(buf, 0x55, count << 3);
    if (0 < count) {
        do {
            i = 0;
            numRead = 0;
            if (count != 0) {
                block = buf;
                do {
                    for (j = 0; j < EEPROM_BLOCK_SIZE; j++) {
                        if (buf[i * 8 + j] != pattern) {
                            break;
                        }
                    }
                    if (j != EEPROM_BLOCK_SIZE) {
                        numRead = numRead + 1;
                    } else {
                        mem_fill_4(block, pattern + 1, EEPROM_BLOCK_SIZE);
                        osWritebackDCache(block, EEPROM_BLOCK_SIZE);
                        osEepromRead((s32)&gSiMesgQueue, (u8)(first + i), block);
                        osInvalDCache(block, EEPROM_BLOCK_SIZE);
                    }
                    i = i + 1;
                    block = block + EEPROM_BLOCK_SIZE;
                } while (i < count);
            }
            pattern = pattern + 1;
        } while (numRead < count);
    }
    osInvalDCache(buf, count << 3);
}

/* Repairs a pak's file system (osPfsRepairId). */
s32 pak_repair(u32 pak)
{
    u8 *pfs;
    s32 ret;

    cont_finish_read_2();
    pfs = &sPakFs[pak];
    osPfsInitPak(&gSiMesgQueue, pfs, pak);
    ret = osPfsRepairId(pfs);
    osPfsInitPak(&gSiMesgQueue, pfs, pak);
    return ret;
}

/* Converts len characters to the Controller Pak's character codes (sPakCharset from code 0xF); other characters
   become spaces. */
void pak_ascii_to_pfs(u8 *str, s32 len)
{
    u8 *q;
    u8 c;
    s32 code;
    s32 i;

    for (i = 0; i < len; i++, str++) {
        code = 0;
        if (sPakCharset[0] == 0) goto nf;
        q = str;
        c = *str;
        while (sPakCharset[code] != 0) {
            if (c == sPakCharset[code]) break;
            code++;
        }
        if (sPakCharset[code] != 0) goto st;
    nf:
        if (*str != 0) goto nz;
        *str = 0;
        goto next;
    nz:
        *str = 0xF;
        goto next;
    st:
        *q = code + 0xF;
    next:
        ;
    }
}

/* Converts len Controller Pak character codes to ASCII; unknown codes become '*'. */
void pak_pfs_to_ascii(u8 *buf, s32 len)
{
    s32 i = 0;
    u8 *p;
    if (len > 0) {
        do {
            p = buf + i;
            if ((u8)(*p - 0xF) < 0x33) {
                *p = sPakCharset[*p - 0xF];
            } else {
                if (*p != 0) {
                    goto nonzero;
                }
                *p = 0;
                goto next;
            nonzero:
                *p = '*';
            }
        next:
            i++;
        } while (i < len);
    }
}

/* Allocates the note table of each pak. */
void pak_alloc_note_tables(void)
{
    s32 i;
    for (i = 0; i < MAXCONTROLLERS; i++) {
        sPakNoteTables[i] = mem_alloc_dbg(PAK_MAX_NOTES * sizeof(PakNote), 0, (s32)"pak.c", 0x75F);
        mem_fill(sPakNoteTables[i], 0x77, PAK_MAX_NOTES * sizeof(PakNote));
    }
}

/* Frees the note tables. */
void pak_free_note_tables(void)
{
    s32 i;
    for (i = 0; i < MAXCONTROLLERS; i++) {
        mem_free(sPakNoteTables[i]);
    }
}

/* The functions below are out-of-line copies of the screen's helpers; nothing calls them. */

/* drop_new_note_entry */
void pak_remove_new_note_entry(s32 pak)
{
    Quad *dst;
    Quad *src;
    Quad *end;

    if (sPakNoteList[pak][0] == &sPakNewNoteEntry && sPakNoteCount[pak] > 0) {
        dst = (Quad *)&sPakNoteList[pak][0];
        src = (Quad *)&sPakNoteList[pak][1];
        end = src + 4;
        do {
            *dst = *src;
            dst++;
            src++;
        } while (src != end);
        sPakNoteCount[pak] = sPakNoteCount[pak] - 1;
    }
}

/* draw_text */
Gfx *pak_draw_text(Gfx *gdl, char *str, s32 font, s32 color, s32 x, s32 y)
{
    char *p;
    if (font == FONT_SMALL) {
        p = str;
        if (*p != 0) {
            do {
                if (*p == '@') {
                    *p = 0x7F;
                }
                p++;
            } while (*p != 0);
        }
    }
    font_text_create(sPakText, 0x80, font, color, 0);
    font_text_add(sPakText, str);
    font_draw_text(sPakText, x, y);
    return font_render(gdl);
}

/* Sets sPakText's text (to measure it). */
void pak_set_text(char *str, s32 font, s32 color)
{
    font_text_create(sPakText, 0x80, font, color, 0);
    font_text_add(sPakText, str);
}

/* draw_text_centered */
Gfx *pak_draw_text_centered(Gfx *gdl, char *str, s32 font, s32 color, s32 centerX, s32 y)
{
    font_text_create(sPakText, 0x80, font, color, 0);
    font_text_add(sPakText, str);
    font_draw_text(sPakText, centerX - (sPakText->w >> 1), y);
    return font_render(gdl);
}

/* Draws a text ending at rightX. */
Gfx *pak_draw_text_right(Gfx *gdl, char *str, s32 font, s32 color, s32 rightX, s32 y)
{
    font_text_create(sPakText, 0x80, font, color, 0);
    font_text_add(sPakText, str);
    font_draw_text(sPakText, rightX - sPakText->w, y);
    return font_render(gdl);
}

/* set_state; sound -1: none */
void pak_set_state(s32 state, s32 sound)
{
    PakScreen *screen;

    screen = &gPakScreen;
    screen->state = state;
    screen->subState = PAK_STEP_START;
    if (sound != -1) {
        sPakSound = sound;
    }
}

/* Takes the controller status a read left behind (gContMask, gContCount, gContFirst) and which controllers have
   a pak plugged in. */
void pak_get_plug_mask(u8 *plugged)
{
    s32 i;
    u8 *status;

    cont_finish_read();
    i = 0;
    gContMask = 0;
    gContCount = 0;
    do {
        status = gContStatus_2 + i * 4;
        if (status[3] == 0) { /* OSContStatus.errno */
            if (gContMask == 0) {
                gContFirst = i;
            }
            gContMask = gContMask | (1 << i);
            gContCount = gContCount + 1;
        }
        i++;
    } while (i < MAXCONTROLLERS);
    osPfsIsPlug(&gSiMesgQueue, plugged);
}
