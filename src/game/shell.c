#include "game.h"

s32 shell_format_pak_file_label(u8 *src, char *dst, s32 flag)
{
    s32 trk, q, w, ret;
    s32 n, m, v;

    dst[0] = 0;
    if (replay_get_info(src, (u32 *)&trk, (u32 *)&q, (u32 *)&w) == 0) {
        w = w - 0x102;
        __builtin_memcpy(dst, D_800D946C, 8);
        str_cat(dst, gTracks_13 + trk * 160);
        if (flag == 0) {
            str_cat(dst, D_800D91CC);
            v = w / 3600;
            if (v < 10) {
                str_cat(dst, D_800D91D0);
            }
            int_to_str(v, dst + strlen(dst));
            str_cat(dst, D_800D9474);
            n = strlen(dst);
            v = w / 60 % 60;
            dst[n] = v / 10 + '0';
            n++;
            dst[n] = v % 10 + '0';
            n++;
            dst[n] = '.';
            v = w % 60 * 100 / 60;
            n++;
            dst[n] = v / 10 + '0';
            n++;
            dst[n] = v % 10 + '0';
            n++;
            dst[n] = 0;
        }
        ret = gGame.pakScreenMode;
        ret = ret ^ 3;
    } else {
        if (src[4] != 0x35) {
            __builtin_memcpy(dst, D_800D9480, 28);
            return 1;
        }
        __builtin_memcpy(dst, D_800D9478, 8);
        m = strlen(dst);
        dst[m + 10] = 0;
        m = strlen(dst);
        __builtin_memcpy(dst + m, src + 6, 10);
        ret = gGame.pakScreenMode;
        ret = ret ^ 1;
    }
    return (ret == 0) << 1;
}

void shell_blend_palette_to_gray(s32 alpha, u16 *src)
{
    s32 i;
    s32 r, g, b;
    u32 p;

    gGame.greyPalette[0] = 0;
    for (i = 1; i < 0x100; i++) {
        if (alpha == 0xFF) {
            b = i;
            g = i;
            r = i;
        } else {
            p = src[i];
            b = (p >> 11) * 8;
            g = (p >> 3) & 0xF8;
            r = (p << 2) & 0xF8;
            b = b + ((i - b) * alpha) / 255;
            g = g + ((i - g) * alpha) / 255;
            r = r + ((i - r) * alpha) / 255;
        }
        gGame.greyPalette[i] = ((r >> 3) << 1) | ((g >> 3) << 6) | ((b >> 3) << 11) | 1;
    }
}

typedef struct {
    u8 pad0[0x34];
    TrackRecord r34;
    s32 w50;
    u8 pad54[0xA0 - 0x54];
} Course_2;

extern Course_2 gTracks_14[];

void shell_init(void)
{
    s32 flag;
    s32 mode;
    s32 i;
    TrackRecord *dst;
    s32 n;
    Course_2 *c;
    Screen *m;
    u8 *p;
    u8 *q;

    flag = 0;
    gPodiumMode = 0;
    gReplayPlayback = 0;
    if (gRaceExitCode == 2) {
        flag = flag < (u32)(gGame.flags & 8);
    }
    gSplitVertical = 0;
    gGame.flags &= ~8;
    if (gRaceExitCode == 0) {
        mem_fill((u8 *)&gGame, 0, sizeof(Game));
        n = 0;
        for (i = 0; i < 27; i++) {
            s32 f = gTracks_14[i].w50;
            TrackRecord *src = &gTracks_14[i].r34;
            if (f & 0x200) {
                dst = &gGame.records[n];
                *dst = *src;
                n++;
            }
        }
        gGame.twoPlayerCpuRiders = 2;
        gGame.riders[0] = 4;
        gGame.mainMenuSel = 0;
        gGame.trackSel = 0;
        gGame.cupSel = 0;
        gGame.riders[1] = 0;
        gGame.riders[2] = 1;
        gGame.riders[4] = 3;
        gGame.riders[5] = 5;
        gGame.riders[3] = 2;
        gGame.numPlayers = 1;
        gGame.numLaps = -1;
        gGame.optionsSel = 0;
        gGame.unk_048 = 0;
        gGame.specialTracksSel = 0;
        gGame.unk_018 = 0;
        gGame.padConfig[0] = 0;
        gGame.padConfig[1] = 0;
        gGame.padConfig[2] = 0;
        gGame.padConfig[3] = 0;
        gGame.unk_2A4 = 0;
        gGame.unk_2A5 = 1;
        gGame.riderSkinSel = 0;
        gGame.padCamMode[0] = 2;
        gGame.padCamMode[1] = 2;
        gGame.padCamMode[2] = 2;
        gGame.padCamMode[3] = 2;
        i = 5;
        p = (u8 *)&gGame + i;
        do {
            p[0x7AA] = i;
            p--;
            i--;
        } while (i >= 0);
        i = 5;
        q = (u8 *)&gGame + i;
        do {
            q[0x43C] = i;
            q--;
            i--;
        } while (i >= 0);
    } else if (gGame.mode == 8) {
        gGame.numPlayers = 1;
    }
    if (gRaceExitCode == 7) {
        gGame.flags &= 0xFF0FFFFF;
    }
    gGame.fadeState = 3;
    res_set_load_buffer((s32)gZBuffer_3);
    i = res_open_set("resource", &D_800E7728);
    res_set_load_buffer(0);
    if (i != 0) {
        fatal_halt(1);
    }
    font_load_menu_set((u32 *)D_800E7728);
    res_close_set((s32 *)D_800E7728);
    gGame.numRecordTracks = 0;
    for (i = 0; i < 27; i++) {
        if ((gTracks_14[i].w50 & 0x200) && !(gTracks_14[i].w50 & 2)) {
            gZBuffer_3[gGame.numRecordTracks++] = i;
        }
    }
    gGame.recordTracks = mem_alloc_dbg(gGame.numRecordTracks * 2, 0, "shell.c", 0x75C);
    memcpy(gGame.recordTracks, gZBuffer_3, gGame.numRecordTracks * 2);
    gGame.numCpuRiders = 5;
    D_800CA924_4 = 0;
    gGame.flags = (gGame.flags & 0x20F88000) | 1;
    res_open_set("guires", &gGuiRes);
    res_register_type("ARN", D_800248EC);
    prim_init();
    sCurScreen = 0;
    sPrevScreen = 0;
    gGame.unk_404 = res_load(gGuiRes, D_800D1438);
    sShellViewport[2] = 0x1FF;
    sShellViewport[6] = 0x1FF;
    sShellViewport[0] = gScreenWidth * 2;
    sShellViewport[1] = gScreenHeight * 2;
    sShellViewport[4] = gScreenWidth * 2;
    sShellViewport[5] = gScreenHeight * 2;
    gGame.greyPalette = mem_alloc_dbg(0x200, 0, "shell.c", 0x777);
    shell_blend_palette_to_gray(0xFF, 0);
    if (gRaceExitCode == 4) {
        gCurTrack_8->w50 &= ~0x180;
        if (gActiveReplay != 0) {
            gActiveReplay = 0;
            gGame.flags &= 0xFFF7FFFF;
        }
    }
    if (gEditor != NULL) {
        if (gEditor->flags & EDIT_PAK_LOAD) {
            gGame.pakScreenMode = 3;
            shell_goto_screen(&sScreens[SCREEN_PAK], 0, 1);
        } else {
            gGame.pakScreenMode = 4;
            shell_goto_screen(&sScreens[SCREEN_PAK], 0, 1);
        }
    } else {
        mode = gRaceExitCode;
        if (mode == 0) {
            shell_goto_screen(&sScreens[SCREEN_BOOT_SPLASH], 0, 1);
        } else if ((gGame.flags & 0x8000) || gCreditsMode != 0 || mode == 3) {
            if (gActiveReplay != 0) {
                if (!(gGame.flags & 0x8000)) {
                    if (!(gGame.flags & 0x80000)) {
                        gActiveReplay = 0;
                    }
                }
            }
            if (gRaceExitCode == 3 && gGame.mode == 1) {
                shell_restore_race_results();
                if (gSeason->round > 0) {
                    gSeason->round--;
                }
            }
            if (gGame.flags & 0x8000) {
                shell_restore_race_results();
            }
            if (gCreditsMode != 0 && gGame.returnScreen != &sScreens[SCREEN_OPTIONS]) {
                gCreditsMode = 0;
                shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 0, 1);
            } else {
                shell_goto_screen(gGame.returnScreen, 0, 1);
            }
            gGame.flags &= 0xFEFF7FFF;
        } else if (flag) {
            shell_goto_screen(&sScreens[SCREEN_BRONZE_TRACKS], 1, 1);
        } else {
            if (gEvent.allFinished != 0 && mode != 0 && mode != 6 && gGame.mode == 1) {
                if (gSeason != NULL && gSeason->cup - gCups == 4) {
                    m = &sScreens[SCREEN_SEASON_STANDINGS];
                } else {
                    m = &sScreens[SCREEN_SEASON_RACE_RESULT];
                }
            } else if ((gEvent.allFinished != 0 && gRaceExitCode != 0 && gRaceExitCode != 6 && gGame.mode == 0)
                       || (gEvent.allFinished != 0 && gRaceExitCode != 0 && gRaceExitCode != 6 && gGame.mode == 8)) {
                m = &sScreens[SCREEN_RACE_RESULT];
            } else {
                if (gActiveReplay != 0) {
                    gActiveReplay = 0;
                }
                switch (gRaceExitCode) {
                case 0:
                    m = &sScreens[SCREEN_TITLE];
                    break;
                case 1:
                    m = &sScreens[SCREEN_RACE_RESULT];
                    break;
                default:
                    m = &sScreens[SCREEN_MAIN_MENU];
                    if (gUserTrackLayout != 0) {
                        mem_free(gUserTrackLayout);
                        gUserTrackLayout = 0;
                    }
                    break;
                }
            }
            if (gRaceExitCode != 0 && gCurTrack_8 != NULL && (gCurTrack_8->w50 & 0x180)) {
                sScreens[SCREEN_INITIALS].items->target = m;
                shell_goto_screen(&sScreens[SCREEN_INITIALS], 1, 1);
            } else {
                shell_goto_screen(m, 1, 1);
            }
        }
    }
    if (gRaceExitCode == 0) {
        save_load(0);
    }
    save_update_unlocks();
    race_apply_volumes();
}

void shell_cb_soccer(s32 param_1)
{
    s32 uVar1;
    s32 uVar2;
    s32 iVar3;
    Game *base;

    if (param_1 != 0) {
        uVar1 = 0;
        do {
            uVar2 = uVar1 + 1;
            gGame.riderSkins[uVar1] = (u8)(((s32)(uVar1 & 3) >> ((gGame.numPlayers - 1) >> 1)) << 1);
            uVar1 = uVar2;
        } while (uVar2 < 6);
        uVar1 = 0;
        iVar3 = uVar1;
        base = &gGame;
        base->mode = 2;
        gCurTrack = sTrackSoccer;
        gSplitVertical = 0;
        base->riders[0] = 4;
        base->riders[1] = 4;
        base->riders[2] = 4;
        base->riders[3] = 4;
        gGame.flags = gGame.flags & 0xffffff7f;
        for (; uVar1 < 4; uVar1++) {
            if (((s32)gContMask >> uVar1 & 1) != 0) {
                gGame.playerPad[iVar3] = (u8)uVar1;
                iVar3 = iVar3 + 1;
                if (iVar3 == gGame.numPlayers) {
                    return;
                }
            }
        }
    }
}

typedef struct {
    u8 unk0[0x2C1];
    u8 trackId;
} Unk1900;

extern Season sSeasonState;
extern Unk1900 gSaveData_8;

void shell_cb_season(s32 arg0) {
    s32 i;

    if (arg0 != 0) {
        gGame.mode = 1;
        gSeason = &sSeasonState;
        gGame.flags |= 0x80;
        mem_fill(&sSeasonState, 0, sizeof(Season));
        gSeason->cup = &gCups[gGame.cupSel];
        if (gGame.difficulty == 0) {
            gCurTrack = sTrackTutorial;
            gGame.mode = 9;
            *(volatile s32 *)&gGame.numPlayers = 1; /* volatile: the store stays before the next one */
            *(volatile s32 *)&gEvent.tutorialBuf = 0;
            gGame.riders[0] = 4;
            gGame.flags &= ~0x80;
            gSeason->cup = &gCups[6];
            gSeason->round = 0;
            for (i = 0; i < 17; i++) {
                if (gTutorialLessons[i].level == gSaveData_8.trackId) {
                    gSeason->round = i;
                    return;
                }
            }
        }
    }
}

#define PRESSED(m) ((gPadInput & (m)) != 0 && (gPadInputPrev & (m)) == 0)

void shell_splash_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    s32 flag;
    s32 ok;
    s16 type;

    flag = 0;
    type = screen->type;
    if (type == SCREEN_TYPE_SPLASH || type == SCREEN_TYPE_SPLASH_3) {
        ok = gGame.screenTimer < 0x78;
    } else if (type == SCREEN_TYPE_PAK_STATUS) {
        ok = gGame.screenTimer < 0x474;
    } else if (type == SCREEN_TYPE_BACKUP_LOST) {
        ok = gGame.screenTimer < 0x21C;
    } else {
        ok = gGame.screenTimer < 0xF0;
    }
    if (ok) {
        if (PRESSED(0x10) | PRESSED(0x40)) {
            flag = 1;
        }
    }
    if (--gGame.screenTimer == 0 || flag) {
        if (flag && gGame.fadeState == 3) {
            snd_play_menu_sound(1);
        }
        shell_goto_screen(item->target, 0, 1);
    }
}

void shell_controller_settings_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    s32 iv4;
    s32 uv5;
    s32 uVar6;
    s32 cv;
    u32 flags;
    u16 u2;
    u16 uP;

    uVar6 = (s16)gGame.padConfigSel;
    gGame.blinkTimer = gGame.blinkTimer + 1;
    if (gGame.blinkTimer == 0x78) {
        gGame.blinkTimer = 0;
    }
    flags = *buttons;
    if ((flags & 0x20) != 0) {
        shell_goto_screen(screen->back, 0, 0);
        iv4 = 2;
        goto CALLPT;
    }
    if ((flags & 4) != 0) {
        do {
            uP = gGame.padConfigSel;
            u2 = uP - 1;
            gGame.padConfigSel = u2;
            if ((s32)((u32)u2 << 16) < 0) {
                gGame.padConfigSel = uP + 3;
            }
        } while (((gContMask >> (s16)gGame.padConfigSel) & 1) == 0);
        goto LAB_800743d8;
    }
    if ((flags & 8) != 0) {
        do {
            uP = gGame.padConfigSel;
            u2 = uP + 1;
            gGame.padConfigSel = u2;
            if ((s16)u2 > 3) {
                gGame.padConfigSel = uP - 3;
            }
        } while (((gContMask >> (s16)gGame.padConfigSel) & 1) == 0);
        goto LAB_800743d8;
    }
    if ((flags & 0x10) == 0) {
        goto LAB_800743d8;
    }
    cv = gGame.padConfig[uVar6];
    gGame.padConfig[uVar6] = cv + 1;
    if ((s8)(cv + 1) > 3) {
        gGame.padConfig[(s16)gGame.padConfigSel] = gGame.padConfig[(s16)gGame.padConfigSel] - 4;
    }
    iv4 = 1;
CALLPT:
    snd_play_menu_sound(iv4);
LAB_800743d8:
    {
        u8 bits;
        bits = gContMask;
        if (((bits >> (s16)gGame.padConfigSel) & 1) == 0) {
            do {
                uv5 = ((u16)gGame.padConfigSel + 1) & 3;
                gGame.padConfigSel = uv5;
            } while (uv5 != uVar6 && ((bits >> uv5) & 1) == 0);
        }
    }
    if (uVar6 != (s16)gGame.padConfigSel) {
        snd_play_menu_sound(0);
    }
    return;
}

void shell_records_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    s32 t;
    s32 t1;
    s16 w;
    u32 flags;
    s32 c1;
    s32 c2;
    s32 c3;
    s32 c4;
    s32 c5;

    c1 = gGame.menuAnimTimer + 1;
    gGame.menuAnimTimer = c1;
    if (c1 == 0x3c) {
        gGame.menuAnimTimer = 0;
    }
    c2 = gGame.blinkTimer + 1;
    gGame.blinkTimer = c2;
    if (c2 == 0x40) {
        gGame.blinkTimer = 0;
    }
    if (gGame.screenTimer != 0) {
        c3 = gGame.screenTimer - 1;
        gGame.screenTimer = c3;
        if (c3 == 0) {
            if (gGame.screenState != 0xb) {
                if (gGame.screenState == 0xd) {
                    gGame.screenState = 0xb;
                    gGame.screenTimer = 0x3b;
                }
            } else {
                gGame.screenState = 0xc;
            }
        }
    }
    flags = *buttons;
    if ((flags & 0x20) != 0) {
        shell_goto_screen(screen->back, 0, 0);
        snd_play_menu_sound(2);
        return;
    }
    t1 = gGame.recordsSel;
    if ((flags & 1) != 0) {
        if (t1 % 5 != 0) {
            gGame.recordsSel = gGame.recordsSel - 1;
        }
    }
    if ((*buttons & 2) != 0) {
        c4 = gGame.recordsSel;
        if (c4 % 5 < 4 && c4 + 1 < gGame.numRecordTracks) {
            gGame.recordsSel = c4 + 1;
        }
    }
    if ((*buttons & 4) != 0) {
        c5 = gGame.recordsSel;
        if (c5 / 5 + (c5 >> 31) != (c5 >> 31)) {
            gGame.recordsSel = c5 - 5;
        }
    }
    if ((*buttons & 8) != 0) {
        t = gGame.recordsSel;
        w = gGame.numRecordTracks;
        if (t + 5 < w) {
            gGame.recordsSel = t + 5;
        } else if (t / 5 < (s16)(w / 5)) {
            gGame.recordsSel = w - 1;
        }
    }
    if (t1 != gGame.recordsSel) {
        gGame.screenState = 0xd;
        gGame.screenTimer = 6;
        snd_play_menu_sound(0);
    }
}

#define BT gPadInput_2[pad]
#define PRESSED_2(pad, m) ((gPadInput_2[pad] & (m)) && !(gPadInputPrev_2[pad] & (m)))

typedef struct {
    u8 c[3];
} Trip;

void shell_text_entry_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads) {
    s32 isCheat;
    s32 pad;
    s32 sel;
    s32 found;
    s32 i;
    s32 snd;
    s32 b;
    s32 old;
    s32 lo, hi, lo2, hi2;
    u8 *p;
    u8 *q;
    s32 m;

    isCheat = screen->type == SCREEN_TYPE_CHEAT_ENTRY;
    if (++gGame.blinkTimer == 120) {
        gGame.blinkTimer = 0;
    }
    if (isCheat) {
        pad = 0;
    } else {
        sel = gGame.recordPlayer[gGame.entryRecord];
        pad = gGame.playerPad[sel];
    }
    if ((u32)(gGame.screenState - 0x11) < 2) {
        if (buttons[pad] & 0x10) {
            shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 0, 0);
        }
        return;
    }
    b = buttons[pad];
    if (b & 0x10) {
        found = 0;
        if (gGame.keyIndex == 0x26 && gGame.nameLen == 0) {
            snd = 3;
        } else if (isCheat && gGame.keyIndex == 0x27) {
            {
                for (i = 0; i < 14; i++) {
                    p = (u8 *)sCheatTable[i].code;
                    q = gGame.name;
                loop:
                    m = *p;
                    if (m == *q) {
                        if (m == 0) {
                            found = 1;
                            m = 1 << i;
                            gGame.cheatEntered = i;
                            if (m == 4) {
                                gGame.flags ^= 0x20000000;
                            } else {
                                sCheatMask ^= m;
                            }
                        } else {
                            p++;
                            q++;
                            goto loop;
                        }
                    }
                    if (found) {
                        goto ok;
                    }
                }
                snd = 3;
            }
        } else {
        ok:
            snd = 1;
        }
        snd_play_menu_sound(snd);
        if (gGame.keyIndex == 0x26) {
            if (gGame.nameLen != 0) {
                gGame.name[gGame.nameLen--] = 0;
            }
        } else if (gGame.keyIndex == 0x27) {
            if (isCheat) {
                if (found) {
                    gGame.screenState = 0x11;
                } else {
                    gGame.screenState = 0x12;
                }
                return;
            }
            for (i = gGame.entryRecord + 1; i < 4; i++) {
                if (gGame.recordPlayer[i] == gGame.recordPlayer[gGame.entryRecord]) {
                    *(Trip *)gEvent.record.initials[i] = *(Trip *)gGame.name;
                    if (i == 3) {
                        gGame.unk_7A8 = gGame.recordPlayer[i];
                    }
                    gGame.recordPlayer[i] = -1;
                }
            }
            { s32 k = gGame.entryRecord; *(Trip *)gEvent.record.initials[k] = *(Trip *)gGame.name; }
            {
                Trip *t = (Trip *)gGame.playerInitials;
                t[sel] = *(Trip *)gGame.name;
            }
            for (i = gGame.entryRecord + 1; i < 4; i++) {
                if (gGame.recordPlayer[i] != gGame.recordPlayer[gGame.entryRecord] && gGame.recordPlayer[i] != -1) {
                    gGame.entryRecord = i;
                    break;
                }
            }
            if (i < 4) {
                s32 k = gGame.recordPlayer[gGame.entryRecord];
                if (gGame.playerInitials[k][0] != 0) {
                    Trip *t = (Trip *)gGame.playerInitials;
                    *(Trip *)gGame.name = t[k];
                    gGame.name[3] = 0;
                    gGame.nameLen = strlen(gGame.name);
                    gGame.keyCursorX = 0x31F;
                    gGame.keyCursorY = 0x31F;
                    gGame.keyIndex = 0x27;
                } else {
                    gGame.name[0] = 0x41;
                    gGame.name[1] = 0;
                    gGame.name[2] = 0;
                    gGame.name[3] = 0;
                    gGame.nameLen = 0;
                    gGame.keyCursorX = 0;
                    gGame.keyCursorY = 0;
                    gGame.keyIndex = 0;
                }
                {
                    Pic *t = screen->bg;
                    screen->bg = (Pic *)gGame.unk_478;
                    gGame.unk_478 = (Img *)t;
                }
                return;
            }
            shell_goto_screen(screen->items->target, 0, 0);
        } else {
            s32 n = (s16)++gGame.nameLen;
            if (n >= (isCheat ? 10 : 3)) {
                gGame.keyCursorX = 0x31F;
                gGame.keyCursorY = 0x31F;
                gGame.keyIndex = 0x27;
            }
        }
        return;
    }
    if (b & 0x20) {
        if (gGame.nameLen != 0) {
            gGame.name[gGame.nameLen - 1] = gGame.name[gGame.nameLen];
            snd = 1;
            gGame.name[gGame.nameLen--] = 0;
        } else {
            snd = 3;
        }
        snd_play_menu_sound(snd);
        return;
    }
    old = gGame.keyIndex;
    if (gGame.nameLen < (isCheat ? 10 : 3)) {
        s32 x, v;
        s32 col, row;
        lo = 0;
        lo2 = lo;
        hi = 0x320;
        hi2 = hi;
        gGame.keyCursorX += (BT & 0xC) ? ((BT & 8) ? ((BT >> 24) & 0xF) : -((BT >> 24) & 0xF)) : 0;
        if (PRESSED_2(pad, 0x2000)) {
            gGame.keyCursorX -= 100;
        }
        if (PRESSED_2(pad, 0x4000)) {
            gGame.keyCursorX += 100;
        }
        if (gGame.keyCursorX >= hi) {
            gGame.keyCursorX = lo;
        } else if (gGame.keyCursorX < lo) {
            gGame.keyCursorX = hi - 1;
        }
        gGame.keyCursorY -= (BT & 3) ? ((BT & 1) ? ((s32)((u32)BT >> 28) * 3 >> 1) : (-(s32)((u32)BT >> 28) * 3) / 2) : 0;
        if (PRESSED_2(pad, 0x800)) {
            gGame.keyCursorY -= 0xA0;
        }
        if (PRESSED_2(pad, 0x1000)) {
            gGame.keyCursorY += 0xA0;
        }
        if (gGame.keyCursorY >= hi2) {
            gGame.keyCursorY = lo2;
        } else if (gGame.keyCursorY < lo2) {
            gGame.keyCursorY = hi2 - 1;
        }
        col = gGame.keyCursorX;
        row = gGame.keyCursorY;
        gGame.keyIndex = col / 100 + row / 160 * 8;
    } else if (buttons[pad] & 0xC) {
        if (gGame.keyIndex == 0x27) {
            gGame.keyCursorX = 0x258;
            gGame.keyCursorY = 0x31F;
            gGame.keyIndex = 0x26;
        } else {
            gGame.keyCursorX = 0x31F;
            gGame.keyCursorY = 0x31F;
            gGame.keyIndex = 0x27;
        }
    }
    if (gGame.keyIndex < 0x26) {
        if (strlen(gGame.name) == 0) {
            gGame.name[1] = 0;
        }
        gGame.name[gGame.nameLen] = *gKeyboardKeys[gGame.keyIndex];
    } else {
        gGame.name[gGame.nameLen] = 0;
    }
    if (old != gGame.keyIndex) {
        snd_play_menu_sound(0);
    }
}

typedef struct {
    u8 pad[0x50];
    u32 f50;
} Trk_11;

extern Trk_11 *gCurTrack_38;

void shell_race_results_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    u32 fl;
    s32 orig, v, i;

    gGame.menuAnimTimer++;
    if (gGame.menuAnimTimer == 60) {
        gGame.menuAnimTimer = 0;
    }
    gGame.blinkTimer++;
    if (gGame.blinkTimer == 120) {
        gGame.blinkTimer = 0;
    }
    fl = *buttons;
    if (fl & 0x20) {
        if (gGame.screenState == 3) {
            gGame.screenState = 0;
        }
        return;
    }
    if (fl & 0x10) {
        snd_play_menu_sound(1);
        if (gGame.screenState == 3) {
            switch (gGame.menuSel) {
            case 0:
                if (gGame.mode == 8) {
                    if (!(gGame.flags & 0x80000)) {
                        gActiveReplay = 0;
                    }
                    shell_goto_screen(0, 0, 0);
                } else if (gCurTrack_38->f50 & 2) {
                    shell_goto_screen(0, 0, 0);
                    gActiveReplay = 0;
                } else {
                    gActiveReplay = 0;
                    shell_goto_screen(&sScreens[SCREEN_BRONZE_TRACKS], 0, 1);
                }
                break;
            case 1:
                if (gGame.mode == 8) {
                    gGame.pakScreenMode = 2;
                    shell_goto_screen(&sScreens[SCREEN_PAK], 1, 1);
                } else {
                    gGame.resultsNumRiders = gEvent.numRiders;
                    for (i = 0; i < 6; i++) {
                        gGame.resultsOrder[i] = gEvent.finishOrder[i];
                        gGame.resultsTimes[i] = gRiders[i].race.finishTime;
                        gGame.resultsCelebration[i] = (u8)gRiders[i].race.finishPlace;
                        if (gSeason != 0) {
                            gGame.resultsSeasonOrder[i] = gSeason->standings[i];
                        }
                    }
                    shell_goto_screen(0, 0, 1);
                    gReplayPlayback = 1;
                    gGame.returnScreen = screen;
                    gGame.flags |= 0x8000;
                }
                break;
            case 2:
                gGame.flags &= ~0x80000;
                if (gCurTrack_38->f50 & 2) {
                    mem_free((void *)gUserTrackLayout);
                    gUserTrackLayout = 0;
                }
                shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 0, 1);
                gActiveReplay = 0;
                break;
            }
        } else if (screen->type == SCREEN_TYPE_SEASON_RACE_RESULT) {
            shell_goto_screen(&sScreens[SCREEN_SEASON_STANDINGS], 1, 1);
        } else {
            gGame.menuSel = 0;
            gGame.screenState = 3;
        }
        return;
    }
    if (gGame.screenState != 3) {
        return;
    }
    orig = gGame.menuSel;
    if (fl & 0x801) {
        if (gReplayWritePtr - gReplayBuf >= gReplayBufSize || gGame.mode == 8) {
            gGame.menuSel = orig ^ 2;
        } else {
            gGame.menuSel = orig - 1;
            if (orig - 1 < 0) {
                gGame.menuSel = orig + 2;
            }
        }
    }
    if (*buttons & 0x1002) {
        if (gReplayWritePtr - gReplayBuf >= gReplayBufSize || gGame.mode == 8) {
            gGame.menuSel ^= 2;
        } else {
            v = gGame.menuSel;
            gGame.menuSel = v + 1;
            if (v + 1 >= 3) {
                gGame.menuSel = v - 2;
            }
        }
    }
    if (gGame.menuSel != orig) {
        snd_play_menu_sound(0);
    }
}


void shell_season_standings_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    u32 b;
    Cup *t;
    s32 i;
    s32 d;

    if (++gGame.menuAnimTimer == 0x3C) {
        gGame.menuAnimTimer = 0;
    }
    if (++gGame.blinkTimer == 0x78) {
        gGame.blinkTimer = 0;
    }
    b = *buttons;
    if (b & 0x20) {
        snd_play_menu_sound(2);
        if (gGame.screenState == 4) {
            gGame.screenState = 0;
            return;
        }
        if (gGame.screenState == 7) {
            gGame.screenState = 4;
            return;
        }
        if (screen->back != NULL) {
            shell_goto_screen(screen->back, 0, 0);
        }
    } else if (b & 0x10) {
        snd_play_menu_sound(1);
        if (gGame.screenState == 4) {
            switch (gGame.menuSel) {
            case 0:
                gGame.flags |= 0x80;
                t = gSeason->cup;
                if (gSeason->round + 1 >= t->numRounds) {
                    if (gSeason->standings[0] == 0 && (t - gCups != 4 || gEvent.finishOrder[0] == 0)) {
                        gGame.flags = (gGame.flags & ~0x80) | 0x40000;
                        shell_goto_screen(0, 0, 1);
                        gActiveReplay = 0;
                        return;
                    }
                } else if (t - gCups != 4 || gEvent.finishOrder[0] == 0) {
                    shell_goto_screen(0, 0, 1);
                    gSeason->round++;
                    race_jitter_ai_order(gGame.numPlayers, gEvent.numRiders - gGame.numPlayers);
                    race_grid_from_standings();
                    gActiveReplay = 0;
                    return;
                }
                shell_goto_screen(&sScreens[SCREEN_ROUND_OVER], 0, 1);
                gActiveReplay = 0;
                return;
            case 1:
                gGame.resultsNumRiders = gEvent.numRiders;
                for (i = 0; i < 6; i++) {
                    gGame.resultsOrder[i] = gEvent.finishOrder[i];
                    gGame.resultsTimes[i] = gRiders[i].race.finishTime;
                    gGame.resultsCelebration[i] = (u8)gRiders[i].race.finishPlace;
                    if (gSeason != 0) {
                        gGame.resultsSeasonOrder[i] = gSeason->standings[i];
                    }
                }
                shell_goto_screen(0, 0, 1);
                gReplayPlayback = 1;
                gGame.returnScreen = screen;
                gGame.flags |= 0x8000;
                return;
            case 2:
                gGame.screenState = 7;
                gGame.dialogSel = 0;
                return;
            }
        } else {
            if (gGame.screenState == 7) {
                switch (gGame.dialogSel) {
                case 1:
                    shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 0, 1);
                    gActiveReplay = 0;
                    return;
                case 0:
                    gGame.screenState = 4;
                    return;
                }
            } else {
                gGame.menuSel = 0;
                gGame.screenState = 4;
            }
        }
    } else {
        if (gGame.screenState == 7) {
            if (b & 3) {
                gGame.dialogSel ^= 1;
                snd_play_menu_sound(0);
            }
        } else if (gGame.screenState == 4) {
            if (b & 0x801) {
                d = gReplayWritePtr - gReplayBuf;
                if (gGame.mode != 8 ? d >= gReplayBufSize : d >= 0x2800) {
                    gGame.menuSel ^= 2;
                } else {
                    if (--gGame.menuSel < 0) {
                        gGame.menuSel += 3;
                    }
                }
                snd_play_menu_sound(0);
            }
            if (*buttons & 0x1002) {
                snd_play_menu_sound(0);
                d = gReplayWritePtr - gReplayBuf;
                if (gGame.mode != 8 ? d >= gReplayBufSize : d >= 0x2800) {
                    gGame.menuSel ^= 2;
                } else {
                    if (++gGame.menuSel >= 3) {
                        gGame.menuSel -= 3;
                    }
                }
            }
        }
    }
}

void shell_options_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads) {
    s32 m;
    s32 old;
    s32 d;

    if (++gGame.menuAnimTimer == 60) {
        gGame.menuAnimTimer = 0;
    }
    if (gGame.optionsSel & 0x100) {
        gGame.optionsSel &= ~0x100;
        if (gGame.optionsSel == 0) {
            tunes_play_song(D_800D11D8);
        }
    }
    if ((gPadInput & 0x20) && !(gPadInputPrev & 0x20) && gGame.fadeState == 3) {
        if (gGame.screenState == 6) {
            gGame.screenState = 0;
            snd_play_menu_sound(2);
        } else if (gGame.screenState != 14 && gGame.screenState != 15) {
            shell_goto_screen(screen->back, 0, 0);
            sfx_stop_all();
            tunes_stop_speech();
            if (tunes_is_song_playing() != 0) {
                tunes_stop_song();
            }
            snd_play_menu_sound(2);
        }
        gCreditsMode = 0;
        return;
    }
    if ((gPadInput & 0x10) && !(gPadInputPrev & 0x10) && gGame.fadeState == 3) {
        switch (gGame.optionsSel) {
        case 3:
                        gSaveData.f2C4 ^= 1;
            snd_play_menu_sound(1);
            return;
        case 8:
                        gSaveData.f2C4 ^= 0x10;
            snd_play_menu_sound(1);
            return;
        case 1:
            sfx_play(gGame.optionsSfx, 0x2B11, D_800F1BC6, 7, 1, 1);
            return;
        case 2:
            if (gGame.repeatTimer == 0) {
                tunes_queue_speech(0x9B);
                gGame.repeatTimer = 0x34;
            }
            return;
        case 10:
                        snd_play_menu_sound(3);
            return;
        case 9:
                        snd_play_menu_sound(3);
            return;
        case 7:
            if (gHasExpansionPak == 0) {
                return;
            }
                        gSaveData.f2C4 ^= 2;
            snd_play_menu_sound(1);
            return;
        case 4:
            shell_goto_screen(&sScreens[SCREEN_CONTROLLER_SETTINGS], 1, 1);
                        snd_play_menu_sound(1);
            return;
        case 5:
            shell_goto_screen(&sScreens[SCREEN_RECORDS], 1, 1);
                        snd_play_menu_sound(1);
            return;
        case 6:
            shell_goto_screen(NULL, 0, 1);
                        gCreditsMode = 1;
            gGame.mode = 0;
            gCurTrack = sTrackGravelPit;
            gGame.numPlayers = 1;
            gGame.returnScreen = screen;
            {
                s32 f = gGame.flags & ~0x80;
                *(volatile s32 *)&gGame.flags = f;
                gGame.flags = f | 0x05000000;
            }
            snd_play_menu_sound(1);
            return;
        }
        return;
    }
    m = gGame.screenState;
    if (m == 6 || m == 15 || m == 14) {
        if (m == 15) {
            save_load(1);
            gGame.screenState = 0;
        } else if (m != 14) {
            if (*buttons & 3) {
                gGame.dialogSel ^= 1;
                snd_play_menu_sound(0);
            }
        }
        return;
    }
    old = gGame.optionsSel;
    if (*buttons & 2) {
        if (old < 3) {
            gGame.optionsSel += 1;
        } else {
            gGame.optionsSel += 2;
        }
        if (gGame.optionsSel >= 11) {
            gGame.optionsSel = 0;
        }
    }
    if (*buttons & 1) {
        if (gGame.optionsSel < 4) {
            gGame.optionsSel -= 1;
        } else {
            gGame.optionsSel -= 2;
        }
        if (gGame.optionsSel < 0) {
            gGame.optionsSel = 9;
        }
    }
    if (old != gGame.optionsSel) {
        switch (old) {
        case 2:
            tunes_stop_speech();
            break;
        case 0:
            tunes_stop_song();
            break;
        case 1:
            break;
        }
        switch (gGame.optionsSel) {
        case 2:
            tunes_queue_speech(0x9B);
            gGame.repeatTimer = 0x34;
            break;
        case 0:
            tunes_play_song(D_800D11D8);
            break;
        case 1:
            sfx_play(gGame.optionsSfx, 0x2B11, D_800F1BC6, 7, 1, 1);
            break;
        }
    }
    if (gGame.optionsSel >= 3) {
        if (*buttons & 0xC) {
            if ((gGame.optionsSel - 3) & 1) {
                gGame.optionsSel--;
            } else {
                gGame.optionsSel++;
            }
        }
    } else {
        if (gGame.repeatTimer != 0) {
            gGame.repeatTimer--;
        }
        if ((gPadInput & 0x2000) && !(gPadInputPrev & 0x2000)) {
            d = -8;
        } else if ((gPadInput & 0x4000) && !(gPadInputPrev & 0x4000)) {
            d = 8;
        } else {
            s32 b = gPadInput;
            if (b & 0xC) {
                d = ((b & 8) ? ((b >> 24) & 0xF) : -((b >> 24) & 0xF)) / 3;
            } else {
                d = 0;
            }
        }
        if (d != 0 && gGame.repeatTimer == 0) {
            switch (gGame.optionsSel) {
            case 1:
                sfx_play(gGame.optionsSfx, 0x2B11, D_800F1BC6, 7, 1, 1);
                gGame.repeatTimer = 0x28;
                break;
            case 2:
                tunes_queue_speech(0x9B);
                gGame.repeatTimer = 0x34;
                break;
            }
        }
        if (d < 0) {
            switch (gGame.optionsSel) {
            case 1:
                if (gSaveData.f2C6 >= -d) {
                    gSaveData.f2C6 += d;
                } else {
                    gSaveData.f2C6 = 0;
                }
                break;
            case 0:
                if (gSaveData.f2C5 >= -d) {
                    gSaveData.f2C5 += d;
                } else {
                    gSaveData.f2C5 = 0;
                }
                break;
            case 2:
                if (gSaveData.f2C7 >= -d) {
                    gSaveData.f2C7 += d;
                } else {
                    gSaveData.f2C7 = 0;
                }
                break;
            }
        } else {
            s32 t;
            switch (gGame.optionsSel) {
            case 1:
                t = gSaveData.f2C6 + d;
                if (t >= 256) {
                    gSaveData.f2C6 = 255;
                } else {
                    gSaveData.f2C6 = t;
                }
                break;
            case 0:
                t = gSaveData.f2C5 + d;
                if (t >= 256) {
                    gSaveData.f2C5 = 255;
                } else {
                    gSaveData.f2C5 = t;
                }
                break;
            case 2:
                t = gSaveData.f2C7 + d;
                if (t >= 256) {
                    gSaveData.f2C7 = 255;
                } else {
                    gSaveData.f2C7 = t;
                }
                break;
            }
        }
        snd_set_volume(1, gSaveData.f2C5);
        snd_set_volume(2, gSaveData.f2C7);
        snd_set_volume(0, gSaveData.f2C6);
    }
    if (gGame.optionsSel != old) {
        snd_play_menu_sound(0);
    }
}

void shell_rider_select_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    Game *g;
    Game *h;
    Game *k;
    Game *m;
    s32 i;
    s32 ok;
    s32 idx;
    s32 t;
    u32 mask;

    g = &gGame;
    for (i = 0; i < 4; i++) {
        if (gGame.riderSelectTimer[i] != 0) {
            gGame.riderSelectTimer[i]--;
        }
        gGame.riderSelectRot[i][0] = (gGame.riderSelectRot[i][0] + 0x23) & 0xFFFF;
    }

    ok = 1;
    h = &gGame;
    for (i = 0; i < h->numPlayers; i++) {
        if (!(gGame.flags & (0x400 << i))) {
            ok = 0;
        }
    }
    race_update(D_800CD090);

    k = &gGame;
    if (k->numPlayers == 1) {
        if (*buttons & 0x780) snd_play_menu_sound(0);
        if (*buttons & 0x200) k->riderSkinSel = (k->riderSkinSel + 5) % 6;
        if (*buttons & 0x400) k->riderSkinSel = (k->riderSkinSel + 1) % 6;
        if (k->mode != 8) {
            if (*buttons & 0x80) k->bikeSkinSel = (k->bikeSkinSel + 5) % 6;
            if (*buttons & 0x100) k->bikeSkinSel = (k->bikeSkinSel + 1) % 6;
        }
    }

    if ((*buttons & 0x50) && ok) {
        snd_play_menu_sound(1);
        if (gGame.savedMode < 8) {
            if (gGame.savedMode >= 2) {
                shell_goto_screen(0, 1, 1);
            } else {
                shell_goto_screen(screen->items->target, 1, 1);
            }
        } else {
            shell_goto_screen(screen->items->target, 1, 1);
        }
        return;
    }
    if (*buttons & 0x20) {
        if (!(gGame.flags & 0x400)) {
            if (screen->back != NULL) {
                if (screen->back == &sScreens[SCREEN_PAK] && gUserTrackLayout != 0) {
                    gGame.flags |= 0x2000000;
                }
                shell_goto_screen(screen->back, 0, 0);
                snd_play_menu_sound(2);
            }
            return;
        }
    }
    m = &gGame;
    if (m->fadeState != 3) return;
    for (i = 0; i < gGame.numPlayers; i++) {
        if (!((gContMask >> (idx = gGame.playerPad[i])) & 1)) continue;
        if (gGame.riderSelectState[i] == 0 && !(gGame.flags & (0x400 << i))) {
            if (buttons[idx] & 4) {
                t = gGame.riders[i];
                gGame.riders[i] = t - 1;
                if (t - 1 < 0) gGame.riders[i] = t + 5;
                if (gGame.numPlayers == 1) gGame.riderSkinSel = (gGame.riderSkinSel + 5) % 6;
                snd_play_menu_sound(0);
            }
            if (buttons[gGame.playerPad[i]] & 8) {
                t = gGame.riders[i];
                gGame.riders[i] = t + 1;
                if (t + 1 >= 6) gGame.riders[i] = t - 5;
                if (gGame.numPlayers == 1) gGame.riderSkinSel = (gGame.riderSkinSel + 1) % 6;
                snd_play_menu_sound(0);
            }
        }
        if (gGame.numPlayers >= 3 || gGame.riderSelectState[i] == 0) {
            if (!(gGame.flags & (0x400 << i)) && (buttons[gGame.playerPad[i]] & 0x50)) {
                gGame.riderSelectTimer[i] = 0x28 - gGame.riderSelectTimer[i];
                gGame.flags |= 0x400 << i;
                snd_play_menu_sound(1);
            } else if ((gPadInput & 0x20000) && gGame.numPlayers == 1) {
                gGame.riderSelectState[i] = 1;
                gGame.riderSelectAnim[i] = 0x3D;
            }
        } else {
            if (gGame.riderSelectState[i] == 1) {
                if (--gGame.riderSelectAnim[i] <= 0) {
                    gGame.riderSelectState[i] = 2;
                } else if (!(gPadInput & 0x20000)) {
                    gGame.riderSelectState[i] = 3;
                    gGame.riderSelectAnim[i] = 0x3E - gGame.riderSelectAnim[i];
                }
            } else if (gGame.riderSelectState[i] == 2) {
                if (!(gPadInput & 0x20000)) {
                    gGame.riderSelectState[i] = 3;
                    gGame.riderSelectAnim[i] = 0x3D;
                }
            } else if (gGame.riderSelectState[i] == 3) {
                if (gPadInput & 0x20000) {
                    gGame.riderSelectState[i] = 1;
                    gGame.riderSelectAnim[i] = 0x3E - gGame.riderSelectAnim[i];
                }
                if (--gGame.riderSelectAnim[i] <= 0) {
                    gGame.riderSelectState[i] = 0;
                }
            }
        }
        mask = 0x400 << i;
        if ((gGame.flags & mask) && (buttons[gGame.playerPad[i]] & 0x20)) {
            snd_play_menu_sound(2);
            gGame.riderSelectTimer[i] = 0x28 - gGame.riderSelectTimer[i];
            gGame.flags &= ~mask;
        }
    }
}

#define OK (gSaveData_9.f2C0 > gGame.difficulty \
    || (gSaveData_9.f2C0 == gGame.difficulty && (*screen->cursor == 0 || ((gSaveData_9.f2BF >> (*screen->cursor - 1)) & 1))) \
    || (gGame.mode == 1 && gSeason != NULL && gSeason->cup - gCups == 5 \
        && gSaveData_9.f2C0 == gGame.difficulty && (gSaveData_9.f2BF & 8)))

typedef struct {
    u8 pad0[0x2A8];
    u8 f2A8[5];
    u8 pad2AD[0x2BF - 0x2AD];
    u8 f2BF;
    u8 f2C0;
} Save_3;

extern Save_3 gSaveData_9;

void shell_season_select_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads) {
    s32 k, j, i;

    if (++gGame.menuAnimTimer == 60) gGame.menuAnimTimer = 0;
    if (++gGame.blinkTimer == 64) gGame.blinkTimer = 0;
    switch (gGame.seasonMenuState) {
    case 0:
        if (++gGame.cupPreviewTimer == 60) {
            if (++gGame.cupPreviewTrack >= gCups[*screen->cursor].numRounds) gGame.cupPreviewTrack = 0;
            gGame.cupPreviewTimer = 0;
        }
        if (*screen->cursor == 5 && gCustomRound != 0) {
            if (*buttons & 1) {
                s32 v = gGame.roundSel;
                gGame.roundSel = v - 1;
                if (v - 1 < 0) gGame.roundSel = v + 1;
            }
            if (*buttons & 2) {
                s32 v = gGame.roundSel;
                if (++gGame.roundSel >= 2) gGame.roundSel -= 2;
            }
        }
        if (gGame.difficulty != 0 && (*buttons & 0xC)) {
            if (*buttons & 8) {
                s32 *p = screen->cursor;
                s32 v = *p;
                switch (gGame.difficulty) {
                case 1:
                    if (v <= 0) *p = v + 1;
                    break;
                case 2:
                    if (v < 2) *p = v + 1;
                    break;
                case 3:
                    if (v < 5) *p = v + 1;
                    break;
                }
                if (v != *screen->cursor) {
                    snd_play_menu_sound(0);
                    gGame.cupPreviewTimer = 0;
                    gGame.cupPreviewTrack = 0;
                }
            } else if (*screen->cursor > 0) {
                snd_play_menu_sound(0);
                (*screen->cursor)--;
                gGame.cupPreviewTimer = 0;
                gGame.cupPreviewTrack = 0;
            }
        }
        if ((*buttons & 0x50) && !OK) {
            snd_play_menu_sound(3);
        } else if ((*buttons & 0x50) && OK) {
            if (*screen->cursor == 5 && (gGame.roundSel == 1 || gCustomRound == 0)) {
                gGame.seasonMenuState = 1;
                gGame.roundSel = 0;
            } else if (!(item->flags & SCREEN_ITEM_LOCKED)) {
                shell_goto_screen(item->target, 1, 1);
                snd_play_menu_sound(1);
            } else {
                snd_play_menu_sound(3);
            }
        } else if ((*buttons & 0x20) && screen->back != NULL) {
            shell_goto_screen(screen->back, 0, 0);
            snd_play_menu_sound(2);
        }
        break;
    case 1:
        if ((*buttons & 1) && gGame.roundSel != 0) gGame.roundSel--;
        if (gGame.roundSel < gCups[*screen->cursor].numRounds) {
            if (*buttons & 4) {
                if (gCups[*screen->cursor].laps[gGame.roundSel] >= 2) gCups[*screen->cursor].laps[gGame.roundSel]--;
            }
            if (*buttons & 8) {
                if (gCups[*screen->cursor].laps[gGame.roundSel] < 6) gCups[*screen->cursor].laps[gGame.roundSel]++;
            }
        }
        if ((*buttons & 2) && gGame.roundSel < gCups[*screen->cursor].numRounds) {
            s32 v = gGame.roundSel + 1;
            if (v < 5) gGame.roundSel = v;
        }
        k = *buttons;
        if (k & 0x50) {
            u16 w;
            gGame.seasonMenuState = 2;
            if (gGame.roundSel == gCups[*screen->cursor].numRounds) {
                gGame.customTrackSel = 0;
                gCups[*screen->cursor].progress[0]++;
                gCups[*screen->cursor].progress[1]++;
                gCups[*screen->cursor].progress[2]++;
                gCups[*screen->cursor].numRounds++;
                gGame.customTrackPrev = -1;
            } else {
                w = gCups[*screen->cursor].tracks[gGame.roundSel];
                gGame.customTrackSel = w;
                gGame.customTrackPrev = w;
            }
            gCups[*screen->cursor].tracks[gGame.roundSel] = gGame.customTrackSel;
        } else if (k & 0x20) {
            gGame.seasonMenuState = 0;
            gGame.roundSel = 0;
            for (i = 0; i < 5; i++) {
                if (i < gCups[5].numRounds) gSaveData_9.f2A8[i] = gCups[5].tracks[i] & 0x1F;
                else gSaveData_9.f2A8[i] = 0x3F;
                gSaveData_9.f2A8[i] |= (gCups[5].laps[i] & 7) << 5;
            }
            save_write(1, 0);
        } else {
            if (gGame.roundSel != gCups[*screen->cursor].numRounds && (k & 0x20000)) {
                for (j = gGame.roundSel; j < gCups[*screen->cursor].numRounds - 1; j++) {
                    gCups[*screen->cursor].tracks[j] = gCups[*screen->cursor].tracks[j + 1];
                }
                { u8 m = --gCups[*screen->cursor].numRounds;
                gCups[*screen->cursor].tracks[m] = 0; }
            }
        }
        break;
    case 2:
        if (*buttons & 4) {
            u16 v = gGame.customTrackSel;
            gGame.customTrackSel = v - 1;
            if ((s16)v % 5 == 0) gGame.customTrackSel = v + 4;
        }
        if (*buttons & 8) {
            u16 v = gGame.customTrackSel;
            gGame.customTrackSel = v + 1;
            if ((s16)(v + 1) % 5 == 0) gGame.customTrackSel = v - 4;
        }
        if (*buttons & 1) {
            gGame.customTrackSel -= 5;
            if (gGame.customTrackSel < 0) gGame.customTrackSel += 20;
        }
        if (*buttons & 2) {
            u16 v = gGame.customTrackSel;
            gGame.customTrackSel = v + 5;
            if (gGame.customTrackSel >= 20) gGame.customTrackSel = v - 15;
        }
        k = *buttons;
        if (k & 0x50) {
            gGame.seasonMenuState = 1;
        } else if (k & 0x20) {
            gGame.seasonMenuState = 1;
            if (gGame.customTrackPrev == -1) gCups[*screen->cursor].numRounds--;
            else gGame.customTrackSel = gGame.customTrackPrev;
        }
        gCups[*screen->cursor].tracks[gGame.roundSel] = gGame.customTrackSel;
        break;
    }
    gSeason->cup = &gCups[*screen->cursor];
}

typedef struct {
    u8 pad[0x2BF];
    u8 mask;
    u8 cup;
} Save_4;

extern Save_4 gSaveData_10;

s32 shell_is_round_unlocked(s32 n)
{
    if (gGame.difficulty == gSaveData_10.cup) {
        s32 r = 0;
        if (n == 0 || ((gSaveData_10.mask >> (n - 1)) & 1)) {
            r = 1;
        }
        return r;
    }
    switch (gSaveData_10.cup) {
    case 1:
        if (n < 2 && ((gSaveData_10.mask >> (n - 1)) & 1)) return 1;
        break;
    case 2:
        if (n < 2 || (n == 2 && ((gSaveData_10.mask >> (n - 1)) & 1))) return 1;
        break;
    case 3:
        if (n < 3 || (n == 3 && ((gSaveData_10.mask >> (n - 1)) & 1))) return 1;
        break;
    }
    return 0;
}

typedef struct {
    u8 pad0[0x2C0];
    u8 b2C0;
    u8 pad2C1[3];
    u8 b2C4;
} S1900;

extern S1900 gSaveData_11;

static inline s32 canGo(Screen *n) {
    s32 k;
    if (gGame.mode == 8 || gGame.mode == 0) {
        k = gSaveData_11.b2C0;
    } else {
        k = gGame.difficulty;
    }
    switch (k) {
    case 1:
        if (n->firstItem < 10) return 1;
        break;
    case 2:
        if (n->firstItem < 15) return 1;
        break;
    case 3:
        return 1;
    }
    return 0;
}

/* sCurScreen again: the read takes its own lui, which the stores below do not share */
extern Screen *sCurScreen_2;

void shell_list_menu_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads) {
    s32 snd;
    s32 m;
    s32 b;
    s32 s;
    s32 v;
    Screen *prev;
    Screen *n;
    s32 k;
    s32 ok;

    gGame.unk_29C = D_800CDB5C;
    gGame.unk_2A0 = D_800CDB60;
    D_800CDB60 -= (gPadInput & 3) ? ((gPadInput & 1) ? (s32)(gPadInput >> 28) / 6.0f : -(s32)(gPadInput >> 28) / 6.0f) : 0.0f;
    if (++gGame.menuAnimTimer == 60) gGame.menuAnimTimer = 0;
    if (++gGame.blinkTimer == 64) gGame.blinkTimer = 0;
    b = *buttons;
    if (b & 0x50) {
        snd = 0;
        if (!(item->flags & SCREEN_ITEM_LOCKED)) {
            snd = 1;
        } else {
            snd_play_menu_sound(3);
        }
        if (screen - sScreens == SCREEN_MAIN_MENU && (gPadInput & 0x8500) == 0x8500) {
            shell_goto_screen(&sScreens[SCREEN_CHEAT_ENTRY], 1, 0);
        } else if ((item->flags & SCREEN_ITEM_ASK_GHOST) && gGame.screenState != 8) {
            gGame.screenState = 8;
            gGame.dialogSel = 0;
        } else if ((item->flags & SCREEN_ITEM_ASK_USER_TRACK) && gGame.screenState != 16 && gGame.screenState != 9
                   && gGame.screenState != 10 && !(gGame.flags & 0x200)) {
            if (!(item->flags & SCREEN_ITEM_LOCKED)) {
                gGame.screenState = 9;
                gGame.dialogSel = 0;
            }
        } else if (gGame.screenState == 16) {
            gGame.mode = 0;
            gGame.screenState = 0;
            gGame.flags |= 0x200;
        } else if (!(item->flags & SCREEN_ITEM_ASK_PLAYERS) || gGame.screenState == 5) {
          if (!(item->flags & SCREEN_ITEM_LOCKED)) {
            if (gGame.screenState == 8 && gGame.dialogSel == 1) {
                gGame.pakScreenMode = 1;
                shell_goto_screen(&sScreens[SCREEN_PAK], 1, 1);
            } else if (gGame.screenState == 10 && !(gGame.flags & 0x200)) {
                gGame.screenState = 16;
                if (gGame.difficulty == 0) {
                    gGame.difficulty = 1;
                }
            } else if (gGame.screenState == 9 && gGame.dialogSel == 0) {
                gGame.screenState = 10;
                if (gSaveData_11.b2C4 & 8) {
                    gGame.dialogSel = 0;
                } else {
                    gGame.dialogSel = 1;
                }
            } else {
                if (gGame.flags & 0x200) {
                    if (gGame.dialogSel != 0) {
                        gGame.flags |= 0x2000000;
                        gGame.pakScreenMode = 3;
                        gGame.flags &= ~0x20;
                        shell_goto_screen(&sScreens[SCREEN_PAK], 1, 1);
                    } else {
                        gCurTrack = (s32)D_800CBF30;
                        shell_goto_screen(&sScreens[SCREEN_RIDER_SELECT], 1, 1);
                        usrtrk_load_layout(D_800F1DF4);
                        gGame.mode = 0;
                        gGame.flags &= ~0x80;
                        gGame.flags &= ~0x20;
                    }
                }
                if (gGame.screenState == 5 && gGame.difficulty == 0) {
                    shell_goto_screen(0, 0, 1);
                } else {
                    shell_goto_screen(item->target, 1, 1);
                }
            }
          }
        } else {
            gGame.screenState = 5;
            if ((item->flags & SCREEN_ITEM_NEED_DIFFICULTY) && gGame.difficulty == 0) {
                gGame.difficulty = 1;
            }
        }
        if (snd) {
            snd_play_menu_sound(1);
        }
    } else if (b & 0x20) {
        if (gGame.flags & 0x200) {
            gGame.screenState = 16;
            snd_play_menu_sound(2);
            gGame.flags &= ~0x200;
            gGame.flags &= ~0x20;
            return;
        }
        s = gGame.screenState;
        if (s == 16) {
            gGame.screenState = 10;
        } else if (s == 8 || s == 5 || s == 9 || s == 10) {
            snd_play_menu_sound(2);
            gGame.screenState = 0;
            gGame.flags &= ~0x20;
        } else if (screen->back) {
            snd_play_menu_sound(2);
            shell_goto_screen(screen->back, 0, 0);
        } else {
            snd_play_menu_sound(3);
        }
    } else if (gGame.flags & 0x200) {
        m = gGame.mode;
        if (m == 2) {
            if (gContCount < 2) {
                snd_play_menu_sound(2);
                gGame.flags &= ~0x200;
            } else if (gGame.numPlayers == 0) {
                gGame.numPlayers = 2;
            } else if (gContCount < 4 && gGame.numPlayers == 4) {
                snd_play_menu_sound(0);
                gGame.numPlayers = 2;
            }
            if (gContCount == 4 && (*buttons & 3)) {
                snd_play_menu_sound(0);
                if (gGame.numPlayers == 2) {
                    gGame.numPlayers = 4;
                } else {
                    gGame.numPlayers = 2;
                }
            }
        } else if (m == 7 || m == 10) {
            if (gGame.numPlayers == 0 || gContMask == 0) {
                gGame.numPlayers = 1;
            } else {
                s32 o = gGame.numPlayers;
                if (gContCount < o) {
                    gGame.numPlayers = gContCount;
                } else if (b & 3) {
                    gGame.numPlayers = (o & 1) + 1;
                    if (gContCount == 1) {
                        gGame.numPlayers = 1;
                    }
                    if (gGame.numPlayers != o) {
                        snd_play_menu_sound(0);
                    }
                }
            }
        } else if (gContMask == 0) {
            gGame.numPlayers = 1;
        } else {
            if (gGame.numPlayers == 0) {
                gGame.numPlayers = 1;
            }
            v = gContCount;
            if (v < gGame.numPlayers) {
                gGame.numPlayers = v;
            }
            if (*buttons & 1) {
                s32 old = gGame.numPlayers;
                if ((gGame.numPlayers = old - 1) <= 0) {
                    gGame.numPlayers = v;
                }
                if (gGame.numPlayers != old) {
                    snd_play_menu_sound(0);
                }
            }
            if (*buttons & 2) {
                s32 old = gGame.numPlayers;
                gGame.numPlayers = old + 1;
                if (gContCount < old + 1) {
                    gGame.numPlayers = 1;
                }
                if (gGame.numPlayers != old) {
                    snd_play_menu_sound(0);
                }
            }
            if (gGame.mode == 0 && gGame.numPlayers == 2) {
                if (*buttons & 4) {
                    s32 old;
                    snd_play_menu_sound(0);
                    old = gGame.twoPlayerCpuRiders;
                    if ((gGame.twoPlayerCpuRiders = old - 1) < 0) {
                        gGame.twoPlayerCpuRiders = old + 2;
                    }
                }
                if (*buttons & 8) {
                    snd_play_menu_sound(0);
                    if (++gGame.twoPlayerCpuRiders >= 3) {
                        gGame.twoPlayerCpuRiders -= 3;
                    }
                }
            }
        }
    } else {
        s = gGame.screenState;
        if ((u32)(s - 8) < 3) {
            if ((s != 10 || (gSaveData_11.b2C4 & 8)) && (b & 3)) {
                snd_play_menu_sound(0);
                gGame.dialogSel ^= 1;
            }
            return;
        }
        if (b & 1) {
            if ((s == 16 || s == 5) && gSaveData_11.b2C0) {
                s32 old = gGame.difficulty;
                if ((gGame.difficulty = old - 1) < 0) {
                    gGame.difficulty = gSaveData_11.b2C0;
                } else if (gGame.difficulty == 0 && ((item->flags & SCREEN_ITEM_NEED_DIFFICULTY) || s == 16)) {
                    gGame.difficulty = gSaveData_11.b2C0;
                }
                if (gGame.difficulty != old) {
                    snd_play_menu_sound(0);
                }
            } else {
                s32 old = *screen->cursor;
                if ((*screen->cursor = old - 1) < 0) {
                    *screen->cursor = screen->numItems - 1;
                }
                if (*screen->cursor != old) {
                    snd_play_menu_sound(0);
                }
            }
        }
        if (*buttons & 2) {
            s32 t = gGame.screenState;
            if ((t == 16 || t == 5) && gSaveData_11.b2C0) {
                s32 old = gGame.difficulty;
                gGame.difficulty = old + 1;
                if (gSaveData_11.b2C0 < old + 1) {
                    if ((item->flags & SCREEN_ITEM_NEED_DIFFICULTY) || t == 16) {
                        gGame.difficulty = 1;
                    } else {
                        gGame.difficulty = 0;
                    }
                }
                if (gGame.difficulty != old) {
                    snd_play_menu_sound(0);
                }
            } else {
                s32 old = *screen->cursor;
                *screen->cursor = old + 1;
                if (old + 1 >= screen->numItems) {
                    *screen->cursor = 0;
                }
                if (*screen->cursor != old) {
                    snd_play_menu_sound(0);
                }
            }
        }
        if (gGame.fadeState != 1) {
            prev = sCurScreen_2;
            if (*buttons & 4) {
                if (screen->left == NULL) {
                    if (screen->right) {
                        sCurScreen = screen;
                    }
                } else {
                    screen = screen->left;
                    sCurScreen = screen;
                }
            }
            if (*buttons & 8) {
                if (screen->right == NULL) {
                    if (screen->left) {
                        sCurScreen = screen;
                    }
                } else {
                    if (canGo(screen->right)) {
                        screen = screen->right;
                    }
                    sCurScreen = screen;
                }
            }
            if (prev != sCurScreen) {
                snd_play_menu_sound(0);
            }
        }
    }
}

void shell_filter_stick_dirs(u32 *p)
{
    u32 v;
    s8 a2;
    s8 a1;
    s8 a3;
    s32 i;

    for (i = 0; i < 4; i++) {
        v = *p;
        a2 = (v >> 24) & 0xF;
        a1 = v >> 28;
        a3 = a1;
        if (a2 < 7) {
            *p = v & 0xFFFFFFF3;
            a2 = 0;
        }
        if (a1 < 7) {
            *p = *p & 0xFFFFFFFC;
            a3 = 0;
        }
        if (a2 < a3) {
            *p = *p & 0xFFFFFFF3;
        } else if (a3 < a2) {
            *p = *p & 0xFFFFFFFC;
        }
        p++;
    }
}

void shell_update(Pad16 *pad) {
    u32 buf[4];
    s32 sp20;
    s32 sp24;
    s32 sp28;
    Screen *screen;
    ScreenItem *item;
    Screen *p;
    s32 vol;
    s32 i;
    s32 any;

    mem_fill((u8 *)buf, 0, 16);
    if (!(gFrameCounter & 0xF)) {
        pak_query_controllers();
    }
    if (gContFirst || !gContMask) {
        gGame.flags |= 0x08000000;
    } else {
        gGame.flags &= ~0x08000000;
    }
    if (gContCount < 2) {
        gSpecialTracksItems[3].flags |= SCREEN_ITEM_LOCKED;
    } else {
        save_update_unlocks();
    }
    if (gGame.fadeState == 0) {
        screen = sPrevScreen;
        if (screen == &sScreens[SCREEN_OPTIONS] ||
            (!(screen->flags & SCREEN_KEEP_MUSIC) && (sCurScreen == NULL || (sCurScreen->flags & SCREEN_KEEP_MUSIC)))) {
            if (gGame.fadeState == 0) {
                vol = gGame.fadeLevel;
            }
            snd_set_master_volume(vol);
        }
    } else {
        screen = sCurScreen;
    }
    if (screen != NULL && (screen->flags & SCREEN_STICK_FILTER)) {
        shell_filter_stick_dirs(pad->w);
    }
    any = 0;
    for (i = 0; i < 4; i++) {
        if (((gContMask >> i) & 1) && pad->w[i] != 0) {
            any = 1;
        }
        if ((((s32)pad->w[i] >> 24) & 0xF) < 6) {
            pad->w[i] &= ~0xC;
        }
        if ((s32)(pad->w[i] >> 28) < 6) {
            pad->w[i] &= ~3;
        }
    }
    if (any) {
        gGame.idleFrames = 0;
    } else {
        gGame.idleFrames++;
    }
    for (i = 0; i < 4; i++) {
        buf[i] = 0;
        if (gGame.fadeState == 3) {
            buf[i] = pad->w[i] & ~sPrevPadButtons.w[i];
            if (buf[i] & 0x800) {
                buf[i] |= 1;
            }
            if (buf[i] & 0x1000) {
                buf[i] |= 2;
            }
            if (buf[i] & 0x2000) {
                buf[i] |= 4;
            }
            if (buf[i] & 0x4000) {
                buf[i] |= 8;
            }
        }
    }
    if (screen != NULL) {
        if (gGame.fadeState == 0) {
            item = &screen->items[sPrevScreenCursor];
        } else {
            item = screen->items;
            if (screen->cursor != NULL) {
                item += *screen->cursor + screen->firstItem;
            }
        }
        if (gGame.fadeState != 1) {
            switch (screen->type) {
            case SCREEN_TYPE_SPLASH:
            case SCREEN_TYPE_SPLASH_2:
            case SCREEN_TYPE_SPLASH_3:
            case SCREEN_TYPE_PAK_STATUS:
            case SCREEN_TYPE_BACKUP_LOST:
                shell_splash_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_CHEAT_ENTRY:
            case SCREEN_TYPE_INITIALS:
                shell_text_entry_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_RECORDS:
                shell_records_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_SEASON_RACE_RESULT:
            case SCREEN_TYPE_RACE_RESULT:
                shell_race_results_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_STANDINGS:
                shell_season_standings_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_ROUND_OVER:
                if (buf[0] & 0x50) {
                    shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 0, 1);
                    snd_play_menu_sound(1);
                } else if ((buf[0] & 0x20) && screen->back != NULL) {
                    shell_goto_screen(screen->back, 0, 0);
                    snd_play_menu_sound(2);
                }
                break;
            case SCREEN_TYPE_LIST:
                if (screen - sScreens == SCREEN_MAIN_MENU) {
                    screen->back = &sScreens[SCREEN_TITLE];
                    if (gGame.idleFrames > 0xE10 && gGame.fadeState == 3) {
                        shell_goto_screen(&sScreens[SCREEN_TITLE], 0, 0);
                    }
                }
            case SCREEN_TYPE_TRACK_LIST:
                shell_list_menu_input(screen, item, buf, pad);
                if (screen->type == SCREEN_TYPE_TRACK_LIST) {
                    gGame.trackSel = screen->firstItem + gGame.trackListSel;
                }
                break;
            case SCREEN_TYPE_CONTROLLER_SETTINGS:
                shell_controller_settings_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_PAK:
                pak_screen_update(pad);
                if (gPakScreenDone == 1 && gGame.fadeState == 3) {
                    switch (gGame.pakScreenMode) {
                    case 0:
                        if (screen->back == NULL) {
                            shell_goto_screen(&sScreens[SCREEN_SPLASH_3], 0, 1);
                        } else {
                            shell_goto_screen(screen->back, 0, 0);
                        }
                        break;
                    case 1:
                        if (gPakScreenAborted == 0) {
                            if (replay_get_info(gActiveReplay + 0x6400, &sp20, &sp24, &sp28)) {
                                shell_goto_screen(screen->back, 0, 0);
                                gActiveReplay = 0;
                                gGame.flags &= ~0x80000;
                            } else {
                                gGame.flags |= 0x80000;
                                gCurTrack = &gTracks[sp20];
                                gGame.trackSel = sp20;
                                shell_goto_screen(&sScreens[SCREEN_RIDER_SELECT], 1, 1);
                            }
                        } else {
                            gGame.flags &= ~0x80000;
                            gActiveReplay = 0;
                            shell_goto_screen(screen->back, 0, 0);
                        }
                        break;
                    case 2:
                        shell_goto_screen(screen->back, 0, 0);
                        break;
                    case 4:
                        gGame.flags |= 0x20;
                        shell_goto_screen(0, 0, 1);
                        break;
                    case 3:
                        if (gGame.flags & 0x02000000) {
                            gGame.flags &= ~0x20;
                            if (gPakScreenAborted != 0) {
                                shell_goto_screen(screen->back, 0, 0);
                            } else {
                                gCurTrack = &gTracks[21];
                                shell_goto_screen(&sScreens[SCREEN_RIDER_SELECT], 1, 1);
                                usrtrk_load_layout(gGame.userTrack + 4);
                                gGame.flags &= ~0x80;
                            }
                            gGame.mode = 0;
                            mem_free(gGame.userTrack);
                            gGame.userTrack = 0;
                        } else {
                            if (gPakScreenAborted != 0) {
                                mem_free(gGame.userTrack);
                                gGame.userTrack = 0;
                            }
                            gGame.flags |= 0x20;
                            shell_goto_screen(0, 0, 1);
                        }
                        break;
                    }
                }
                break;
            case SCREEN_TYPE_OPTIONS:
                shell_options_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_TITLE:
                race_update(D_800CD090);
                if (gGame.idleFrames > 0xE10 && gGame.fadeState == 3 &&
                    !(gGame.flags & 0x08000000)) {
                    shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 1, 1);
                }
                break;
            case SCREEN_TYPE_RIDER_SELECT:
                shell_rider_select_input(screen, item, buf, pad);
                break;
            case SCREEN_TYPE_ROUND_SELECT:
                shell_season_select_input(screen, item, buf, pad);
                break;
            }
            if (gGame.fadeState == 3 && (screen->flags & SCREEN_A_B_NAVIGATION)) {
                if (screen->type == SCREEN_TYPE_TITLE && D_800E3704 != 0) {
                    shell_goto_screen(screen->items->target, 1, 1);
                } else if (buf[0] & 0x50) {
                    if ((screen->type != SCREEN_TYPE_TITLE || !(gGame.flags & 0x08000000)) && !(item->flags & SCREEN_ITEM_LOCKED)) {
                        snd_play_menu_sound(1);
                        shell_goto_screen(item->target, 1, 1);
                    }
                } else if (buf[0] & 0x20) {
                    if (screen->back != NULL) {
                        snd_play_menu_sound(2);
                        shell_goto_screen(screen->back, 0, 0);
                    } else {
                        snd_play_menu_sound(3);
                    }
                }
            }
        }
    }
    switch (gGame.fadeState) {
    case 0:
        if (gGame.fadeLevel == 0) {
            gGame.fadeLevel = 30;
            gGame.fadeState = 1;
            gGame.fadeTimer = 2;
        } else {
            gGame.fadeLevel -= 16;
            if (gGame.fadeLevel <= 0) {
                gGame.fadeLevel = 0;
            }
        }
        break;
    case 1:
        if (gGame.fadeTimer == 0) {
            snd_drain();
            snd_set_master_volume(0xFF);
            if (sPrevScreen != NULL) {
                if (tunes_is_song_playing() && (sCurScreen == NULL || (sCurScreen->flags & SCREEN_KEEP_MUSIC))) {
                    tunes_stop_song();
                }
                for (p = sPrevScreen; p != NULL; p = p->right) {
                    shell_unload_screen(p);
                }
                for (p = sPrevScreen->left; p != NULL; p = p->left) {
                    shell_unload_screen(p);
                }
            }
            if (sCurScreen != NULL) {
                for (p = sCurScreen; p != NULL; p = p->right) {
                    p->back = sCurScreen->back;
                    shell_load_screen(p);
                }
                for (p = sCurScreen->left; p != NULL; p = p->left) {
                    p->back = sCurScreen->back;
                    shell_load_screen(p);
                }
                if (!tunes_is_song_playing() && !(sCurScreen->flags & SCREEN_KEEP_MUSIC)) {
                    tunes_play_song(D_800D11D4);
                    snd_set_master_volume(0xFF);
                }
                if (sCurScreen->type == SCREEN_TYPE_TRACK_LIST) {
                    while (sCurScreen->left != NULL) {
                        sCurScreen = sCurScreen->left;
                    }
                    for (i = gGame.trackSel - 5; i >= 0; i -= 5) {
                        sCurScreen = sCurScreen->right;
                    }
                    *sCurScreen->cursor = gGame.trackSel % 5;
                }
            } else {
                gCurGameState->mode = 3;
                if (gGame.mode == 9) {
                    gCurTrack = &gTracks[26];
                } else if (gGame.mode == 1) {
                    gCurTrack = &gTracks[gSeason->cup->tracks[gSeason->round]];
                }
                if (!(gGame.flags & 0x8000)) {
                    if (!(gGame.flags & 0x20)) {
                        if (gGame.mode == 8) {
                            if (gActiveReplay == 0) {
                                gActiveReplay = gReplayBuffer;
                            }
                            gActiveReplaySize = 0x6400;
                        } else if (gGame.mode != 9) {
                            gActiveReplaySize = 0xC800;
                            gActiveReplay = gReplayBuffer;
                        }
                    }
                }
            }
            gGame.fadeTimer = -1;
        }
        if (--gGame.fadeLevel == 0) {
            if (gGame.fadeTimer != -2) {
                gGame.fadeLevel++;
            } else {
                gGame.fadeState = 2;
                gGame.fadeLevel = 255;
            }
        }
        break;
    case 2:
        gGame.fadeLevel -= 16;
        if (gGame.fadeLevel <= 0) {
            gGame.fadeState = 3;
            gGame.fadeLevel = 0;
        }
        break;
    }
    sPrevPadButtons = *pad;
}

typedef struct {
    u8 pad[0x1E];
    s16 f1E;
} Lvl_2;

extern Lvl_2 *gCurTrack_39;

void shell_exit(void)
{
    s32 i;
    s32 j;
    s32 v;
    s32 k;
    Game *g;

    gGame.flags |= 0x4000000;
    gCheatFlags = sCheatMask;
    switch (gGame.mode) {
    case 8:
    case 9:
        gCheatFlags = sCheatMask & ~0x210;
        break;
    }
    switch (gGame.mode) {
    case 0:
    case 1:
    case 8:
        if (!(gGame.flags & 0x1000000)) {
            gGame.flags &= ~0x4000000;
        }
        break;
    }
    if (gCurTrack_39 != &D_800CBF30 && gUserTrackLayout != 0) {
        mem_free(gUserTrackLayout);
        gUserTrackLayout = 0;
    }
    if (gGame.mode == 1 && gSeason != 0 && gSeason->cup - gCups == 4) {
        gGame.riders[1] = gSeason->round;
        if (gGame.riders[1] == gGame.riders[0]) {
            gGame.riders[1] = 5;
        }
    } else if (gGame.mode != 8) {
        v = 0;
        for (i = gGame.numPlayers; i < 8; i++) {
            for (j = 0; j < gGame.numPlayers; j++) {
                if (v == gGame.riders[j]) {
                    v++;
                    v %= 8;
                }
            }
            gGame.riders[i] = v;
            v++;
            v %= 8;
        }
    }
    if (gGame.mode != 7 && gGame.mode != 2 && gCreditsMode == 0 && !(gGame.flags & 0x40000)) {
        switch (gGame.numPlayers) {
        default:
            D_800EFDB4 = 0; /* its own symbol: gGame is in a register here */
            break;
        case 1:
            gGame.numCpuRiders = 5;
            break;
        case 2:
            gGame.numCpuRiders = gGame.numPlayers;
            break;
        }
    } else {
        gGame.numCpuRiders = 0;
    }
    if (!(gGame.flags & 0x8000)) {
        g = &gGame;
        g->numLaps = (g->mode != 1 || gSeason == 0 || gSeason->cup - gCups != 4) ?
            ((g->mode == 1 && gSeason != 0 && gSeason->cup - gCups == 5) ?
             gSeason->cup->laps[gSeason->round] : gCurTrack_39->f1E) : 1;
    }
    mem_free(gGame.greyPalette);
    prim_exit();
    res_close_set(gGuiRes);
    gGuiRes = 0;
    osViSetSpecialFeatures(0xAA);
    gGame.flags &= ~1;
    if (gGame.flags & 0x40000) {
        gGame.numPlayers = gEvent.numRiders;
        if (gGame.numPlayers >= 4) {
            gGame.numPlayers = 3;
        }
        for (k = 0; k < 6; k++) {
            gGame.savedRiders[k] = gGame.riders[k];
            gGame.savedRiderSkins[k] = gGame.riderSkins[k];
        }
        for (k = 0; k < gGame.numPlayers; k++) {
            gGame.riders[k] = gGame.savedRiders[gSeason->standings[k]];
            gGame.riderSkins[k] = gGame.savedRiderSkins[gSeason->standings[k]];
        }
    }
    if (res_open_set(D_800D94A4, &D_800E7728) != 0) {
        fatal_halt(1);
    }
    res_close_set(D_800E7728);
    mem_free(gGame.recordTracks);
    gRaceExitCode = 6;
    font_unload_set();
}

#define LOAD(id) res_unload(&gGuiRes->handles[id])

typedef struct {
    u8 pad0[0x34];
    TrackRecord f34;
    u32 f50;
    u8 pad54[0xA0 - 0x54];
} Bike_41;

extern Bike_41 *gCurTrack_40;
extern Bike_41 *D_800F0634;
extern Bike_41 gTracks_15[];

void shell_unload_screen(Screen *screen)
{
    s32 i;

    if (screen == NULL) {
        return;
    }
    gGame.flags &= ~0x200;
    LOAD(D_800D1398);
    LOAD(D_800D1410);
    if ((screen->flags & SCREEN_TEXT_OBJECTS) && screen->left == NULL) {
        for (i = 0; i < 0x30; i++) {
            func_800C204C(gGame.texts[i]);
            mem_free(gGame.texts[i]);
        }
    }
    if (screen->flags & SCREEN_PANELS) {
        LOAD(D_800D1450);
        LOAD(D_800D14C8);
    }
    if (screen->bgResId != NULL) {
        if (screen->type == SCREEN_TYPE_ROUND_SELECT) {
            for (i = 0; i < 7; i++) {
                LOAD(*((s32 **)screen->bgResId)[i]);
            }
        } else {
            LOAD(*screen->bgResId);
        }
    }
    switch (screen->type) {
    case SCREEN_TYPE_SPLASH_3:
        gGame.flags &= ~0x10000000;
        break;
    case SCREEN_TYPE_SPLASH_2:
        pak_probe_all();
        LOAD(D_800D13B4);
        break;
    case SCREEN_TYPE_TITLE:
        i = 0;
        LOAD(D_800D1490);
        LOAD(D_800D14A4);
        race_exit();
        gGame.numPlayers = 1;
        D_800F0634 = gCurTrack_40;
        for (; i < 6; i++) {
            gGame.riders[i] = gGame.savedRiders[i];
            gGame.riderSkins[i] = gGame.savedRiderSkins[i];
        }
        break;
    case SCREEN_TYPE_RIDER_SELECT:
        race_exit();
        if (!(gGame.flags & 0x400)) {
            gSplitVertical = 0;
        }
        gGame.flags &= ~0x3C10;
        switch (gGame.savedMode) {
        case 8:
            if (gGhostSlot == 0) {
                gGame.riders[1] = gGame.riders[0];
            }
        case 0:
        case 1:
            gGame.flags |= 0x80;
            if (gGame.savedTrack != NULL && (((Bike_41 *)gGame.savedTrack)->f50 & 2)) {
                gGame.flags &= ~0x80;
            }
            break;
        }
        i = 0;
        res_unload(&D_800E7728->handles[D_800D09F8]);
        LOAD(D_800D149C);
        LOAD(D_800D1388);
        LOAD(D_800D144C);
        LOAD(D_800D1494);
        LOAD(D_800D1414);
        LOAD(D_800D13A8);
        LOAD(D_800D1388);
        LOAD(D_800D13EC);
        gGame.screenState = 0;
        gGame.mode = gGame.savedMode;
        gCurTrack_40 = gGame.savedTrack;
        for (; i < 6; i++) {
            if (gGame.numPlayers >= 2) {
                gGame.riderSkins[i] = i;
            } else {
                gGame.riderSkins[i] = (gGame.riderSkinSel + i) % 6;
            }
            gGame.bikeSkins[i] = gGame.bikeSkinSel;
            gGame.bikeSkins[i] += gGame.riders[i];
            gGame.bikeSkins[i] %= 6;
        }
        break;
    case SCREEN_TYPE_CONTROLLER_SETTINGS:
        shell_blend_palette_to_gray(0xFF, NULL);
        LOAD(D_800D1470);
        LOAD(D_800D1498);
        LOAD(D_800D139C);
        LOAD(D_800D13A0);
        mem_free(gGame.playerPalettes);
        break;
    case SCREEN_TYPE_OPTIONS:
        save_write(5, 0);
        save_write(1, 0);
        sfx_free_sample(gGame.optionsSfx);
        LOAD(D_800D1470);
        LOAD(D_800D1424);
        LOAD(D_800D1444);
        LOAD(D_800D1428);
        break;
    case SCREEN_TYPE_PAK_STATUS:
        LOAD(D_800D14A0);
        LOAD(D_800D13BC);
        break;
    case SCREEN_TYPE_BACKUP_LOST:
        LOAD(D_800D13E0);
        LOAD(D_800D142C);
        break;
    case SCREEN_TYPE_CHEAT_ENTRY:
    case SCREEN_TYPE_INITIALS:
        if (screen->type == SCREEN_TYPE_CHEAT_ENTRY) {
            LOAD(D_800D13CC);
        } else {
            gCurTrack_40->f34 = *(TrackRecord *)&gEvent.record; /* copied through a pointer, as in event_init */
            gCurTrack_40->f50 &= ~0x180;
            save_write(2, gCurTrack_40 - gTracks_15);
            for (i = 0; i < gGame.numRecordTracks; i++) {
                if (gGame.recordTracks[i] == gCurTrack_40 - gTracks_15) {
                    break;
                }
            }
            LOAD(*sTrackPicResIds[sTrackPicIndex[i]]);
        }
        mem_free(gGame.playerPalettes);
        LOAD(D_800D1488);
        LOAD(D_800D1380);
        LOAD(D_800D148C);
        LOAD(D_800D13A0);
        break;
    case SCREEN_TYPE_SEASON_RACE_RESULT:
    case SCREEN_TYPE_RACE_RESULT:
    case SCREEN_TYPE_STANDINGS:
        mem_free(gGame.playerPalettes);
        LOAD(D_800D13A0);
        LOAD(D_800D143C);
        LOAD(D_800D13C0);
        LOAD(D_800D1428);
        LOAD(D_800D1424);
        break;
    case SCREEN_TYPE_PAK:
        pak_screen_exit();
        gGame.flags &= ~0x02000000;
        break;
    case SCREEN_TYPE_LIST:
        LOAD(D_800D141C);
        LOAD(D_800D1430);
    case SCREEN_TYPE_TRACK_LIST:
        LOAD(D_800D149C);
        gGame.screenState = 0;
        LOAD(D_800D14AC);
        LOAD(D_800D140C);
        LOAD(D_800D13F4);
        LOAD(D_800D1394);
        LOAD(D_800D13C8);
        LOAD(D_800D1418);
        LOAD(D_800D1428);
        LOAD(D_800D1424);
        for (i = 0; i < screen->numItems; i++) {
            if (screen->type == SCREEN_TYPE_TRACK_LIST) {
                LOAD(*sTrackPicResIds[sTrackPicIndex[i + screen->firstItem]]);
            } else {
                LOAD(*sMenuItemPicResIds[i + screen->firstPic]);
            }
        }
        break;
    case SCREEN_TYPE_ROUND_SELECT:
    case SCREEN_TYPE_RECORDS:
        i = 0;
        LOAD(D_800D14AC);
        LOAD(D_800D1394);
        LOAD(D_800D13C8);
        LOAD(D_800D1418);
        LOAD(D_800D1448);
        LOAD(D_800D13F4);
        LOAD(D_800D1428);
        LOAD(D_800D1424);
        for (; i < 0x18; i++) {
            LOAD(*sTrackPicResIds[i]);
        }
        shell_blend_palette_to_gray(0xFF, NULL);
        break;
    }
}

void shell_build_player_palettes(void)
{
    s32 i;
    s32 v;

    gGame.playerPalettes = (u16 *)mem_alloc_dbg(0x800, 0, D_800D94B0, 0x174F);
    gGame.playerPalettes[0] = 0;
    gGame.playerPalettes[0x100] = 0;
    gGame.playerPalettes[0x200] = 0;
    gGame.playerPalettes[0x300] = 0;
    i = 1;
    do {
        gGame.playerPalettes[i] = ((i >> 3) << 1) | 0x18C1;
        v = i >> 3;
        gGame.playerPalettes[i + 0x100] = (v << 11) | 0xC7;
        gGame.playerPalettes[i + 0x200] = (v << 11) | (v << 6) | 7;
        gGame.playerPalettes[i + 0x300] = (v << 6) | 0x1807;
        i++;
    } while (i < 0x100);
}


void shell_update_season_progress(void)
{
    u8 *q;
    s32 old;
    s32 idx;
    u32 fl;
    Game *g;
    Game *g2;
    s32 m;
    s32 idx2;
    s32 mode;
    s32 nv;
    u32 v;
    u32 w;
    s32 md;
    s32 lv;
    u32 f;
    u8 *pb;
    u8 *e;
    u8 t8;
    Season *r;
    u8 *p0;
    u8 *cb;
    u8 *pb2;

    fl = gGame.flags;
    g = &gGame;
    if ((fl & 0x28000) == 0 && gRaceExitCode != 3) {
        v = fl | 0x20000;
        gGame.flags = v;
        mode = g->difficulty;
        switch (mode) {
        case 1: m = 3; break;
        case 2: m = 7; break;
        case 3: m = 0x1f; break;
        }
        save_update_unlocks();
        r = gSeason;
        p0 = (u8 *)r->cup;
        cb = (u8 *)gCups;
        idx = ((p0 - cb) * 0x38E38E39) >> 2;
        if (idx < 5) if (idx >= 0) {
            pb = (u8 *)r->cup;
            if ((s32)*pb <= (s32)r->round + 1) {
                if (!(idx == 4 && gEvent.finishOrder[0] != 0) && r->standings[0] == 0) {
                    g2 = &gGame;
                    q = &gSaveData_12;
                    lv = q[0x2C0];
                    md = g2->difficulty;
                    if (md == lv && ((old = q[0x2BF]) >> idx & 1) == 0) {
                        v = old | (1 << idx);
                        q[0x2BF] = v;
                        if ((((u8)v) & m) == m) {
                            if (md != 3) {
                                t8 = q[0x2C0];
                                gGame.cupSel = 0;
                                do {
                                q[0x2BF] = 0;
                                q[0x2C0] = t8 + 1;
                                } while (0);
                                gGame.flags |= 0x400000;
                            } else {
                                gGame.flags |= 0x800000;
                            }
                        } else {
                            gGame.flags |= 0x100000;
                        }
                    }
                }
            }
            idx2 = (((u8 *)gSeason->cup - (u8 *)gCups) * 0x38E38E39) >> 2;
            if (idx2 != 4 || gEvent.finishOrder[0] == 0) {
                pb2 = (u8 *)gSeason->cup;
                e = pb2 + (gGame.difficulty - 1);
                if (gSeason->round == e[1]) {
                    nv = gSeason->round + 1;
                    if (nv < *pb2) {
                        e[1] = nv;
                    } else if (gSeason->standings[0] == 0) {
                        e[1] = nv;
                    }
                }
            }
            save_write(1, 0);
        }
        f = save_update_unlocks();
        gGame.newUnlocks = f;
        if (f != 0) {
            gGame.flags |= 0x200000;
        }
    }
}

#define LD(x) ((void *)res_load(gGuiRes, x))


void shell_load_screen(Screen *screen) {
    s32 i;
    s32 j;
    s32 c;

    snd_drain();
    if (screen == NULL) {
        return;
    }
    gGame.idleFrames = 0;
    gGame.unk_480 = LD(D_800D1410);
    gGame.unk_484 = LD(D_800D1398);
    if ((screen->flags & SCREEN_TEXT_OBJECTS) && screen->left == NULL) {
        for (i = 0; i < 48; i++) {
            gGame.texts[i] = font_text_create(0, 0x40, 4, 2, 0x20);
        }
    }
    if (screen->flags & SCREEN_PANELS) {
        gGame.unk_390 = LD(D_800D1450);
        gGame.unk_3DC = LD(D_800D14C8);
    }
    if (screen->bgResId != NULL) {
        if (screen->type == SCREEN_TYPE_ROUND_SELECT) {
            for (i = 0; i < 7; i++) {
                gGame.screenPics[i] = LD(*((s32 **)screen->bgResId)[i]);
            }
        } else {
            screen->bg = LD(*screen->bgResId);
        }
    } else {
        screen->bg = NULL;
    }
    osViSetSpecialFeatures((screen->flags & SCREEN_DITHER_FILTER) ? 0x6A : 0xAA);

    switch (screen->type) {
    case SCREEN_TYPE_SPLASH_3:
        gGame.flags |= 0x10000000;
    case SCREEN_TYPE_SPLASH:
        gGame.screenTimer = 0xB4;
        break;
    case SCREEN_TYPE_PAK_STATUS:
        gGame.unk_390 = LD(D_800D14A0);
        gGame.unk_3DC = LD(D_800D13BC);
        gGame.screenTimer = 0x4B0;
        break;
    case SCREEN_TYPE_BACKUP_LOST:
        gGame.unk_390 = LD(D_800D13E0);
        gGame.unk_3DC = LD(D_800D142C);
        gGame.screenTimer = 0x258;
        break;
    case SCREEN_TYPE_SPLASH_2:
        gGame.screenTimer = 0x12C;
        gGame.unk_3A0 = LD(D_800D13B4);
        break;
    case SCREEN_TYPE_TITLE:
        for (i = 0; i < 6; i++) {
            gGame.savedRiders[i] = gGame.riders[i];
            gGame.savedRiderSkins[i] = gGame.riderSkins[i];
        }
        gGame.riders[0] = 4;
        gGame.riders[2] = 1;
        gGame.riders[3] = 2;
        gGame.riders[4] = 3;
        gGame.riders[1] = 0;
        gGame.riders[5] = 5;
        for (i = 0; i < 6; i++) {
            gGame.riderSkins[i] = i;
        }
        gGame.flags &= ~0x80;
        gGame.unk_38C = LD(D_800D1490);
        gGame.unk_390 = LD(D_800D14A4);
        gCurTrack_2 = gTracks;
        gGame.mode = 0;
        gGame.numPlayers = 1;
        race_set_grid_order(0, 6);
        race_init(0);
        break;
    case SCREEN_TYPE_OPTIONS:
        gGame.repeatTimer = 0;
        gGame.screenState = 0;
        softmix_set_decompress_buf(gZBuffer, 0x25800);
        gGame.optionsSfx = sfx_load_sample_id(D_800D1204);
        softmix_set_decompress_buf(0, 0);
        gGame.unk_390 = LD(D_800D1444);
        gGame.cursorImg = LD(D_800D1428);
        gGame.cursorPal = LD(D_800D1424);
        gGame.unk_3A0 = LD(D_800D1470);
        if (gCreditsMode != 0) {
            gGame.optionsSel = 6;
            gCreditsMode = 0;
        }
        gGame.optionsSel |= 0x100;
        break;
    case SCREEN_TYPE_CONTROLLER_SETTINGS:
        for (i = 1; i < 0x100; i++) {
            c = (i / 3) >> 3;
            gGame.greyPalette[i] = (c << 1) | (c << 6) | (c << 11) | 1;
        }
        gGame.padConfigSel = 0;
        gGame.unk_3A0 = LD(D_800D1470);
        gGame.unk_45C = LD(D_800D139C);
        gGame.unk_390 = LD(D_800D1498);
        gGame.unk_39C = LD(D_800D13A0);
        shell_build_player_palettes();
        break;
    case SCREEN_TYPE_CHEAT_ENTRY:
    case SCREEN_TYPE_INITIALS:
        gGame.screenState = 0;
        i = 0;
        if (screen->type == SCREEN_TYPE_CHEAT_ENTRY) {
            gGame.itemPics[0] = LD(D_800D13CC);
            gGame.name[0] = 0x41;
            for (i = 1; i < 16; i++) {
                gGame.name[i] = 0;
            }
            gGame.nameLen = 0;
            gGame.keyCursorX = 0;
            gGame.keyCursorY = 0;
            gGame.keyIndex = 0;
        } else {
            gGame.entryRecord = -1;
            gGame.unk_7A8 = 0xFF;
            for (i = 0; i < 4; i++) {
                gGame.recordPlayer[i] = -1;
                if (gEvent.record.initials[i][0] == 0xFF && gEvent.record.initials[i][1] == 0xFF) {
                    gGame.recordPlayer[i] = gEvent.record.initials[i][2];
                    if (gGame.entryRecord == -1) {
                        gGame.entryRecord = i;
                    }
                }
            }
            if (gGame.playerInitials[gGame.recordPlayer[gGame.entryRecord]][0] != 0) {
                __builtin_memcpy(gGame.name, gGame.playerInitials[gGame.recordPlayer[gGame.entryRecord]], 3);
                gGame.name[3] = 0;
                gGame.nameLen = strlen(gGame.name);
                gGame.keyCursorX = 0x31F;
                gGame.keyCursorY = 0x31F;
                gGame.keyIndex = 0x27;
            } else {
                gGame.name[0] = 0x41;
                gGame.name[1] = 0;
                gGame.name[2] = 0;
                gGame.name[3] = 0;
                gGame.nameLen = 0;
                gGame.keyCursorX = 0;
                gGame.keyCursorY = 0;
                gGame.keyIndex = 0;
            }
            for (i = 0; i < gGame.numRecordTracks; i++) {
                if (gGame.recordTracks[i] == gCurTrack_2 - gTracks) {
                    break;
                }
            }
            gGame.itemPics[0] = LD(*sTrackPicResIds[sTrackPicIndex[i]]);
        }
        gGame.unk_390 = LD(D_800D148C);
        gGame.unk_3A0 = LD(D_800D1380);
        gGame.unk_478 = LD(D_800D1488);
        gGame.blinkTimer = 0;
        gGame.unk_39C = LD(D_800D13A0);
        shell_build_player_palettes();
        break;
    case SCREEN_TYPE_RACE_RESULT:
    case SCREEN_TYPE_STANDINGS:
        if (screen->type == SCREEN_TYPE_STANDINGS && gGame.mode == 1 && gSeason != NULL
            && gSeason->cup - gCups == 4) {
            shell_update_season_progress();
        }
        if (gGame.flags & 0x8000) {
            gGame.screenState = (screen->type == SCREEN_TYPE_RACE_RESULT) ? 3 : 4;
            gGame.flags = (gGame.flags | 0x20000) & ~0x8000;
        } else {
            gGame.screenState = 0;
        }
        gGame.blinkTimer = 0;
        gGame.unk_39C = LD(D_800D13A0);
        shell_build_player_palettes();
        gGame.unk_390 = LD(D_800D143C);
        gGame.unk_3DC = LD(D_800D14C8);
        gGame.unk_414 = LD(D_800D13C0);
        gGame.cursorImg = LD(D_800D1428);
        gGame.cursorPal = LD(D_800D1424);
        break;
    case SCREEN_TYPE_SEASON_RACE_RESULT:
        shell_update_season_progress();
        gGame.screenState = 0;
        gGame.blinkTimer = 0;
        shell_build_player_palettes();
        gGame.unk_39C = LD(D_800D13A0);
        gGame.unk_390 = LD(D_800D143C);
        gGame.cursorImg = LD(D_800D1428);
        gGame.cursorPal = LD(D_800D1424);
        gGame.unk_414 = LD(D_800D13C0);
        gGame.unk_3DC = LD(D_800D14C8);
        break;
    case SCREEN_TYPE_PAK:
        gPakScreen.validate = NULL;
        switch (gGame.pakScreenMode) {
        case 0:
            gPakScreen.mode = 0;
            gPakScreen.data = NULL;
            gPakScreen.pages = 0;
            break;
        case 1:
            if (gActiveReplay == 0) {
                gActiveReplay = gReplayBuffer;
            }
            gPakScreen.pages = 100;
            gPakScreen.mode = 1;
            gPakScreen.validate = replay_validate;
            gPakScreen.data = (u8 *)(gActiveReplay + 0x6400);
            break;
        case 2:
            gPakScreen.pages = 40;
            gPakScreen.mode = 2;
            gPakScreen.data = (u8 *)gActiveReplay;
            break;
        case 3:
            if (gGame.userTrack != NULL) {
                mem_free(gGame.userTrack);
            }
            gGame.userTrack = mem_alloc_dbg(0x400, 0, D_800D94B0, 0x18E6);
            gPakScreen.data = gGame.userTrack;
            gPakScreen.pages = 4;
            gPakScreen.mode = 1;
            gPakScreen.validate = (PakValidateFunc)shell_pak_track_invalid;
            break;
        case 4:
            gPakScreen.pages = 4;
            gPakScreen.mode = 2;
            gPakScreen.data = gGame.userTrack;
            break;
        }
        gPakScreen.describe = shell_format_pak_file_label;
        pak_screen_init();
        break;
    case SCREEN_TYPE_LIST:
        gSplitVertical = 0;
        if (screen - sScreens == SCREEN_SPECIAL_TRACKS) {
            gGame.mode = 0;
            while (gSpecialTracksItems[*screen->cursor].flags & SCREEN_ITEM_LOCKED) {
                (*screen->cursor)--;
            }
        }
        gGame.flags &= 0xFDFFFFFF;
        gGame.unk_744 = LD(D_800D141C);
        gGame.unk_748 = LD(D_800D1430);
        gGame.screenState = 0;
        if (screen - sScreens == SCREEN_MAIN_MENU) {
            gGame.numPlayers = 1;
        }
        if (screen - sScreens == SCREEN_BRONZE_TRACKS) {
            gGame.trackSel = 0;
        }
    case SCREEN_TYPE_TRACK_LIST:
        gGame.unk_74C = LD(D_800D149C);
        gGame.menuAnimTimer = 0;
        gGame.blinkTimer = 0;
        gGame.unk_438 = LD(D_800D14AC);
        gGame.unk_388 = LD(D_800D140C);
        gGame.unk_3A0 = LD(D_800D1418);
        gGame.unk_398 = LD(D_800D13F4);
        gGame.unk_39C = LD(D_800D1394);
        gGame.unk_3A8 = LD(D_800D13C8);
        gGame.cursorImg = LD(D_800D1428);
        gGame.cursorPal = LD(D_800D1424);
        for (i = 0; i < screen->numItems; i++) {
            if (screen->type == SCREEN_TYPE_TRACK_LIST) {
                gGame.itemPics[i + screen->firstItem] = LD(*sTrackPicResIds[sTrackPicIndex[i + screen->firstItem]]);
            } else {
                gGame.itemPics[i + screen->firstItem] = LD(*sMenuItemPicResIds[i + screen->firstPic]);
            }
        }
        break;
    case SCREEN_TYPE_RECORDS:
        gGame.screenState = 13;
        gGame.screenTimer = 6;
    case SCREEN_TYPE_ROUND_SELECT:
        gGame.seasonMenuState = 0;
        switch (gGame.difficulty) {
        case 1:
            if (gGame.cupSel >= 2) {
                gGame.cupSel = 0;
            }
            break;
        case 2:
            if (gGame.cupSel >= 3) {
                gGame.cupSel = 0;
            }
            break;
        }
        if (gGame.difficulty == 0) {
            gGame.cupSel = 6;
        } else if (gGame.cupSel == 6) {
            gGame.cupSel = 0;
        }
        gGame.roundSel = 0;
        gGame.menuAnimTimer = 0;
        gGame.blinkTimer = 0;
        gGame.unk_438 = LD(D_800D14AC);
        gGame.cursorImg = LD(D_800D1428);
        gGame.cursorPal = LD(D_800D1424);
        gGame.unk_3A0 = LD(D_800D1418);
        gGame.unk_398 = LD(D_800D13F4);
        gGame.unk_39C = LD(D_800D1394);
        gGame.unk_3A8 = LD(D_800D13C8);
        gGame.unk_3B0 = LD(D_800D1448);
        for (i = 0; i < 24; i++) {
            gGame.itemPics[i] = LD(*sTrackPicResIds[i]);
        }
        gGame.cupPreviewTimer = 0;
        gGame.cupPreviewTrack = 0;
        break;
    case SCREEN_TYPE_RIDER_SELECT:
        gGame.riderSelectState[0] = 0;
        gGame.riderSelectState[1] = 0;
        for (i = 3; i >= 0; i--) {
            gGame.riderSelectTimer[i] = 0;
        }
        for (i = 0; i < 6; i++) {
            gGame.bikeSkins[i] = i;
        }
        gGame.screenState = 0;
        if (gGame.numPlayers == 1) {
            gGame.unk_3C8 = LD(D_800D1494);
        }
        j = 0;
        gGame.playerPad[1] = -1;
        gGame.playerPad[2] = -1;
        gGame.playerPad[3] = -1;
        gGame.padPlayer[0] = -1;
        gGame.savedMode = gGame.mode;
        gGame.savedTrack = gCurTrack_2;
        gGame.flags &= ~0x3C90;
        for (i = 0; i < 4; i++) {
            if ((gContMask >> i) & 1) {
                gGame.playerPad[j] = i;
                gGame.padPlayer[i] = j;
                j++;
            }
        }
        for (i = 0; i < 4; i++) {
            gGame.riderSelectRot[i][0] = (((gFrameCounter & 0x1FF) << 7) + ((-(i & 1) + (i & 2)) << 14)) & 0xFFFF;
            gGame.riderSelectRot[i][1] = 0;
        }
        gCurTrack_2 = sTrackLongIsland;
        gGame.mode = 0;
        gGame.flags |= 0x10;
        gGame.unk_390 = LD(D_800D1388);
        gGame.unk_3DC = LD(D_800D144C);
        gGame.unk_74C = LD(D_800D149C);
        race_set_grid_order(0, 6);
        race_init(0);
        gGame.hudGlyphs = (HudGlyphs *)res_load(D_800E7728, D_800D09F8);
        break;
    }
}

void shell_goto_screen(Screen *screen, s32 linkBack, s32 runCallback)
{
    Screen *p;

    if (gGame.fadeState != 0) {
        if (sCurScreen == NULL) {
            if (screen != NULL) {
                p = screen;
                do {
                    if (gRaceExitCode != 3 && gCreditsMode == 0) {
                        p->back = NULL;
                    }
                    shell_load_screen(p);
                    p = p->right;
                } while (p != NULL);
                for (p = screen->left; p != NULL; p = p->left) {
                    if (gRaceExitCode != 3 && gCreditsMode == 0) {
                        p->back = NULL;
                    }
                    shell_load_screen(p);
                }
                if (tunes_is_song_playing() == 0 && (screen->flags & SCREEN_KEEP_MUSIC) == 0) {
                    tunes_play_song(D_800D11D4);
                }
            }
            gGame.fadeState = 1;
            gGame.fadeLevel = 1;
            gGame.fadeTimer = -1;
        } else {
            if (runCallback != 0) {
                ScreenItem *items = sCurScreen->items;
                s32 *cursor = sCurScreen->cursor;
                if ((cursor != NULL ? &items[*cursor] : items)->callback != NULL) {
                    (cursor != NULL ? &items[*cursor] : items)->callback(1);
                }
            }
            gGame.fadeState = 0;
            gGame.fadeLevel = 0xFF;
            sPrevScreenCursor = sCurScreen->cursor != NULL ? *sCurScreen->cursor : 0;
        }
        if (sCurScreen != NULL) {
            sPrevScreen = sCurScreen;
        }
        if (screen != NULL && linkBack != 0) {
            screen->back = sCurScreen;
        }
        gGame.returnScreen = sPrevScreen;
        sCurScreen = screen;
    }
}



void shell_draw_rider_stats(s32 x, s32 y, s32 p3, s32 p4)
{
    Game *g = &gGame;
    s32 mode = g->numPlayers;
    s32 yy, v, i, j, k, sy, t;
    Rect *q;
    u8 *base;

    if (mode == 1) {
        return;
    }
    yy = y;
    if (gGame.flags & (0x400 << p4)) {
        yy += 600;
    }
    base = gViewRects;
    q = (Rect *)(base + (p4 * 16 + (mode - 1) * 64));
    v = q->y + q->h + 5;
    y = yy;
    if (v < yy) {
        y = v;
    }
    font_text_create(gGame.texts[p4 * 9], 0x40, 1, 6, 0x21);
    for (i = 1; i < 9; i++) {
        font_text_create(gGame.texts[i + p4 * 9], 0x40, 1, 2, 0x20);
    }
    font_text_add(gGame.texts[p4 * 9], sRiderBios[p3].fullName);
    font_draw_text(gGame.texts[p4 * 9], x, y - 0x1F);
    i = 0;
    do {
        k = i + 1;
        font_text_add(gGame.texts[p4 * 9 + k], (char *)sRiderStatLabels[i]);
        t = i * 14 - 11;
        sy = yy + t;
        font_draw_text(gGame.texts[p4 * 9 + k], x - 0x46, sy);
        j = i + 5;
        font_text_add(gGame.texts[p4 * 9 + j], (char *)sRiderStatRatings[sRiderBios[p3].stats[i]]);
        font_draw_text(gGame.texts[p4 * 9 + j], x + 0x12, sy);
        i = k;
    } while (i < 4);
}

Gfx *shell_draw_results_menu(Gfx *gdl, s32 p2, s32 sel, char **strs)
{
    s32 flag;
    s32 i;
    u8 *img;
    s32 x, y, a, c;

    flag = 0;
    if (gGame.mode == 8 || gReplayWritePtr - gReplayBuf >= gReplayBufSize) {
        flag = 1;
    }
    font_reset_queue();
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPLoadTLUT_pal256(gdl++, gGame.unk_3DC->addr);
    i = 0;
    x = -25;
    y = -13;
    c = 0;
    for (; i < 3; i++) {
        gdl = render_draw_image(0xC0 - c, i * 25 + 0x52, &gGame.unk_3DC[1], gdl, p2, 1, 1, 3, 0);
        font_text_create(gGame.texts[i], 0x40, 1, (sel == i) ? 4 : 2, (i == 1 && flag) ? 0x20000021 : 0x21);
        font_text_add(gGame.texts[i], strs[i]);
        font_draw_text(gGame.texts[i], gScreenWidth / 2 - y, gScreenHeight / 2 + x);
        x += 25;
        y += 7;
        c += 7;
    }
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    img = (u8 *)&gGame.unk_404[2];
    gdl = render_draw_image((gScreenWidth - img[0] * 8) / 2, (gScreenHeight - img[1]) / 2, img, gdl, p2, 1, 1, 6, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
    return render_draw_image(0xA1 - (sel - 1) * 7, sel * 25 + 0x56,
                         &gGame.cursorImg[gGame.menuAnimTimer / 2 % 30 + 1], gdl, p2, 1, 1, 3, 0);
}

Gfx *shell_draw_players_difficulty_popup(Gfx *gdl, s32 p2, s32 p3)
{
    s32 dx, dy, dz;
    s32 i;
    u8 *img;
    char buf[32];
    char num[2];

    if (p3 != 1 || gContCount < 2 || gGame.mode != 0) {
        dx = 0;
        if (p3 == -1) {
            dx = 7;
        }
    } else {
        dx = -0x18;
    }
    dy = 0;
    if (p3 == -1) {
        dy = -0xC;
    }
    dz = 0;
    if (p3 == 1 && gContCount >= 2 && gGame.mode == 0) {
        dz = -0x17;
    }
    font_reset_queue();
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPLoadTLUT_pal256(gdl++, gGame.unk_3DC->addr);
    for (i = (p3 == -1); i < 4; i++) {
        if (p3 == 1 && gContCount >= 2 && gGame.mode == 0) {
            if (i == p3) {
                img = (u8 *)&gGame.unk_3DC[2];
            } else {
                img = (u8 *)&gGame.unk_3DC[3];
            }
        } else {
            img = (u8 *)&gGame.unk_3DC[1];
        }
        gdl = render_draw_image(0xC0 - i * 7 + dx, i * 25 + 0x46 + dy, img, gdl, p2, 1, 1, 3, 0);
        if (p3 == 1) {
            if (i >= gContCount) {
                font_text_create(gGame.texts[i], 0x40, 1, 2, 0x20000021);
                font_text_add(gGame.texts[i], D_800D9AAC);
                font_draw_text(gGame.texts[i], gScreenWidth / 2 + 17 - i * 7 + dx,
                              gScreenHeight / 2 - 39 + i * 25 + dy);
            } else {
                if (i == p3 && gGame.mode == 0) {
                    font_text_create(gGame.texts[i], 0x40, 1, (gGame.numPlayers == 2) ? 4 : 2, 0x21);
                    font_text_add(gGame.texts[i], D_800D9AC0);
                    font_draw_text(gGame.texts[i], gScreenWidth / 2 + (dx + 10) + 0x50 + dz,
                                  gScreenHeight / 2 + (dy - 14));
                    font_text_create(gGame.texts[i], 0x40, 1, (gGame.numPlayers == 2) ? 6 : 2, 0x21);
                    int_to_str(gGame.twoPlayerCpuRiders, buf);
                    font_text_add(gGame.texts[i], buf);
                    font_draw_text(gGame.texts[i], gScreenWidth / 2 + (dx + 10) + 0x7A + dz,
                                  gScreenHeight / 2 + (dy - 14));
                }
                if (((gGame.mode == 2 || gGame.mode == 3) && !(i & 1)) ||
                    (gGame.mode == 7 && i >= 2) || (gGame.mode == 10 && i >= 2)) {
                    font_text_create(gGame.texts[i], 0x40, 1, (i == gGame.numPlayers - 1) ? 4 : 2, 0x20000021);
                } else {
                    font_text_create(gGame.texts[i], 0x40, 1, (i == gGame.numPlayers - 1) ? 4 : 2, 0x21);
                }
                num[0] = i + '1';
                num[1] = 0;
                font_text_add(gGame.texts[i], num);
                font_text_add(gGame.texts[i], D_800D9AC4);
                font_draw_text(gGame.texts[i], gScreenWidth / 2 + 17 - i * 7 + dx + dz,
                              gScreenHeight / 2 - 39 + i * 25 + dy);
            }
        } else {
            font_text_create(gGame.texts[i], 0x40, 1, (i == gGame.difficulty) ? 4 : 2,
                          (i >= D_800F1BC0 + 1) ? 0x20000021 : 0x21);
            font_text_add(gGame.texts[i], sDifficultyNames[i]);
            font_draw_text(gGame.texts[i], gScreenWidth / 2 + 17 - i * 7 + dx,
                          gScreenHeight / 2 - 39 + i * 25 + dy);
        }
    }
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    if (p3 == -1) {
        img = (u8 *)&gGame.unk_404[2];
    } else if (p3 == 1 && gContCount >= 2 && gGame.mode == 0) {
        img = (u8 *)&gGame.unk_404[1];
    } else {
        img = (u8 *)&gGame.unk_390[3];
    }
    gdl = render_draw_image((gScreenWidth - img[0] * 8) / 2, (gScreenHeight - img[1]) / 2, img, gdl, p2, 1, 1, 6, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
    img = (u8 *)&gGame.cursorImg[gGame.menuAnimTimer / 2 % 30 + 1];
    if (p3 == 1) {
        gdl = render_draw_image(0xA7 - (gGame.numPlayers - 1) * 7 + dx, (gGame.numPlayers - 1) * 25 + 0x49 + dy, img,
                            gdl, p2, 1, 1, 3, 0);
    } else {
        gdl = render_draw_image(0xA7 - gGame.difficulty * 7 + dx, gGame.difficulty * 25 + 0x49 + dy, img,
                            gdl, p2, 1, 1, 3, 0);
    }
    if (p3 == 1 && gContCount >= 2 && gGame.mode == 0 && gGame.numPlayers == 2 && (gFrameCounter & 0x20)) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        gDPLoadTLUT_pal256(gdl++, gGame.unk_74C->addr);
        gdl = render_draw_image(gScreenWidth / 2 + 10 + dx + 0x60 + dz, gScreenHeight / 2 - 18 + dy, &gGame.unk_74C[3],
                            gdl, p2, 1, 1, 3, 0);
        gdl = render_draw_image(gScreenWidth / 2 + 10 + dx + 0x84 + dz, gScreenHeight / 2 - 18 + dy, &gGame.unk_74C[4],
                            gdl, p2, 1, 1, 3, 0);
    }
    return font_render(gdl);
}

Gfx *shell_draw_yes_no_dialog(Gfx *gdl, s32 p2, char *str)
{
    u8 *img;
    s32 n;
    s32 y;
    s32 t;

    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, gGame.unk_484->addr);
    gdl = render_draw_image(0xA3, 0x58, &gGame.unk_484[1], gdl, p2, 1, 1, 3, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    img = (u8 *)&gGame.unk_480[1];
    gdl = render_draw_image(0x100 - img[0] * 4, 0x78 - (img[1] >> 1), img, gdl, p2, 1, 1, 6, 0);
    font_reset_queue();
    font_text_create(gGame.texts[0], 0x40, 1, 2, 0x21);
    font_text_add(gGame.texts[0], str);
    font_draw_text(gGame.texts[0], 0x105, 0x64);
    font_text_create(gGame.texts[0], 0x40, 1, gGame.dialogSel ? 4 : 2, 0x21);
    font_text_add(gGame.texts[0], D_800D9ACC);
    font_draw_text(gGame.texts[0], 0x108, 0x76);
    font_text_create(gGame.texts[0], 0x40, 1, gGame.dialogSel ? 2 : 4, 0x21);
    font_text_add(gGame.texts[0], D_800D9AD0);
    font_draw_text(gGame.texts[0], 0x103, 0x88);
    gdl = font_render(gdl);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
    n = gGame.dialogSel;
    return render_draw_image(n * 7 + 0x9D, (t = n * 5, y = 0x82, y - t * 4),
                         &gGame.cursorImg[gGame.menuAnimTimer / 2 % 30 + 1], gdl, p2, 1, 1, 3, 0);
}

void shell_draw_rider_bio(s32 p, s32 q)
{
    s32 i, n, off;

    if (gGame.numPlayers != 1) {
        return;
    }
    font_text_create(gGame.texts[20], 0x40, 2, 2, 0x20);
    font_text_add(gGame.texts[20], D_800D9AD4);
    font_draw_text(gGame.texts[20], 0x140, 0x34);
    font_text_add(gGame.texts[p * 20 + 17], sRiderBios[gGame.riders[p]].text[0]);
    font_draw_text(gGame.texts[p * 20 + 17], gViewRects_12.x + 0x10E, q + gViewRects_12.y - 0x6F);
    font_text_add(gGame.texts[p * 20 + 18], sRiderBios[gGame.riders[p]].text[1]);
    font_draw_text(gGame.texts[p * 20 + 18], gViewRects_12.x + 0x10B, q + gViewRects_12.y - 0x62);
    font_text_add(gGame.texts[p * 20 + 19], sRiderBios[gGame.riders[p]].text[2]);
    font_draw_text(gGame.texts[p * 20 + 19], gViewRects_12.x + 0x108, q + gViewRects_12.y - 0x55);
    for (i = 0; i < 4; i++) {
        font_text_add(gGame.texts[p * 20 + 6 + i], sRiderBios[gGame.riders[p]].text[i + 4]);
        font_draw_text(gGame.texts[p * 20 + 6 + i], gViewRects_12.x + 60 - i * 3, q + gViewRects_12.y - 0x6F + i * 13);
    }
    n = 4;
    while (sRiderBios[gGame.riders[p]].text[n + 4] != 0) {
        n++;
    }
    off = -100;
    for (; sRiderBios[gGame.riders[p]].text[i + 4] != 0; i++) {
        font_text_add(gGame.texts[p * 20 + 6 + i], sRiderBios[gGame.riders[p]].text[i + 4]);
        font_draw_text(gGame.texts[p * 20 + 6 + i], gViewRects_12.x + 60 - i * 3, q + gViewRects_12.y + off + i * 13);
    }
}

#define SAVE gSaveData_13
#define VP gViewRects
#define CUSTOM5 (gGame.mode == 1 && gSeason != NULL && gSeason->cup - gCups == 5)
#define CHAMP4 (gGame.mode == 1 && gSeason != NULL && gSeason->cup - gCups == 4)
#define UNLOCKED(n) (gGame.difficulty < SAVE.b2C0 || (SAVE.b2C0 == gGame.difficulty && ((n) == 0 || ((SAVE.b2BF >> ((n) - 1)) & 1))))
#define LVLDONE (gGame.difficulty < SAVE.b2C0 || (SAVE.b2C0 == gGame.difficulty && (SAVE.b2BF & 8)))
#define AVAILD(n) ((gGame.mode == 8) ? avail8d(n) : shell_is_round_unlocked((n) / 5))
#define AVAIL(n) ((gGame.mode == 8) ? avail8(n) : shell_is_round_unlocked((n) / 5))

#define PREAMBLE()                                                  \
    gDPPipeSync(gdl++);                                             \
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);                           \
    gDPSetTexturePersp(gdl++, G_TP_NONE);                           \
    gDPSetTextureFilter(gdl++, G_TF_BILERP);                        \
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);    \
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);                           \
    gDPSetRenderMode(gdl++, 0x00404240, 0);                         \
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279)

#define CMB_TEX() gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279)
#define CMB_PRIM() gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF)
#define DRAW(x, y, img, f) gdl = render_draw_image((x), (y), (img), gdl, fb, 1, 1, (f), 0)
#define PSYNCK(k) { Gfx *_g = gdl++; _g->words.w0 = 0xE7000000; _g->words.w1 = (u16)(k) >> 16; }
#define DRAWK(x, y, img, f, k) gdl = render_draw_image((x), (y), (img), gdl, fb, 1, 1, (f), (u16)(k) >> 16)
#define DRAWV(x, y, img, f) { s32 _f = (f); gdl = render_draw_image((x), (y), (img), gdl, fb, 1, 1, _f, 0); }
#define TDRAW(spr, x, y, f) { Img *_t = &(spr)[1]; gDPLoadTLUT_pal256(gdl++, (spr)->addr); DRAW((x), (y), _t, (f)); }
#define TDRAWP(spr, x, y, f) { Img *_t = &(spr)[1]; gDPLoadTLUT_pal256(gdl++, (spr)->addr); gDPPipeSync(gdl++); DRAW((x), (y), _t, (f)); }

typedef struct {
    char name[0x1E];
    s16 h1E;
    u8 pad20[0x34 - 0x20];
    s32 f34[3];
    s32 f40;
    char init[3][3];
    char init4D[3];
    u32 f50;
    u8 pad54[0xA0 - 0x54];
} Track_17; /* 0xA0 */

typedef struct {
    u8 pad0[0x2BF];
    u8 b2BF;
    u8 b2C0;
    u8 pad2C1[3];
    u8 b2C4;
    u8 vol[3];
} Save_5;

extern Track_17 *gCurTrack_41;
extern Save_5 gSaveData_13;
extern Track_17 gTracks_16[];

static inline s32 avail8(s32 n)
{
    s32 k = n / 5;
    s32 v;

    if (k == 0) {
        goto ret1;
    }
    switch (gSaveData_13.b2C0) {
    case 1:
        if (k >= 2) {
            goto ret0;
        }
        k--;
        v = gSaveData_13.b2BF >> k;
        break;
    case 2:
        if (k < 2) {
            return 1;
        }
        if (k != 2) {
            goto ret0;
        }
        v = gSaveData_13.b2BF >> (k - 1);
        break;
    case 3:
        if (k < 3) {
            return 1;
        }
        if (k != 3) {
            return 0;
        }
        v = gSaveData_13.b2BF >> (k - 1);
        break;
    default:
        return 0;
    }
    if (!(v & 1)) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

static inline s32 avail8d(s32 n)
{
    s32 k = n / 5;
    s32 v;

    if (k == 0) {
        goto ret1;
    }
    if (k == 9) {
        v = 1;
        n = 2;
    }
    switch (gSaveData_13.b2C0) {
    case 1:
        if (k >= 2) {
            goto ret0;
        }
        k--;
        v = gSaveData_13.b2BF >> k;
        break;
    case 2:
        if (k < 2) {
            return 1;
        }
        if (k != 2) {
            goto ret0;
        }
        v = gSaveData_13.b2BF >> (k - 1);
        break;
    case 3:
        if (k < 3) {
            return 1;
        }
        if (k != 3) {
            return 0;
        }
        v = gSaveData_13.b2BF >> (k - 1);
        break;
    default:
        return 0;
    }
    if (!(v & 1)) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

static inline s32 objOk(Screen *p)
{
    s32 lvl;
    s32 v;

    if (gGame.mode == 8 || gGame.mode == 0) {
        lvl = gSaveData_13.b2C0;
    } else {
        lvl = gGame.difficulty;
    }
    switch (lvl) {
    case 1:
        v = p->firstItem < 10;
        break;
    case 2:
        v = p->firstItem < 15;
        break;
    case 3:
        goto ret1;
    default:
        goto ret0;
    }
    if (!v) {
        goto ret0;
    }
ret1:
    return 1;
ret0:
    return 0;
}

Gfx *shell_draw_screen(Mtx *m, s32 fb, Gfx *gdl, ScreenItem *item, s32 e)
{
    char unk[0xE8];
    char buf[0x40];
    s16 wid[4];
    char buf2[0x10];
    char buf3[0x20];
    Screen *s;
    s32 k;
    s32 rowh;
    s32 i;
    Track_17 *trk;
    s32 yoff3;
    s32 ry;
    s32 rx;
    s32 w;
    s32 w2;
    s32 w3;
    s32 u1;
    s32 u2;
    s32 u3;
    s32 r;
    s32 j;
    s32 col;
    s32 x;
    s32 y;
    s32 yo2;
    s32 n;
    s32 c;
    s32 tmp;
    s32 pl;
    s32 gr;
    s32 gc;
    s32 t;
    s32 a;
    void *p;
    Img *img;

    if (gGame.fadeState == 0) {
        s = sPrevScreen;
    } else {
        s = sCurScreen;
    }
    switch (gGame.fadeState) {
    case 0:
        w = gScreenWidth; w2 = gScreenWidth;
        break;
    case 1:
        w3 = gScreenWidth;
        break;
    case 2:
        w3 = gScreenWidth;
        break;
    }
    gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    switch (s->type) {
    case SCREEN_TYPE_SPLASH_2:
        PREAMBLE();
        if (gHasExpansionPak_2 != 0 && (gFrameCounter & 0x20)) {
            Img *t = &gGame.unk_3A0[1];

            gDPLoadTLUT_pal256(gdl++, gGame.unk_3A0->addr);
            gdl = render_draw_image(0x77, 0x8D, t, gdl, fb, 1, 1, 3, 0);
        }
        break;
    case SCREEN_TYPE_PAK:
        if (gGame.fadeState != 1) {
            pak_screen_draw(m, fb, &gdl);
        }
        break;
    case SCREEN_TYPE_LIST:
    case SCREEN_TYPE_TRACK_LIST:
        yoff3 = 0;
        if (s->type == SCREEN_TYPE_TRACK_LIST) {
            yoff3 = 12;
        }
        font_reset_queue();
        PREAMBLE();
        if (s->type == SCREEN_TYPE_LIST) {
            s32 w;
            s32 r;
            Img *sp;

            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0x32);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            sp = &gGame.unk_388[1];
            w = sp[gGame.unk_048].w << 3;
            r = gFrameCounter % (w + 0x200);
            if (r == 0) {
                gGame.unk_048 = (s16)(gGame.unk_048 + 1) % 10;
            }
            gdl = render_draw_image(r - w, ((0xF0 - (w + 0x200) / 2) >> 1) + r / 2, &gGame.unk_388[gGame.unk_048 + 1], gdl, fb, 1, 1,
                                6, 0);
        } else if (gGame.blinkTimer < 0x20) {
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetRenderMode(gdl++, 0x00404240, 0);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, gGame.unk_438->addr);
            if (s->left != NULL) {
                gdl = render_draw_image(0x1C, yoff3 + 0x17, &gGame.unk_438[1], gdl, fb, 1, 1, 3, 0);
            }
            if (s->right != NULL && objOk(s->right)) {
                gdl = render_draw_image(0x128, yoff3 + 0x17, &gGame.unk_438[2], gdl, fb, 1, 1, 3, 0);
            }
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        TDRAWP(gGame.unk_3A0, 0x45, yoff3 + 0x1F, 3);
        if ((s->type == SCREEN_TYPE_TRACK_LIST && !AVAILD(s->firstItem + *s->cursor)) || (s->items[*s->cursor + s->firstItem].flags & SCREEN_ITEM_LOCKED)) {
            gDPSetPrimColor(gdl++, 0, 0, 0x64, 0x64, 0x64, 0xFF);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            gDPLoadTLUT_pal256(gdl++, gGame.greyPalette);
            font_text_create(gGame.texts[0], 0x40, 4, 2, 0x21);
            font_text_add(gGame.texts[0], D_800D9C18);
            font_draw_text(gGame.texts[0], 0x16B, 0x82);
        } else {
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, gGame.itemPics[s->firstItem + *s->cursor]->addr);
        }
        img = &gGame.itemPics[s->firstItem + *s->cursor][1];
        if (s->type != SCREEN_TYPE_TRACK_LIST) {
            gdl = render_draw_image(0x102, yoff3 + 0x3B, img, gdl, fb, 1, 1, 3, 0);
        } else {
            gdl = render_draw_image(0x108, yoff3 + 0x3B, img, gdl, fb, 1, 1, 3, 0);
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        if (s->type == SCREEN_TYPE_LIST) {
            img = &gGame.unk_390[2];
            gdl = render_draw_image((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, gdl, fb, 1, 1, 6, 0);
        } else {
            img = &gGame.unk_390[6];
            gdl = render_draw_image((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, gdl, fb, 1, 1, 6, 0);
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        font_text_create(gGame.texts[s->numItems * 2], 0x20, 4, 2, 0x21);
        font_text_add(gGame.texts[s->numItems * 2], s->title);
        font_draw_text(gGame.texts[s->numItems * 2], 0xB9, yoff3 + 0x28);
        for (i = 0; i < s->numItems; i++) {
            if ((s->type == SCREEN_TYPE_TRACK_LIST && !AVAIL(s->firstItem + i)) || (s->items[i + s->firstItem].flags & SCREEN_ITEM_LOCKED)) {
                font_text_create(gGame.texts[i], 0x20, 1, (i == *s->cursor) ? 4 : 2, 0x20000020);
                font_text_create(gGame.texts[i + s->numItems], 0x20, 1, (i == *s->cursor) ? 4 : 2, 0x20000021);
            } else {
                font_text_create(gGame.texts[i], 0x20, 1, (i == *s->cursor) ? 4 : 2, 0x20);
                font_text_create(gGame.texts[i + s->numItems], 0x20, 1, (i == *s->cursor) ? 4 : 2, 0x21);
            }
            if (s->type == SCREEN_TYPE_TRACK_LIST && !AVAIL(gGame.trackSel)) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                gDPSetRenderMode(gdl++, 0x00404240, 0);
                gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
                gDPLoadTLUT_pal256(gdl++, gGame.unk_398->addr);
                gdl = render_draw_image(0x44 - i * 7, 0x3E + i * 25 + yoff3, &gGame.unk_398[(gGame.menuAnimTimer / 2 + i * 4) % 30 + 1], gdl,
                                    fb, 1, 1, 3, 0);
            } else if (i == *s->cursor) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                gDPSetRenderMode(gdl++, 0x00404240, 0);
                gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
                gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
                gdl = render_draw_image(0x45 - i * 7, 0x3D + i * 25 + yoff3, &gGame.cursorImg[gGame.menuAnimTimer / 2 % 30 + 1], gdl, fb, 1,
                                    1, 3, 0);
            }
            if (s->type == SCREEN_TYPE_TRACK_LIST) {
                font_text_add(gGame.texts[i], gTracks_16[s->firstItem + i].name);
                int_to_str(gTracks_16[s->firstItem + i].h1E, buf);
                font_text_add(gGame.texts[i + s->numItems], buf);
                font_draw_text(gGame.texts[i + s->numItems], 0x112 - i * 7, 0x46 + i * 25 + yoff3);
            } else {
                font_text_add(gGame.texts[i], s->items[i].text);
            }
            font_draw_text(gGame.texts[i], 0x6E - i * 7, 0x46 + i * 25 + yoff3);
        }
        gdl = font_render(gdl);
        gDPPipeSync(gdl++);
        if (gGame.flags & 0x200) {
            gdl = shell_draw_players_difficulty_popup(gdl, fb, 1);
        } else if (gGame.screenState >= 8 && gGame.screenState <= 10) {
            gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
            gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
            gDPSetRenderMode(gdl++, 0x00404240, 0);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
            gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
            gDPPipeSync(gdl++);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, gGame.unk_748->addr);
            img = &gGame.unk_748[1];
            gdl = render_draw_image(0x9B, 0x5C, img, gdl, fb, 1, 1, 3, 0);
            gdl = render_draw_image(0x94, 0x75, img, gdl, fb, 1, 1, 3, 0);
            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            img = &gGame.unk_744[1];
            gdl = render_draw_image(0x104 - img->w * 4, 0x78 - img->h / 2, img, gdl, fb, 1, 1, 6, 0);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
            gdl = render_draw_image(0x81 - gGame.dialogSel * 7, gGame.dialogSel * 25 + 0x62, &gGame.cursorImg[gGame.menuAnimTimer / 2 % 30 + 1], gdl, fb, 1, 1,
                                3, 0);
            font_reset_queue();
            font_text_create(gGame.texts[0], 0x20, 1, (gGame.dialogSel == 0) ? 4 : 2,
                          (gGame.screenState == 10 && !(gSaveData_13.b2C4 & 8)) ? 0x20000021 : 0x21);
            switch (gGame.screenState) {
            case 9:
                font_text_add(gGame.texts[0], D_800D9C20);
                break;
            case 8:
                font_text_add(gGame.texts[0], D_800D9A4C);
                break;
            case 10:
                font_text_add(gGame.texts[0], D_800D9C34);
                break;
            }
            font_draw_text(gGame.texts[0], 0x113, 0x6A);
            font_text_create(gGame.texts[0], 0x20, 1, (gGame.dialogSel == 1) ? 4 : 2, 0x21);
            switch (gGame.screenState) {
            case 9:
                font_text_add(gGame.texts[0], D_800D9C48);
                break;
            case 8:
                font_text_add(gGame.texts[0], D_800D9C58);
                break;
            case 10:
                font_text_add(gGame.texts[0], D_800D9C70);
                break;
            }
            font_draw_text(gGame.texts[0], 0x10C, 0x83);
            gdl = font_render(gdl);
        } else if (gGame.screenState == 5 || gGame.screenState == 16) {
            if ((item->flags & SCREEN_ITEM_NEED_DIFFICULTY) || gGame.screenState == 16) {
                gdl = shell_draw_players_difficulty_popup(gdl, fb, -1);
            } else {
                gdl = shell_draw_players_difficulty_popup(gdl, fb, 0);
            }
        }
        break;
    case SCREEN_TYPE_ROUND_SELECT:
    case SCREEN_TYPE_RECORDS: {
        s32 xoff;
        s32 yoff;
        s32 u4;

        if (s->type == SCREEN_TYPE_RECORDS) {
            xoff = 0;
            yoff = 0;
        } else {
            xoff = 0;
            yoff = -11;
        }
        PREAMBLE();
        font_reset_queue();
        gDPLoadTLUT_pal256(gdl++, gGame.unk_438->addr);
        {
        if (gGame.blinkTimer < 0x20 && (s->type == SCREEN_TYPE_RECORDS || gGame.seasonMenuState == 0)) {
            if ((s->type == SCREEN_TYPE_RECORDS && *s->cursor / 5 < (gGame.numRecordTracks + 4) / 5 - 1) ||
                (s->type == SCREEN_TYPE_ROUND_SELECT &&
                 ((gGame.difficulty == 1 && *s->cursor < 1) || (gGame.difficulty == 2 && *s->cursor < 2) || (gGame.difficulty == 3 && *s->cursor < 5)))) {
                DRAW(xoff + 0x128, yoff + 0x22, &gGame.unk_438[2], 3);
            }
            if (((s->type != SCREEN_TYPE_RECORDS) ? *s->cursor : *s->cursor / 5) > 0) {
                DRAW(xoff + 0x1C, yoff + 0x22, &gGame.unk_438[1], 3);
            }
        }
        }
        TDRAWP(gGame.unk_3A0, xoff + 0x49, yoff + 0x2A, 3);
        {
        if (gCups[*s->cursor / 5].numRounds != 0) {
            s32 a;
            void *p;

            if (s->type == SCREEN_TYPE_RECORDS) {
                t = *s->cursor;
            } else {
                switch (gGame.seasonMenuState) {
                case 2:
                    t = gGame.customTrackSel;
                    break;
                case 1:
                    t = gCups[gGame.cupSel].tracks[gGame.roundSel];
                    break;
                default:
                    t = gCups[gGame.cupSel].tracks[gGame.cupPreviewTrack];
                    break;
                }
            }
            if (s->type == SCREEN_TYPE_RECORDS) {
                if (gGame.screenState == 13) {
                    shell_blend_palette_to_gray(0, gGame.itemPics[sTrackPicIndex[t]]->addr);
                } else {
                    shell_blend_palette_to_gray((0x3B - gGame.screenTimer) * 0xFF / 0x3B, gGame.itemPics[sTrackPicIndex[t]]->addr);
                }
            } else {
                shell_blend_palette_to_gray(0xFF, NULL);
            }
            img = &gGame.itemPics[sTrackPicIndex[t]][1];
            if (s->type != SCREEN_TYPE_RECORDS &&
                !((CUSTOM5 || ((gGame.seasonMenuState != 0 || UNLOCKED(*s->cursor)) && gGame.seasonMenuState != 3 &&
                              (gGame.seasonMenuState != 2 || (gUnlockedTracks & (1 << gGame.customTrackSel))))) &&
                 (!CUSTOM5 || LVLDONE))) {
                if (gGame.seasonMenuState == 0 || gGame.seasonMenuState == 3 || s->type == SCREEN_TYPE_RECORDS) {
                    font_text_create(gGame.texts[20], 0x40, 4, 2, 0x21);
                    font_text_add(gGame.texts[20], D_800D9C18);
                    font_draw_text(gGame.texts[20], xoff + 0x16F, yoff + 0x82);
                } else {
                    font_text_create(gGame.texts[20], 0x40, 1, 2, 0x21);
                    font_text_add(gGame.texts[20], D_800D9C8C);
                    font_draw_text(gGame.texts[20], xoff + 0x172, yoff + 0x74);
                    font_text_create(gGame.texts[21], 0x40, 1, 2, 0x21);
                    font_text_add(gGame.texts[21], D_800D9C98);
                    font_draw_text(gGame.texts[21], xoff + 0x16F, yoff + 0x82);
                    font_text_create(gGame.texts[22], 0x40, 1, 2, 0x21);
                    font_text_add(gGame.texts[22], D_800D9CAC);
                    font_draw_text(gGame.texts[22], xoff + 0x16E, yoff + 0x90);
                }
                gDPSetPrimColor(gdl++, 0, 0, 0x64, 0x64, 0x64, 0xFF);
                gDPLoadTLUT_pal256(gdl++, gGame.greyPalette);
                CMB_PRIM();
            } else {
                if (s->type == SCREEN_TYPE_RECORDS) {
                    if (gGame.screenState == 13) {
                        gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    } else {
                        gDPSetPrimColor(gdl++, 0, 0, 0xFF - (0x3B - gGame.screenTimer) * 0x82 / 0x3B, 0xFF - (0x3B - gGame.screenTimer) * 0x82 / 0x3B,
                                        0xFF - (0x3B - gGame.screenTimer) * 0x82 / 0x3B, 0xFF);
                    }
                    gDPLoadTLUT_pal256(gdl++, gGame.greyPalette);
                    CMB_PRIM();
                } else {
                    CMB_TEX();
                    gDPLoadTLUT_pal256(gdl++, gGame.itemPics[sTrackPicIndex[t]]->addr);
                }
            }
            DRAW(xoff + 0x10C, yoff + 0x47, img, 3);
        }
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        img = &gGame.unk_390[(s->type == SCREEN_TYPE_RECORDS) ? 6 : 7];
        DRAW(((0x200 - img->w * 8) >> 1) + 4, (0xF0 - img->h) / 2, img, 6);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        CMB_TEX();
        for (i = 0; i < 5; i++) {
            if (s->type == SCREEN_TYPE_RECORDS || ((gGame.seasonMenuState == 0 || gGame.seasonMenuState == 3) && i < gCups[*s->cursor].numRounds) ||
                (gGame.seasonMenuState != 0 && i <= gCups[*s->cursor].numRounds)) {
                if (s->type == SCREEN_TYPE_RECORDS) {
                    if (i == *s->cursor % 5) {
                        gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
                        DRAW(0x48 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &gGame.cursorImg[(gGame.menuAnimTimer / 2 + i * 4) % 30 + 1], 3);
                        font_text_create(gGame.texts[i], 0x20, 1, 4, 0x20);
                        font_text_create(gGame.texts[i + 5], 0x20, 1, 4, 0x22);
                    } else {
                        font_text_create(gGame.texts[i], 0x20, 1, 2, 0x20);
                        font_text_create(gGame.texts[i + 5], 0x20, 1, 2, 0x22);
                    }
                } else if (gGame.seasonMenuState != 0 && gGame.seasonMenuState != 3) {
                    if (i == gGame.roundSel) {
                        if (gGame.seasonMenuState == 1) {
                            gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
                        } else {
                            gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 0x204);
                        }
                        DRAW(0x48 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &gGame.cursorImg[(gGame.menuAnimTimer / 2 + i * 4) % 30 + 1], 3);
                        font_text_create(gGame.texts[i], 0x20, 1, 4, 0x20);
                        font_text_create(gGame.texts[i + 5], 0x20, 1, 4, 0x22);
                    } else {
                        font_text_create(gGame.texts[i], 0x20, 1, 2, 0x20);
                        font_text_create(gGame.texts[i + 5], 0x20, 1, 2, 0x22);
                    }
                } else {
                    if (!((UNLOCKED(*s->cursor) || CUSTOM5) && (!CUSTOM5 || LVLDONE))) {
                        gDPLoadTLUT_pal256(gdl++, gGame.unk_398->addr);
                        DRAW(0x48 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &gGame.unk_398[(gGame.menuAnimTimer / 2 + i * 4) % 30 + 1], 3);
                    } else if (((gCups[*s->cursor].won[gGame.difficulty - 1] >> i) & 1) && !CUSTOM5) {
                        gDPLoadTLUT_pal256(gdl++, gGame.unk_3A8->addr);
                        DRAW(0x49 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &gGame.unk_3A8[(gGame.menuAnimTimer / 2 + i * 4) % 30 + 1], 3);
                    }
                    if (i == gGame.cupPreviewTrack) {
                        if (!((UNLOCKED(*s->cursor) || CUSTOM5) && (!CUSTOM5 || LVLDONE))) {
                            font_text_create(gGame.texts[i], 0x20, 1, 4, 0x20000020);
                            font_text_create(gGame.texts[i + 5], 0x20, 1, 4, 0x20000022);
                        } else {
                            font_text_create(gGame.texts[i], 0x20, 1, 4, 0x20);
                            font_text_create(gGame.texts[i + 5], 0x20, 1, 4, 0x22);
                        }
                    } else {
                        if (!((UNLOCKED(*s->cursor) || CUSTOM5) && (!CUSTOM5 || LVLDONE))) {
                            font_text_create(gGame.texts[i], 0x20, 1, 2, 0x20000020);
                            font_text_create(gGame.texts[i + 5], 0x20, 1, 2, 0x20000022);
                        } else {
                            font_text_create(gGame.texts[i], 0x20, 1, 2, 0x20);
                            font_text_create(gGame.texts[i + 5], 0x20, 1, 2, 0x22);
                        }
                    }
                }
                if (s->type == SCREEN_TYPE_RECORDS || i < gCups[*s->cursor].numRounds) {
                    if ((s->type != SCREEN_TYPE_RECORDS && gGame.seasonMenuState != 2) || (s->type == SCREEN_TYPE_RECORDS && (*s->cursor / 5) * 5 + i < gGame.numRecordTracks)) {
                        if (s->type == SCREEN_TYPE_RECORDS) {
                            switch ((*s->cursor / 5) * 5 + i) {
                            case 0x15:
                            case 0x16:
                            case 0x17:
                                strcpy(buf2, "-");
                                break;
                            default:
                                int_to_str(gTracks_16[gGame.recordTracks[(*s->cursor / 5) * 5 + i]].h1E, buf2);
                                break;
                            }
                        } else if (*s->cursor == 5) {
                            int_to_str(gCups[5].laps[i], buf2);
                        } else if (*s->cursor == 4) {
                            int_to_str(1, buf2);
                        } else {
                            int_to_str(gTracks_16[gCups[*s->cursor].tracks[i]].h1E, buf2);
                        }
                        font_text_add(gGame.texts[i + 5], buf2);
                        font_draw_text(gGame.texts[i + 5], 0x11A - i * 7 + xoff, 0x52 + i * 25 + yoff);
                    }
                    if (s->type == SCREEN_TYPE_RECORDS) {
                        if ((*s->cursor / 5) * 5 + i < gGame.numRecordTracks) {
                            font_text_add(gGame.texts[i], gTracks_16[gGame.recordTracks[(*s->cursor / 5) * 5 + i]].name);
                        }
                    } else {
                        font_text_add(gGame.texts[i], gTracks_16[gCups[*s->cursor].tracks[i]].name);
                    }
                    font_draw_text(gGame.texts[i], 0x6F - i * 7 + xoff, 0x52 + i * 25 + yoff);
                } else {
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    CMB_PRIM();
                    if (*s->cursor == 5) {
                        img = &gGame.unk_390[5];
                    }
                    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
                    DRAW(0x63 - i * 7 + xoff, 0x45 + i * 25 + yoff, img, 6);
                    if (gGame.seasonMenuState != 1) {
                        img = &gGame.unk_390[4];
                        DRAW(0x47 - i * 7 + xoff, 0x46 + i * 25 + yoff, img, 6);
                    }
                }
            } else {
                gDPSetTextureLUT(gdl++, G_TT_NONE);
                CMB_PRIM();
                img = &gGame.unk_390[4];
                gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xF0);
                DRAW(0x47 - i * 7 + xoff, 0x46 + i * 25 + yoff, img, 6);
                if (*s->cursor == 5) {
                    img = &gGame.unk_390[5];
                }
                DRAW(0x63 - i * 7 + xoff, 0x45 + i * 25 + yoff, img, 6);
            }
        }
        if (s->type == SCREEN_TYPE_RECORDS) {
            strcpy(buf, "Records");
        } else if (*s->cursor == 4) {
            strcpy(buf, "Challenge");
        } else if (*s->cursor == 5) {
            if (gGame.seasonMenuState == 2 && (gFrameCounter & 0x20)) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                CMB_TEX();
                gDPLoadTLUT_pal256(gdl++, gGame.unk_3B0->addr);
                DRAW(xoff + 0x16E, yoff + 0x34, &gGame.unk_3B0[2], 3);
                DRAW(xoff + 0x149, yoff + 0xBE, &gGame.unk_3B0[2], 0x23);
                DRAW(xoff + 0xFE, yoff + 0x79, &gGame.unk_3B0[1], 0x43);
                DRAW(xoff + 0x1C1, yoff + 0x79, &gGame.unk_3B0[1], 3);
            }
            switch (gGame.seasonMenuState) {
            case 0:
                strcpy(buf, "Custom");
                if (LVLDONE) {
                    for (i = 0; i < 3; i++) {
                        font_text_create(gGame.texts[i + 12], 0x40, 1, (i == gGame.roundSel) ? 4 : 2, 0x21);
                    }
                    if (gCups[*s->cursor].numRounds == 0) {
                        font_text_add(gGame.texts[12], D_800D9CDC);
                        font_draw_text(gGame.texts[12], xoff + 0x16E, yoff + 0x80);
                    } else {
                        font_text_add(gGame.texts[12], D_800D9A30);
                        font_draw_text(gGame.texts[12], xoff + 0x16E, yoff + 0x79);
                        font_text_add(gGame.texts[13], D_800D9A28);
                        font_draw_text(gGame.texts[13], xoff + 0x16E, yoff + 0x87);
                    }
                }
                break;
            case 1:
                strcpy(buf, "Select Slot");
                font_text_create(gGame.texts[12], 0x40, 5, 4, 0x20);
                if (gGame.roundSel == gCups[*s->cursor].numRounds) {
                    font_text_add(gGame.texts[12], D_800D9CF0);
                } else {
                    font_text_add(gGame.texts[12], D_800D9D14);
                }
                font_draw_text(gGame.texts[12], 0x2D, yoff + 0xCF);
                break;
            case 2:
                strcpy(buf, "Choose Track");
                font_text_create(gGame.texts[12], 0x40, 1, 4, 0x20);
                switch ((s16)(gGame.customTrackSel / 5)) {
                case 0:
                    font_text_add(gGame.texts[12], D_800D9D54);
                    break;
                case 1:
                    font_text_add(gGame.texts[12], D_800D9D68);
                    break;
                case 2:
                    font_text_add(gGame.texts[12], D_800D9D7C);
                    break;
                case 3:
                    font_text_add(gGame.texts[12], D_800D9D90);
                    break;
                }
                font_draw_text(gGame.texts[12], 0x2D, yoff + 0xCF);
                break;
            }
        } else {
            switch (*s->cursor) {
            case 0:
                strcpy(buf, "Bronze Round");
                break;
            case 1:
                strcpy(buf, "Silver Round");
                break;
            case 2:
                strcpy(buf, "Gold Round");
                break;
            case 3:
                strcpy(buf, "Platinum Round");
                break;
            }
        }
        font_text_create(gGame.texts[11], 0x40, 4, 2, 0x21);
        font_text_add(gGame.texts[11], buf);
        font_draw_text(gGame.texts[11], xoff + 0xBD, yoff + 0x34);
        if (s->type != SCREEN_TYPE_RECORDS && gGame.seasonMenuState == 0) {
            font_text_create(gGame.texts[10], 0x40, 1, 2, 0x20);
            if (gGame.difficulty < SAVE.b2C0 || (SAVE.b2C0 == gGame.difficulty && ((SAVE.b2BF >> *s->cursor) & 1))) {
                font_text_add(gGame.texts[10], D_800D9DE4);
            } else if ((SAVE.b2C0 > gGame.difficulty || (SAVE.b2C0 == gGame.difficulty && (*s->cursor == 0 || ((SAVE.b2BF >> (*s->cursor - 1)) & 1)))) || (CUSTOM5 && SAVE.b2C0 == gGame.difficulty && (SAVE.b2BF & 8))) {
                s32 fl;

                fl = 0;
                switch (gGame.difficulty) {
                case 1:
                    if (*s->cursor == 1) {
                        fl = 1;
                    }
                    break;
                case 2:
                    if (*s->cursor == 2) {
                        fl = 1;
                    }
                    break;
                case 3:
                    if (*s->cursor == 4) {
                        fl = 1;
                    }
                    break;
                }
                if (fl) {
                    if (gGame.difficulty == 3) {
                        font_text_add(gGame.texts[10], D_800D9DF4);
                    } else {
                        font_text_add(gGame.texts[10], D_800D9E24);
                    }
                } else if (*s->cursor == 5) {
                    if (gCups[5].numRounds != 0) {
                        if (gGame.roundSel == 0) {
                            font_text_add(gGame.texts[10], D_800D9E50);
                        } else {
                            font_text_add(gGame.texts[10], D_800D9E6C);
                        }
                    } else {
                        font_text_add(gGame.texts[10], D_800D9E94);
                    }
                } else {
                    font_text_add(gGame.texts[10], D_800D9EC0);
                }
            } else if (CUSTOM5) {
                font_text_add(gGame.texts[10], D_800D9EEC);
            } else {
                font_text_add(gGame.texts[10], D_800D9F14);
            }
            font_draw_text(gGame.texts[10], xoff + 0x2D, yoff + 0xCF);
        }
        gdl = font_render(gdl);
        PSYNCK(u4);
        if (s->type == SCREEN_TYPE_RECORDS) {
            s32 dx, dy;

            trk = &gTracks_16[gGame.recordTracks[gGame.recordsSel]];

            if (gGame.screenState == 13) {
                D_800CA80F = 0;
            } else {
                D_800CA80F = (0x3B - gGame.screenTimer) * 0xFF / 0x3B;
            }
            font_reset_queue();
            if (trk->f50 & 0x400) {
                font_text_create(gGame.texts[0], 0x20, 1, 9, 0x21);
                font_text_add(gGame.texts[0], D_800D9F40);
                font_draw_text(gGame.texts[0], 0x175, 0x6D);
                for (j = 0; j < 3; j++) {
                    font_text_create(gGame.texts[0], 0x20, 1, 9, 0x20);
                    int_to_str(trk->f34[j], buf);
                    font_text_add(gGame.texts[0], buf);
                    font_draw_text(gGame.texts[0], 0x134 - j * 4, 0x7E + j * 13);
                    font_text_create(gGame.texts[0], 0x20, 1, 9, 0x22);
                    __builtin_memcpy(buf, trk->init[j], 3);
                    buf[3] = 0;
                    font_text_add(gGame.texts[0], buf);
                    font_draw_text(gGame.texts[0], 0x1AC - j * 4, 0x7E + j * 13);
                }
            } else {
                if (gGame.recordsSel == 0x15 || gGame.recordsSel == 0x17) {
                    dx = -4;
                    dy = 0x14;
                } else {
                    dx = 0;
                    dy = 0;
                }
                font_text_create(gGame.texts[0], 0x20, 1, 9, 0x21);
                font_text_add(gGame.texts[0], D_800D9F4C);
                font_draw_text(gGame.texts[0], dx + 0x17B, dy + 0x59);
                for (j = 0; j < 3; j++) {
                    font_text_create(gGame.texts[0], 0x20, 1, 9, 0x20);
                    event_format_time(trk->f34[j], buf);
                    font_text_add(gGame.texts[0], buf);
                    font_draw_text(gGame.texts[0], dx + (0x175 - j * 4) - 0x3C, (0x6A + j * 13) + dy);
                    font_text_create(gGame.texts[0], 0x20, 1, 9, 0x22);
                    __builtin_memcpy(buf, trk->init[j], 3);
                    buf[3] = 0;
                    font_text_add(gGame.texts[0], buf);
                    font_draw_text(gGame.texts[0], dx + (0x175 - j * 4) + 0x3C, (0x6A + j * 13) + dy);
                }
                if (gGame.recordsSel != 0x15 && gGame.recordsSel != 0x17) {
                    font_text_create(gGame.texts[0], 0x20, 1, 9, 0x21);
                    font_text_add(gGame.texts[0], D_800D9F58);
                    font_draw_text(gGame.texts[0], 0x168, 0x9C);
                    font_text_create(gGame.texts[0], 0x20, 1, 9, 0x20);
                    event_format_time(trk->f40, buf);
                    font_text_add(gGame.texts[0], buf);
                    font_draw_text(gGame.texts[0], 0x129, 0xAC);
                    font_text_create(gGame.texts[0], 0x20, 1, 9, 0x22);
                    __builtin_memcpy(buf, trk->init4D, 3);
                    buf[3] = 0;
                    font_text_add(gGame.texts[0], buf);
                    font_draw_text(gGame.texts[0], 0x1A1, 0xAC);
                }
            }
            gdl = font_render(gdl);
        } else if (gGame.flags & 0x200) {
            gdl = shell_draw_players_difficulty_popup(gdl, fb, 1);
        }
        break;
    }
    case SCREEN_TYPE_OPTIONS: {
        s32 u5;

        font_reset_queue();
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        if (gGame.screenState == 14) {
            gGame.screenState = 15;
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        CMB_TEX();
        {
            Img *_t = &gGame.unk_3A0[1];

            gDPLoadTLUT_pal256(gdl++, gGame.unk_3A0->addr);
            gDPPipeSync(gdl++);
            DRAWK(0x4E, 0x1B, _t, 3, u5);
        }
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        CMB_PRIM();
        for (j = 0; j < 3; j++) {
            for (k = 0; k < 29; k++) {
                x = 0x124 - j * 7 + k * 6;
                y = sOptionsRowY[j] - 1;
                if (*(SAVE.vol + j) * 29 / 255 >= k) {
                    if (j == gGame.optionsSel) {
                        gDPSetPrimColor(gdl++, 0, 0, 0xFF, k * 0xFF / 29, 0, 0xFF);
                    } else {
                        gDPSetPrimColor(gdl++, 0, 0, 0x8C, k * 0x8C / 29, 0, 0xFF);
                    }
                } else {
                    gDPSetPrimColor(gdl++, 0, 0, 0x4B, 0x4B, 0x4B, 0xFF);
                }
                DRAW(x, y, &gGame.unk_390[1], 6);
            }
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        img = &gGame.unk_390[2];
        DRAW(0x100 - img->w * 4, 0x78 - (img->h >> 1), img, 6);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        gDPLoadTLUT_pal256(gdl++, gGame.cursorPal + 4);
        img = &gGame.cursorImg[gGame.menuAnimTimer / 2 % 30 + 1];
        for (j = 0; j < 11; j++) {
            if (j < 3) {
                x = 0x52 - j * 7;
            } else {
                x = 0x52 - ((j - 3) / 2 + 3) * 7 + ((j - 3) & 1) * 194;
            }
            y = sOptionsRowY[j];
            if (j == 9 || j == 10) {
                font_text_create(gGame.texts[0], 0x40, 1, (j == gGame.optionsSel) ? 4 : 2, 0x20000020);
            } else {
                font_text_create(gGame.texts[0], 0x40, 1, (j == gGame.optionsSel) ? 4 : 2,
                              (gHasExpansionPak_2 == 0 && j == 7) ? 0x20000020 : 0x20);
            }
            font_text_add(gGame.texts[0], gOptionsLabels[j]);
            font_draw_text(gGame.texts[0], x + 0x24 + ((u16)y >> 16), y + 8);
            if (j == gGame.optionsSel) {
                DRAW(x - 2, y, img, 3);
            }
            switch (j) {
            case 3:
                font_text_create(gGame.texts[0], 0x40, 1, (gGame.optionsSel == j) ? 6 : 2, 0x22);
                font_text_add(gGame.texts[0], gMonoStereoStrings[SAVE.b2C4 & 1]);
                font_draw_text(gGame.texts[0], x + 0xB4 + ((u16)x >> 16), y + 8);
                break;
            case 8:
                font_text_create(gGame.texts[0], 0x40, 1, (gGame.optionsSel == j) ? 6 : 2, 0x22);
                font_text_add(gGame.texts[0], sOffOnStrings[(SAVE.b2C4 & 0x10) == 0]);
                font_draw_text(gGame.texts[0], x + 0xB4, y + 8);
                break;
            case 7:
                tmp = 2;
                if (gGame.optionsSel == j) {
                    tmp = 4;
                    if (gHasExpansionPak_2 != 0) {
                        tmp = 6;
                    }
                }
                font_text_create(gGame.texts[0], 0x40, 1, tmp, (gHasExpansionPak_2 == 0) ? 0x20000022 : 0x22);
                font_text_add(gGame.texts[0], sOffOnStrings[(SAVE.b2C4 & 2) >> 1]);
                font_draw_text(gGame.texts[0], x + 0xB4, y + 8);
                break;
            }
        }
        font_text_create(gGame.texts[0], 0x40, 4, 2, 0x20);
        font_text_add(gGame.texts[0], D_800D9370);
        font_draw_text(gGame.texts[0], 100, 0x23);
        gdl = font_render(gdl);
        if (gGame.screenState == 6 || gGame.screenState == 15 || gGame.screenState == 14) {
            gdl = shell_draw_yes_no_dialog(gdl, fb, D_800D9F64);
        }
        break;
    }
    case SCREEN_TYPE_PAK_STATUS:
    case SCREEN_TYPE_BACKUP_LOST:
        font_reset_queue();
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        img = &gGame.unk_3DC[1];
        gDPLoadTLUT_pal256(gdl++, gGame.unk_3DC->addr);
        if (s->type == SCREEN_TYPE_BACKUP_LOST) {
            DRAW(0x106 - img->w * 4, 0x41, img, 3);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            CMB_PRIM();
            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
            img = &gGame.unk_390[1];
            DRAWV((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, 6);
            font_text_create(gGame.texts[0], 0x40, 4, 2, 0x21);
            font_text_add(gGame.texts[0], D_800D9F7C);
            font_draw_text(gGame.texts[0], 0x106, 0x4C);
            for (i = 0; sBackupLostMsg[i] != NULL; i++) {
                font_text_create(gGame.texts[0], 0x40, 1, 4, 0x21);
                str_copy(buf, sBackupLostMsg[i]);
                font_text_add(gGame.texts[0], buf);
                font_draw_text(gGame.texts[0], 0x100 - i * 4, i * 13 + 0x6D);
            }
        } else {
            s32 slot;
            s32 found = 0;
            s32 line = 0;
            s32 col;
            char **pp;
            s32 k2;

            DRAW(0x110 - img->w * 4, 0x1F, img, 3);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            CMB_PRIM();
            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
            img = &gGame.unk_390[1];
            DRAWV((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, 6);
            font_text_create(gGame.texts[0], 0x40, 4, 2, 0x21);
            font_text_add(gGame.texts[0], D_800D9F84);
            font_draw_text(gGame.texts[0], 0x110, 0x28);
            font_text_create(gGame.texts[0], 0x40, 5, 2, 0x21);
            font_text_add(gGame.texts[0], D_800D9F94);
            font_draw_text(gGame.texts[0], 400, 0x25);
            for (slot = 0; slot < 5; slot++) {
                if (slot == 4) {
                    if (found) {
                        break;
                    }
                    sprintf(buf3, D_800D9F98);
                    line = 4;
                    gPakFreePages[4] = 0;
                    col = 8;
                    pp = sPakMissingMsg;
                } else if (gPakFreePages[slot] != -2) {
                    strcpy(buf3, "Controller Pak #X ");
                    found = 1;
                    if (gPakFreePages[slot] <= 0) {
                        str_cat(buf3, D_800D9FD4);
                    } else {
                        str_cat(buf3, D_800D9FE0);
                        int_to_str(gPakFreePages[slot], buf3 + strlen_2(buf3));
                        str_cat(buf3, D_800D9FE8);
                        if (gPakFreePages[slot] != found) {
                            str_cat(buf3, D_800D9FF4);
                        }
                        str_cat(buf3, D_800D91C8);
                    }
                    buf3[16] = slot + '1';
                    if (gPakFreePages[slot] < 40) {
                        col = 8;
                        pp = sPakFewPagesMsg;
                    } else {
                        col = 7;
                        pp = sPakOkMsg;
                    }
                } else {
                    continue;
                }
                font_text_create(gGame.texts[0], 0x40, 5, 2, 0x21);
                font_text_add(gGame.texts[0], buf3);
                font_draw_text(gGame.texts[0], 0x106 - line * 3, line * 11 + 0x46);
                line++;
                for (k2 = 0; pp[k2] != NULL; k2++) {
                    font_text_create(gGame.texts[0], 0x40, 5, col, 0x21);
                    font_text_add(gGame.texts[0], pp[k2]);
                    font_draw_text(gGame.texts[0], 0x106 - line * 3, line * 11 + 0x46);
                    line++;
                }
            }
        }
        gdl = font_render(gdl);
        break;
    case SCREEN_TYPE_CHEAT_ENTRY:
    case SCREEN_TYPE_INITIALS: {
        s32 u6;
        s32 plr;
        s32 cheat;

        font_reset_queue();
        cheat = (s->type == SCREEN_TYPE_CHEAT_ENTRY);
        if (!cheat) {
            plr = gGame.recordPlayer[gGame.entryRecord];
        }
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        gDPLoadTLUT_pal256(gdl++, gGame.itemPics[0]->addr);
        DRAW(0x108, 0x47, &gGame.itemPics[0][1], 3);
        TDRAWP(gGame.unk_3A0, 0x45, 0x2A, 3);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        img = &gGame.unk_390[1];
        DRAWV(0x100 - img->w * 4, 0x78 - (img->h >> 1), img, 6);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        img = &gGame.unk_39C[gGame.blinkTimer / 2 % 60 + 1];
        if (gGame.numPlayers >= 2 && !cheat) {
            gDPLoadTLUT_pal256(gdl++, gGame.playerPalettes + plr * 0x100);
        } else {
            gDPLoadTLUT_pal256(gdl++, gGame.unk_39C->addr);
        }
        DRAWK(0x18B, 0x2F, img, 3, u6);
        font_text_create(gGame.texts[0], 0x20, 4, 2, 0x20);
        if (cheat) {
            strcpy(buf, "Enter A Cheat Code");
            font_text_add(gGame.texts[0], buf);
            font_draw_text(gGame.texts[0], 0x55, 0x35);
        } else {
            strcpy(buf, "Player ");
            int_to_str(plr + 1, buf + strlen_2(buf));
            str_cat(buf, D_800DA014);
            font_text_add(gGame.texts[0], buf);
            font_draw_text(gGame.texts[0], 0x55, 0x35);
        }
        if (cheat && gGame.screenState == 17) {
            font_text_create(gGame.texts[0], 0x20, 1, 2, 0x21);
            font_text_add(gGame.texts[0], sCheatTable[gGame.cheatEntered].name);
            if (((1 << gGame.cheatEntered) == 4 && (gGame.flags & 0x20000000)) || ((sCheatMask >> gGame.cheatEntered) & 1)) {
                font_text_add(gGame.texts[0], D_800DA020);
            } else {
                font_text_add(gGame.texts[0], D_800DA02C);
            }
            font_draw_text(gGame.texts[0], 0xAC, 0x71);
            font_text_create(gGame.texts[0], 0x20, 1, 2, 0x21);
            font_text_add(gGame.texts[0], D_800DA038);
            font_draw_text(gGame.texts[0], 0xA8, 0x7F);
        } else if (cheat && gGame.screenState == 18) {
            font_text_create(gGame.texts[0], 0x20, 1, 2, 0x21);
            font_text_add(gGame.texts[0], D_800DA04C);
            font_draw_text(gGame.texts[0], 0xAC, 0x71);
            font_text_create(gGame.texts[0], 0x20, 1, 2, 0x21);
            font_text_add(gGame.texts[0], D_800DA038);
            font_draw_text(gGame.texts[0], 0xA8, 0x7F);
        } else {
            s32 xx;

            for (y = 0; y < 5; y++) {
                for (x = 0; x < 8; x++) {
                    xx = x * 26;
                    font_text_create(gGame.texts[0], 0x20, (y * 8 + x > 0x25) ? 2 : 1, 2,
                                  (gGame.nameLen == ((cheat) ? 10 : 3) && y * 8 + x < 0x26) ? 0x20000021 : 0x21);
                    font_text_add(gGame.texts[0], gKeyboardKeys[y * 8 + x]);
                    font_draw_text(gGame.texts[0], xx + 0x54 - y * 4, y * 17 + 0x56);
                    if (y * 8 + x == gGame.keyIndex) {
                        gDPSetRenderMode(gdl++, 0x00404240, 0);
                        gDPSetTextureLUT(gdl++, G_TT_NONE);
                        CMB_PRIM();
                        img = &gGame.unk_390[3];
                        DRAW(xx + 0x54 - y * 4 - img->w * 4 + 4, y * 17 - (img->h >> 1) + 0x57, img, 6);
                    }
                }
            }
        }
        font_text_create(gGame.texts[0], 0x20, 2, (j == gGame.nameLen) ? 4 : 2, 0x20);
        if (!cheat) {
            switch (gGame.entryRecord) {
            case 0:
            case 1:
            case 2:
                if (gGame.entryRecord != 0) {
                    buf[0] = gGame.entryRecord + '1';
                    buf[1] = gGame.entryRecord + '{';
                    buf[2] = ' ';
                    buf[3] = '\0';
                    font_text_add(gGame.texts[0], buf);
                }
                if (gCurTrack_41->f50 & 0x400) {
                    font_text_add(gGame.texts[0], D_800DA060);
                } else {
                    font_text_add(gGame.texts[0], D_800DA06C);
                }
                if (gGame.recordPlayer[3] == plr || gGame.unk_7A8 == plr) {
                    font_text_add(gGame.texts[0], D_800DA078);
                }
                break;
            case 3:
                font_text_add(gGame.texts[0], D_800D9F58);
                break;
            }
            font_draw_text(gGame.texts[0], 0x2F, 0xB6);
        }
        for (j = 0; (cheat) ? (j < 10) : (j < 3); j++) {
            font_text_create(gGame.texts[0], 0x20, 1, (j == gGame.nameLen) ? 4 : 2, 0x21);
            font_text_add(gGame.texts[0], D_800DA084);
            font_draw_text(gGame.texts[0], (j - 1) * 20 + ((cheat) ? 0x50 : 0xE2), 0xB9);
            if (j < strlen_2(gGame.name)) {
                font_text_create(gGame.texts[0], 0x20, 1, (j == gGame.nameLen) ? 4 : 2, 0x21);
                buf[0] = gGame.name[j];
                buf[1] = '\0';
                font_text_add(gGame.texts[0], buf);
                font_draw_text(gGame.texts[0], (j - 1) * 20 + ((cheat) ? 0x50 : 0xE3), 0xB3);
            }
        }
        gdl = font_render(gdl);
        break;
    }
    case SCREEN_TYPE_CONTROLLER_SETTINGS: {
        Game *gp = &gGame;

        font_reset_queue();
        {
            Gfx *_g = gdl++;

            _g->words.w0 = 0xE7000000 | ((u8)(u32)_g >> 8);
            _g->words.w1 = ((u16)(u32)_g >> 16) | (((u32)_g & 0x7F) >> 7);
        }
        gDPSetCycleType(gdl++, G_CYC_1CYCLE);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetTextureFilter(gdl++, G_TF_BILERP);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        img = &gp->unk_45C[1];
        gDPLoadTLUT_pal256(gdl++, gp->unk_45C->addr);
        DRAW(0xFE - img->w * 4, 0x74 - (img->h >> 1), img, 3);
        TDRAWP(gp->unk_3A0, 0x4A, 0x20, 3);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        font_text_create(gp->texts[0], 0x40, 4, 2, 0x21);
        strcpy(buf, "Controller ");
        int_to_str(gp->padConfigSel + 1, buf + strlen_2(buf));
        font_text_add(gp->texts[0], buf);
        font_draw_text(gp->texts[0], 0xBA, 0x29);
        font_text_create(gp->texts[0], 0x40, 4, 2, 0x21);
        strcpy(buf, "Configuration ");
        buf[strlen_2(buf) + 1] = '\0';
        buf[strlen_2(buf)] = gp->padConfig[gp->padConfigSel] + 'A';
        font_text_add(gp->texts[0], buf);
        font_draw_text(gp->texts[0], 0xE9, 0xBF);
        for (j = 0; j < 9; j++) {
            tmp = 6;
            if (j & 1) {
                tmp = 4;
            }
            a = 0x20;
            if (j < 5) {
                a = 0x22;
            }
            if (!(j & 1)) {
                a |= 0x40000000;
            }
            font_text_create(gp->texts[0], 0x40, 5, tmp, a);
            font_text_add(gp->texts[0], gControlActionLabels[gControlConfigs[(s8)*(gp->padConfig + gp->padConfigSel)][j]]);
            font_draw_text(gp->texts[0], gControlLabelPos[j].x, gControlLabelPos[j].y);
            if (gControlConfigs[(s8)*(gp->padConfig + gp->padConfigSel)][j] == 6) {
                tmp = 6;
                if (j & 1) {
                    tmp = 4;
                }
                a = 0x20;
                if (j < 5) {
                    a = 0x22;
                }
                if (!(j & 1)) {
                    a |= 0x40000000;
                }
                font_text_create(gp->texts[0], 0x40, 5, tmp, a);
                font_text_add(gp->texts[0], D_800CDAC8_2);
                font_draw_text(gp->texts[0], gControlLabelPos[j].x, gControlLabelPos[j].y + 11);
            }
        }
        for (j = 0; j < 3; j++) {
            font_text_create(gGame.texts[0], 0x40, 5, (j == 2) ? 4 : 6, (j != 2) ? 0x40000020 : 0x20);
            font_text_add(gGame.texts[0], gCameraButtonLabels[j]);
            font_draw_text(gGame.texts[0], gCameraLabelPos[j].x, gCameraLabelPos[j].y);
        }
        for (j = 0; j < gGame.numPlayers; j++) {
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        img = &gGame.unk_390[1];
        DRAWV(0x100 - img->w * 4, 0x78 - (img->h >> 1), img, 6);
        for (j = 0; j < 4; j++) {
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            CMB_TEX();
            img = &gGame.unk_39C[gGame.blinkTimer / 2 % 60 + 1];
            if (j == gGame.padConfigSel) {
                gDPLoadTLUT_pal256(gdl++, gGame.playerPalettes + j * 0x100);
            } else {
                gDPLoadTLUT_pal256(gdl++, gGame.greyPalette);
            }
            DRAW(0x132 + j * 0x2C, 0x24, img, 3);
        }
        gdl = font_render(gdl);
        break;
    }
    case SCREEN_TYPE_SEASON_RACE_RESULT:
#define YR (0x46 + j * rowh + ry)
    case SCREEN_TYPE_RACE_RESULT:
    case SCREEN_TYPE_STANDINGS: {
        s32 x;
        s32 y;
        s32 px;

        font_reset_queue();
        rx = 0;
        if (s->type == SCREEN_TYPE_STANDINGS) {
            if (CHAMP4) {
                rx = -8;
                ry = 0x1B;
            } else {
                ry = -7;
            }
        } else if (gGame.mode == 8) {
            ry = 0x32;
            rx = -0xD;
        } else {
            ry = 0;
        }
        for (j = 0; j < 3; j++) {
            font_text_create(gGame.texts[j], 0x20, 4, 2, 0x20);
        }
        if (s->type == SCREEN_TYPE_STANDINGS && !CHAMP4) {
            font_text_add(gGame.texts[0], D_800DA0A4);
            font_text_add(gGame.texts[1], D_800DA0B8);
            font_draw_text(gGame.texts[1], 0x18F, ry + 0x28);
        } else {
            if (gGame.mode == 8) {
                font_text_add(gGame.texts[0], D_800DA0BC);
            } else {
                font_text_add(gGame.texts[0], D_800DA0D0);
            }
            font_text_add(gGame.texts[2], D_800DA0E0);
            if (s->type == SCREEN_TYPE_SEASON_RACE_RESULT) {
                font_text_add(gGame.texts[1], D_800DA0B8);
                font_draw_text(gGame.texts[1], 0x147, ry + 0x28);
                font_draw_text(gGame.texts[2], 0x18D, ry + 0x28);
            } else if (s->type == SCREEN_TYPE_STANDINGS && CHAMP4) {
                font_draw_text(gGame.texts[2], 0x14D, ry + 0x2A);
            } else if (gGame.mode == 8) {
                font_draw_text(gGame.texts[2], 0x178, ry + 0x28);
            } else {
                font_draw_text(gGame.texts[2], 0x184, ry + 0x28);
            }
        }
        if (s->type == SCREEN_TYPE_STANDINGS && CHAMP4) {
            font_draw_text(gGame.texts[0], 0x50, ry + 0x2A);
            font_text_create(gGame.texts[0], 0x20, 4, 2, 0x21);
            if (gEvent.finishOrder[0] == 0) {
                font_text_add(gGame.texts[0], D_800DA0E8);
            } else {
                font_text_add(gGame.texts[0], D_800DA0FC);
            }
            font_draw_text(gGame.texts[0], 0xEE, 0x97);
        } else {
            font_draw_text(gGame.texts[0], 0x55, ry + 0x28);
        }
        rowh = 0x19;
        if (s->type == SCREEN_TYPE_STANDINGS && CHAMP4) {
            rowh = 0x17;
        }
        for (j = 0; j < gEvent.numRiders; j++) {
            if (s->type == SCREEN_TYPE_STANDINGS && !CHAMP4) {
                r = gSeason->standings[j];
            } else {
                r = gEvent.finishOrder[j];
            }
            img = (Img *)((r < gGame.numPlayers || gGame.mode == 8) ? ((gGame.mode == 8) ? ((r == D_800C8CE0) ? 4 : 2) : 4) : 2);
            font_text_create(gGame.texts[j * 4 + 5], 0x20, 1, (s32)img, 0x20);
            pl = j;
            while (pl > 0 && s->type == SCREEN_TYPE_RACE_RESULT && gRiders[gEvent.finishOrder[pl]].race.finishTime == gRiders[gEvent.finishOrder[pl - 1]].race.finishTime) {
                s32 d0;

                pl--;
            }
            int_to_str(pl + 1, buf);
            font_text_add(gGame.texts[j * 4 + 5], buf);
            x = rx + 0x4E - j * 7;
            font_draw_text(gGame.texts[j * 4 + 5], (s->type == SCREEN_TYPE_STANDINGS && CHAMP4) ? x - 2 : x, YR);
            str_copy(buf, sRiderBios[gGame.riders[r]].name);
            font_text_create(gGame.texts[j * 4 + 6], 0x20, 1, (s32)img, 0x20);
            font_text_add(gGame.texts[j * 4 + 6], buf);
            font_draw_text(gGame.texts[j * 4 + 6], rx - (j * 7 - 0x6A), YR);
            if (r < 4) {
                wid[r] = font_get_char_pos(gGame.texts[j * 4 + 6], 0x6A - j * 7, YR, strlen_2(buf) - 1) >> 16;
            }
            if (s->type == SCREEN_TYPE_SEASON_RACE_RESULT && gRacePoints[j] != 0) {
                font_text_create(gGame.texts[j * 4 + 7], 0x20, 1, (s32)img, 0x20);
                int_to_str(gRacePoints[j], buf + 1);
                buf[0] = '+';
                font_text_add(gGame.texts[j * 4 + 7], buf);
                font_draw_text(gGame.texts[j * 4 + 7], 0x14C - j * 7, YR);
            }
            font_text_create(gGame.texts[j * 4 + 7], 0x20, 1, (s32)img, 0x20);
            if (s->type == SCREEN_TYPE_STANDINGS && !CHAMP4) {
                int_to_str(gSeason->points[r], buf);
            } else {
                event_format_time(gRiders[r].race.finishTime, buf);
            }
            font_text_add(gGame.texts[j * 4 + 7], buf);
            switch (s->type) {
            case SCREEN_TYPE_SEASON_RACE_RESULT:
                font_draw_text(gGame.texts[j * 4 + 7], 0x186 - j * 7, YR);
                break;
            case SCREEN_TYPE_RACE_RESULT:
                if (gGame.mode == 8) {
                    font_draw_text(gGame.texts[j * 4 + 7], 0x170 - j * 7, YR);
                } else {
                    font_draw_text(gGame.texts[j * 4 + 7], 0x17C - j * 7, YR);
                }
                break;
            case SCREEN_TYPE_STANDINGS:
                if (CHAMP4) {
                    font_draw_text(gGame.texts[j * 4 + 7], 0x146 - j * 7, YR);
                } else {
                    font_draw_text(gGame.texts[j * 4 + 7], 0x199 - j * 7, YR);
                }
                break;
            }
        }
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        TDRAW(gGame.unk_414, rx + 0x44, ry + 0x1F, 3);
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        if (s->type == SCREEN_TYPE_STANDINGS && CHAMP4) {
            img = &gGame.unk_390[1];
            DRAWV(((0x200 - img->w * 8) >> 1) | 1, (0xF0 - img->h) / 2, img, 6);
        } else if (gGame.mode == 8) {
            img = &gGame.unk_390[4];
            DRAWV(((0x200 - img->w * 8) >> 1) | 1, (0xF0 - img->h) / 2, img, 6);
        } else {
            img = &gGame.unk_390[(s->type == SCREEN_TYPE_SEASON_RACE_RESULT) ? 2 : 3];
            DRAWV(((0x200 - img->w * 8) >> 1) | 1, (0xF0 - img->h) / 2 + ry, img, 6);
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        img = &gGame.unk_39C[gGame.blinkTimer / 2 % 60 + 1];
        for (j = 0; j < gGame.numPlayers; j++) {
            if (gGame.numPlayers >= 2 && gGame.mode != 8) {
                gDPLoadTLUT_pal256(gdl++, gGame.playerPalettes + j * 0x100);
                font_text_create(gGame.texts[gEvent.numRiders * 4 + 5 + j], 0x20, 5, 6, 0x21);
                buf[0] = j + '1';
                buf[1] = '\0';
                font_text_add(gGame.texts[gEvent.numRiders * 4 + 5 + j], buf);
                font_draw_text(gGame.texts[gEvent.numRiders * 4 + 5 + j], wid[j] + 5 + img->w * 4,
                              (img->h >> 1) + 0x37 + gRiders[j].race.finishPlace * 25 + ((s->type != SCREEN_TYPE_STANDINGS) ? 7 : 0));
            } else {
                gDPLoadTLUT_pal256(gdl++, gGame.unk_39C->addr);
            }
            if (s->type == SCREEN_TYPE_STANDINGS) {
                for (k = 0; k < gEvent.numRiders; k++) {
                    if (gSeason->standings[k] == j) {
                        break;
                    }
                }
            }
            px = 5 + wid[j + D_800C8CE0] + rx;
            if (s->type != SCREEN_TYPE_STANDINGS || CHAMP4) {
                y = 0x3E + rowh * gRiders[j + D_800C8CE0].race.finishPlace + ry;
            } else {
                y = 0x3E + rowh * k + ry;
            }
            DRAW(px, y, img, 3);
        }
        gdl = font_render(gdl);
        switch (gGame.screenState) {
        case 7:
            gdl = shell_draw_yes_no_dialog(gdl, fb, D_800DA110);
            break;
        case 4:
            gdl = shell_draw_results_menu(gdl, fb, gGame.menuSel, sSeasonResultOptions);
            gdl = font_render(gdl);
            break;
        case 3:
            gdl = shell_draw_results_menu(gdl, fb, gGame.menuSel, (gGame.mode != 8) ? ((gCurTrack_41->f50 & 2) ? sSpecialTrackResultOptions : sExhibitionResultOptions) : sTimeTrialResultOptions);
            gdl = font_render(gdl);
            break;
        }
        gDPPipeSync(gdl++);
        break;
    }
    case SCREEN_TYPE_TITLE:
        if (gGame.fadeState != 1) {
            render_draw_race(m, fb, &gdl);
            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
            gDPSetTexturePersp(gdl++, G_TP_NONE);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gDPSetRenderMode(gdl++, 0x00404240, 0);
            CMB_PRIM();
            DRAWV(0x43, 0x14, &gGame.unk_38C[1], 6);
            gDPSetPrimColor(gdl++, 0, 0, 0xE1, 0xD7, 0x1E, 0xFF);
            DRAWV(0x43, 0x14, &gGame.unk_38C[2], 6);
            gDPSetPrimColor(gdl++, 0, 0, 0xF5, 0xF5, 0xF5, 0xFA);
            DRAWV(0xFE - gGame.unk_390[1].w * 4, 200, &gGame.unk_390[1], 6);
        }
        font_reset_queue();
        gDPPipeSync(gdl++);
        if (gGame.flags & 0x8000000) {
            font_text_create(gGame.texts[0], 0, 1, 4, 0x11);
            font_text_add(gGame.texts[0], D_800DA120);
            font_draw_text(gGame.texts[0], gScreenWidth / 2, gScreenHeight * 0xB6 / 0xF0);
        } else if (gFrameCounter & 0x20) {
            font_text_create(gGame.texts[0], 0, 1, 2, 0x11);
            font_text_add(gGame.texts[0], D_800DA144);
            font_draw_text(gGame.texts[0], gScreenWidth / 2, gScreenHeight * 0xB6 / 0xF0);
        }
        gdl = font_render(gdl);
        gDPPipeSync(gdl++);
        break;
    case SCREEN_TYPE_RIDER_SELECT: {
        s32 u7;
        s32 u8;
        s32 u9;

        render_draw_race(m, fb, &gdl);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        CMB_PRIM();
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        for (i = 0; i < gGame.numPlayers; i++) {
            s32 yo;
            s32 bx;
            s32 by;
            s32 x;
            s32 y;

            switch (gGame.riderSelectState[0]) {
            case 1:
                yo = (0x3E - gGame.riderSelectAnim[0]) * 2;
                break;
            case 2:
                yo = 0x7C;
                break;
            case 3:
                yo = gGame.riderSelectAnim[0] * 2;
                break;
            default:
                yo = 0;
                break;
            }
            yo2 = yo * 190 / 100;
            font_reset_queue();
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, VP[gNumViews - 1][i].x, VP[gNumViews - 1][i].y,
                          VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w - 1,
                          VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 1);
            if (gGame.numPlayers < 3) {
                if (!(gGame.flags & (0x400 << i))) {
                    gDPSetTexturePersp(gdl++, G_TP_NONE);
                    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                    CMB_TEX();
                    if (gGame.numPlayers == 1) {
                        gDPLoadTLUT_pal256(gdl++, gGame.unk_3DC->addr);
                        DRAW(VP[gNumViews - 1][i].x + 0x10, VP[gNumViews - 1][i].y + 5 + yo, &gGame.unk_3DC[1], 3);
                        if ((gFrameCounter & 0x20) && gGame.riderSelectState[0] == 0) {
                            gDPLoadTLUT_pal256(gdl++, gGame.unk_74C->addr);
                            DRAW(0x28, yo + 0x17, &gGame.unk_74C[1], 3);
                            DRAW(0xF5, yo + 0x17, &gGame.unk_74C[2], 3);
                        }
                    } else if (gFrameCounter & 0x20) {
                        if (gSplitVertical != 0 && i == 1) {
                            x = 0x120;
                            y = 0x3C;
                        } else {
                            x = 0x41;
                            y = i * 120 + 0x3C;
                        }
                        gDPLoadTLUT_pal256(gdl++, gGame.unk_74C->addr);
                        DRAW(x, y, &gGame.unk_74C[1], 3);
                        DRAW(x | 0x8C, y, &gGame.unk_74C[2], 3);
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    gDPSetRenderMode(gdl++, 0x00404240, 0);
                    CMB_PRIM();
                    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
                    img = &gGame.unk_390[8];
                    if (gGame.numPlayers == 2 && i == 0 && gSplitVertical == 0) {
                        s32 dx = yo2 + VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w - img->w * 8;
                        s32 dy = VP[gNumViews - 1][i].y;

                        if (gGame.flags & (0x400 << i)) {
                            dy += 600;
                        }
                        DRAWV(dx, dy, img, 6);
                    } else {
                        s32 dy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - img->h;
                        s32 dx = yo2 + VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w - img->w * 8;

                        if (gGame.flags & (0x400 << i)) {
                            dy += 600;
                        }
                        DRAWV(dx, dy, img, 6);
                    }
                    img = &gGame.unk_390[8];
                    bx = yo2 + VP[gGame.numPlayers - 1][i].x + VP[gGame.numPlayers - 1][i].w - img->w * 8 + 0x1F;
                    if (gGame.numPlayers == 2 && i == 0 && gSplitVertical == 0) {
                        by = VP[gGame.numPlayers - 1][i].y + 11;
                    } else {
                        by = VP[gGame.numPlayers - 1][i].y + VP[gGame.numPlayers - 1][i].h - img->h + 11;
                    }
                    for (j = 1; j < 17; j++) {
                        font_text_create(gGame.texts[j + i * 20], 0x40, 1, 2, 0x20);
                    }
                    for (; j < 20; j++) {
                        font_text_create(gGame.texts[j + i * 20], 0x40, 1, 2, 0x22);
                    }
                }
                font_text_create(gGame.texts[i * 20], 0x40, 5, 4, 0x21);
                font_text_add(gGame.texts[i * 20], sRiderBios[gGame.riders[i]].fullName);
                if (gGame.numPlayers == 1) {
                    if (gGame.flags & (0x400 << i)) {
                        font_draw_text(gGame.texts[i * 20], VP[0][0].x + VP[0][0].w / 2, VP[0][0].y + VP[0][0].h / 2 + 0x46);
                        font_text_create(gGame.texts[i * 20 + 1], 0x40, 5, 2, 0x21);
                        font_text_add(gGame.texts[i * 20 + 1], D_800DA150);
                        font_draw_text(gGame.texts[i * 20 + 1], VP[0][0].x + VP[0][0].w / 2, VP[0][0].y + VP[0][0].h / 2 + 0x53);
                    } else {
                        font_draw_text(gGame.texts[i * 20], VP[0][0].x + 0x7E, VP[0][0].y + 0xF + yo);
                    }
                } else if (gSplitVertical != 0) {
                    if (gGame.flags & (0x400 << i)) {
                        font_draw_text(gGame.texts[i * 20], VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 2,
                                      VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 0x24);
                    } else {
                        font_draw_text(gGame.texts[i * 20], VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 2,
                                      VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2);
                    }
                } else {
                    s32 ty = i * 10 + 0x1C;

                    if (gGame.flags & (0x400 << i)) {
                        font_draw_text(gGame.texts[i * 20], VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 2,
                                      VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2 + ty);
                    } else {
                        font_draw_text(gGame.texts[i * 20], VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 4 - 10,
                                      VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2 + ty);
                    }
                }
                if (!(gGame.flags & (0x400 << i))) {
                    for (j = 1; j < 5; j++) {
                        font_text_add(gGame.texts[i * 20 + j], sRiderStatLabels[j - 1]);
                        font_draw_text(gGame.texts[i * 20 + j], bx - (j - 1) * 6, by + (j - 1) * 20);
                    }
                    if (gGame.numPlayers == 1) {
                        shell_draw_rider_bio(i, yo);
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    CMB_PRIM();
                    if (gGame.numPlayers == 1) {
                        j = 0;
                        DRAWV(VP[0][0].x, VP[0][0].y + yo, &gGame.unk_390[1], 6);
                        img = &gGame.unk_390[2];
                        DRAWV(VP[0][0].x, VP[0][0].y - img->h + yo, img, 6);
                        for (j = 0; j < 16; j++) {
                            if (str_icmp(sRiderBios[gGame.riders[i]].text[2], gRiderAnims[j].name) == 0) {
                                break;
                            }
                        }
                        if (gRiderAnims[j].inputGlyphs != NULL) {
                            gdl = event_draw_glyph_string(gdl, fb, gRiderAnims[j].inputGlyphs, 0x8C, yo + VP[0][0].y - 0x48);
                        }
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    CMB_PRIM();
                    img = &gGame.unk_390[6];
                    for (j = 0; j < 4; j++) {
                        s32 ox;
                        s32 oy;

                        ox = j * 6 - 0x55;
                        n = (sRiderBios[gGame.riders[i]].stats[j] + 1) * 2;
                        switch (n >> 1) {
                        case 5:
                            gDPSetPrimColor(gdl++, 0, 0, 0x14, 0xC8, 0x14, 0xFF);
                            break;
                        case 4:
                            gDPSetPrimColor(gdl++, 0, 0, 0x96, 0xB4, 0x14, 0xFF);
                            break;
                        case 3:
                            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0x14, 0xFF);
                            break;
                        case 2:
                            gDPSetPrimColor(gdl++, 0, 0, 0xBE, 0x91, 0x14, 0xFF);
                            break;
                        case 1:
                            gDPSetPrimColor(gdl++, 0, 0, 0xC8, 0x14, 0x14, 0xFF);
                            break;
                        }
                        c = 0;
                        oy = j * 20 - 7;
                        for (; c < 10; c++) {
                            rowh = n;
                            ry = n;
                            rx = n;
                            t = n;
                            if (c == n) {
                                gDPSetPrimColor(gdl++, 0, 0, 0x28, 0x28, 0x28, 0xFF);
                            }
                            DRAW(bx - ox + c * 9, by + oy, img, 6);
                        }
                    }
                }
            } else {
                s32 u10;
                s32 cx;
                s32 cy;
                s32 u11;
                s32 msk;
                s32 nn;

                gDPSetTextureLUT(gdl++, G_TT_NONE);
                gDPSetRenderMode(gdl++, 0x00404240, 0);
                CMB_PRIM();
                if (!(gGame.flags & (0x400 << i))) {
                    img = &gGame.unk_390[7];
                    for (j = 0; j < 4; j++) {
                        nn = (sRiderBios[gGame.riders[i]].stats[j] + 1) * 2;
                        nn &= ~1;
                        switch (nn >> 1) {
                        case 5:
                            gDPSetPrimColor(gdl++, 0, 0, 0x14, 0xC8, 0x14, 0xFF);
                            break;
                        case 4:
                            gDPSetPrimColor(gdl++, 0, 0, 0x96, 0xB4, 0x14, 0xFF);
                            break;
                        case 3:
                            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0x14, 0xFF);
                            break;
                        case 2:
                            gDPSetPrimColor(gdl++, 0, 0, 0xBE, 0x91, 0x14, 0xFF);
                            break;
                        case 1:
                            gDPSetPrimColor(gdl++, 0, 0, 0xC8, 0x14, 0x14, 0xFF);
                            break;
                        }
                        switch (j) {
                        case 0:
                            cx = VP[gNumViews - 1][i].x + 0x12;
                            cy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 0x1A;
                            break;
                        case 1:
                            cx = VP[gNumViews - 1][i].x + 0x74;
                            cy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 0x1A;
                            break;
                        case 2:
                            cx = VP[gNumViews - 1][i].x + 0xF;
                            cy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 0xD;
                            break;
                        case 3:
                            cx = VP[gNumViews - 1][i].x + 0x71;
                            cy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 0xD;
                            break;
                        }
                        for (c = 0; c < 10; c++) {
                            if (c == nn) {
                                gDPSetPrimColor(gdl++, 0, 0, 0x28, 0x28, 0x28, 0xFF);
                            }
                            DRAWV(cx + c * 9, cy, img, 6);
                        }
                    }
                    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
                    img = &gGame.unk_390[10];
                    DRAWV(VP[gNumViews - 1][i].x, VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - img->h, img, 6);
                }
                font_text_create(gGame.texts[i * 4], 0x40, 5, 4, 0x21);
                font_text_add(gGame.texts[i * 4], sRiderBios[gGame.riders[i]].fullName);
                {
                    s32 yy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2;
                    s32 xx = VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 2;
                    s32 ty2;

                    msk = 0x400 << i;
                    ty2 = yy + 0xA;
                    if (gGame.flags & msk) {
                        ty2 = yy + 0x23;
                    }
                    font_draw_text(gGame.texts[i * 4], xx, ty2);
                }
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                CMB_TEX();
                if (!(gGame.flags & msk) && (gFrameCounter & 0x20)) {
                    gDPLoadTLUT_pal256(gdl++, gGame.unk_74C->addr);
                    DRAWK(VP[gNumViews - 1][i].x + 5, VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2 + 3, &gGame.unk_74C[1], 3, u10);
                    DRAWK(VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w - 0x14,
                         VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2 + 3, &gGame.unk_74C[2], 3, u11);
                }
            }
            if (gGame.numPlayers < 3) {
                gDPSetScissor(gdl++, G_SC_NON_INTERLACE, VP[gNumViews - 1][0].x, VP[gNumViews - 1][0].y,
                              VP[gNumViews - 1][gNumViews - 1].x + VP[gNumViews - 1][gNumViews - 1].w - 1,
                              VP[gNumViews - 1][gNumViews - 1].y + VP[gNumViews - 1][gNumViews - 1].h - 1);
            }
            gdl = font_render(gdl);
            gDPPipeSync(gdl++);
        }
        gDPSetScissor(gdl++, G_SC_NON_INTERLACE, VP[0][0].x, VP[0][0].y, VP[0][0].x + VP[0][0].w - 1,
                      VP[0][0].y + VP[0][0].h - 1);
        gDPPipeSync(gdl++);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        CMB_PRIM();
        if (gGame.numPlayers == 1) {
            if (gGame.mode != 8) {
                font_text_create(gGame.texts[0], 0x40, 2, 2, 0x20);
                font_text_add(gGame.texts[0], D_800DA170);
                font_draw_text(gGame.texts[0], 0x16C, 0x25);
            }
            font_text_create(gGame.texts[1], 0x40, 2, 2, 0x20);
            font_text_add(gGame.texts[1], D_800DA184);
            font_draw_text(gGame.texts[1], 0x16C, 0x19);
            gDPSetTexturePersp(gdl++, G_TP_NONE);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            CMB_TEX();
            gDPSetRenderMode(gdl++, 0x00404240, 0);
            gDPLoadTLUT_pal256(gdl++, gGame.unk_3C8->addr);
            if (gGame.mode != 8) {
                DRAWK(0x140, 0x21, &gGame.unk_3C8[1], 3, u9);
                DRAW(0x154, 0x21, &gGame.unk_3C8[3], 3);
            }
            DRAWK(0x140, 0x13, &gGame.unk_3C8[4], 3, u7);
            DRAWK(0x154, 0x13, &gGame.unk_3C8[2], 3, u8);
        }
        gdl = font_render(gdl);
        break;
    }
    }
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    if (gGame.fadeState != 3) {
        gdl = screen_draw_wipe(gdl, m, gGfxFrameIndex);
    }
    {
        Gfx *_g = gdl++;

        _g->words.w0 = 0xE7000000;
        _g->words.w1 = ((u16)u1 >> 16) + ((u16)u2 >> 16) + ((u16)u3 >> 16);
    }
    return gdl;
}

#define IMG(s) ((s)->type == SCREEN_TYPE_ROUND_SELECT ? gGame.screenPics[*(s)->cursor] : (s)->bg)

typedef struct {
    s16 x, y, w, h;
} Rect_3;

extern Rect_3 sTitleSceneRect;

void shell_draw(Mtx *m, s32 fb, Gfx **pgdl)
{
    u8 unk28[0x70];
    Mtx persp;
    u16 pn;
    Gfx *gdl;
    Screen *s;
    ScreenItem *item;
    u16 alpha;

    gdl = *pgdl;
    if (gGame.fadeState == 0) {
        s = sPrevScreen;
    } else {
        s = sCurScreen;
    }
    if (s != NULL) {
        if (gGame.fadeState == 0) {
            item = s->items + sPrevScreenCursor;
        } else {
            item = s->items;
            if (s->cursor != NULL) {
                item += *s->cursor;
            }
        }
    }
    alpha = 0xFF;
    if (gGame.fadeState == 1) goto c1;
    if (gGame.fadeState == 0) goto c0;
    if (gGame.fadeState == 2) goto c2;
    goto done;
c1:
    alpha = 0;
    goto done;
c0:
    alpha = gGame.fadeLevel;
    goto done;
c2:
    alpha = 0xFF - gGame.fadeLevel;
done:
    mtx_from_angles_pos_scale(&m[16], 0, 0, 0, 0, 0, 0, 0x100);
    guOrtho(&m[12], -gScreenWidth / 2, gScreenWidth / 2, -gScreenHeight / 2, gScreenHeight / 2, 50.0f, 4192.0f, 1.0f);
    gSPSegment(gdl++, 0, 0);
    gSPDisplayList(gdl++, D_800C2A38);
    gSPDisplayList(gdl++, D_800C2A00);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
    gSPViewport(gdl++, &sShellViewport);
    gDPPipeSync(gdl++);
    guPerspective(&persp, &pn, 33.0f, 1.9512196f, 25.0f, 4192.0f, 1.0f);
    mtx_mul(&gViewAxisMtx, &persp, m);
    gSPPerspNormalize(gdl++, pn);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gZBuffer);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gDPSetFillColor(gdl++, 0xFFFCFFFC);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gFramebuffers[fb]);
    if (gGame.fadeState == 1 || s == NULL || s->bgResId == NULL) {
        gDPSetFillColor(gdl++, 0);
        gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    }
    gDPPipeSync(gdl++);
    gDPSetDepthImage(gdl++, gZBuffer);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gSPSetGeometryMode(gdl++, G_CULL_BACK);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetRenderMode(gdl++, G_RM_OPA_SURF, G_RM_NOOP2);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
    gDPSetTextureFilter(gdl++, G_TF_BILERP);
    gSPMatrix(gdl++, &m[16], G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, &m[12], G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClipRatio(gdl++, FRUSTRATIO_1);
    font_reset_queue();
    gDPPipeSync(gdl++);
    if (gGame.fadeState == 1) {
        if (gGame.fadeTimer > 0 || gGame.fadeTimer == -1) {
            gGame.fadeTimer--;
        }
    } else {
        if (s->type == SCREEN_TYPE_ROUND_SELECT || s->bgResId != NULL) {
            gDPLoadTLUT_pal256(gdl++, s->type == SCREEN_TYPE_ROUND_SELECT ? gGame.screenPics[*s->cursor]->pal : s->bg->pal);
            gdl = render_draw_image(0, 0, IMG(s)->img, gdl, fb, 1, 1, 3, 0);
        }
        if (s->type == SCREEN_TYPE_TITLE) {
            Rect_3 *r = &sTitleSceneRect;
            sTitleSceneViewport.vp.vscale[2] = 0x1FF;
            sTitleSceneViewport.vp.vscale[3] = 0;
            sTitleSceneViewport.vp.vtrans[2] = 0x1FF;
            sTitleSceneViewport.vp.vtrans[3] = 0;
            sTitleSceneViewport.vp.vscale[0] = r->w * 2;
            sTitleSceneViewport.vp.vscale[1] = r->h * 2;
            sTitleSceneViewport.vp.vtrans[0] = (r->x + r->w / 2) * 4;
            sTitleSceneViewport.vp.vtrans[1] = (r->y + r->h / 2) * 4;
            gSPViewport(gdl++, &sTitleSceneViewport);
            gdl = shell_draw_screen(m, fb, gdl, item, 0);
        } else if (s != NULL) {
            gdl = shell_draw_screen(m, fb, gdl, item, 0);
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
            D_800CA80F_2[0] = alpha;
        }
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}

void shell_load_gui_res(void) {
    res_open_set("guires", &gGuiRes);
}

void shell_free_gui_res(void)
{
    res_close_set(gGuiRes);
    gGuiRes = 0;
}

void shell_cb_desert(s32 p) {
    if (p != 0) {
        gCurTrack = "Desert";
        gGame.mode = 5;
        D_800CA924_5.b = 0;
        D_800CA924_5.a = 1;
        D_800CA924_5.c = 0;
        gGame.flags &= 0xFFFFFF7F;
    }
}

void shell_cb_select_track(s32 a) {
    if (a != 0) {
        gCurTrack = gTracks_2 + gGame.trackSel * 0xA0;
        gGame.flags |= 0x80;
    }
}

void shell_cb_time_trials(s32 a0)
{
    Game *base;

    if (a0 != 0) {
        base = &gGame;
        base->mode = 8;
        base->numPlayers = 1;
        base->flags = base->flags | 0x80;
    }
}

typedef struct {
    u8 pad0[0x2C1];
    u8 track;
} Opts_2;

extern Opts_2 gSaveData_14;

void shell_cb_tutorial(s32 arg)
{
    s32 i;

    if (arg != 0) {
        gCurTrack = sTrackTutorial;
        gGame.numPlayers = 1;
        gGame.mode = 9;
        gEvent.tutorialBuf = 0;
        gGame.riders[0] = 4;
        gGame.flags &= ~0x80;
        gSeason->cup = sTutorialRound;
        gSeason->round = 0;
        for (i = 0; i < 0x11; i++) {
            if (gTutorialLessons[i].level == gSaveData_14.track) {
                gSeason->round = i;
                return;
            }
        }
    }
}

void shell_cb_exhibition(s32 x)
{
    if (x != 0) {
        gGame.mode = 0;
    }
}

void shell_cb_excite3d(s32 p) {
    s32 i;

    if (p != 0) {
        i = 0;
        do {
            if (i == 0) {
                gGame.riderSkins[0] = (u8)gGame.riderSkinSel;
            } else {
                gGame.riderSkins[i] = i;
            }
            i++;
        } while (i < 6);
        gGame.mode = 6;
        gCurTrack = "Excite 3D";
        gGame.numPlayers = 1;
        gGame.difficulty = 2;
        gGame.flags &= 0xFFFFFF7F;
    }
}

void shell_cb_hill_climb(s32 p)
{
    s32 i;
    if (p != 0) {
        for (i = 0; i < 6; i++) {
            if (gGame.numPlayers < 2) {
                if (i == 0) {
                    gGame.riderSkins[0] = (u8)gGame.riderSkinSel;
                } else {
                    gGame.riderSkins[i] = i;
                }
            } else {
                gGame.riderSkins[i] = i;
            }
        }
        gGame.numCpuRiders = 0;
        gSplitVertical = 1;
        gGame.mode = 7;
        gCurTrack = D_800CC070;
        gGame.flags &= ~0x80;
    }
}

void shell_cb_custom_tracks(s32 x)
{
    if (x != 0) {
        gGame.flags = (gGame.flags & ~0x80) | 0x20;
    }
}

void func_800880DC(s32 a0) {
    s32 i;
    Game *b;
    Game *w;
    if (a0 == 0) {
        return;
    }
    w = &gGame;
    w->numPlayers = 1;
    b = w;
    gCurTrack = D_800CC1B0;
    w->mode = 4;
    w->flags = w->flags & 0xFFFFFF7F;
    for (i = 0; i < 4; i++) {
        if (i == 0) {
            b->playerPad[0] = 0;
        } else {
            b->playerPad[i] = -1;
        }
        if (i == 0) {
            b->padPlayer[0] = 0;
        } else {
            b->padPlayer[i] = -1;
        }
    }
    D_800CA924[1] = 0;
    D_800CA924[0] = 1;
    D_800CA924[2] = 0;
}

void shell_cb_original_excitebike(s32 chosen)
{
    gGame.mode = 12;
    gGame.flags = (gGame.flags & ~0x80) | 0x40;
}

void shell_cb_stunt_course(s32 param_1)
{
    if (param_1 != 0) {
        Game *b = &gGame;
        b->mode = 10;
        gCurTrack = "Stunt Course";
        gGame.flags = gGame.flags & 0xffffff7f;
    }
}

s32 shell_pak_track_invalid(u32 *p, s32 n)
{
    s32 r;
    if (n != 0x400) {
        return 1;
    }
    r = calc_checksum24((u8 *)(p + 1), 0x3FC);
    return r != *p;
}

void shell_backup_race_results(void)
{
    s32 i = 0;

    gGame.resultsNumRiders = gEvent.numRiders;
    do {
        gGame.resultsOrder[i] = gEvent.finishOrder[i];
        gGame.resultsTimes[i] = gRiders[i].race.finishTime;
        gGame.resultsCelebration[i] = (u8)gRiders[i].race.finishPlace;
        if (gSeason != 0) {
            gGame.resultsSeasonOrder[i] = gSeason->standings[i];
        }
        i++;
    } while (i < 6);
}

void shell_restore_race_results(void)
{
    s32 i;

    gEvent.numRiders = gGame.resultsNumRiders;
    for (i = 0; i < 6; i++) {
        gEvent.finishOrder[i] = gGame.resultsOrder[i];
        gRiders[i].race.finishTime = gGame.resultsTimes[i];
        gRiders[i].race.finishPlace = gGame.resultsCelebration[i];
        if (gSeason != 0) {
            gSeason->standings[i] = gGame.resultsSeasonOrder[i];
        }
    }
}

s32 shell_is_round_screen_unlocked(Screen *screen)
{
    s32 mode;
    s32 t;

    if (gGame.mode == 8 || gGame.mode == 0) {
        mode = D_800F1BC0;
    } else {
        mode = gGame.difficulty;
    }
    if (mode == 2) {
        goto two;
    }
    if (mode < 3) {
        if (mode == 1) {
            goto one;
        }
        goto ret0;
    }
    if (mode == 3) {
        goto ret1;
    }
    goto ret0;
one:
    t = screen->firstItem < 10;
tail:
    if (t == 0) {
        goto ret0;
    }
ret1:
    return 1;
two:
    t = screen->firstItem < 0xF;
    goto tail;
ret0:
    return 0;
}

void shell_main_menu_or_back_input(Screen *screen, ScreenItem *item, u32 *buttons, Pad16 *pads)
{
    s32 n;
    Screen *back;

    if ((*buttons & 0x50) != 0) {
        shell_goto_screen(&sScreens[SCREEN_MAIN_MENU], 0, 1);
        n = 1;
    } else {
        if ((*buttons & 0x20) == 0) {
            return;
        }
        back = screen->back;
        if (back == NULL) {
            return;
        }
        shell_goto_screen(back, 0, 0);
        n = 2;
    }
    snd_play_menu_sound(n);
}

typedef struct {
    u8 pad[0x2BF];
    u8 bits;
    u8 mode;
} Save_6;

extern Save_6 gSaveData_15;

s32 shell_is_round_unlocked_tt(s32 p)
{
    s32 v;
    if (p == 0) goto one;
    switch (gSaveData_15.mode) {
    case 1:
        if (p >= 2) goto zero;
        v = (gSaveData_15.bits >> (p - 1)) & 1;
        break;
    one:
        return 1;
    case 2:
        if (p < 2) goto one;
        if (p != 2) goto zero;
        v = (gSaveData_15.bits >> (p - 1)) & 1;
        break;
    case 3:
        if (p < 3) goto one;
        if (p != 3) goto zero;
        v = (gSaveData_15.bits >> (p - 1)) & 1;
        break;
    default:
        goto zero;
    }
    if (v) goto one;
zero:
    return 0;
}

s32 func_800884E0(void)
{
    return *(u32 *)0xA4001000 == 0x17D7;
}
