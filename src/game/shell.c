#include "game.h"

typedef struct {
    u8 unk0[0xD];
    u8 id;
    u8 unkE[0x28 - 0xE];
} Track_2;

typedef struct {
    u8 pad[0x22];
    u16 f22;
    u8 pad24[0x50 - 0x24];
    u8 f50[6];
} Cfg_3;

typedef struct {
    u8 pad[0x669];
    u8 f669;
    u8 pad66A[0x68C - 0x66A];
    s32 f68C;
    u8 pad690[0x7E0 - 0x690];
} Ent_3;

extern Track_2 gTutorialLessons_2[];
extern Cfg_3 gEvent_6;
extern Ent_3 gRiders_3[];

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
        ret = D_800F01F8;
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
        ret = D_800F01F8;
        ret = ret ^ 1;
    }
    return (ret == 0) << 1;
}

typedef struct {
    u8 pad[0x13C];
    u16 *p13C;
} S70_3;

extern S70_3 gGame_53;

void shell_blend_palette_to_gray(s32 alpha, u16 *src)
{
    s32 i;
    s32 r, g, b;
    u32 p;

    gGame_53.p13C[0] = 0;
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
        gGame_53.p13C[i] = ((r >> 3) << 1) | ((g >> 3) << 6) | ((b >> 3) << 11) | 1;
    }
}

typedef struct {
    s32 w0;
    s32 w4;
    s32 w8;
    s32 wC;
    s32 w10;
    s32 w14;
    s32 w18;
    s32 w1C;
    s32 w20;
    s32 w24;
    s32 w28;
    s32 w2C;
    s32 w30;
    u8 pad34[8];
    s32 w3C;
    s32 w40;
    s32 w44;
    s16 h48;
    s16 h4A;
    s32 w4C;
    u8 b50;
    u8 b51;
    u8 b52;
    u8 b53;
    s32 w54;
    u8 pad58[0x13C - 0x58];
    s32 w13C;
    u8 pad140[0x2A4 - 0x140];
    u8 b2A4;
    u8 b2A5;
    u8 pad2A6[0x3C0 - 0x2A6];
    s32 w3C0;
    u8 pad3C4[0x3FC - 0x3C4];
    void *p3FC;
    s32 w400;
    s32 w404;
    u8 pad408[0x43C - 0x408];
    u8 b43C[6];
    u8 pad442[0x448 - 0x442];
    s16 h448;
    s16 h44A;
    s32 w44C;
    u8 pad450[0x490 - 0x450];
    V7 rec[25];
    u8 pad74C[0x75C - 0x74C];
    s32 w75C;
    u8 b760[4];
    u8 pad764[0x7AA - 0x764];
    u8 b7AA[6];
} Game_27;

typedef struct {
    u8 pad0[0x34];
    V7 r34;
    s32 w50;
    u8 pad54[0xA0 - 0x54];
} Course_2;

typedef struct {
    u8 pad0[4];
    void *p4;
} Link;

typedef struct {
    u8 pad0[0xC];
    Link *pC;
    u8 pad10[0x2C - 0x10];
} Menu;

typedef struct {
    Ent24 *p0;
    u8 pad4[0x60];
    s16 h64;
} Sel_3;

typedef struct { u8 pad0[0x174]; s32 w174; } Ctl_18;

extern Game_27 gGame_54;
extern Course_2 gTracks_14[];
extern Ctl_18 *gEditor_3;
extern Menu sScreens[];
extern Sel_3 *gSeason_15;

void shell_init(void)
{
    s32 flag;
    s32 mode;
    s32 i;
    V7 *dst;
    s32 n;
    Course_2 *c;
    void *m;
    u8 *p;
    u8 *q;

    flag = 0;
    gPodiumMode = 0;
    gReplayPlayback = 0;
    if (gRaceExitCode == 2) {
        flag = flag < (u32)(gGame_54.w0 & 8);
    }
    gSplitVertical = 0;
    gGame_54.w0 &= ~8;
    if (gRaceExitCode == 0) {
        mem_fill((u8 *)&gGame_54, 0, 0x7B0);
        n = 0;
        for (i = 0; i < 27; i++) {
            s32 f = gTracks_14[i].w50;
            V7 *src = &gTracks_14[i].r34;
            if (f & 0x200) {
                dst = &gGame_54.rec[n];
                *dst = *src;
                n++;
            }
        }
        gGame_54.w75C = 2;
        gGame_54.w1C = 4;
        gGame_54.w4 = 0;
        gGame_54.w10 = 0;
        gGame_54.wC = 0;
        gGame_54.w20 = 0;
        gGame_54.w24 = 1;
        gGame_54.w2C = 3;
        gGame_54.w30 = 5;
        gGame_54.w28 = 2;
        gGame_54.w3C = 1;
        gGame_54.w40 = -1;
        gGame_54.h448 = 0;
        gGame_54.h48 = 0;
        gGame_54.w8 = 0;
        gGame_54.w18 = 0;
        gGame_54.b50 = 0;
        gGame_54.b51 = 0;
        gGame_54.b52 = 0;
        gGame_54.b53 = 0;
        gGame_54.b2A4 = 0;
        gGame_54.b2A5 = 1;
        gGame_54.w3C0 = 0;
        gGame_54.b760[0] = 2;
        gGame_54.b760[1] = 2;
        gGame_54.b760[2] = 2;
        gGame_54.b760[3] = 2;
        i = 5;
        p = (u8 *)&gGame_54 + i;
        do {
            p[0x7AA] = i;
            p--;
            i--;
        } while (i >= 0);
        i = 5;
        q = (u8 *)&gGame_54 + i;
        do {
            q[0x43C] = i;
            q--;
            i--;
        } while (i >= 0);
    } else if (gGame_54.w4C == 8) {
        gGame_54.w3C = 1;
    }
    if (gRaceExitCode == 7) {
        gGame_54.w0 &= 0xFF0FFFFF;
    }
    gGame_54.w54 = 3;
    res_set_load_buffer((s32)gZBuffer_3);
    i = res_open_set("resource", &D_800E7728);
    res_set_load_buffer(0);
    if (i != 0) {
        fatal_halt(1);
    }
    font_load_menu_set((u32 *)D_800E7728);
    res_close_set((s32 *)D_800E7728);
    gGame_54.h44A = 0;
    for (i = 0; i < 27; i++) {
        if ((gTracks_14[i].w50 & 0x200) && !(gTracks_14[i].w50 & 2)) {
            gZBuffer_3[gGame_54.h44A++] = i;
        }
    }
    gGame_54.w44C = mem_alloc_dbg(gGame_54.h44A * 2, 0, "shell.c", 0x75C);
    memcpy(gGame_54.w44C, gZBuffer_3, gGame_54.h44A * 2);
    gGame_54.w44 = 5;
    D_800CA924_4 = 0;
    gGame_54.w0 = (gGame_54.w0 & 0x20F88000) | 1;
    res_open_set("guires", &gGuiRes);
    res_register_type("ARN", D_800248EC);
    prim_init();
    sCurScreen = 0;
    sPrevScreen = 0;
    gGame_54.w404 = res_load(gGuiRes, D_800D1438);
    sShellViewport[2] = 0x1FF;
    sShellViewport[6] = 0x1FF;
    sShellViewport[0] = gScreenWidth * 2;
    sShellViewport[1] = gScreenHeight * 2;
    sShellViewport[4] = gScreenWidth * 2;
    sShellViewport[5] = gScreenHeight * 2;
    gGame_54.w13C = mem_alloc_dbg(0x200, 0, "shell.c", 0x777);
    shell_blend_palette_to_gray(0xFF, 0);
    if (gRaceExitCode == 4) {
        gCurTrack_8->w50 &= ~0x180;
        if (gActiveReplay != 0) {
            gActiveReplay = 0;
            gGame_54.w0 &= 0xFFF7FFFF;
        }
    }
    if (gEditor_3 != NULL) {
        if (gEditor_3->w174 & 0x2000) {
            D_800F01F8 = 3;
            shell_goto_screen_3(&sScreens[15], 0, 1);
        } else {
            D_800F01F8 = 4;
            shell_goto_screen_3(&sScreens[15], 0, 1);
        }
    } else {
        mode = gRaceExitCode;
        if (mode == 0) {
            shell_goto_screen_3(&sScreens[19], 0, 1);
        } else if ((gGame_54.w0 & 0x8000) || gCreditsMode != 0 || mode == 3) {
            if (gActiveReplay != 0) {
                if (!(gGame_54.w0 & 0x8000)) {
                    if (!(gGame_54.w0 & 0x80000)) {
                        gActiveReplay = 0;
                    }
                }
            }
            if (gRaceExitCode == 3 && gGameMode == 1) {
                shell_restore_race_results();
                if (gSeason_15->h64 > 0) {
                    gSeason_15->h64--;
                }
            }
            if (gGame_54.w0 & 0x8000) {
                shell_restore_race_results();
            }
            if (gCreditsMode != 0 && gGame_54.p3FC != &sScreens[5]) {
                gCreditsMode = 0;
                shell_goto_screen_3(&sScreens[8], 0, 1);
            } else {
                shell_goto_screen_3(D_800F016C, 0, 1);
            }
            gGame_54.w0 &= 0xFEFF7FFF;
        } else if (flag) {
            shell_goto_screen_3(&sScreens[10], 1, 1);
        } else {
            if (gEventAllFinished != 0 && mode != 0 && mode != 6 && gGame_54.w4C == 1) {
                if (gSeason_15 != NULL && gSeason_15->p0 - D_800CA9C8 == 4) {
                    m = &sScreens[2];
                } else {
                    m = &sScreens[0];
                }
            } else if ((gEventAllFinished != 0 && gRaceExitCode != 0 && gRaceExitCode != 6 && gGameMode == 0)
                       || (gEventAllFinished != 0 && gRaceExitCode != 0 && gRaceExitCode != 6 && gGameMode == 8)) {
                m = &sScreens[1];
            } else {
                if (gActiveReplay != 0) {
                    gActiveReplay = 0;
                }
                switch (gRaceExitCode) {
                case 0:
                    m = &sScreens[14];
                    break;
                case 1:
                    m = &sScreens[1];
                    break;
                default:
                    m = &sScreens[8];
                    if (gUserTrackLayout != 0) {
                        mem_free(gUserTrackLayout);
                        gUserTrackLayout = 0;
                    }
                    break;
                }
            }
            if (gRaceExitCode != 0 && gCurTrack_8 != NULL && (gCurTrack_8->w50 & 0x180)) {
                sScreens[18].pC->p4 = m;
                shell_goto_screen_3(&sScreens[18], 1, 1);
            } else {
                shell_goto_screen_3(m, 1, 1);
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
    u8 *base;

    if (param_1 != 0) {
        uVar1 = 0;
        do {
            uVar2 = uVar1 + 1;
            gGame_2[0x43C + uVar1] = (u8)(((s32)(uVar1 & 3) >> ((*(s32 *)(gGame_2 + 0x3C) - 1) >> 1)) << 1);
            uVar1 = uVar2;
        } while (uVar2 < 6);
        uVar1 = 0;
        iVar3 = uVar1;
        base = gGame_2;
        *(s32 *)(base + 0x4C) = 2;
        gCurTrack = sTrackSoccer;
        gSplitVertical = 0;
        *(s32 *)(base + 0x1C) = 4;
        *(s32 *)(base + 0x20) = 4;
        *(s32 *)(base + 0x24) = 4;
        *(s32 *)(base + 0x28) = 4;
        *(s32 *)gGame_2 = *(s32 *)gGame_2 & 0xffffff7f;
        for (; uVar1 < 4; uVar1++) {
            if (((s32)gContMask >> uVar1 & 1) != 0) {
                gGame_2[0x2C8 + iVar3] = (u8)uVar1;
                iVar3 = iVar3 + 1;
                if (iVar3 == *(s32 *)(gGame_2 + 0x3C)) {
                    return;
                }
            }
        }
    }
}

typedef struct {
    u32 flags;
    u8 unk4[0xC - 0x4];
    s32 unkC;
    u8 unk10[0x1C - 0x10];
    s32 unk1C;
    u8 unk20[0x3C - 0x20];
    volatile s32 unk3C;
    u8 unk40[0x4C - 0x40];
    s32 mode;
    u8 unk50[0x410 - 0x50];
    s32 unk410;
} GameState;

typedef struct {
    Ent24 *cup;
    u8 unk4[0x64 - 0x4];
    s16 track;
    u8 unk66[0x74 - 0x66];
} Season;

typedef struct {
    u8 unk0[0x2C1];
    u8 trackId;
} Unk1900;

extern GameState gGame_55;
extern Season sSeasonState;
extern Season *gSeason_16;
extern Unk1900 gSaveData_8;

void shell_cb_season(s32 arg0) {
    s32 i;

    if (arg0 != 0) {
        gGame_55.mode = 1;
        gSeason_16 = &sSeasonState;
        gGame_55.flags |= 0x80;
        mem_fill(&sSeasonState, 0, sizeof(Season));
        gSeason_16->cup = &D_800CA9C8[gGame_55.unkC];
        if (gGame_55.unk410 == 0) {
            gCurTrack = sTrackTutorial;
            gGame_55.mode = 9;
            gGame_55.unk3C = 1;
            D_800F0624_2 = 0;
            gGame_55.unk1C = 4;
            gGame_55.flags &= ~0x80;
            gSeason_16->cup = &D_800CA9C8[6];
            gSeason_16->track = 0;
            for (i = 0; i < 17; i++) {
                if (gTutorialLessons_2[i].id == gSaveData_8.trackId) {
                    gSeason_16->track = i;
                    return;
                }
            }
        }
    }
}

#define PRESSED(m) ((gPadInput & (m)) != 0 && (gPadInputPrev & (m)) == 0)

typedef struct {
    u8 pad0[0x54];
    s32 f54;
    u8 pad58[0x750 - 0x58];
    s32 timer;
} Glob_4;

struct Item_8 {
    u8 pad0[0x16];
    s16 type;
};

extern Glob_4 gGame_56;

void shell_splash_input(Item_8 *a, Pal *b, u32 *arg2, Pad16 *arg3)
{
    s32 flag;
    s32 ok;
    s16 type;

    flag = 0;
    type = a->type;
    if (type == 0x15 || type == 0x17) {
        ok = gGame_56.timer < 0x78;
    } else if (type == 0x18) {
        ok = gGame_56.timer < 0x474;
    } else if (type == 0x19) {
        ok = gGame_56.timer < 0x21C;
    } else {
        ok = gGame_56.timer < 0xF0;
    }
    if (ok) {
        if (PRESSED(0x10) | PRESSED(0x40)) {
            flag = 1;
        }
    }
    if (--gGame_56.timer == 0 || flag) {
        if (flag && gGame_56.f54 == 3) {
            snd_play_menu_sound(1);
        }
        shell_goto_screen_2(b->f4, 0, 1);
    }
}

typedef struct {
    u8 pad0[0x50];
    u8 dc[0x39C];
    s32 cnt;
    u8 pad1[0x86];
    u16 pos;
} Blk_18;

extern Blk_18 gGame_57;

void shell_controller_settings_input(s32 param_1, s32 param_2, u32 *param_3, s8 *param_4)
{
    s32 iv4;
    s32 uv5;
    s32 uVar6;
    s32 cv;
    u32 flags;
    u16 u2;
    u16 uP;

    uVar6 = (s16)gGame_57.pos;
    gGame_57.cnt = gGame_57.cnt + 1;
    if (gGame_57.cnt == 0x78) {
        gGame_57.cnt = 0;
    }
    flags = *param_3;
    if ((flags & 0x20) != 0) {
        shell_goto_screen_2(*(s32 *)(param_1 + 0x28), 0, 0);
        iv4 = 2;
        goto CALLPT;
    }
    if ((flags & 4) != 0) {
        do {
            uP = gGame_57.pos;
            u2 = uP - 1;
            gGame_57.pos = u2;
            if ((s32)((u32)u2 << 16) < 0) {
                gGame_57.pos = uP + 3;
            }
        } while (((gContMask >> (s16)gGame_57.pos) & 1) == 0);
        goto LAB_800743d8;
    }
    if ((flags & 8) != 0) {
        do {
            uP = gGame_57.pos;
            u2 = uP + 1;
            gGame_57.pos = u2;
            if ((s16)u2 > 3) {
                gGame_57.pos = uP - 3;
            }
        } while (((gContMask >> (s16)gGame_57.pos) & 1) == 0);
        goto LAB_800743d8;
    }
    if ((flags & 0x10) == 0) {
        goto LAB_800743d8;
    }
    cv = gGame_57.dc[uVar6];
    gGame_57.dc[uVar6] = cv + 1;
    if ((s8)(cv + 1) > 3) {
        gGame_57.dc[(s16)gGame_57.pos] = gGame_57.dc[(s16)gGame_57.pos] - 4;
    }
    iv4 = 1;
CALLPT:
    snd_play_menu_sound(iv4);
LAB_800743d8:
    {
        u8 bits;
        bits = gContMask;
        if (((bits >> (s16)gGame_57.pos) & 1) == 0) {
            do {
                uv5 = (gGame_57.pos + 1) & 3;
                gGame_57.pos = uv5;
            } while (uv5 != uVar6 && ((bits >> uv5) & 1) == 0);
        }
    }
    if (uVar6 != (s16)gGame_57.pos) {
        snd_play_menu_sound(0);
    }
    return;
}

typedef struct {
    u8 p000[0x17C];
    s32 f17C;
    u8 p180[0x2E8 - 0x180];
    s32 f2E8;
    u8 p2EC[0x3EC - 0x2EC];
    s32 f3EC;
    u8 p3F0[0x44A - 0x3F0];
    s16 f44A;
    u8 p44C[0x450 - 0x44C];
    s32 f450;
    u8 p454[0x750 - 0x454];
    s32 f750;
} GameState_2;

extern GameState_2 gGame_58;

void shell_records_input(s32 param_1, s32 param_2, u32 *param_3, s32 param_4)
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

    c1 = gGame_58.f17C + 1;
    gGame_58.f17C = c1;
    if (c1 == 0x3c) {
        gGame_58.f17C = 0;
    }
    c2 = gGame_58.f3EC + 1;
    gGame_58.f3EC = c2;
    if (c2 == 0x40) {
        gGame_58.f3EC = 0;
    }
    if (gGame_58.f750 != 0) {
        c3 = gGame_58.f750 - 1;
        gGame_58.f750 = c3;
        if (c3 == 0) {
            param_4 = 0xb;
            if (gGame_58.f2E8 != 0xb) {
                if (gGame_58.f2E8 == 0xd) {
                    gGame_58.f2E8 = 0xb;
                    gGame_58.f750 = 0x3b;
                }
            } else {
                gGame_58.f2E8 = 0xc;
            }
        }
    }
    flags = *param_3;
    if ((flags & 0x20) != 0) {
        shell_goto_screen(*(s32 *)(param_1 + 0x28), 0, 0, param_4);
        snd_play_menu_sound(2);
        return;
    }
    t1 = gGame_58.f450;
    if ((flags & 1) != 0) {
        if (t1 % 5 != 0) {
            gGame_58.f450 = gGame_58.f450 - 1;
        }
    }
    if ((*param_3 & 2) != 0) {
        c4 = gGame_58.f450;
        if (c4 % 5 < 4 && c4 + 1 < gGame_58.f44A) {
            gGame_58.f450 = c4 + 1;
        }
    }
    if ((*param_3 & 4) != 0) {
        c5 = gGame_58.f450;
        if (c5 / 5 + (c5 >> 31) != (c5 >> 31)) {
            gGame_58.f450 = c5 - 5;
        }
    }
    if ((*param_3 & 8) != 0) {
        t = gGame_58.f450;
        w = gGame_58.f44A;
        if (t + 5 < w) {
            gGame_58.f450 = t + 5;
        } else if (t / 5 < (s16)(w / 5)) {
            gGame_58.f450 = w - 1;
        }
    }
    if (t1 != gGame_58.f450) {
        gGame_58.f2E8 = 0xd;
        gGame_58.f750 = 6;
        snd_play_menu_sound(0);
    }
}

#define G_6 gGame_59
#define BT gPadInput_2[pad]
#define PRESSED_2(pad, m) ((gPadInput_2[pad] & (m)) && !(gPadInputPrev_2[pad] & (m)))

typedef struct {
    u8 c[3];
} Trip;

typedef struct {
    s32 f0;
    u8 pad4[0x2C8 - 0x4];
    s8 f2C8[0x20];
    s32 f2E8;
    u8 pad2EC[0x3EC - 0x2EC];
    s32 f3EC;
    u8 pad3F0[0x454 - 0x3F0];
    s16 f454;
    s16 f456;
    s16 f458;
    u8 pad45A[0x460 - 0x45A];
    s32 f460;
    s16 f464;
    u8 f466[0x12];
    s32 f478;
    u8 pad47C[0x734 - 0x47C];
    Trip f734[12];
    s8 f758[4];
    u8 pad75C[0x7A8 - 0x75C];
    u8 f7A8;
    u8 f7A9;
    u8 pad7AA[0x84C - 0x7AA];
    Trip f84C[4];
} Game_28;

typedef struct {
    s32 f0;
    char *name;
} NameEnt;

struct Obj_61 {
    u8 pad0[0xC];
    Pal *fC;
    u8 pad10[0x16 - 0x10];
    s16 f16;
    u8 pad18[0x24 - 0x18];
    s32 f24;
};

extern Game_28 gGame_59;
extern NameEnt sCheatTable[];
extern Trip D_800F05BC[];

void shell_text_entry_input(Obj_61 *o, s32 unused, s32 *btn, Pad16 *arg3) {
    s32 isNine;
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

    isNine = o->f16 == 9;
    if (++G_6.f3EC == 120) {
        G_6.f3EC = 0;
    }
    if (isNine) {
        pad = 0;
    } else {
        sel = G_6.f758[G_6.f460];
        pad = G_6.f2C8[sel];
    }
    if ((u32)(G_6.f2E8 - 0x11) < 2) {
        if (btn[pad] & 0x10) {
            shell_goto_screen_2((s32)sScreenMainMenu, 0, 0);
        }
        return;
    }
    b = btn[pad];
    if (b & 0x10) {
        found = 0;
        if (G_6.f454 == 0x26 && G_6.f464 == 0) {
            snd = 3;
        } else if (isNine && G_6.f454 == 0x27) {
            {
                for (i = 0; i < 14; i++) {
                    p = (u8 *)sCheatTable[i].name;
                    q = G_6.f466;
                loop:
                    m = *p;
                    if (m == *q) {
                        if (m == 0) {
                            found = 1;
                            m = 1 << i;
                            G_6.f7A9 = i;
                            if (m == 4) {
                                G_6.f0 ^= 0x20000000;
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
        if (G_6.f454 == 0x26) {
            if (G_6.f464 != 0) {
                G_6.f466[G_6.f464--] = 0;
            }
        } else if (G_6.f454 == 0x27) {
            if (isNine) {
                if (found) {
                    G_6.f2E8 = 0x11;
                } else {
                    G_6.f2E8 = 0x12;
                }
                return;
            }
            for (i = G_6.f460 + 1; i < 4; i++) {
                if (G_6.f758[i] == G_6.f758[G_6.f460]) {
                    D_800F05BC[i] = *(Trip *)G_6.f466;
                    if (i == 3) {
                        G_6.f7A8 = G_6.f758[i];
                    }
                    G_6.f758[i] = -1;
                }
            }
            { s32 k = G_6.f460; D_800F05BC[k] = *(Trip *)G_6.f466; }
            {
                Trip *t = G_6.f734;
                t[sel] = *(Trip *)G_6.f466;
            }
            for (i = G_6.f460 + 1; i < 4; i++) {
                if (G_6.f758[i] != G_6.f758[G_6.f460] && G_6.f758[i] != -1) {
                    G_6.f460 = i;
                    break;
                }
            }
            if (i < 4) {
                s32 k = G_6.f758[G_6.f460];
                if (G_6.f734[k].c[0] != 0) {
                    Trip *t = G_6.f734;
                    *(Trip *)G_6.f466 = t[k];
                    G_6.f466[3] = 0;
                    G_6.f464 = strlen(G_6.f466);
                    G_6.f456 = 0x31F;
                    G_6.f458 = 0x31F;
                    G_6.f454 = 0x27;
                } else {
                    G_6.f466[0] = 0x41;
                    G_6.f466[1] = 0;
                    G_6.f466[2] = 0;
                    G_6.f466[3] = 0;
                    G_6.f464 = 0;
                    G_6.f456 = 0;
                    G_6.f458 = 0;
                    G_6.f454 = 0;
                }
                {
                    s32 t = o->f24;
                    o->f24 = G_6.f478;
                    G_6.f478 = t;
                }
                return;
            }
            shell_goto_screen_2(o->fC->f4, 0, 0);
        } else {
            s32 n = (s16)++G_6.f464;
            if (n >= (isNine ? 10 : 3)) {
                G_6.f456 = 0x31F;
                G_6.f458 = 0x31F;
                G_6.f454 = 0x27;
            }
        }
        return;
    }
    if (b & 0x20) {
        if (G_6.f464 != 0) {
            G_6.f466[G_6.f464 - 1] = G_6.f466[G_6.f464];
            snd = 1;
            G_6.f466[G_6.f464--] = 0;
        } else {
            snd = 3;
        }
        snd_play_menu_sound(snd);
        return;
    }
    old = G_6.f454;
    if (G_6.f464 < (isNine ? 10 : 3)) {
        s32 x, v;
        s32 col, row;
        lo = 0;
        lo2 = lo;
        hi = 0x320;
        hi2 = hi;
        G_6.f456 += (BT & 0xC) ? ((BT & 8) ? ((BT >> 24) & 0xF) : -((BT >> 24) & 0xF)) : 0;
        if (PRESSED_2(pad, 0x2000)) {
            G_6.f456 -= 100;
        }
        if (PRESSED_2(pad, 0x4000)) {
            G_6.f456 += 100;
        }
        if (G_6.f456 >= hi) {
            G_6.f456 = lo;
        } else if (G_6.f456 < lo) {
            G_6.f456 = hi - 1;
        }
        G_6.f458 -= (BT & 3) ? ((BT & 1) ? ((s32)((u32)BT >> 28) * 3 >> 1) : (-(s32)((u32)BT >> 28) * 3) / 2) : 0;
        if (PRESSED_2(pad, 0x800)) {
            G_6.f458 -= 0xA0;
        }
        if (PRESSED_2(pad, 0x1000)) {
            G_6.f458 += 0xA0;
        }
        if (G_6.f458 >= hi2) {
            G_6.f458 = lo2;
        } else if (G_6.f458 < lo2) {
            G_6.f458 = hi2 - 1;
        }
        col = G_6.f456;
        row = G_6.f458;
        G_6.f454 = col / 100 + row / 160 * 8;
    } else if (btn[pad] & 0xC) {
        if (G_6.f454 == 0x27) {
            G_6.f456 = 0x258;
            G_6.f458 = 0x31F;
            G_6.f454 = 0x26;
        } else {
            G_6.f456 = 0x31F;
            G_6.f458 = 0x31F;
            G_6.f454 = 0x27;
        }
    }
    if (G_6.f454 < 0x26) {
        if (strlen(G_6.f466) == 0) {
            G_6.f466[1] = 0;
        }
        G_6.f466[G_6.f464] = *gKeyboardKeys[G_6.f454];
    } else {
        G_6.f466[G_6.f464] = 0;
    }
    if (old != G_6.f454) {
        snd_play_menu_sound(0);
    }
}

typedef struct {
    u32 flags;
    u8 pad04[0x4C - 0x4];
    s32 f4C;
    u8 pad50[0x17C - 0x50];
    s32 f17C;
    u8 pad180[0x2E8 - 0x180];
    s32 state;
    u8 pad2EC[0x3EC - 0x2EC];
    s32 f3EC;
    u8 pad3F0[0x3FC - 0x3F0];
    s32 f3FC;
    s32 sel;
    u8 pad404[0x418 - 0x404];
    s16 f418;
    u8 f41A[6];
    s32 f420[6];
    u8 pad438[0x488 - 0x438];
    s16 f488;
    u8 pad48A[0x764 - 0x48A];
    u8 f764[6];
    u8 f76A[6];
} Menu_2;

typedef struct {
    u8 pad[0x50];
    u32 f50;
} Trk_11;

struct Obj_62 {
    u8 pad[0x16];
    s16 f16;
};

extern Menu_2 gGame_60;
extern Trk_11 *gCurTrack_38;

void shell_race_results_input(Obj_62 *p1, s32 p2, u32 *p3, Pad16 *arg3)
{
    u32 fl;
    s32 orig, v, i;

    gGame_60.f17C++;
    if (gGame_60.f17C == 60) {
        gGame_60.f17C = 0;
    }
    gGame_60.f3EC++;
    if (gGame_60.f3EC == 120) {
        gGame_60.f3EC = 0;
    }
    fl = *p3;
    if (fl & 0x20) {
        if (gGame_60.state == 3) {
            gGame_60.state = 0;
        }
        return;
    }
    if (fl & 0x10) {
        snd_play_menu_sound(1);
        if (gGame_60.state == 3) {
            switch (gGame_60.sel) {
            case 0:
                if (gGame_60.f4C == 8) {
                    if (!(gGame_60.flags & 0x80000)) {
                        gActiveReplay = 0;
                    }
                    shell_goto_screen_2(0, 0, 0);
                } else if (gCurTrack_38->f50 & 2) {
                    shell_goto_screen_2(0, 0, 0);
                    gActiveReplay = 0;
                } else {
                    gActiveReplay = 0;
                    shell_goto_screen_2((s32)sScreenBronzeTracks, 0, 1);
                }
                break;
            case 1:
                if (gGame_60.f4C == 8) {
                    gGame_60.f488 = 2;
                    shell_goto_screen_2((s32)sScreenPak, 1, 1);
                } else {
                    gGame_60.f418 = gEvent_6.f22;
                    for (i = 0; i < 6; i++) {
                        gGame_60.f41A[i] = gEvent_6.f50[i];
                        gGame_60.f420[i] = gRiders_3[i].f68C;
                        gGame_60.f76A[i] = gRiders_3[i].f669;
                        if (gSeason != 0) {
                            gGame_60.f764[i] = (gSeason + i)[0x6C];
                        }
                    }
                    shell_goto_screen_2(0, 0, 1);
                    gReplayPlayback = 1;
                    gGame_60.f3FC = (s32)p1;
                    gGame_60.flags |= 0x8000;
                }
                break;
            case 2:
                gGame_60.flags &= ~0x80000;
                if (gCurTrack_38->f50 & 2) {
                    mem_free((void *)gUserTrackLayout);
                    gUserTrackLayout = 0;
                }
                shell_goto_screen_2((s32)sScreenMainMenu, 0, 1);
                gActiveReplay = 0;
                break;
            }
        } else if (p1->f16 == 7) {
            shell_goto_screen_2((s32)sScreenSeasonStandings, 1, 1);
        } else {
            gGame_60.sel = 0;
            gGame_60.state = 3;
        }
        return;
    }
    if (gGame_60.state != 3) {
        return;
    }
    orig = gGame_60.sel;
    if (fl & 0x801) {
        if (gReplayWritePtr - gReplayBuf >= gReplayBufSize || gGame_60.f4C == 8) {
            gGame_60.sel = orig ^ 2;
        } else {
            gGame_60.sel = orig - 1;
            if (orig - 1 < 0) {
                gGame_60.sel = orig + 2;
            }
        }
    }
    if (*p3 & 0x1002) {
        if (gReplayWritePtr - gReplayBuf >= gReplayBufSize || gGame_60.f4C == 8) {
            gGame_60.sel ^= 2;
        } else {
            v = gGame_60.sel;
            gGame_60.sel = v + 1;
            if (v + 1 >= 3) {
                gGame_60.sel = v - 2;
            }
        }
    }
    if (gGame_60.sel != orig) {
        snd_play_menu_sound(0);
    }
}

typedef struct {
    s32 f0;
    u8 pad4[0x3C - 0x4];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    s32 f4C;
    u8 pad50[0x17C - 0x50];
    s32 f17C;
    u8 pad180[0x2E8 - 0x180];
    s32 f2E8;
    u8 pad2EC[0x3EC - 0x2EC];
    s32 f3EC;
    u8 pad3F0[0x3FC - 0x3F0];
    s32 f3FC;
    s32 f400;
    u8 pad404[0x418 - 0x404];
    u16 f418;
    u8 f41A[6];
    s32 f420[6];
    u8 pad438[0x47C - 0x438];
    s16 f47C;
    u8 pad47E[0x764 - 0x47E];
    u8 f764[6];
    u8 f76A[6];
} GState_3;

typedef struct {
    u8 n;
    u8 pad[0x23];
} Trk_12;

typedef struct {
    Trk_12 *trk;
    u8 pad4[0x64 - 4];
    s16 f64;
    u8 pad66[0x6C - 0x66];
    u8 f6C[6];
} Race_7;

extern GState_3 gGame_61;
extern Race_7 *gSeason_17;
extern Trk_12 D_800CA9C8_4[];

void shell_season_standings_input(Info *arg0, s32 arg1, u32 *arg2, Pad16 *arg3)
{
    u32 b;
    Trk_12 *t;
    s32 i;
    s32 d;

    if (++gGame_61.f17C == 0x3C) {
        gGame_61.f17C = 0;
    }
    if (++gGame_61.f3EC == 0x78) {
        gGame_61.f3EC = 0;
    }
    b = *arg2;
    if (b & 0x20) {
        snd_play_menu_sound(2);
        if (gGame_61.f2E8 == 4) {
            gGame_61.f2E8 = 0;
            return;
        }
        if (gGame_61.f2E8 == 7) {
            gGame_61.f2E8 = 4;
            return;
        }
        if (arg0->f28 != 0) {
            shell_goto_screen_2(arg0->f28, 0, 0);
        }
    } else if (b & 0x10) {
        snd_play_menu_sound(1);
        if (gGame_61.f2E8 == 4) {
            switch (gGame_61.f400) {
            case 0:
                gGame_61.f0 |= 0x80;
                t = gSeason_17->trk;
                if (gSeason_17->f64 + 1 >= t->n) {
                    if (gSeason_17->f6C[0] == 0 && (t - D_800CA9C8_4 != 4 || D_800F05F8 == 0)) {
                        gGame_61.f0 = (gGame_61.f0 & ~0x80) | 0x40000;
                        shell_goto_screen_2(0, 0, 1);
                        gActiveReplay = 0;
                        return;
                    }
                } else if (t - D_800CA9C8_4 != 4 || D_800F05F8 == 0) {
                    shell_goto_screen_2(0, 0, 1);
                    gSeason_17->f64++;
                    race_jitter_ai_order(gGame_61.f3C, gEventNumRiders - gGame_61.f3C);
                    race_grid_from_standings();
                    gActiveReplay = 0;
                    return;
                }
                shell_goto_screen_2((s32)D_800CD6C0, 0, 1);
                gActiveReplay = 0;
                return;
            case 1:
                gGame_61.f418 = gEvent_6.f22;
                for (i = 0; i < 6; i++) {
                    gGame_61.f41A[i] = gEvent_6.f50[i];
                    gGame_61.f420[i] = gRiders_3[i].f68C;
                    gGame_61.f76A[i] = gRiders_3[i].f669;
                    if (gSeason_17 != 0) {
                        gGame_61.f764[i] = gSeason_17->f6C[i];
                    }
                }
                shell_goto_screen_2(0, 0, 1);
                gReplayPlayback = 1;
                gGame_61.f3FC = (s32)arg0;
                gGame_61.f0 |= 0x8000;
                return;
            case 2:
                gGame_61.f2E8 = 7;
                gGame_61.f47C = 0;
                return;
            }
        } else {
            if (gGame_61.f2E8 == 7) {
                switch (gGame_61.f47C) {
                case 1:
                    shell_goto_screen_2((s32)sScreenMainMenu, 0, 1);
                    gActiveReplay = 0;
                    return;
                case 0:
                    gGame_61.f2E8 = 4;
                    return;
                }
            } else {
                gGame_61.f400 = 0;
                gGame_61.f2E8 = 4;
            }
        }
    } else {
        if (gGame_61.f2E8 == 7) {
            if (b & 3) {
                gGame_61.f47C ^= 1;
                snd_play_menu_sound(0);
            }
        } else if (gGame_61.f2E8 == 4) {
            if (b & 0x801) {
                d = gReplayWritePtr - gReplayBuf;
                if (gGame_61.f4C != 8 ? d >= gReplayBufSize : d >= 0x2800) {
                    gGame_61.f400 ^= 2;
                } else {
                    if (--gGame_61.f400 < 0) {
                        gGame_61.f400 += 3;
                    }
                }
                snd_play_menu_sound(0);
            }
            if (*arg2 & 0x1002) {
                snd_play_menu_sound(0);
                d = gReplayWritePtr - gReplayBuf;
                if (gGameMode != 8 ? d >= gReplayBufSize : d >= 0x2800) {
                    gGame_61.f400 ^= 2;
                } else {
                    if (++gGame_61.f400 >= 3) {
                        gGame_61.f400 -= 3;
                    }
                }
            }
        }
    }
}

typedef struct {
    /* 0x000 */ s32 f0;
    /* 0x004 */ u8 pad4[0x3C - 0x4];
    /* 0x03C */ s32 f3C;
    /* 0x040 */ u8 pad40[0x4C - 0x40];
    /* 0x04C */ s32 f4C;
    /* 0x050 */ u8 pad50[0x54 - 0x50];
    /* 0x054 */ s32 f54;
    /* 0x058 */ u8 pad58[0x17C - 0x58];
    /* 0x17C */ s32 f17C;
    /* 0x180 */ u8 pad180[0x2E8 - 0x180];
    /* 0x2E8 */ s32 f2E8;
    /* 0x2EC */ u8 pad2EC[0x3FC - 0x2EC];
    /* 0x3FC */ void *f3FC;
    /* 0x400 */ u8 pad400[0x448 - 0x400];
    /* 0x448 */ s16 f448;
    /* 0x44A */ u8 pad44A[0x47C - 0x44A];
    /* 0x47C */ u16 f47C;
    /* 0x47E */ u8 pad47E[0x770 - 0x47E];
    /* 0x770 */ s32 f770;
    /* 0x774 */ s32 f774;
} State_7;

extern State_7 gGame_62;

void shell_options_input(Info *arg0, s32 arg1, s32 *arg2, Pad16 *arg3) {
    s32 m;
    s32 old;
    s32 d;

    if (++gGame_62.f17C == 60) {
        gGame_62.f17C = 0;
    }
    if (gGame_62.f448 & 0x100) {
        gGame_62.f448 &= ~0x100;
        if (gGame_62.f448 == 0) {
            tunes_play_song(D_800D11D8);
        }
    }
    if ((gPadInput & 0x20) && !(gPadInputPrev & 0x20) && gGame_62.f54 == 3) {
        if (gGame_62.f2E8 == 6) {
            gGame_62.f2E8 = 0;
            snd_play_menu_sound(2);
        } else if (gGame_62.f2E8 != 14 && gGame_62.f2E8 != 15) {
            shell_goto_screen_3((void *)arg0->f28, 0, 0);
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
    if ((gPadInput & 0x10) && !(gPadInputPrev & 0x10) && gGame_62.f54 == 3) {
        switch (gGame_62.f448) {
        case 3:
                        gSaveData.f2C4 ^= 1;
            snd_play_menu_sound(1);
            return;
        case 8:
                        gSaveData.f2C4 ^= 0x10;
            snd_play_menu_sound(1);
            return;
        case 1:
            sfx_play(gGame_62.f774, 0x2B11, D_800F1BC6, 7, 1, 1);
            return;
        case 2:
            if (gGame_62.f770 == 0) {
                tunes_queue_speech(0x9B);
                gGame_62.f770 = 0x34;
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
            shell_goto_screen_3(sScreenControllerSettings, 1, 1);
                        snd_play_menu_sound(1);
            return;
        case 5:
            shell_goto_screen_3(sScreenRecords, 1, 1);
                        snd_play_menu_sound(1);
            return;
        case 6:
            shell_goto_screen_3(NULL, 0, 1);
                        gCreditsMode = 1;
            gGame_62.f4C = 0;
            gCurTrack = sTrackGravelPit;
            gGame_62.f3C = 1;
            gGame_62.f3FC = arg0;
            {
                s32 f = gGame_62.f0 & ~0x80;
                *(volatile s32 *)&gGame_62.f0 = f;
                gGame_62.f0 = f | 0x05000000;
            }
            snd_play_menu_sound(1);
            return;
        }
        return;
    }
    m = gGame_62.f2E8;
    if (m == 6 || m == 15 || m == 14) {
        if (m == 15) {
            save_load(1);
            gGame_62.f2E8 = 0;
        } else if (m != 14) {
            if (*arg2 & 3) {
                gGame_62.f47C ^= 1;
                snd_play_menu_sound(0);
            }
        }
        return;
    }
    old = gGame_62.f448;
    if (*arg2 & 2) {
        if (old < 3) {
            gGame_62.f448 += 1;
        } else {
            gGame_62.f448 += 2;
        }
        if (gGame_62.f448 >= 11) {
            gGame_62.f448 = 0;
        }
    }
    if (*arg2 & 1) {
        if (gGame_62.f448 < 4) {
            gGame_62.f448 -= 1;
        } else {
            gGame_62.f448 -= 2;
        }
        if (gGame_62.f448 < 0) {
            gGame_62.f448 = 9;
        }
    }
    if (old != gGame_62.f448) {
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
        switch (gGame_62.f448) {
        case 2:
            tunes_queue_speech(0x9B);
            gGame_62.f770 = 0x34;
            break;
        case 0:
            tunes_play_song(D_800D11D8);
            break;
        case 1:
            sfx_play(gGame_62.f774, 0x2B11, D_800F1BC6, 7, 1, 1);
            break;
        }
    }
    if (gGame_62.f448 >= 3) {
        if (*arg2 & 0xC) {
            if ((gGame_62.f448 - 3) & 1) {
                gGame_62.f448--;
            } else {
                gGame_62.f448++;
            }
        }
    } else {
        if (gGame_62.f770 != 0) {
            gGame_62.f770--;
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
        if (d != 0 && gGame_62.f770 == 0) {
            switch (gGame_62.f448) {
            case 1:
                sfx_play(gGame_62.f774, 0x2B11, D_800F1BC6, 7, 1, 1);
                gGame_62.f770 = 0x28;
                break;
            case 2:
                tunes_queue_speech(0x9B);
                gGame_62.f770 = 0x34;
                break;
            }
        }
        if (d < 0) {
            switch (gGame_62.f448) {
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
            switch (gGame_62.f448) {
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
    if (gGame_62.f448 != old) {
        snd_play_menu_sound(0);
    }
}

typedef struct {
    u32 flags;          /* 0x000 */
    u8 pad4[0x18];
    s32 sel[4];         /* 0x01C */
    u8 pad2C[0x10];
    s32 count;          /* 0x03C */
    u8 pad40[0xC];
    s32 f4C;            /* 0x04C */
    s32 f50;
    s32 f54;            /* 0x054 */
    u8 pad58[0x2A8 - 0x58];
    Pair f2A8[4];       /* 0x2A8 */
    s8 f2C8[4];         /* 0x2C8 */
    u8 pad2CC[0x14];
    s16 f2E0[4];        /* 0x2E0 */
    u8 pad2E8[0x3C0 - 0x2E8];
    s32 f3C0;           /* 0x3C0 */
    s32 f3C4;           /* 0x3C4 */
    u8 pad3C8[0x408 - 0x3C8];
    s16 f408[2];        /* 0x408 */
    u8 f40C[4];         /* 0x40C */
} State_8;

typedef struct {
    s32 pad0;
    s32 f4;
} Sub_11;

struct Obj_63 {
    u8 pad0[0xC];
    Sub_11 *fC;
    u8 pad10[0x18];
    s32 f28;
};

extern State_8 gGame_63;

void shell_rider_select_input(Obj_63 *p1, s32 p2, u32 *p3, Pad16 *arg3)
{
    State_8 *g;
    State_8 *h;
    State_8 *k;
    State_8 *m;
    s32 i;
    s32 ok;
    s32 idx;
    s32 t;
    u32 mask;

    g = &gGame_63;
    for (i = 0; i < 4; i++) {
        if (gGame_63.f2E0[i] != 0) {
            gGame_63.f2E0[i]--;
        }
        gGame_63.f2A8[i].a = (gGame_63.f2A8[i].a + 0x23) & 0xFFFF;
    }

    ok = 1;
    h = &gGame_63;
    for (i = 0; i < h->count; i++) {
        if (!(gGame_63.flags & (0x400 << i))) {
            ok = 0;
        }
    }
    race_update(D_800CD090);

    k = &gGame_63;
    if (k->count == 1) {
        if (*p3 & 0x780) snd_play_menu_sound(0);
        if (*p3 & 0x200) k->f3C0 = (k->f3C0 + 5) % 6;
        if (*p3 & 0x400) k->f3C0 = (k->f3C0 + 1) % 6;
        if (k->f4C != 8) {
            if (*p3 & 0x80) k->f3C4 = (k->f3C4 + 5) % 6;
            if (*p3 & 0x100) k->f3C4 = (k->f3C4 + 1) % 6;
        }
    }

    if ((*p3 & 0x50) && ok) {
        snd_play_menu_sound(1);
        if (D_800F0128 < 8) {
            if (D_800F0128 >= 2) {
                shell_goto_screen_2(0, 1, 1);
            } else {
                shell_goto_screen_2(p1->fC->f4, 1, 1);
            }
        } else {
            shell_goto_screen_2(p1->fC->f4, 1, 1);
        }
        return;
    }
    if (*p3 & 0x20) {
        if (!(gGame_63.flags & 0x400)) {
            if (p1->f28 != 0) {
                if (p1->f28 == (s32)sScreenPak && gUserTrackLayout != 0) {
                    gGame_63.flags |= 0x2000000;
                }
                shell_goto_screen_2(p1->f28, 0, 0);
                snd_play_menu_sound(2);
            }
            return;
        }
    }
    m = &gGame_63;
    if (m->f54 != 3) return;
    for (i = 0; i < gGame_63.count; i++) {
        if (!((gContMask >> (idx = gGame_63.f2C8[i])) & 1)) continue;
        if (gGame_63.f40C[i] == 0 && !(gGame_63.flags & (0x400 << i))) {
            if (p3[idx] & 4) {
                t = gGame_63.sel[i];
                gGame_63.sel[i] = t - 1;
                if (t - 1 < 0) gGame_63.sel[i] = t + 5;
                if (gGame_63.count == 1) gGame_63.f3C0 = (gGame_63.f3C0 + 5) % 6;
                snd_play_menu_sound(0);
            }
            if (p3[gGame_63.f2C8[i]] & 8) {
                t = gGame_63.sel[i];
                gGame_63.sel[i] = t + 1;
                if (t + 1 >= 6) gGame_63.sel[i] = t - 5;
                if (gGame_63.count == 1) gGame_63.f3C0 = (gGame_63.f3C0 + 1) % 6;
                snd_play_menu_sound(0);
            }
        }
        if (gGame_63.count >= 3 || gGame_63.f40C[i] == 0) {
            if (!(gGame_63.flags & (0x400 << i)) && (p3[gGame_63.f2C8[i]] & 0x50)) {
                gGame_63.f2E0[i] = 0x28 - gGame_63.f2E0[i];
                gGame_63.flags |= 0x400 << i;
                snd_play_menu_sound(1);
            } else if ((gPadInput & 0x20000) && gGame_63.count == 1) {
                gGame_63.f40C[i] = 1;
                gGame_63.f408[i] = 0x3D;
            }
        } else {
            if (gGame_63.f40C[i] == 1) {
                if (--gGame_63.f408[i] <= 0) {
                    gGame_63.f40C[i] = 2;
                } else if (!(gPadInput & 0x20000)) {
                    gGame_63.f40C[i] = 3;
                    gGame_63.f408[i] = 0x3E - gGame_63.f408[i];
                }
            } else if (gGame_63.f40C[i] == 2) {
                if (!(gPadInput & 0x20000)) {
                    gGame_63.f40C[i] = 3;
                    gGame_63.f408[i] = 0x3D;
                }
            } else if (gGame_63.f40C[i] == 3) {
                if (gPadInput & 0x20000) {
                    gGame_63.f40C[i] = 1;
                    gGame_63.f408[i] = 0x3E - gGame_63.f408[i];
                }
                if (--gGame_63.f408[i] <= 0) {
                    gGame_63.f40C[i] = 0;
                }
            }
        }
        mask = 0x400 << i;
        if ((gGame_63.flags & mask) && (p3[gGame_63.f2C8[i]] & 0x20)) {
            snd_play_menu_sound(2);
            gGame_63.f2E0[i] = 0x28 - gGame_63.f2E0[i];
            gGame_63.flags &= ~mask;
        }
    }
}

#define OK (gSaveData_9.f2C0 > gGame_64.f410 \
    || (gSaveData_9.f2C0 == gGame_64.f410 && (*a0->f20 == 0 || ((gSaveData_9.f2BF >> (*a0->f20 - 1)) & 1))) \
    || (gGame_64.f4C == 1 && gSeason_18 != NULL && *gSeason_18 - D_800CA9C8_5 == 5 \
        && gSaveData_9.f2C0 == gGame_64.f410 && (gSaveData_9.f2BF & 8)))

typedef struct {
    u8 n, f1, f2, f3;
    u8 pad4[4];
    u16 f8[7];
    s16 f16[7];
} Track_16;

typedef struct {
    u8 pad0[0x4C];
    s32 f4C;
    u8 pad50[0x64 - 0x50];
    s32 f64, f68, f6C; u32 f70;
    u8 pad74[0x134 - 0x74];
    s16 f134;
    s16 pad136;
    s16 f138;
    u8 pad13A[0x17C - 0x13A];
    s32 f17C;
    u8 pad180[0x3EC - 0x180];
    s32 f3EC;
    u8 pad3F0[0x410 - 0x3F0];
    s32 f410;
} Menu_3;

typedef struct {
    u8 pad0[0x2A8];
    u8 f2A8[5];
    u8 pad2AD[0x2BF - 0x2AD];
    u8 f2BF;
    u8 f2C0;
} Save_3;

struct Arg0 { u8 pad[0x20]; s32 *f20; u8 pad24[4]; s32 f28; };
struct Arg1 { u8 pad[4]; s32 f4; u16 f8; };

extern Menu_3 gGame_64;
extern Save_3 gSaveData_9;
extern Track_16 D_800CA9C8_5[];
extern Track_16 **gSeason_18;

void shell_season_select_input(Arg0 *a0, Arg1 *a1, s32 *pad, Pad16 *arg3) {
    s32 k, j, i;

    if (++gGame_64.f17C == 60) gGame_64.f17C = 0;
    if (++gGame_64.f3EC == 64) gGame_64.f3EC = 0;
    switch (gGame_64.f70) {
    case 0:
        if (++gGame_64.f64 == 60) {
            if (++gGame_64.f68 >= D_800CA9C8_5[*a0->f20].n) gGame_64.f68 = 0;
            gGame_64.f64 = 0;
        }
        if (*a0->f20 == 5 && gCustomRound != 0) {
            if (*pad & 1) {
                s32 v = gGame_64.f6C;
                gGame_64.f6C = v - 1;
                if (v - 1 < 0) gGame_64.f6C = v + 1;
            }
            if (*pad & 2) {
                s32 v = gGame_64.f6C;
                if (++gGame_64.f6C >= 2) gGame_64.f6C -= 2;
            }
        }
        if (gGame_64.f410 != 0 && (*pad & 0xC)) {
            if (*pad & 8) {
                s32 *p = a0->f20;
                s32 v = *p;
                switch (gGame_64.f410) {
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
                if (v != *a0->f20) {
                    snd_play_menu_sound(0);
                    gGame_64.f64 = 0;
                    gGame_64.f68 = 0;
                }
            } else if (*a0->f20 > 0) {
                snd_play_menu_sound(0);
                (*a0->f20)--;
                gGame_64.f64 = 0;
                gGame_64.f68 = 0;
            }
        }
        if ((*pad & 0x50) && !OK) {
            snd_play_menu_sound(3);
        } else if ((*pad & 0x50) && OK) {
            if (*a0->f20 == 5 && (gGame_64.f6C == 1 || gCustomRound == 0)) {
                gGame_64.f70 = 1;
                gGame_64.f6C = 0;
            } else if (!(a1->f8 & 1)) {
                shell_goto_screen_2(a1->f4, 1, 1);
                snd_play_menu_sound(1);
            } else {
                snd_play_menu_sound(3);
            }
        } else if ((*pad & 0x20) && a0->f28 != 0) {
            shell_goto_screen_2(a0->f28, 0, 0);
            snd_play_menu_sound(2);
        }
        break;
    case 1:
        if ((*pad & 1) && gGame_64.f6C != 0) gGame_64.f6C--;
        if (gGame_64.f6C < D_800CA9C8_5[*a0->f20].n) {
            if (*pad & 4) {
                if (D_800CA9C8_5[*a0->f20].f16[gGame_64.f6C] >= 2) D_800CA9C8_5[*a0->f20].f16[gGame_64.f6C]--;
            }
            if (*pad & 8) {
                if (D_800CA9C8_5[*a0->f20].f16[gGame_64.f6C] < 6) D_800CA9C8_5[*a0->f20].f16[gGame_64.f6C]++;
            }
        }
        if ((*pad & 2) && gGame_64.f6C < D_800CA9C8_5[*a0->f20].n) {
            s32 v = gGame_64.f6C + 1;
            if (v < 5) gGame_64.f6C = v;
        }
        k = *pad;
        if (k & 0x50) {
            u16 w;
            gGame_64.f70 = 2;
            if (gGame_64.f6C == D_800CA9C8_5[*a0->f20].n) {
                gGame_64.f134 = 0;
                D_800CA9C8_5[*a0->f20].f1++;
                D_800CA9C8_5[*a0->f20].f2++;
                D_800CA9C8_5[*a0->f20].f3++;
                D_800CA9C8_5[*a0->f20].n++;
                gGame_64.f138 = -1;
            } else {
                w = D_800CA9C8_5[*a0->f20].f8[gGame_64.f6C];
                gGame_64.f134 = w;
                gGame_64.f138 = w;
            }
            D_800CA9C8_5[*a0->f20].f8[gGame_64.f6C] = gGame_64.f134;
        } else if (k & 0x20) {
            gGame_64.f70 = 0;
            gGame_64.f6C = 0;
            for (i = 0; i < 5; i++) {
                if (i < D_800CA9C8_5[5].n) gSaveData_9.f2A8[i] = D_800CA9C8_5[5].f8[i] & 0x1F;
                else gSaveData_9.f2A8[i] = 0x3F;
                gSaveData_9.f2A8[i] |= (D_800CA9C8_5[5].f16[i] & 7) << 5;
            }
            save_write(1, 0);
        } else {
            if (gGame_64.f6C != D_800CA9C8_5[*a0->f20].n && (k & 0x20000)) {
                for (j = gGame_64.f6C; j < D_800CA9C8_5[*a0->f20].n - 1; j++) {
                    D_800CA9C8_5[*a0->f20].f8[j] = D_800CA9C8_5[*a0->f20].f8[j + 1];
                }
                { u8 m = --D_800CA9C8_5[*a0->f20].n;
                D_800CA9C8_5[*a0->f20].f8[m] = 0; }
            }
        }
        break;
    case 2:
        if (*pad & 4) {
            u16 v = gGame_64.f134;
            gGame_64.f134 = v - 1;
            if ((s16)v % 5 == 0) gGame_64.f134 = v + 4;
        }
        if (*pad & 8) {
            u16 v = gGame_64.f134;
            gGame_64.f134 = v + 1;
            if ((s16)(v + 1) % 5 == 0) gGame_64.f134 = v - 4;
        }
        if (*pad & 1) {
            gGame_64.f134 -= 5;
            if (gGame_64.f134 < 0) gGame_64.f134 += 20;
        }
        if (*pad & 2) {
            u16 v = gGame_64.f134;
            gGame_64.f134 = v + 5;
            if (gGame_64.f134 >= 20) gGame_64.f134 = v - 15;
        }
        k = *pad;
        if (k & 0x50) {
            D_800EFDE0 = 1;
        } else if (k & 0x20) {
            gGame_64.f70 = 1;
            if (gGame_64.f138 == -1) D_800CA9C8_5[*a0->f20].n--;
            else gGame_64.f134 = gGame_64.f138;
        }
        D_800CA9C8_5[*a0->f20].f8[gGame_64.f6C] = gGame_64.f134;
        break;
    }
    *gSeason_18 = &D_800CA9C8_5[*a0->f20];
}

typedef struct {
    u8 pad[0x2BF];
    u8 mask;
    u8 cup;
} Save_4;

extern Save_4 gSaveData_10;

s32 shell_is_round_unlocked(s32 n)
{
    if (D_800F0180 == gSaveData_10.cup) {
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

struct Ctl_19 {
    u8 pad0[4];
    s32 p4;
    u16 h8;
};

typedef struct {
    s32 w0;
    u8 pad4[0x3C - 0x4];
    s32 w3C;
    u8 pad40[0x4C - 0x40];
    s32 w4C;
    u8 pad50[4];
    s32 w54;
    u8 pad58[0x17C - 0x58];
    s32 w17C;
    u8 pad180[0x29C - 0x180];
    s32 w29C;
    s32 w2A0;
    u8 pad2A4[0x2E8 - 0x2A4];
    s32 w2E8;
    u8 pad2EC[0x3EC - 0x2EC];
    s32 w3EC;
    u8 pad3F0[0x410 - 0x3F0];
    s32 w410;
    u8 pad414[0x47C - 0x414];
    s16 h47C;
    u8 pad47E[0x488 - 0x47E];
    s16 h488;
    u8 pad48A[0x75C - 0x48A];
    s32 w75C;
} Game_29;

typedef struct {
    u8 pad0[0x2C0];
    u8 b2C0;
    u8 pad2C1[3];
    u8 b2C4;
} S1900;

extern Game_29 gGame_65;
extern S1900 gSaveData_11;

static inline s32 canGo(Item_9 *n) {
    s32 k;
    if (gGame_65.w4C == 8 || gGame_65.w4C == 0) {
        k = gSaveData_11.b2C0;
    } else {
        k = gGame_65.w410;
    }
    switch (k) {
    case 1:
        if (n->h14 < 10) return 1;
        break;
    case 2:
        if (n->h14 < 15) return 1;
        break;
    case 3:
        return 1;
    }
    return 0;
}

void shell_list_menu_input(Item_9 *item, Ctl_19 *ctl, s32 *btn, Pad16 *arg3) {
    s32 snd;
    s32 m;
    s32 b;
    s32 s;
    s32 v;
    Item_9 *prev;
    Item_9 *n;
    s32 k;
    s32 ok;

    gGame_65.w29C = D_800CDB5C;
    gGame_65.w2A0 = D_800CDB60;
    D_800CDB60 -= (gPadInput & 3) ? ((gPadInput & 1) ? (s32)(gPadInput >> 28) / 6.0f : -(s32)(gPadInput >> 28) / 6.0f) : 0.0f;
    if (++gGame_65.w17C == 60) gGame_65.w17C = 0;
    if (++gGame_65.w3EC == 64) gGame_65.w3EC = 0;
    b = *btn;
    if (b & 0x50) {
        snd = 0;
        if (!(ctl->h8 & 1)) {
            snd = 1;
        } else {
            snd_play_menu_sound(3);
        }
        if (item - sScreens_2 == 8 && (gPadInput & 0x8500) == 0x8500) {
            shell_goto_screen_2((s32)&sScreens_2[4], 1, 0);
        } else if ((ctl->h8 & 0x800) && gGame_65.w2E8 != 8) {
            gGame_65.w2E8 = 8;
            gGame_65.h47C = 0;
        } else if ((ctl->h8 & 0x1000) && gGame_65.w2E8 != 16 && gGame_65.w2E8 != 9
                   && gGame_65.w2E8 != 10 && !(gGame_65.w0 & 0x200)) {
            if (!(ctl->h8 & 1)) {
                gGame_65.w2E8 = 9;
                gGame_65.h47C = 0;
            }
        } else if (gGame_65.w2E8 == 16) {
            gGame_65.w4C = 0;
            gGame_65.w2E8 = 0;
            gGame_65.w0 |= 0x200;
        } else if (!(ctl->h8 & 0x100) || gGame_65.w2E8 == 5) {
          if (!(ctl->h8 & 1)) {
            if (gGame_65.w2E8 == 8 && gGame_65.h47C == 1) {
                gGame_65.h488 = 1;
                shell_goto_screen_2((s32)&sScreens_2[15], 1, 1);
            } else if (gGame_65.w2E8 == 10 && !(gGame_65.w0 & 0x200)) {
                gGame_65.w2E8 = 16;
                if (gGame_65.w410 == 0) {
                    gGame_65.w410 = 1;
                }
            } else if (gGame_65.w2E8 == 9 && gGame_65.h47C == 0) {
                gGame_65.w2E8 = 10;
                if (gSaveData_11.b2C4 & 8) {
                    gGame_65.h47C = 0;
                } else {
                    gGame_65.h47C = 1;
                }
            } else {
                if (gGame_65.w0 & 0x200) {
                    if (gGame_65.h47C != 0) {
                        gGame_65.w0 |= 0x2000000;
                        gGame_65.h488 = 3;
                        gGame_65.w0 &= ~0x20;
                        shell_goto_screen_2((s32)&sScreens_2[15], 1, 1);
                    } else {
                        gCurTrack = (s32)D_800CBF30;
                        shell_goto_screen_2((s32)&sScreens_2[9], 1, 1);
                        usrtrk_load_layout(D_800F1DF4);
                        gGame_65.w4C = 0;
                        gGame_65.w0 &= ~0x80;
                        gGame_65.w0 &= ~0x20;
                    }
                }
                if (gGame_65.w2E8 == 5 && gGame_65.w410 == 0) {
                    shell_goto_screen_2(0, 0, 1);
                } else {
                    shell_goto_screen_2(ctl->p4, 1, 1);
                }
            }
          }
        } else {
            gGame_65.w2E8 = 5;
            if ((ctl->h8 & 0x400) && gGame_65.w410 == 0) {
                gGame_65.w410 = 1;
            }
        }
        if (snd) {
            snd_play_menu_sound(1);
        }
    } else if (b & 0x20) {
        if (gGame_65.w0 & 0x200) {
            gGame_65.w2E8 = 16;
            snd_play_menu_sound(2);
            gGame_65.w0 &= ~0x200;
            gGame_65.w0 &= ~0x20;
            return;
        }
        s = gGame_65.w2E8;
        if (s == 16) {
            gGame_65.w2E8 = 10;
        } else if (s == 8 || s == 5 || s == 9 || s == 10) {
            snd_play_menu_sound(2);
            gGame_65.w2E8 = 0;
            gGame_65.w0 &= ~0x20;
        } else if (item->p28) {
            snd_play_menu_sound(2);
            shell_goto_screen_2(item->p28, 0, 0);
        } else {
            snd_play_menu_sound(3);
        }
    } else if (gGame_65.w0 & 0x200) {
        m = gGame_65.w4C;
        if (m == 2) {
            if (gContCount < 2) {
                snd_play_menu_sound(2);
                gGame_65.w0 &= ~0x200;
            } else if (gGame_65.w3C == 0) {
                gGame_65.w3C = 2;
            } else if (gContCount < 4 && gGame_65.w3C == 4) {
                snd_play_menu_sound(0);
                gGame_65.w3C = 2;
            }
            if (gContCount == 4 && (*btn & 3)) {
                snd_play_menu_sound(0);
                if (gGame_65.w3C == 2) {
                    gGame_65.w3C = 4;
                } else {
                    gGame_65.w3C = 2;
                }
            }
        } else if (m == 7 || m == 10) {
            if (gGame_65.w3C == 0 || gContMask == 0) {
                gGame_65.w3C = 1;
            } else {
                s32 o = gGame_65.w3C;
                if (gContCount < o) {
                    gGame_65.w3C = gContCount;
                } else if (b & 3) {
                    gGame_65.w3C = (o & 1) + 1;
                    if (gContCount == 1) {
                        gGame_65.w3C = 1;
                    }
                    if (gGame_65.w3C != o) {
                        snd_play_menu_sound(0);
                    }
                }
            }
        } else if (gContMask == 0) {
            gGame_65.w3C = 1;
        } else {
            if (gGame_65.w3C == 0) {
                gGame_65.w3C = 1;
            }
            v = gContCount;
            if (v < gGame_65.w3C) {
                gGame_65.w3C = v;
            }
            if (*btn & 1) {
                s32 old = gGame_65.w3C;
                if ((gGame_65.w3C = old - 1) <= 0) {
                    gGame_65.w3C = v;
                }
                if (gGame_65.w3C != old) {
                    snd_play_menu_sound(0);
                }
            }
            if (*btn & 2) {
                s32 old = gGame_65.w3C;
                gGame_65.w3C = old + 1;
                if (gContCount < old + 1) {
                    gGame_65.w3C = 1;
                }
                if (gGame_65.w3C != old) {
                    snd_play_menu_sound(0);
                }
            }
            if (gGame_65.w4C == 0 && gGame_65.w3C == 2) {
                if (*btn & 4) {
                    s32 old;
                    snd_play_menu_sound(0);
                    old = gGame_65.w75C;
                    if ((gGame_65.w75C = old - 1) < 0) {
                        gGame_65.w75C = old + 2;
                    }
                }
                if (*btn & 8) {
                    snd_play_menu_sound(0);
                    if (++gGame_65.w75C >= 3) {
                        gGame_65.w75C -= 3;
                    }
                }
            }
        }
    } else {
        s = gGame_65.w2E8;
        if ((u32)(s - 8) < 3) {
            if ((s != 10 || (gSaveData_11.b2C4 & 8)) && (b & 3)) {
                snd_play_menu_sound(0);
                gGame_65.h47C ^= 1;
            }
            return;
        }
        if (b & 1) {
            if ((s == 16 || s == 5) && gSaveData_11.b2C0) {
                s32 old = gGame_65.w410;
                if ((gGame_65.w410 = old - 1) < 0) {
                    gGame_65.w410 = gSaveData_11.b2C0;
                } else if (gGame_65.w410 == 0 && ((ctl->h8 & 0x400) || s == 16)) {
                    gGame_65.w410 = gSaveData_11.b2C0;
                }
                if (gGame_65.w410 != old) {
                    snd_play_menu_sound(0);
                }
            } else {
                s32 old = *item->p20;
                if ((*item->p20 = old - 1) < 0) {
                    *item->p20 = item->h18 - 1;
                }
                if (*item->p20 != old) {
                    snd_play_menu_sound(0);
                }
            }
        }
        if (*btn & 2) {
            s32 t = gGame_65.w2E8;
            if ((t == 16 || t == 5) && gSaveData_11.b2C0) {
                s32 old = gGame_65.w410;
                gGame_65.w410 = old + 1;
                if (gSaveData_11.b2C0 < old + 1) {
                    if ((ctl->h8 & 0x400) || t == 16) {
                        gGame_65.w410 = 1;
                    } else {
                        gGame_65.w410 = 0;
                    }
                }
                if (gGame_65.w410 != old) {
                    snd_play_menu_sound(0);
                }
            } else {
                s32 old = *item->p20;
                *item->p20 = old + 1;
                if (old + 1 >= item->h18) {
                    *item->p20 = 0;
                }
                if (*item->p20 != old) {
                    snd_play_menu_sound(0);
                }
            }
        }
        if (gGame_65.w54 != 1) {
            prev = sCurScreen_4;
            if (*btn & 4) {
                if (item->p4 == NULL) {
                    if (item->p8) {
                        sCurScreen = item;
                    }
                } else {
                    item = item->p4;
                    sCurScreen = item;
                }
            }
            if (*btn & 8) {
                if (item->p8 == NULL) {
                    if (item->p4) {
                        sCurScreen = item;
                    }
                } else {
                    if (canGo(item->p8)) {
                        item = item->p8;
                    }
                    sCurScreen = item;
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

typedef struct {
    u8 pad0[4];
    s32 p4;
    u16 h8;
    u8 padA[6];
} Ctl_20;

typedef struct Item_10 {
    u8 pad0[4];
    struct Item_10 *p4;
    struct Item_10 *p8;
    Ctl_20 *pC;
    u16 h10;
    u8 pad12[2];
    s16 h14;
    s16 h16;
    u8 pad18[0x20 - 0x18];
    s32 *p20;
    u8 pad24[4];
    s32 p28;
} Item_10;

typedef struct {
    u32 w0;
    u8 pad4[0x10 - 4];
    s32 w10;
    s32 w14;
    u8 pad18[0x4C - 0x18];
    s32 w4C;
    u8 pad50[4];
    u32 w54;
    s16 h58;
    s16 h5A;
    u8 pad5C[0x3E8 - 0x5C];
    u8 *w3E8;
    u8 pad3EC[0x488 - 0x3EC];
    s16 h488;
    u8 pad48A[0x740 - 0x48A];
    s32 w740;
} Game_30;

typedef struct { u8 pad0[8]; s16 h8[1]; } Trk_13;
typedef struct { Trk_13 *p0; u8 pad4[0x64 - 4]; s16 h64; } TrkSel;
typedef struct { u8 pad[0x38]; u16 h38; } S370;
typedef struct { u8 pad[0x10]; s32 w10; } S29CC_2;

extern Game_30 gGame_66;
extern Item_10 sScreens_3[];
extern Item_10 *sPrevScreen_2;
extern Item_10 *sCurScreen_2;
extern S370 gSpecialTracksItems_2;
extern S29CC_2 *gCurGameState_3;
extern TrkSel *gSeason_19;

void shell_update(Pad16 *pad) {
    u32 buf[4];
    s32 sp20;
    s32 sp24;
    s32 sp28;
    Item_10 *item;
    Ctl_20 *ctl;
    Item_10 *p;
    s32 vol;
    s32 i;
    s32 any;

    mem_fill((u8 *)buf, 0, 16);
    if (!(gFrameCounter & 0xF)) {
        pak_query_controllers();
    }
    if (gContFirst || !gContMask) {
        gGame_66.w0 |= 0x08000000;
    } else {
        gGame_66.w0 &= ~0x08000000;
    }
    if (gContCount < 2) {
        gSpecialTracksItems_2.h38 |= 1;
    } else {
        save_update_unlocks();
    }
    if (gGame_66.w54 == 0) {
        item = sPrevScreen_2;
        if (item == &sScreens_3[5] ||
            (!(item->h10 & 0x200) && (sCurScreen_2 == NULL || (sCurScreen_2->h10 & 0x200)))) {
            if (gGame_66.w54 == 0) {
                vol = gGame_66.h58;
            }
            snd_set_master_volume(vol);
        }
    } else {
        item = sCurScreen_2;
    }
    if (item != NULL && (item->h10 & 0x2000)) {
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
        gGame_66.w740 = 0;
    } else {
        gGame_66.w740++;
    }
    for (i = 0; i < 4; i++) {
        buf[i] = 0;
        if (gGame_66.w54 == 3) {
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
    if (item != NULL) {
        if (gGame_66.w54 == 0) {
            ctl = &item->pC[sPrevScreenCursor];
        } else {
            ctl = item->pC;
            if (item->p20 != NULL) {
                ctl += *item->p20 + item->h14;
            }
        }
        if (gGame_66.w54 != 1) {
            switch (item->h16) {
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
                shell_splash_input(item, ctl, buf, pad);
                break;
            case 9:
            case 20:
                shell_text_entry_input(item, ctl, buf, pad);
                break;
            case 18:
                shell_records_input(item, ctl, buf, pad);
                break;
            case 7:
            case 13:
                shell_race_results_input(item, ctl, buf, pad);
                break;
            case 14:
                shell_season_standings_input(item, ctl, buf, pad);
                break;
            case 8:
                if (buf[0] & 0x50) {
                    shell_goto_screen_2((s32)&sScreens_3[8], 0, 1);
                    snd_play_menu_sound(1);
                } else if ((buf[0] & 0x20) && item->p28 != 0) {
                    shell_goto_screen_2(item->p28, 0, 0);
                    snd_play_menu_sound(2);
                }
                break;
            case 3:
                if (item - sScreens_3 == 8) {
                    item->p28 = (s32)&sScreens_3[14];
                    if (gGame_66.w740 > 0xE10 && gGame_66.w54 == 3) {
                        shell_goto_screen_2((s32)&sScreens_3[14], 0, 0);
                    }
                }
            case 11:
                shell_list_menu_input(item, ctl, buf, pad);
                if (item->h16 == 11) {
                    gGame_66.w10 = item->h14 + gGame_66.w14;
                }
                break;
            case 19:
                shell_controller_settings_input(item, ctl, buf, pad);
                break;
            case 12:
                pak_screen_update(pad);
                if (gPakScreenDone == 1 && gGame_66.w54 == 3) {
                    switch (gGame_66.h488) {
                    case 0:
                        if (item->p28 == 0) {
                            shell_goto_screen_2((s32)&sScreens_3[21], 0, 1);
                        } else {
                            shell_goto_screen_2(item->p28, 0, 0);
                        }
                        break;
                    case 1:
                        if (gPakScreenAborted == 0) {
                            if (replay_get_info(gActiveReplay + 0x6400, &sp20, &sp24, &sp28)) {
                                shell_goto_screen_2(item->p28, 0, 0);
                                gActiveReplay = 0;
                                gGame_66.w0 &= ~0x80000;
                            } else {
                                gGame_66.w0 |= 0x80000;
                                gCurTrack = &gTracks[sp20];
                                gGame_66.w10 = sp20;
                                shell_goto_screen_2((s32)&sScreens_3[9], 1, 1);
                            }
                        } else {
                            gGame_66.w0 &= ~0x80000;
                            gActiveReplay = 0;
                            shell_goto_screen_2(item->p28, 0, 0);
                        }
                        break;
                    case 2:
                        shell_goto_screen_2(item->p28, 0, 0);
                        break;
                    case 4:
                        gGame_66.w0 |= 0x20;
                        shell_goto_screen_2(0, 0, 1);
                        break;
                    case 3:
                        if (gGame_66.w0 & 0x02000000) {
                            gGame_66.w0 &= ~0x20;
                            if (gPakScreenAborted != 0) {
                                shell_goto_screen_2(item->p28, 0, 0);
                            } else {
                                gCurTrack = &gTracks[21];
                                shell_goto_screen_2((s32)&sScreens_3[9], 1, 1);
                                usrtrk_load_layout(gGame_66.w3E8 + 4);
                                gGame_66.w0 &= ~0x80;
                            }
                            gGame_66.w4C = 0;
                            mem_free(gGame_66.w3E8);
                            gGame_66.w3E8 = 0;
                        } else {
                            if (gPakScreenAborted != 0) {
                                mem_free(gGame_66.w3E8);
                                gGame_66.w3E8 = 0;
                            }
                            gGame_66.w0 |= 0x20;
                            shell_goto_screen_2(0, 0, 1);
                        }
                        break;
                    }
                }
                break;
            case 6:
                shell_options_input(item, ctl, buf, pad);
                break;
            case 0:
                race_update(D_800CD090);
                if (gGame_66.w740 > 0xE10 && gGame_66.w54 == 3 &&
                    !(gGame_66.w0 & 0x08000000)) {
                    shell_goto_screen_2((s32)&sScreens_3[8], 1, 1);
                }
                break;
            case 2:
                shell_rider_select_input(item, ctl, buf, pad);
                break;
            case 4:
                shell_season_select_input(item, ctl, buf, pad);
                break;
            }
            if (gGame_66.w54 == 3 && (item->h10 & 0x10)) {
                if (item->h16 == 0 && D_800E3704 != 0) {
                    shell_goto_screen_2(item->pC->p4, 1, 1);
                } else if (buf[0] & 0x50) {
                    if ((item->h16 != 0 || !(gGame_66.w0 & 0x08000000)) && !(ctl->h8 & 1)) {
                        snd_play_menu_sound(1);
                        shell_goto_screen_2(ctl->p4, 1, 1);
                    }
                } else if (buf[0] & 0x20) {
                    if (item->p28 != 0) {
                        snd_play_menu_sound(2);
                        shell_goto_screen_2(item->p28, 0, 0);
                    } else {
                        snd_play_menu_sound(3);
                    }
                }
            }
        }
    }
    switch (gGame_66.w54) {
    case 0:
        if (gGame_66.h58 == 0) {
            gGame_66.h58 = 30;
            gGame_66.w54 = 1;
            gGame_66.h5A = 2;
        } else {
            gGame_66.h58 -= 16;
            if (gGame_66.h58 <= 0) {
                gGame_66.h58 = 0;
            }
        }
        break;
    case 1:
        if (gGame_66.h5A == 0) {
            snd_drain();
            snd_set_master_volume(0xFF);
            if (sPrevScreen_2 != NULL) {
                if (tunes_is_song_playing() && (sCurScreen_2 == NULL || (sCurScreen_2->h10 & 0x200))) {
                    tunes_stop_song();
                }
                for (p = sPrevScreen_2; p != NULL; p = p->p8) {
                    shell_unload_screen(p);
                }
                for (p = sPrevScreen_2->p4; p != NULL; p = p->p4) {
                    shell_unload_screen(p);
                }
            }
            if (sCurScreen_2 != NULL) {
                for (p = sCurScreen_2; p != NULL; p = p->p8) {
                    p->p28 = sCurScreen_2->p28;
                    shell_load_screen(p);
                }
                for (p = sCurScreen_2->p4; p != NULL; p = p->p4) {
                    p->p28 = sCurScreen_2->p28;
                    shell_load_screen(p);
                }
                if (!tunes_is_song_playing() && !(sCurScreen_2->h10 & 0x200)) {
                    tunes_play_song(D_800D11D4);
                    snd_set_master_volume(0xFF);
                }
                if (sCurScreen_2->h16 == 11) {
                    while (sCurScreen_2->p4 != NULL) {
                        sCurScreen_2 = sCurScreen_2->p4;
                    }
                    for (i = gGame_66.w10 - 5; i >= 0; i -= 5) {
                        sCurScreen_2 = sCurScreen_2->p8;
                    }
                    *sCurScreen_2->p20 = gGame_66.w10 % 5;
                }
            } else {
                gCurGameState_3->w10 = 3;
                if (gGame_66.w4C == 9) {
                    gCurTrack = &gTracks[26];
                } else if (gGame_66.w4C == 1) {
                    gCurTrack = &gTracks[gSeason_19->p0->h8[gSeason_19->h64]];
                }
                if (!(gGame_66.w0 & 0x8000)) {
                    if (!(gGame_66.w0 & 0x20)) {
                        if (gGame_66.w4C == 8) {
                            if (gActiveReplay == 0) {
                                gActiveReplay = gReplayBuffer;
                            }
                            gActiveReplaySize = 0x6400;
                        } else if (gGame_66.w4C != 9) {
                            gActiveReplaySize = 0xC800;
                            gActiveReplay = gReplayBuffer;
                        }
                    }
                }
            }
            gGame_66.h5A = -1;
        }
        if (--gGame_66.h58 == 0) {
            if (gGame_66.h5A != -2) {
                gGame_66.h58++;
            } else {
                gGame_66.w54 = 2;
                gGame_66.h58 = 255;
            }
        }
        break;
    case 2:
        gGame_66.h58 -= 16;
        if (gGame_66.h58 <= 0) {
            gGame_66.w54 = 3;
            gGame_66.h58 = 0;
        }
        break;
    }
    sPrevPadButtons = *pad;
}

typedef struct {
    u32 f0;
    u8 pad4[0x1C - 0x4];
    s32 arr[8];
    s32 f3C;
    s32 f40;
    s32 f44;
    u8 pad48[0x4C - 0x48];
    u32 f4C;
    u8 pad50[0x13C - 0x50];
    void *f13C;
    u8 pad140[0x43C - 0x140];
    u8 f43C[6];
    u8 pad442[0x778 - 0x442];
    s32 f778[6];
    s32 f790[6];
} GState_4;

typedef struct {
    u8 n;
    u8 pad[0x15];
    s16 f16[7];
} Trk_14;

typedef struct {
    Trk_14 *trk;
    u8 pad4[0x64 - 4];
    s16 f64;
    u8 pad66[0x6C - 0x66];
    u8 f6C[6];
} Race_8;

typedef struct {
    u8 pad[0x1E];
    s16 f1E;
} Lvl_2;

extern GState_4 gGame_67;
extern Lvl_2 *gCurTrack_39;
extern Race_8 *gSeason_20;
extern Trk_14 D_800CA9C8_6[];

void shell_exit(void)
{
    s32 i;
    s32 j;
    s32 v;
    s32 k;
    GState_4 *g;

    gGame_67.f0 |= 0x4000000;
    gCheatFlags = sCheatMask;
    switch (gGame_67.f4C) {
    case 8:
    case 9:
        gCheatFlags = sCheatMask & ~0x210;
        break;
    }
    switch (gGame_67.f4C) {
    case 0:
    case 1:
    case 8:
        if (!(gGame_67.f0 & 0x1000000)) {
            gGame_67.f0 &= ~0x4000000;
        }
        break;
    }
    if (gCurTrack_39 != &D_800CBF30 && gUserTrackLayout != 0) {
        mem_free(gUserTrackLayout);
        gUserTrackLayout = 0;
    }
    if (gGame_67.f4C == 1 && gSeason_20 != 0 && gSeason_20->trk - D_800CA9C8_6 == 4) {
        gGame_67.arr[1] = gSeason_20->f64;
        if (gGame_67.arr[1] == gGame_67.arr[0]) {
            gGame_67.arr[1] = 5;
        }
    } else if (gGame_67.f4C != 8) {
        v = 0;
        for (i = gGame_67.f3C; i < 8; i++) {
            for (j = 0; j < gGame_67.f3C; j++) {
                if (v == gGame_67.arr[j]) {
                    v++;
                    v %= 8;
                }
            }
            gGame_67.arr[i] = v;
            v++;
            v %= 8;
        }
    }
    if (gGame_67.f4C != 7 && gGame_67.f4C != 2 && gCreditsMode == 0 && !(gGame_67.f0 & 0x40000)) {
        switch (gGame_67.f3C) {
        default:
            D_800EFDB4 = 0;
            break;
        case 1:
            gGame_67.f44 = 5;
            break;
        case 2:
            gGame_67.f44 = gGame_67.f3C;
            break;
        }
    } else {
        D_800EFDB4 = 0;
    }
    if (!(gGame_67.f0 & 0x8000)) {
        g = &gGame_67;
        g->f40 = (g->f4C != 1 || gSeason_20 == 0 || gSeason_20->trk - D_800CA9C8_6 != 4) ?
            ((g->f4C == 1 && gSeason_20 != 0 && gSeason_20->trk - D_800CA9C8_6 == 5) ?
             gSeason_20->trk->f16[gSeason_20->f64] : gCurTrack_39->f1E) : 1;
    }
    mem_free(gGame_67.f13C);
    prim_exit();
    res_close_set(gGuiRes);
    gGuiRes = 0;
    osViSetSpecialFeatures(0xAA);
    gGame_67.f0 &= ~1;
    if (gGame_67.f0 & 0x40000) {
        gGame_67.f3C = gEventNumRiders;
        if (gGame_67.f3C >= 4) {
            gGame_67.f3C = 3;
        }
        for (k = 0; k < 6; k++) {
            gGame_67.f778[k] = gGame_67.arr[k];
            gGame_67.f790[k] = gGame_67.f43C[k];
        }
        for (k = 0; k < gGame_67.f3C; k++) {
            gGame_67.arr[k] = gGame_67.f778[gSeason_20->f6C[k]];
            gGame_67.f43C[k] = gGame_67.f790[gSeason_20->f6C[k]];
        }
    }
    if (res_open_set(D_800D94A4, &D_800E7728) != 0) {
        fatal_halt(1);
    }
    res_close_set(D_800E7728);
    mem_free(D_800F01BC);
    gRaceExitCode = 6;
    font_unload_set();
}

#define LOAD(id) res_unload(&gGuiRes_2->tab[id])

typedef struct {
    s32 f0;
    u32 *tab;
} AssetMgr;

typedef struct {
    u8 pad0[0x34];
    V7 f34;
    u32 f50;
    u8 pad54[0xA0 - 0x54];
} Bike_41;

typedef struct {
    u32 f0;
    u8 pad4[0x1C - 0x4];
    s32 f1C[8];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    u32 f4C;
    u8 pad50[0x74 - 0x50];
    void *f74[0x30];
    u8 pad134[0x2E8 - 0x134];
    s32 f2E8;
    u8 pad2EC[0x3B8 - 0x2EC];
    u32 f3B8;
    Bike_41 *f3BC;
    s32 f3C0;
    s32 f3C4;
    u8 pad3C8[0x43C - 0x3C8];
    u8 f43C[6];
    u8 pad442[0x44A - 0x442];
    s16 f44A;
    s16 *f44C;
    u8 pad450[0x778 - 0x450];
    s32 f778[6];
    s32 f790[6];
    u8 pad7A8[0x7AA - 0x7A8];
    u8 f7AA[6];
} GState_5;

struct Scr {
    s32 f0;
    s32 f4;
    u8 pad8[0x10 - 0x8];
    u16 f10;
    s16 f12;
    s16 f14;
    s16 f16;
    s16 f18;
    u8 pad1A[0x1C - 0x1A];
    void *f1C;
};

extern GState_5 gGame_68;
extern AssetMgr *gGuiRes_2;
extern AssetMgr *D_800E7728_5;
extern Bike_41 *gCurTrack_40;
extern Bike_41 *D_800F0634;
extern Bike_41 gTracks_15[];

void shell_unload_screen(Scr *s)
{
    s32 i;

    if (s == NULL) {
        return;
    }
    gGame_68.f0 &= ~0x200;
    LOAD(D_800D1398);
    LOAD(D_800D1410);
    if ((s->f10 & 0x20) && s->f4 == 0) {
        for (i = 0; i < 0x30; i++) {
            func_800C204C(gGame_68.f74[i]);
            mem_free(gGame_68.f74[i]);
        }
    }
    if (s->f10 & 0x80) {
        LOAD(D_800D1450);
        LOAD(D_800D14C8);
    }
    if (s->f1C != NULL) {
        if (s->f16 == 4) {
            for (i = 0; i < 7; i++) {
                LOAD(*((s32 **)s->f1C)[i]);
            }
        } else {
            LOAD(*(s32 *)s->f1C);
        }
    }
    switch (s->f16) {
    case 23:
        gGame_68.f0 &= ~0x10000000;
        break;
    case 22:
        pak_probe_all();
        LOAD(D_800D13B4);
        break;
    case 0:
        i = 0;
        LOAD(D_800D1490);
        LOAD(D_800D14A4);
        race_exit();
        gGame_68.f3C = 1;
        D_800F0634 = gCurTrack_40;
        for (; i < 6; i++) {
            gGame_68.f1C[i] = gGame_68.f778[i];
            gGame_68.f43C[i] = gGame_68.f790[i];
        }
        break;
    case 2:
        race_exit();
        if (!(gGame_68.f0 & 0x400)) {
            gSplitVertical = 0;
        }
        gGame_68.f0 &= ~0x3C10;
        switch (gGame_68.f3B8) {
        case 8:
            if (gGhostSlot == 0) {
                gGame_68.f1C[1] = gGame_68.f1C[0];
            }
        case 0:
        case 1:
            gGame_68.f0 |= 0x80;
            if (gGame_68.f3BC != NULL && (gGame_68.f3BC->f50 & 2)) {
                gGame_68.f0 &= ~0x80;
            }
            break;
        }
        i = 0;
        res_unload(&D_800E7728_5->tab[D_800D09F8]);
        LOAD(D_800D149C);
        LOAD(D_800D1388);
        LOAD(D_800D144C);
        LOAD(D_800D1494);
        LOAD(D_800D1414);
        LOAD(D_800D13A8);
        LOAD(D_800D1388);
        LOAD(D_800D13EC);
        gGame_68.f2E8 = 0;
        gGame_68.f4C = gGame_68.f3B8;
        gCurTrack_40 = gGame_68.f3BC;
        for (; i < 6; i++) {
            if (gGame_68.f3C >= 2) {
                gGame_68.f43C[i] = i;
            } else {
                gGame_68.f43C[i] = (gGame_68.f3C0 + i) % 6;
            }
            gGame_68.f7AA[i] = gGame_68.f3C4;
            gGame_68.f7AA[i] += gGame_68.f1C[i];
            gGame_68.f7AA[i] %= 6;
        }
        break;
    case 19:
        shell_blend_palette_to_gray(0xFF, NULL);
        LOAD(D_800D1470);
        LOAD(D_800D1498);
        LOAD(D_800D139C);
        LOAD(D_800D13A0);
        mem_free(D_800F01B4);
        break;
    case 6:
        save_write(5, 0);
        save_write(1, 0);
        sfx_free_sample(D_800F04E4);
        LOAD(D_800D1470);
        LOAD(D_800D1424);
        LOAD(D_800D1444);
        LOAD(D_800D1428);
        break;
    case 24:
        LOAD(D_800D14A0);
        LOAD(D_800D13BC);
        break;
    case 25:
        LOAD(D_800D13E0);
        LOAD(D_800D142C);
        break;
    case 9:
    case 20:
        if (s->f16 == 9) {
            LOAD(D_800D13CC);
        } else {
            gCurTrack_40->f34 = D_800F05AC;
            gCurTrack_40->f50 &= ~0x180;
            save_write(2, gCurTrack_40 - gTracks_15);
            for (i = 0; i < gGame_68.f44A; i++) {
                if (gGame_68.f44C[i] == gCurTrack_40 - gTracks_15) {
                    break;
                }
            }
            LOAD(*sTrackPicResIds[sTrackPicIndex[i]]);
        }
        mem_free(D_800F01B4);
        LOAD(D_800D1488);
        LOAD(D_800D1380);
        LOAD(D_800D148C);
        LOAD(D_800D13A0);
        break;
    case 7:
    case 13:
    case 14:
        mem_free(D_800F01B4);
        LOAD(D_800D13A0);
        LOAD(D_800D143C);
        LOAD(D_800D13C0);
        LOAD(D_800D1428);
        LOAD(D_800D1424);
        break;
    case 12:
        pak_screen_exit();
        gGame_68.f0 &= ~0x02000000;
        break;
    case 3:
        LOAD(D_800D141C);
        LOAD(D_800D1430);
    case 11:
        LOAD(D_800D149C);
        gGame_68.f2E8 = 0;
        LOAD(D_800D14AC);
        LOAD(D_800D140C);
        LOAD(D_800D13F4);
        LOAD(D_800D1394);
        LOAD(D_800D13C8);
        LOAD(D_800D1418);
        LOAD(D_800D1428);
        LOAD(D_800D1424);
        for (i = 0; i < s->f18; i++) {
            if (s->f16 == 11) {
                LOAD(*sTrackPicResIds[sTrackPicIndex[i + s->f14]]);
            } else {
                LOAD(*sMenuItemPicResIds[i + s->f12]);
            }
        }
        break;
    case 4:
    case 18:
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

typedef struct {
    u8 pad[0x444];
    u16 *buf;
} Ctx_2;

extern Ctx_2 gGame_69;

void shell_build_player_palettes(void)
{
    s32 i;
    s32 v;

    gGame_69.buf = (u16 *)mem_alloc_dbg(0x800, 0, D_800D94B0, 0x174F);
    gGame_69.buf[0] = 0;
    gGame_69.buf[0x100] = 0;
    gGame_69.buf[0x200] = 0;
    gGame_69.buf[0x300] = 0;
    i = 1;
    do {
        gGame_69.buf[i] = ((i >> 3) << 1) | 0x18C1;
        v = i >> 3;
        gGame_69.buf[i + 0x100] = (v << 11) | 0xC7;
        gGame_69.buf[i + 0x200] = (v << 11) | (v << 6) | 7;
        gGame_69.buf[i + 0x300] = (v << 6) | 0x1807;
        i++;
    } while (i < 0x100);
}

typedef struct {
    u8 *p;
    u8 pad0[0x60];
    s16 cnt;
    u8 pad1[6];
    u8 flag;
} Rec_46;

typedef struct {
    u32 flags;
    u8 pad0[8];
    u32 f0c;
    u8 pad1[0x400];
    s32 mode;
    u8 pad2[0x340];
    u32 f754;
} Gs;

extern Gs gGame_70;
extern Rec_46 *gSeason_21;

void shell_update_season_progress(void)
{
    u8 *q;
    s32 old;
    s32 idx;
    u32 fl;
    Gs *g;
    Gs *g2;
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
    Rec_46 *r;
    u8 *p0;
    u8 *cb;
    u8 *pb2;

    fl = gGame_70.flags;
    g = &gGame_70;
    if ((fl & 0x28000) == 0 && gRaceExitCode != 3) {
        v = fl | 0x20000;
        gGame_70.flags = v;
        mode = g->mode;
        switch (mode) {
        case 1: m = 3; break;
        case 2: m = 7; break;
        case 3: m = 0x1f; break;
        }
        save_update_unlocks();
        r = gSeason_21;
        p0 = r->p;
        cb = &D_800CA9C8_7;
        idx = ((p0 - cb) * 0x38E38E39) >> 2;
        if (idx < 5) if (idx >= 0) {
            pb = r->p;
            if ((s32)*pb <= (s32)r->cnt + 1) {
                if (!(idx == 4 && D_800F05F8 != 0) && r->flag == 0) {
                    g2 = &gGame_70;
                    q = &gSaveData_12;
                    lv = q[0x2C0];
                    md = g2->mode;
                    if (md == lv && ((old = q[0x2BF]) >> idx & 1) == 0) {
                        v = old | (1 << idx);
                        q[0x2BF] = v;
                        if ((((u8)v) & m) == m) {
                            if (md != 3) {
                                t8 = q[0x2C0];
                                gGame_70.f0c = 0;
                                do {
                                q[0x2BF] = 0;
                                q[0x2C0] = t8 + 1;
                                } while (0);
                                gGame_70.flags |= 0x400000;
                            } else {
                                gGame_70.flags |= 0x800000;
                            }
                        } else {
                            gGame_70.flags |= 0x100000;
                        }
                    }
                }
            }
            idx2 = ((gSeason_21->p - &D_800CA9C8_7) * 0x38E38E39) >> 2;
            if (idx2 != 4 || D_800F05F8 == 0) {
                pb2 = gSeason_21->p;
                e = pb2 + (gGame_70.mode - 1);
                if (gSeason_21->cnt == e[1]) {
                    nv = gSeason_21->cnt + 1;
                    if (nv < *pb2) {
                        e[1] = nv;
                    } else if (gSeason_21->flag == 0) {
                        e[1] = nv;
                    }
                }
            }
            save_write(1, 0);
        }
        f = save_update_unlocks();
        gGame_70.f754 = f;
        if (f != 0) {
            gGame_70.flags |= 0x200000;
        }
    }
}

#define LD(x) res_load(gGuiRes, x)

typedef struct {
    u32 w0;
    u8 pad4[0xC - 0x4];
    s32 wC;
    s32 w10;
    u8 pad14[0x1C - 0x14];
    s32 w1C[6];
    u8 pad34[0x3C - 0x34];
    s32 w3C;
    u8 pad40[0x4C - 0x40];
    s32 w4C;
    u8 pad50[0x64 - 0x50];
    s32 w64;
    s32 w68;
    s32 w6C;
    s32 w70;
    s32 w74[48];
    u8 pad134[0x13C - 0x134];
    u16 *p13C;
    u8 pad140[0x17C - 0x140];
    s32 w17C;
    u8 pad180[0x2A8 - 0x180];
    Pair s2A8[4];
    s8 b2C8[4];
    s8 b2CC[4];
    u8 pad2D0[0x2E0 - 0x2D0];
    s16 h2E0[4];
    s32 w2E8;
    u8 pad2EC[0x2F4 - 0x2EC];
    s32 w2F4[24];
    u8 pad354[0x36C - 0x354];
    s32 w36C[7];
    s32 w388;
    s32 w38C;
    s32 w390;
    u8 pad394[0x398 - 0x394];
    s32 w398;
    s32 w39C;
    s32 w3A0;
    s32 w3A4;
    s32 w3A8;
    s32 w3AC;
    s32 w3B0;
    u8 pad3B4[0x3B8 - 0x3B4];
    s32 w3B8;
    s32 w3BC;
    u8 pad3C0[0x3C8 - 0x3C0];
    s32 w3C8;
    u8 pad3CC[0x3DC - 0x3CC];
    s32 w3DC;
    u8 pad3E0[0x3E8 - 0x3E0];
    void *p3E8;
    s32 w3EC;
    u8 pad3F0[0x40C - 0x3F0];
    u8 b40C;
    u8 b40D;
    u8 pad40E[0x410 - 0x40E];
    s32 w410;
    s32 w414;
    u8 pad418[0x438 - 0x418];
    s32 w438;
    u8 b43C[6];
    u8 pad442[0x448 - 0x442];
    u16 h448;
    s16 h44A;
    s16 *p44C;
    u8 pad450[0x454 - 0x450];
    s16 h454;
    s16 h456;
    s16 h458;
    u8 pad45A[0x45C - 0x45A];
    s32 w45C;
    s32 w460;
    s16 h464;
    s8 name[16];
    s16 h476;
    s32 w478;
    u8 pad47C[0x480 - 0x47C];
    s32 w480;
    s32 w484;
    s16 h488;
    u8 pad48A[0x730 - 0x48A];
    s32 w730;
    u8 rgb734[4][3];
    s32 w740;
    s32 w744;
    s32 w748;
    s32 w74C;
    s32 w750;
    u8 pad754[0x758 - 0x754];
    s8 b758[4];
    u8 pad75C[0x770 - 0x75C];
    s32 w770;
    s32 w774;
    s32 w778[6];
    s32 w790[6];
    u8 b7A8;
    u8 pad7A9;
    s8 b7AA[6];
} Game_31;

typedef struct { Ent24 *p0; } RecPtr;
typedef struct { u8 pad[8]; u16 h8; u8 padA[6]; } Rec10;
typedef struct { u8 pad[0x14]; u8 b14; u8 b15; u8 b16; } Slot_12;

typedef struct {
    s32 w0;
    u8 pad4[0x1C - 0x4];
    s32 w1C;
    u8 pad20[0x24 - 0x20];
    s32 w24;
    void *w28;
} Snd_4;

extern Game_31 gGame_71;
extern Rec10 gSpecialTracksItems_3[];
extern RecPtr *gSeason_22;
extern Snd_4 gPakScreen;

void shell_load_screen(Item_11 *p) {
    s32 i;
    s32 j;
    s32 c;

    snd_drain();
    if (p == NULL) {
        return;
    }
    gGame_71.w740 = 0;
    gGame_71.w480 = LD(D_800D1410);
    gGame_71.w484 = LD(D_800D1398);
    if ((p->h10 & 0x20) && p->w4 == 0) {
        for (i = 0; i < 48; i++) {
            gGame_71.w74[i] = (s32)font_text_create(0, 0x40, 4, 2, 0x20);
        }
    }
    if (p->h10 & 0x80) {
        gGame_71.w390 = LD(D_800D1450);
        gGame_71.w3DC = LD(D_800D14C8);
    }
    if (p->p1C != NULL) {
        if (p->h16 == 4) {
            for (i = 0; i < 7; i++) {
                gGame_71.w36C[i] = LD(*p->p1C[i]);
            }
        } else {
            p->w24 = LD((s32)*p->p1C);
        }
    } else {
        p->w24 = 0;
    }
    osViSetSpecialFeatures((p->h10 & 4) ? 0x6A : 0xAA);

    switch (p->h16) {
    case 23:
        gGame_71.w0 |= 0x10000000;
    case 21:
        gGame_71.w750 = 0xB4;
        break;
    case 24:
        gGame_71.w390 = LD(D_800D14A0);
        gGame_71.w3DC = LD(D_800D13BC);
        gGame_71.w750 = 0x4B0;
        break;
    case 25:
        gGame_71.w390 = LD(D_800D13E0);
        gGame_71.w3DC = LD(D_800D142C);
        gGame_71.w750 = 0x258;
        break;
    case 22:
        gGame_71.w750 = 0x12C;
        gGame_71.w3A0 = LD(D_800D13B4);
        break;
    case 0:
        for (i = 0; i < 6; i++) {
            gGame_71.w778[i] = gGame_71.w1C[i];
            gGame_71.w790[i] = gGame_71.b43C[i];
        }
        gGame_71.w1C[0] = 4;
        gGame_71.w1C[2] = 1;
        gGame_71.w1C[3] = 2;
        gGame_71.w1C[4] = 3;
        gGame_71.w1C[1] = 0;
        gGame_71.w1C[5] = 5;
        for (i = 0; i < 6; i++) {
            gGame_71.b43C[i] = i;
        }
        gGame_71.w0 &= ~0x80;
        gGame_71.w38C = LD(D_800D1490);
        gGame_71.w390 = LD(D_800D14A4);
        gCurTrack_2 = gTracks;
        gGame_71.w4C = 0;
        gGame_71.w3C = 1;
        race_set_grid_order(0, 6);
        race_init(0);
        break;
    case 6:
        gGame_71.w770 = 0;
        gGame_71.w2E8 = 0;
        softmix_set_decompress_buf(gZBuffer, 0x25800);
        gGame_71.w774 = sfx_load_sample_id(D_800D1204);
        softmix_set_decompress_buf(0, 0);
        gGame_71.w390 = LD(D_800D1444);
        gGame_71.w3A4 = LD(D_800D1428);
        gGame_71.w3AC = LD(D_800D1424);
        gGame_71.w3A0 = LD(D_800D1470);
        if (gCreditsMode != 0) {
            gGame_71.h448 = 6;
            gCreditsMode = 0;
        }
        gGame_71.h448 |= 0x100;
        break;
    case 19:
        for (i = 1; i < 0x100; i++) {
            c = (i / 3) >> 3;
            gGame_71.p13C[i] = (c << 1) | (c << 6) | (c << 11) | 1;
        }
        gGame_71.h476 = 0;
        gGame_71.w3A0 = LD(D_800D1470);
        gGame_71.w45C = LD(D_800D139C);
        gGame_71.w390 = LD(D_800D1498);
        gGame_71.w39C = LD(D_800D13A0);
        shell_build_player_palettes();
        break;
    case 9:
    case 20:
        gGame_71.w2E8 = 0;
        i = 0;
        if (p->h16 == 9) {
            gGame_71.w2F4[0] = LD(D_800D13CC);
            gGame_71.name[0] = 0x41;
            for (i = 1; i < 16; i++) {
                gGame_71.name[i] = 0;
            }
            gGame_71.h464 = 0;
            gGame_71.h456 = 0;
            gGame_71.h458 = 0;
            gGame_71.h454 = 0;
        } else {
            gGame_71.w460 = -1;
            gGame_71.b7A8 = 0xFF;
            for (i = 0; i < 4; i++) {
                gGame_71.b758[i] = -1;
                if (((Slot_12 *)(gEvent_2 + i * 3))->b14 == 0xFF && ((Slot_12 *)(gEvent_2 + i * 3))->b15 == 0xFF) {
                    gGame_71.b758[i] = ((Slot_12 *)(gEvent_2 + i * 3))->b16;
                    if (gGame_71.w460 == -1) {
                        gGame_71.w460 = i;
                    }
                }
            }
            if (gGame_71.rgb734[gGame_71.b758[gGame_71.w460]][0] != 0) {
                __builtin_memcpy(gGame_71.name, gGame_71.rgb734[gGame_71.b758[gGame_71.w460]], 3);
                gGame_71.name[3] = 0;
                gGame_71.h464 = strlen(gGame_71.name);
                gGame_71.h456 = 0x31F;
                gGame_71.h458 = 0x31F;
                gGame_71.h454 = 0x27;
            } else {
                gGame_71.name[0] = 0x41;
                gGame_71.name[1] = 0;
                gGame_71.name[2] = 0;
                gGame_71.name[3] = 0;
                gGame_71.h464 = 0;
                gGame_71.h456 = 0;
                gGame_71.h458 = 0;
                gGame_71.h454 = 0;
            }
            for (i = 0; i < gGame_71.h44A; i++) {
                if (gGame_71.p44C[i] == gCurTrack_2 - gTracks) {
                    break;
                }
            }
            gGame_71.w2F4[0] = LD(*sTrackPicResIds[sTrackPicIndex[i]]);
        }
        gGame_71.w390 = LD(D_800D148C);
        gGame_71.w3A0 = LD(D_800D1380);
        gGame_71.w478 = LD(D_800D1488);
        gGame_71.w3EC = 0;
        gGame_71.w39C = LD(D_800D13A0);
        shell_build_player_palettes();
        break;
    case 13:
    case 14:
        if (p->h16 == 14 && gGame_71.w4C == 1 && gSeason_22 != NULL
            && gSeason_22->p0 - D_800CA9C8 == 4) {
            shell_update_season_progress();
        }
        if (gGame_71.w0 & 0x8000) {
            gGame_71.w2E8 = (p->h16 == 13) ? 3 : 4;
            gGame_71.w0 = (gGame_71.w0 | 0x20000) & ~0x8000;
        } else {
            gGame_71.w2E8 = 0;
        }
        gGame_71.w3EC = 0;
        gGame_71.w39C = LD(D_800D13A0);
        shell_build_player_palettes();
        gGame_71.w390 = LD(D_800D143C);
        gGame_71.w3DC = LD(D_800D14C8);
        gGame_71.w414 = LD(D_800D13C0);
        gGame_71.w3A4 = LD(D_800D1428);
        gGame_71.w3AC = LD(D_800D1424);
        break;
    case 7:
        shell_update_season_progress();
        gGame_71.w2E8 = 0;
        gGame_71.w3EC = 0;
        shell_build_player_palettes();
        gGame_71.w39C = LD(D_800D13A0);
        gGame_71.w390 = LD(D_800D143C);
        gGame_71.w3A4 = LD(D_800D1428);
        gGame_71.w3AC = LD(D_800D1424);
        gGame_71.w414 = LD(D_800D13C0);
        gGame_71.w3DC = LD(D_800D14C8);
        break;
    case 12:
        D_800F4A50 = 0;
        switch (gGame_71.h488) {
        case 0:
            gPakScreen.w0 = 0;
            gPakScreen.w24 = 0;
            gPakScreen.w1C = 0;
            break;
        case 1:
            if (gActiveReplay == 0) {
                gActiveReplay = gReplayBuffer;
            }
            gPakScreen.w1C = 100;
            gPakScreen.w0 = 1;
            gPakScreen.w28 = D_800600D4;
            gPakScreen.w24 = gActiveReplay + 0x6400;
            break;
        case 2:
            gPakScreen.w1C = 40;
            gPakScreen.w0 = 2;
            gPakScreen.w24 = gActiveReplay;
            break;
        case 3:
            if (gGame_71.p3E8 != NULL) {
                mem_free(gGame_71.p3E8);
            }
            gGame_71.p3E8 = mem_alloc_dbg(0x400, 0, D_800D94B0, 0x18E6);
            gPakScreen.w24 = (s32)gGame_71.p3E8;
            gPakScreen.w1C = 4;
            gPakScreen.w0 = 1;
            gPakScreen.w28 = D_800881D4;
            break;
        case 4:
            gPakScreen.w1C = 4;
            gPakScreen.w0 = 2;
            gPakScreen.w24 = (s32)gGame_71.p3E8;
            break;
        }
        D_800F4A54 = D_800732D0;
        pak_screen_init();
        break;
    case 3:
        gSplitVertical = 0;
        if (p - sScreens_4 == 7) {
            gGame_71.w4C = 0;
            while (gSpecialTracksItems_3[*p->p20].h8 & 1) {
                (*p->p20)--;
            }
        }
        gGame_71.w0 &= 0xFDFFFFFF;
        gGame_71.w744 = LD(D_800D141C);
        gGame_71.w748 = LD(D_800D1430);
        gGame_71.w2E8 = 0;
        if (p - sScreens_4 == 8) {
            gGame_71.w3C = 1;
        }
        if (p - sScreens_4 == 10) {
            gGame_71.w10 = 0;
        }
    case 11:
        gGame_71.w74C = LD(D_800D149C);
        gGame_71.w17C = 0;
        gGame_71.w3EC = 0;
        gGame_71.w438 = LD(D_800D14AC);
        gGame_71.w388 = LD(D_800D140C);
        gGame_71.w3A0 = LD(D_800D1418);
        gGame_71.w398 = LD(D_800D13F4);
        gGame_71.w39C = LD(D_800D1394);
        gGame_71.w3A8 = LD(D_800D13C8);
        gGame_71.w3A4 = LD(D_800D1428);
        gGame_71.w3AC = LD(D_800D1424);
        for (i = 0; i < p->h18; i++) {
            if (p->h16 == 11) {
                gGame_71.w2F4[i + p->h14] = LD(*sTrackPicResIds[sTrackPicIndex[i + p->h14]]);
            } else {
                gGame_71.w2F4[i + p->h14] = LD(*sMenuItemPicResIds[i + p->h12]);
            }
        }
        break;
    case 18:
        gGame_71.w2E8 = 13;
        gGame_71.w750 = 6;
    case 4:
        gGame_71.w70 = 0;
        switch (gGame_71.w410) {
        case 1:
            if (gGame_71.wC >= 2) {
                gGame_71.wC = 0;
            }
            break;
        case 2:
            if (gGame_71.wC >= 3) {
                gGame_71.wC = 0;
            }
            break;
        }
        if (gGame_71.w410 == 0) {
            gGame_71.wC = 6;
        } else if (gGame_71.wC == 6) {
            gGame_71.wC = 0;
        }
        gGame_71.w6C = 0;
        gGame_71.w17C = 0;
        gGame_71.w3EC = 0;
        gGame_71.w438 = LD(D_800D14AC);
        gGame_71.w3A4 = LD(D_800D1428);
        gGame_71.w3AC = LD(D_800D1424);
        gGame_71.w3A0 = LD(D_800D1418);
        gGame_71.w398 = LD(D_800D13F4);
        gGame_71.w39C = LD(D_800D1394);
        gGame_71.w3A8 = LD(D_800D13C8);
        gGame_71.w3B0 = LD(D_800D1448);
        for (i = 0; i < 24; i++) {
            gGame_71.w2F4[i] = LD(*sTrackPicResIds[i]);
        }
        gGame_71.w64 = 0;
        gGame_71.w68 = 0;
        break;
    case 2:
        gGame_71.b40C = 0;
        gGame_71.b40D = 0;
        for (i = 3; i >= 0; i--) {
            gGame_71.h2E0[i] = 0;
        }
        for (i = 0; i < 6; i++) {
            gGame_71.b7AA[i] = i;
        }
        gGame_71.w2E8 = 0;
        if (gGame_71.w3C == 1) {
            gGame_71.w3C8 = LD(D_800D1494);
        }
        j = 0;
        gGame_71.b2C8[1] = -1;
        gGame_71.b2C8[2] = -1;
        gGame_71.b2C8[3] = -1;
        gGame_71.b2CC[0] = -1;
        gGame_71.w3B8 = gGame_71.w4C;
        gGame_71.w3BC = (s32)gCurTrack_2;
        gGame_71.w0 &= ~0x3C90;
        for (i = 0; i < 4; i++) {
            if ((gContMask >> i) & 1) {
                gGame_71.b2C8[j] = i;
                gGame_71.b2CC[i] = j;
                j++;
            }
        }
        for (i = 0; i < 4; i++) {
            gGame_71.s2A8[i].a = (((gFrameCounter & 0x1FF) << 7) + ((-(i & 1) + (i & 2)) << 14)) & 0xFFFF;
            gGame_71.s2A8[i].b = 0;
        }
        gCurTrack_2 = sTrackLongIsland;
        gGame_71.w4C = 0;
        gGame_71.w0 |= 0x10;
        gGame_71.w390 = LD(D_800D1388);
        gGame_71.w3DC = LD(D_800D144C);
        gGame_71.w74C = LD(D_800D149C);
        race_set_grid_order(0, 6);
        race_init(0);
        gGame_71.w730 = res_load(D_800E7728, D_800D09F8);
        break;
    }
}

typedef struct Game_32 {
    u8 pad0[0x54];
    s32 f54;
    u16 f58;
    s16 f5A;
} Game_32;

extern Game_32 gGame_72;

void shell_goto_screen(Item_12 *p1, s32 p2, s32 p3, s32 arg3)
{
    Item_12 *p;

    if (gGame_72.f54 != 0) {
        if (sCurScreen_3 == NULL) {
            if (p1 != NULL) {
                p = p1;
                do {
                    if (gRaceExitCode != 3 && gCreditsMode == 0) {
                        p->parent = NULL;
                    }
                    shell_load_screen(p);
                    p = p->next;
                } while (p != NULL);
                for (p = p1->child; p != NULL; p = p->child) {
                    if (gRaceExitCode != 3 && gCreditsMode == 0) {
                        p->parent = NULL;
                    }
                    shell_load_screen(p);
                }
                if (tunes_is_song_playing() == 0 && (p1->flags & 0x200) == 0) {
                    tunes_play_song(D_800D11D4);
                }
            }
            gGame_72.f54 = 1;
            gGame_72.f58 = 1;
            gGame_72.f5A = -1;
        } else {
            if (p3 != 0) {
                MenuItem *items = sCurScreen_3->items;
                s32 *sel = sCurScreen_3->sel;
                if ((sel != NULL ? &items[*sel] : items)->func != NULL) {
                    (sel != NULL ? &items[*sel] : items)->func(1);
                }
            }
            gGame_72.f54 = 0;
            gGame_72.f58 = 0xFF;
            sPrevScreenCursor = sCurScreen_3->sel != NULL ? *sCurScreen_3->sel : 0;
        }
        if (sCurScreen_3 != NULL) {
            sPrevScreen = sCurScreen_3;
        }
        if (p1 != NULL && p2 != 0) {
            p1->parent = sCurScreen_3;
        }
        D_800F016C = sPrevScreen;
        sCurScreen_3 = p1;
    }
}

typedef struct { s32 f0; s32 pad[14]; s32 mode; s32 pad2[13]; s32 *slot[64]; } G_3;
typedef struct { s32 f0; s32 f4; s32 pad[16]; s32 a48[4]; } Tb;

extern G_3 gGame_73;
extern Tb sRiderBios[];

void shell_draw_rider_stats(s32 x, s32 y, s32 p3, s32 p4)
{
    G_3 *g = &gGame_73;
    s32 mode = g->mode;
    s32 yy, v, i, j, k, sy, t;
    Rect *q;
    u8 *base;

    if (mode == 1) {
        return;
    }
    yy = y;
    if (gGame_73.f0 & (0x400 << p4)) {
        yy += 600;
    }
    base = gViewRects;
    q = (Rect *)(base + (p4 * 16 + (mode - 1) * 64));
    v = q->y + q->h + 5;
    y = yy;
    if (v < yy) {
        y = v;
    }
    font_text_create(gGame_73.slot[p4 * 9], 0x40, 1, 6, 0x21);
    for (i = 1; i < 9; i++) {
        font_text_create(gGame_73.slot[i + p4 * 9], 0x40, 1, 2, 0x20);
    }
    font_text_add(gGame_73.slot[p4 * 9], (char *)sRiderBios[p3].f4);
    font_draw_text(gGame_73.slot[p4 * 9], x, y - 0x1F);
    i = 0;
    do {
        k = i + 1;
        font_text_add(gGame_73.slot[p4 * 9 + k], (char *)sRiderStatLabels[i]);
        t = i * 14 - 11;
        sy = yy + t;
        font_draw_text(gGame_73.slot[p4 * 9 + k], x - 0x46, sy);
        j = i + 5;
        font_text_add(gGame_73.slot[p4 * 9 + j], (char *)sRiderStatRatings[sRiderBios[p3].a48[i]]);
        font_draw_text(gGame_73.slot[p4 * 9 + j], x + 0x12, sy);
        i = k;
    } while (i < 4);
}

typedef struct {
    u8 pad0[0x4C];
    s32 mode;
    u8 pad50[0x74 - 0x50];
    s32 *f74[3];
    u8 pad80[0x17C - 0x80];
    s32 f17C;
    u8 pad180[0x3A4 - 0x180];
    struct { u8 b[8]; } *f3A4;
    u8 pad3A8[4];
    u8 *f3AC;
    u8 pad3B0[0x3DC - 0x3B0];
    s32 *f3DC;
    u8 pad3E0[0x404 - 0x3E0];
    u8 *f404;
} Glob_5;

extern Glob_5 gGame_74;

Gfx *shell_draw_results_menu(Gfx *gdl, s32 p2, s32 sel, char **strs)
{
    s32 flag;
    s32 i;
    u8 *img;
    s32 x, y, a, c;

    flag = 0;
    if (gGame_74.mode == 8 || gReplayWritePtr - gReplayBuf >= gReplayBufSize) {
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
    gDPLoadTLUT_pal256(gdl++, gGame_74.f3DC[1]);
    i = 0;
    x = -25;
    y = -13;
    c = 0;
    for (; i < 3; i++) {
        gdl = render_draw_image(0xC0 - c, i * 25 + 0x52, (u8 *)gGame_74.f3DC + 8, gdl, p2, 1, 1, 3, 0);
        font_text_create(gGame_74.f74[i], 0x40, 1, (sel == i) ? 4 : 2, (i == 1 && flag) ? 0x20000021 : 0x21);
        font_text_add(gGame_74.f74[i], strs[i]);
        font_draw_text(gGame_74.f74[i], gScreenWidth / 2 - y, gScreenHeight / 2 + x);
        x += 25;
        y += 7;
        c += 7;
    }
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    img = gGame_74.f404 + 0x10;
    gdl = render_draw_image((gScreenWidth - img[0] * 8) / 2, (gScreenHeight - img[1]) / 2, img, gdl, p2, 1, 1, 6, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, gGame_74.f3AC + 4);
    return render_draw_image(0xA1 - (sel - 1) * 7, sel * 25 + 0x56,
                         &gGame_74.f3A4[gGame_74.f17C / 2 % 30 + 1], gdl, p2, 1, 1, 3, 0);
}

typedef struct {
    u8 b[8];
} Ent8_2;

typedef struct {
    u8 pad0[0x3C];
    s32 f3C;
    u8 pad40[0x4C - 0x40];
    s32 f4C;
    u8 pad50[0x74 - 0x50];
    s32 *f74[4];
    u8 pad84[0x17C - 0x84];
    s32 f17C;
    u8 pad180[0x390 - 0x180];
    u8 *f390;
    u8 pad394[0x3A4 - 0x394];
    Ent8_2 *f3A4;
    u8 pad3A8[4];
    u8 *f3AC;
    u8 pad3B0[0x3DC - 0x3B0];
    s32 *f3DC;
    u8 pad3E0[0x404 - 0x3E0];
    u8 *f404;
    u8 pad408[0x410 - 0x408];
    s32 f410;
    u8 pad414[0x74C - 0x414];
    s32 *f74C;
    u8 pad750[0x75C - 0x750];
    s32 f75C;
} Glob_6;

extern Glob_6 gGame_75;

Gfx *shell_draw_players_difficulty_popup(Gfx *gdl, s32 p2, s32 p3)
{
    s32 dx, dy, dz;
    s32 i;
    u8 *img;
    char buf[32];
    char num[2];

    if (p3 != 1 || gContCount < 2 || gGame_75.f4C != 0) {
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
    if (p3 == 1 && gContCount >= 2 && gGame_75.f4C == 0) {
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
    gDPLoadTLUT_pal256(gdl++, gGame_75.f3DC[1]);
    for (i = (p3 == -1); i < 4; i++) {
        if (p3 == 1 && gContCount >= 2 && gGame_75.f4C == 0) {
            if (i == p3) {
                img = (u8 *)gGame_75.f3DC + 0x10;
            } else {
                img = (u8 *)gGame_75.f3DC + 0x18;
            }
        } else {
            img = (u8 *)gGame_75.f3DC + 8;
        }
        gdl = render_draw_image(0xC0 - i * 7 + dx, i * 25 + 0x46 + dy, img, gdl, p2, 1, 1, 3, 0);
        if (p3 == 1) {
            if (i >= gContCount) {
                font_text_create(gGame_75.f74[i], 0x40, 1, 2, 0x20000021);
                font_text_add(gGame_75.f74[i], D_800D9AAC);
                font_draw_text(gGame_75.f74[i], gScreenWidth / 2 + 17 - i * 7 + dx,
                              gScreenHeight / 2 - 39 + i * 25 + dy);
            } else {
                if (i == p3 && gGame_75.f4C == 0) {
                    font_text_create(gGame_75.f74[i], 0x40, 1, (gGame_75.f3C == 2) ? 4 : 2, 0x21);
                    font_text_add(gGame_75.f74[i], D_800D9AC0);
                    font_draw_text(gGame_75.f74[i], gScreenWidth / 2 + (dx + 10) + 0x50 + dz,
                                  gScreenHeight / 2 + (dy - 14));
                    font_text_create(gGame_75.f74[i], 0x40, 1, (gGame_75.f3C == 2) ? 6 : 2, 0x21);
                    int_to_str(gGame_75.f75C, buf);
                    font_text_add(gGame_75.f74[i], buf);
                    font_draw_text(gGame_75.f74[i], gScreenWidth / 2 + (dx + 10) + 0x7A + dz,
                                  gScreenHeight / 2 + (dy - 14));
                }
                if (((gGame_75.f4C == 2 || gGame_75.f4C == 3) && !(i & 1)) ||
                    (gGame_75.f4C == 7 && i >= 2) || (gGame_75.f4C == 10 && i >= 2)) {
                    font_text_create(gGame_75.f74[i], 0x40, 1, (i == gGame_75.f3C - 1) ? 4 : 2, 0x20000021);
                } else {
                    font_text_create(gGame_75.f74[i], 0x40, 1, (i == gGame_75.f3C - 1) ? 4 : 2, 0x21);
                }
                num[0] = i + '1';
                num[1] = 0;
                font_text_add(gGame_75.f74[i], num);
                font_text_add(gGame_75.f74[i], D_800D9AC4);
                font_draw_text(gGame_75.f74[i], gScreenWidth / 2 + 17 - i * 7 + dx + dz,
                              gScreenHeight / 2 - 39 + i * 25 + dy);
            }
        } else {
            font_text_create(gGame_75.f74[i], 0x40, 1, (i == gGame_75.f410) ? 4 : 2,
                          (i >= D_800F1BC0 + 1) ? 0x20000021 : 0x21);
            font_text_add(gGame_75.f74[i], sDifficultyNames[i]);
            font_draw_text(gGame_75.f74[i], gScreenWidth / 2 + 17 - i * 7 + dx,
                          gScreenHeight / 2 - 39 + i * 25 + dy);
        }
    }
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetPrimColor(gdl++, 0, 0, 0xE6, 0xE6, 0xE6, 0xFF);
    if (p3 == -1) {
        img = gGame_75.f404 + 0x10;
    } else if (p3 == 1 && gContCount >= 2 && gGame_75.f4C == 0) {
        img = gGame_75.f404 + 8;
    } else {
        img = gGame_75.f390 + 0x18;
    }
    gdl = render_draw_image((gScreenWidth - img[0] * 8) / 2, (gScreenHeight - img[1]) / 2, img, gdl, p2, 1, 1, 6, 0);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, gGame_75.f3AC + 4);
    img = (u8 *)&gGame_75.f3A4[gGame_75.f17C / 2 % 30 + 1];
    if (p3 == 1) {
        gdl = render_draw_image(0xA7 - (gGame_75.f3C - 1) * 7 + dx, (gGame_75.f3C - 1) * 25 + 0x49 + dy, img,
                            gdl, p2, 1, 1, 3, 0);
    } else {
        gdl = render_draw_image(0xA7 - gGame_75.f410 * 7 + dx, gGame_75.f410 * 25 + 0x49 + dy, img,
                            gdl, p2, 1, 1, 3, 0);
    }
    if (p3 == 1 && gContCount >= 2 && gGame_75.f4C == 0 && gGame_75.f3C == 2 && (gFrameCounter & 0x20)) {
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        gDPLoadTLUT_pal256(gdl++, gGame_75.f74C[1]);
        gdl = render_draw_image(gScreenWidth / 2 + 10 + dx + 0x60 + dz, gScreenHeight / 2 - 18 + dy, (u8 *)gGame_75.f74C + 0x18,
                            gdl, p2, 1, 1, 3, 0);
        gdl = render_draw_image(gScreenWidth / 2 + 10 + dx + 0x84 + dz, gScreenHeight / 2 - 18 + dy, (u8 *)gGame_75.f74C + 0x20,
                            gdl, p2, 1, 1, 3, 0);
    }
    return font_render(gdl);
}

typedef struct {
    u8 pad0[0x74];
    s32 *f74;
    u8 pad78[0x17C - 0x78];
    s32 f17C;
    u8 pad180[0x3A4 - 0x180];
    struct { u8 b[8]; } *f3A4;
    u8 pad3A8[4];
    u8 *f3AC;
    u8 pad3B0[0x47C - 0x3B0];
    s16 f47C;
    u8 pad47E[2];
    u8 *f480;
    s32 *f484;
} Glob_7;

extern Glob_7 gGame_76;

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
    gDPLoadTLUT_pal256(gdl++, gGame_76.f484[1]);
    gdl = render_draw_image(0xA3, 0x58, (u8 *)gGame_76.f484 + 8, gdl, p2, 1, 1, 3, 0);
    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    img = gGame_76.f480 + 8;
    gdl = render_draw_image(0x100 - img[0] * 4, 0x78 - (img[1] >> 1), img, gdl, p2, 1, 1, 6, 0);
    font_reset_queue();
    font_text_create(gGame_76.f74, 0x40, 1, 2, 0x21);
    font_text_add(gGame_76.f74, str);
    font_draw_text(gGame_76.f74, 0x105, 0x64);
    font_text_create(gGame_76.f74, 0x40, 1, gGame_76.f47C ? 4 : 2, 0x21);
    font_text_add(gGame_76.f74, D_800D9ACC);
    font_draw_text(gGame_76.f74, 0x108, 0x76);
    font_text_create(gGame_76.f74, 0x40, 1, gGame_76.f47C ? 2 : 4, 0x21);
    font_text_add(gGame_76.f74, D_800D9AD0);
    font_draw_text(gGame_76.f74, 0x103, 0x88);
    gdl = font_render(gdl);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPLoadTLUT_pal256(gdl++, gGame_76.f3AC + 4);
    n = gGame_76.f47C;
    return render_draw_image(n * 7 + 0x9D, (t = n * 5, y = 0x82, y - t * 4),
                         &gGame_76.f3A4[gGame_76.f17C / 2 % 30 + 1], gdl, p2, 1, 1, 3, 0);
}

typedef struct {
    s32 pad0[2];
    char *names[20];
} Entry_5; /* 0x58 */

typedef struct {
    u8 pad0[0x1C];
    s32 sel[8];
    s32 flag;
    u8 pad40[0x34];
    s32 *obj[40];
} State_9;

extern State_9 gGame_77;
extern Entry_5 sRiderBios_2[];

void shell_draw_rider_bio(s32 p, s32 q)
{
    s32 i, n, off;

    if (gGame_77.flag != 1) {
        return;
    }
    font_text_create(gGame_77.obj[20], 0x40, 2, 2, 0x20);
    font_text_add(gGame_77.obj[20], D_800D9AD4);
    font_draw_text(gGame_77.obj[20], 0x140, 0x34);
    font_text_add(gGame_77.obj[p * 20 + 17], sRiderBios_2[gGame_77.sel[p]].names[0]);
    font_draw_text(gGame_77.obj[p * 20 + 17], gViewRects_12.x + 0x10E, q + gViewRects_12.y - 0x6F);
    font_text_add(gGame_77.obj[p * 20 + 18], sRiderBios_2[gGame_77.sel[p]].names[1]);
    font_draw_text(gGame_77.obj[p * 20 + 18], gViewRects_12.x + 0x10B, q + gViewRects_12.y - 0x62);
    font_text_add(gGame_77.obj[p * 20 + 19], sRiderBios_2[gGame_77.sel[p]].names[2]);
    font_draw_text(gGame_77.obj[p * 20 + 19], gViewRects_12.x + 0x108, q + gViewRects_12.y - 0x55);
    for (i = 0; i < 4; i++) {
        font_text_add(gGame_77.obj[p * 20 + 6 + i], sRiderBios_2[gGame_77.sel[p]].names[i + 4]);
        font_draw_text(gGame_77.obj[p * 20 + 6 + i], gViewRects_12.x + 60 - i * 3, q + gViewRects_12.y - 0x6F + i * 13);
    }
    n = 4;
    while (sRiderBios_2[gGame_77.sel[p]].names[n + 4] != 0) {
        n++;
    }
    off = -100;
    for (; sRiderBios_2[gGame_77.sel[p]].names[i + 4] != 0; i++) {
        font_text_add(gGame_77.obj[p * 20 + 6 + i], sRiderBios_2[gGame_77.sel[p]].names[i + 4]);
        font_draw_text(gGame_77.obj[p * 20 + 6 + i], gViewRects_12.x + 60 - i * 3, q + gViewRects_12.y + off + i * 13);
    }
}

#define D gGame_78
#define SAVE gSaveData_13
#define R gEvent_40
#define VP gViewRects
#define CUSTOM5 (D.f4C == 1 && gSeason_23 != NULL && gSeason_23->course - D_800CA9C8_8 == 5)
#define CHAMP4 (D.f4C == 1 && gSeason_23 != NULL && gSeason_23->course - D_800CA9C8_8 == 4)
#define UNLOCKED(n) (D.f410 < SAVE.b2C0 || (SAVE.b2C0 == D.f410 && ((n) == 0 || ((SAVE.b2BF >> ((n) - 1)) & 1))))
#define LVLDONE (D.f410 < SAVE.b2C0 || (SAVE.b2C0 == D.f410 && (SAVE.b2BF & 8)))
#define AVAILD(n) ((D.f4C == 8) ? avail8d(n) : shell_is_round_unlocked((n) / 5))
#define AVAIL(n) ((D.f4C == 8) ? avail8(n) : shell_is_round_unlocked((n) / 5))

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
#define TDRAW(spr, x, y, f) { Spr_2 *_t = &(spr)[1]; gDPLoadTLUT_pal256(gdl++, (spr)->data); DRAW((x), (y), _t, (f)); }
#define TDRAWP(spr, x, y, f) { Spr_2 *_t = &(spr)[1]; gDPLoadTLUT_pal256(gdl++, (spr)->data); gDPPipeSync(gdl++); DRAW((x), (y), _t, (f)); }

typedef struct {
    u8 w;
    u8 h;
    u8 pad[2];
    void *data;
} Spr_2;

typedef struct {
    char *name;
    s32 w4;
    u16 flags;
    u8 padA[6];
} Item_13;

typedef struct {
    s16 h0;
    s16 h2;
    u8 pad4[0x14 - 4];
    s16 h14;
} Obj_64;

typedef struct {
    char *title;
    s32 w4;
    Obj_64 *p8;
    Item_13 *items;
    u8 pad10[4];
    s16 h14;
    s16 state;
    s16 count;
    u8 pad1A[6];
    s32 *sel;
} Menu_4;

struct Ctl_21 {
    u8 pad0[8];
    u16 h8;
};

typedef struct {
    u8 n;
    u8 pad1[3];
    u8 won[4];
    s16 trk[7];
    s16 laps[7];
} Course_3; /* 0x24 */

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
    u32 flags;
    u8 pad4[0xC - 0x4];
    s32 fC;
    s32 f10;
    u8 pad14[0x1C - 0x14];
    s32 f1C[8];
    s32 f3C;
    u8 pad40[0x48 - 0x40];
    s16 h48;
    s16 h4A;
    s32 f4C;
    s8 b50[4];
    u32 f54;
    u8 pad58[0x68 - 0x58];
    s32 f68;
    s32 f6C;
    u32 f70;
    s32 *txt[48];
    s16 h134;
    u8 pad136[0x13C - 0x136];
    void *p13C;
    u8 pad140[0x17C - 0x140];
    s32 f17C;
    u8 pad180[0x2E8 - 0x180];
    u32 f2E8;
    u8 pad2EC[0x2F4 - 0x2EC];
    Spr_2 *spr2F4[37];
    Spr_2 *p388;
    Spr_2 *p38C;
    Spr_2 *p390;
    Spr_2 *p394;
    Spr_2 *p398;
    Spr_2 *p39C;
    Spr_2 *p3A0;
    Spr_2 *p3A4;
    Spr_2 *p3A8;
    u8 *p3AC;
    Spr_2 *p3B0;
    u8 pad3B4[0x3C8 - 0x3B4];
    Spr_2 *p3C8;
    u8 pad3CC[0x3DC - 0x3CC];
    Spr_2 *p3DC;
    u8 pad3E0[0x3EC - 0x3E0];
    s32 f3EC;
    u8 pad3F0[0x400 - 0x3F0];
    s32 f400;
    u8 pad404[0x408 - 0x404];
    s16 h408;
    u8 pad40A[2];
    u8 b40C;
    u8 pad40D[3];
    s32 f410;
    Spr_2 *p414;
    u8 pad418[0x438 - 0x418];
    Spr_2 *p438;
    u8 pad43C[0x444 - 0x43C];
    u8 *p444;
    s16 h448;
    s16 h44A;
    s16 *p44C;
    s32 f450;
    s16 h454;
    u8 pad456[0x45C - 0x456];
    Spr_2 *p45C;
    s32 f460;
    s16 h464;
    char s466[0x10];
    s16 h476;
    u8 pad478[0x47C - 0x478];
    s16 h47C;
    u8 pad47E[0x744 - 0x47E];
    Spr_2 *p744;
    Spr_2 *p748;
    Spr_2 *p74C;
    s32 f750;
    u8 pad754[0x758 - 0x754];
    s8 b758[4];
    u8 pad75C[0x7A8 - 0x75C];
    u8 b7A8;
    u8 b7A9;
} Glob_8;

typedef struct {
    u8 pad0[0x2BF];
    u8 b2BF;
    u8 b2C0;
    u8 pad2C1[3];
    u8 b2C4;
    u8 vol[3];
} Save_5;

typedef struct {
    char *name;
    s32 x;
} Cheat;

typedef struct {
    Course_3 *course;
    s16 pts[8];
    u8 pad14[0x6C - 0x14];
    u8 order[8];
} Champ;

typedef struct {
    u8 pad0[0x22];
    s16 h22;
    u8 pad24[0x50 - 0x24];
    u8 b50[8];
} Race_9;

typedef struct {
    char *name;
    char *f4;
    u8 pad8[8];
    char *f10;
    u8 pad14[0x48 - 0x14];
    s32 stats[4];
} Rider_8;

typedef struct {
    u8 pad0[0x14];
    void *f14;
    char *f18;
    u8 pad1C[0x3C - 0x1C];
} BikeDef;

typedef struct {
    u8 pad0[0x668];
    s16 h668;
    u8 pad66A[0x68C - 0x66A];
    s32 f68C;
    u8 pad690[0x7E0 - 0x690];
} Racer_3;

extern Glob_8 gGame_78;
extern Track_17 *gCurTrack_41;
extern Cheat sCheatTable_2[];
extern Save_5 gSaveData_13;
extern Track_17 gTracks_16[];
extern Course_3 D_800CA9C8_8[];
extern BikeDef gRiderAnims_17[];
extern Champ *gSeason_23;
extern Race_9 gEvent_40;
extern Rider_8 sRiderBios_3[];
extern Racer_3 gRiders_91[];

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

static inline s32 objOk(Obj_64 *p)
{
    s32 lvl;
    s32 v;

    if (D.f4C == 8 || D.f4C == 0) {
        lvl = gSaveData_13.b2C0;
    } else {
        lvl = D.f410;
    }
    switch (lvl) {
    case 1:
        v = p->h14 < 10;
        break;
    case 2:
        v = p->h14 < 15;
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

Gfx *shell_draw_screen(Mtx *m, s32 fb, Gfx *gdl, Ctl_21 *pad, s32 e)
{
    char unk[0xE8];
    char buf[0x40];
    s16 wid[4];
    char buf2[0x10];
    char buf3[0x20];
    Menu_4 *s;
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
    Spr_2 *img;

    if (D.f54 == 0) {
        s = sPrevScreen;
    } else {
        s = sCurScreen;
    }
    switch (D.f54) {
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
    switch (s->state) {
    case 22:
        PREAMBLE();
        if (gHasExpansionPak_2 != 0 && (gFrameCounter & 0x20)) {
            Spr_2 *t = &D.p3A0[1];

            gDPLoadTLUT_pal256(gdl++, D.p3A0->data);
            gdl = render_draw_image(0x77, 0x8D, t, gdl, fb, 1, 1, 3, 0);
        }
        break;
    case 12:
        if (D.f54 != 1) {
            pak_screen_draw(m, fb, &gdl);
        }
        break;
    case 3:
    case 11:
        yoff3 = 0;
        if (s->state == 11) {
            yoff3 = 12;
        }
        font_reset_queue();
        PREAMBLE();
        if (s->state == 3) {
            s32 w;
            s32 r;
            Spr_2 *sp;

            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0x32);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            sp = D.p388 + 1;
            w = sp[D.h48].w << 3;
            r = gFrameCounter % (w + 0x200);
            if (r == 0) {
                D.h48 = (s16)(D.h48 + 1) % 10;
            }
            gdl = render_draw_image(r - w, ((0xF0 - (w + 0x200) / 2) >> 1) + r / 2, &D.p388[D.h48 + 1], gdl, fb, 1, 1,
                                6, 0);
        } else if (D.f3EC < 0x20) {
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetRenderMode(gdl++, 0x00404240, 0);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, D.p438->data);
            if (s->w4 != 0) {
                gdl = render_draw_image(0x1C, yoff3 + 0x17, &D.p438[1], gdl, fb, 1, 1, 3, 0);
            }
            if (s->p8 != NULL && objOk(s->p8)) {
                gdl = render_draw_image(0x128, yoff3 + 0x17, &D.p438[2], gdl, fb, 1, 1, 3, 0);
            }
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        TDRAWP(D.p3A0, 0x45, yoff3 + 0x1F, 3);
        if ((s->state == 11 && !AVAILD(s->h14 + *s->sel)) || (s->items[*s->sel + s->h14].flags & 1)) {
            gDPSetPrimColor(gdl++, 0, 0, 0x64, 0x64, 0x64, 0xFF);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            gDPLoadTLUT_pal256(gdl++, D.p13C);
            font_text_create(D.txt[0], 0x40, 4, 2, 0x21);
            font_text_add(D.txt[0], D_800D9C18);
            font_draw_text(D.txt[0], 0x16B, 0x82);
        } else {
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, D.spr2F4[s->h14 + *s->sel]->data);
        }
        img = &D.spr2F4[s->h14 + *s->sel][1];
        if (s->state != 11) {
            gdl = render_draw_image(0x102, yoff3 + 0x3B, img, gdl, fb, 1, 1, 3, 0);
        } else {
            gdl = render_draw_image(0x108, yoff3 + 0x3B, img, gdl, fb, 1, 1, 3, 0);
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        if (s->state == 3) {
            img = &D.p390[2];
            gdl = render_draw_image((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, gdl, fb, 1, 1, 6, 0);
        } else {
            img = &D.p390[6];
            gdl = render_draw_image((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, gdl, fb, 1, 1, 6, 0);
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        font_text_create(D.txt[s->count * 2], 0x20, 4, 2, 0x21);
        font_text_add(D.txt[s->count * 2], s->title);
        font_draw_text(D.txt[s->count * 2], 0xB9, yoff3 + 0x28);
        for (i = 0; i < s->count; i++) {
            if ((s->state == 11 && !AVAIL(s->h14 + i)) || (s->items[i + s->h14].flags & 1)) {
                font_text_create(D.txt[i], 0x20, 1, (i == *s->sel) ? 4 : 2, 0x20000020);
                font_text_create(D.txt[i + s->count], 0x20, 1, (i == *s->sel) ? 4 : 2, 0x20000021);
            } else {
                font_text_create(D.txt[i], 0x20, 1, (i == *s->sel) ? 4 : 2, 0x20);
                font_text_create(D.txt[i + s->count], 0x20, 1, (i == *s->sel) ? 4 : 2, 0x21);
            }
            if (s->state == 11 && !AVAIL(D.f10)) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                gDPSetRenderMode(gdl++, 0x00404240, 0);
                gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
                gDPLoadTLUT_pal256(gdl++, D.p398->data);
                gdl = render_draw_image(0x44 - i * 7, 0x3E + i * 25 + yoff3, &D.p398[(D.f17C / 2 + i * 4) % 30 + 1], gdl,
                                    fb, 1, 1, 3, 0);
            } else if (i == *s->sel) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                gDPSetRenderMode(gdl++, 0x00404240, 0);
                gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
                gDPLoadTLUT_pal256(gdl++, D.p3AC + 4);
                gdl = render_draw_image(0x45 - i * 7, 0x3D + i * 25 + yoff3, &D.p3A4[D.f17C / 2 % 30 + 1], gdl, fb, 1,
                                    1, 3, 0);
            }
            if (s->state == 11) {
                font_text_add(D.txt[i], gTracks_16[s->h14 + i].name);
                int_to_str(gTracks_16[s->h14 + i].h1E, buf);
                font_text_add(D.txt[i + s->count], buf);
                font_draw_text(D.txt[i + s->count], 0x112 - i * 7, 0x46 + i * 25 + yoff3);
            } else {
                font_text_add(D.txt[i], s->items[i].name);
            }
            font_draw_text(D.txt[i], 0x6E - i * 7, 0x46 + i * 25 + yoff3);
        }
        gdl = font_render(gdl);
        gDPPipeSync(gdl++);
        if (D.flags & 0x200) {
            gdl = shell_draw_players_difficulty_popup(gdl, fb, 1);
        } else if (D.f2E8 >= 8 && D.f2E8 <= 10) {
            gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 0xBE);
            gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
            gDPSetRenderMode(gdl++, 0x00404240, 0);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_OFF);
            gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
            gDPPipeSync(gdl++);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, 0, G_ON);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, D.p748->data);
            img = &D.p748[1];
            gdl = render_draw_image(0x9B, 0x5C, img, gdl, fb, 1, 1, 3, 0);
            gdl = render_draw_image(0x94, 0x75, img, gdl, fb, 1, 1, 3, 0);
            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
            img = &D.p744[1];
            gdl = render_draw_image(0x104 - img->w * 4, 0x78 - img->h / 2, img, gdl, fb, 1, 1, 6, 0);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
            gDPLoadTLUT_pal256(gdl++, D.p3AC + 4);
            gdl = render_draw_image(0x81 - D.h47C * 7, D.h47C * 25 + 0x62, &D.p3A4[D.f17C / 2 % 30 + 1], gdl, fb, 1, 1,
                                3, 0);
            font_reset_queue();
            font_text_create(D.txt[0], 0x20, 1, (D.h47C == 0) ? 4 : 2,
                          (D.f2E8 == 10 && !(gSaveData_13.b2C4 & 8)) ? 0x20000021 : 0x21);
            switch (D.f2E8) {
            case 9:
                font_text_add(D.txt[0], D_800D9C20);
                break;
            case 8:
                font_text_add(D.txt[0], D_800D9A4C);
                break;
            case 10:
                font_text_add(D.txt[0], D_800D9C34);
                break;
            }
            font_draw_text(D.txt[0], 0x113, 0x6A);
            font_text_create(D.txt[0], 0x20, 1, (D.h47C == 1) ? 4 : 2, 0x21);
            switch (D.f2E8) {
            case 9:
                font_text_add(D.txt[0], D_800D9C48);
                break;
            case 8:
                font_text_add(D.txt[0], D_800D9C58);
                break;
            case 10:
                font_text_add(D.txt[0], D_800D9C70);
                break;
            }
            font_draw_text(D.txt[0], 0x10C, 0x83);
            gdl = font_render(gdl);
        } else if (D.f2E8 == 5 || D.f2E8 == 16) {
            if ((pad->h8 & 0x400) || D.f2E8 == 16) {
                gdl = shell_draw_players_difficulty_popup(gdl, fb, -1);
            } else {
                gdl = shell_draw_players_difficulty_popup(gdl, fb, 0);
            }
        }
        break;
    case 4:
    case 18: {
        s32 xoff;
        s32 yoff;
        s32 u4;

        if (s->state == 18) {
            xoff = 0;
            yoff = 0;
        } else {
            xoff = 0;
            yoff = -11;
        }
        PREAMBLE();
        font_reset_queue();
        gDPLoadTLUT_pal256(gdl++, D.p438->data);
        {
        if (D.f3EC < 0x20 && (s->state == 18 || D.f70 == 0)) {
            if ((s->state == 18 && *s->sel / 5 < (D.h44A + 4) / 5 - 1) ||
                (s->state == 4 &&
                 ((D.f410 == 1 && *s->sel < 1) || (D.f410 == 2 && *s->sel < 2) || (D.f410 == 3 && *s->sel < 5)))) {
                DRAW(xoff + 0x128, yoff + 0x22, &D.p438[2], 3);
            }
            if (((s->state != 18) ? *s->sel : *s->sel / 5) > 0) {
                DRAW(xoff + 0x1C, yoff + 0x22, &D.p438[1], 3);
            }
        }
        }
        TDRAWP(D.p3A0, xoff + 0x49, yoff + 0x2A, 3);
        {
        if (D_800CA9C8_8[*s->sel / 5].n != 0) {
            s32 a;
            void *p;

            if (s->state == 18) {
                t = *s->sel;
            } else {
                switch (D.f70) {
                case 2:
                    t = D.h134;
                    break;
                case 1:
                    t = D_800CA9C8_8[D.fC].trk[D.f6C];
                    break;
                default:
                    t = D_800CA9C8_8[D.fC].trk[D.f68];
                    break;
                }
            }
            if (s->state == 18) {
                if (D.f2E8 == 13) {
                    shell_blend_palette_to_gray(0, D.spr2F4[sTrackPicIndex[t]]->data);
                } else {
                    shell_blend_palette_to_gray((0x3B - D.f750) * 0xFF / 0x3B, D.spr2F4[sTrackPicIndex[t]]->data);
                }
            } else {
                shell_blend_palette_to_gray(0xFF, NULL);
            }
            img = &D.spr2F4[sTrackPicIndex[t]][1];
            if (s->state != 18 &&
                !((CUSTOM5 || ((D.f70 != 0 || UNLOCKED(*s->sel)) && D.f70 != 3 &&
                              (D.f70 != 2 || (gUnlockedTracks & (1 << D.h134))))) &&
                 (!CUSTOM5 || LVLDONE))) {
                if (D.f70 == 0 || D.f70 == 3 || s->state == 18) {
                    font_text_create(D.txt[20], 0x40, 4, 2, 0x21);
                    font_text_add(D.txt[20], D_800D9C18);
                    font_draw_text(D.txt[20], xoff + 0x16F, yoff + 0x82);
                } else {
                    font_text_create(D.txt[20], 0x40, 1, 2, 0x21);
                    font_text_add(D.txt[20], D_800D9C8C);
                    font_draw_text(D.txt[20], xoff + 0x172, yoff + 0x74);
                    font_text_create(D.txt[21], 0x40, 1, 2, 0x21);
                    font_text_add(D.txt[21], D_800D9C98);
                    font_draw_text(D.txt[21], xoff + 0x16F, yoff + 0x82);
                    font_text_create(D.txt[22], 0x40, 1, 2, 0x21);
                    font_text_add(D.txt[22], D_800D9CAC);
                    font_draw_text(D.txt[22], xoff + 0x16E, yoff + 0x90);
                }
                gDPSetPrimColor(gdl++, 0, 0, 0x64, 0x64, 0x64, 0xFF);
                gDPLoadTLUT_pal256(gdl++, D.p13C);
                CMB_PRIM();
            } else {
                if (s->state == 18) {
                    if (D.f2E8 == 13) {
                        gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    } else {
                        gDPSetPrimColor(gdl++, 0, 0, 0xFF - (0x3B - D.f750) * 0x82 / 0x3B, 0xFF - (0x3B - D.f750) * 0x82 / 0x3B,
                                        0xFF - (0x3B - D.f750) * 0x82 / 0x3B, 0xFF);
                    }
                    gDPLoadTLUT_pal256(gdl++, D.p13C);
                    CMB_PRIM();
                } else {
                    CMB_TEX();
                    gDPLoadTLUT_pal256(gdl++, D.spr2F4[sTrackPicIndex[t]]->data);
                }
            }
            DRAW(xoff + 0x10C, yoff + 0x47, img, 3);
        }
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        img = &D.p390[(s->state == 18) ? 6 : 7];
        DRAW(((0x200 - img->w * 8) >> 1) + 4, (0xF0 - img->h) / 2, img, 6);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        CMB_TEX();
        for (i = 0; i < 5; i++) {
            if (s->state == 18 || ((D.f70 == 0 || D.f70 == 3) && i < D_800CA9C8_8[*s->sel].n) ||
                (D.f70 != 0 && i <= D_800CA9C8_8[*s->sel].n)) {
                if (s->state == 18) {
                    if (i == *s->sel % 5) {
                        gDPLoadTLUT_pal256(gdl++, D.p3AC + 4);
                        DRAW(0x48 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &D.p3A4[(D.f17C / 2 + i * 4) % 30 + 1], 3);
                        font_text_create(D.txt[i], 0x20, 1, 4, 0x20);
                        font_text_create(D.txt[i + 5], 0x20, 1, 4, 0x22);
                    } else {
                        font_text_create(D.txt[i], 0x20, 1, 2, 0x20);
                        font_text_create(D.txt[i + 5], 0x20, 1, 2, 0x22);
                    }
                } else if (D.f70 != 0 && D.f70 != 3) {
                    if (i == D.f6C) {
                        if (D.f70 == 1) {
                            gDPLoadTLUT_pal256(gdl++, D.p3AC + 4);
                        } else {
                            gDPLoadTLUT_pal256(gdl++, D.p3AC + 0x204);
                        }
                        DRAW(0x48 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &D.p3A4[(D.f17C / 2 + i * 4) % 30 + 1], 3);
                        font_text_create(D.txt[i], 0x20, 1, 4, 0x20);
                        font_text_create(D.txt[i + 5], 0x20, 1, 4, 0x22);
                    } else {
                        font_text_create(D.txt[i], 0x20, 1, 2, 0x20);
                        font_text_create(D.txt[i + 5], 0x20, 1, 2, 0x22);
                    }
                } else {
                    if (!((UNLOCKED(*s->sel) || CUSTOM5) && (!CUSTOM5 || LVLDONE))) {
                        gDPLoadTLUT_pal256(gdl++, D.p398->data);
                        DRAW(0x48 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &D.p398[(D.f17C / 2 + i * 4) % 30 + 1], 3);
                    } else if (((D_800CA9C8_8[*s->sel].won[D.f410 - 1] >> i) & 1) && !CUSTOM5) {
                        gDPLoadTLUT_pal256(gdl++, D.p3A8->data);
                        DRAW(0x49 - i * 7 + xoff, 0x49 + i * 25 + yoff, img = &D.p3A8[(D.f17C / 2 + i * 4) % 30 + 1], 3);
                    }
                    if (i == D.f68) {
                        if (!((UNLOCKED(*s->sel) || CUSTOM5) && (!CUSTOM5 || LVLDONE))) {
                            font_text_create(D.txt[i], 0x20, 1, 4, 0x20000020);
                            font_text_create(D.txt[i + 5], 0x20, 1, 4, 0x20000022);
                        } else {
                            font_text_create(D.txt[i], 0x20, 1, 4, 0x20);
                            font_text_create(D.txt[i + 5], 0x20, 1, 4, 0x22);
                        }
                    } else {
                        if (!((UNLOCKED(*s->sel) || CUSTOM5) && (!CUSTOM5 || LVLDONE))) {
                            font_text_create(D.txt[i], 0x20, 1, 2, 0x20000020);
                            font_text_create(D.txt[i + 5], 0x20, 1, 2, 0x20000022);
                        } else {
                            font_text_create(D.txt[i], 0x20, 1, 2, 0x20);
                            font_text_create(D.txt[i + 5], 0x20, 1, 2, 0x22);
                        }
                    }
                }
                if (s->state == 18 || i < D_800CA9C8_8[*s->sel].n) {
                    if ((s->state != 18 && D.f70 != 2) || (s->state == 18 && (*s->sel / 5) * 5 + i < D.h44A)) {
                        if (s->state == 18) {
                            switch ((*s->sel / 5) * 5 + i) {
                            case 0x15:
                            case 0x16:
                            case 0x17:
                                strcpy(buf2, "-");
                                break;
                            default:
                                int_to_str(gTracks_16[D.p44C[(*s->sel / 5) * 5 + i]].h1E, buf2);
                                break;
                            }
                        } else if (*s->sel == 5) {
                            int_to_str(D_800CA9C8_8[5].laps[i], buf2);
                        } else if (*s->sel == 4) {
                            int_to_str(1, buf2);
                        } else {
                            int_to_str(gTracks_16[D_800CA9C8_8[*s->sel].trk[i]].h1E, buf2);
                        }
                        font_text_add(D.txt[i + 5], buf2);
                        font_draw_text(D.txt[i + 5], 0x11A - i * 7 + xoff, 0x52 + i * 25 + yoff);
                    }
                    if (s->state == 18) {
                        if ((*s->sel / 5) * 5 + i < D.h44A) {
                            font_text_add(D.txt[i], gTracks_16[D.p44C[(*s->sel / 5) * 5 + i]].name);
                        }
                    } else {
                        font_text_add(D.txt[i], gTracks_16[D_800CA9C8_8[*s->sel].trk[i]].name);
                    }
                    font_draw_text(D.txt[i], 0x6F - i * 7 + xoff, 0x52 + i * 25 + yoff);
                } else {
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    CMB_PRIM();
                    if (*s->sel == 5) {
                        img = &D.p390[5];
                    }
                    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
                    DRAW(0x63 - i * 7 + xoff, 0x45 + i * 25 + yoff, img, 6);
                    if (D.f70 != 1) {
                        img = &D.p390[4];
                        DRAW(0x47 - i * 7 + xoff, 0x46 + i * 25 + yoff, img, 6);
                    }
                }
            } else {
                gDPSetTextureLUT(gdl++, G_TT_NONE);
                CMB_PRIM();
                img = &D.p390[4];
                gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xF0);
                DRAW(0x47 - i * 7 + xoff, 0x46 + i * 25 + yoff, img, 6);
                if (*s->sel == 5) {
                    img = &D.p390[5];
                }
                DRAW(0x63 - i * 7 + xoff, 0x45 + i * 25 + yoff, img, 6);
            }
        }
        if (s->state == 18) {
            strcpy(buf, "Records");
        } else if (*s->sel == 4) {
            strcpy(buf, "Challenge");
        } else if (*s->sel == 5) {
            if (D.f70 == 2 && (gFrameCounter & 0x20)) {
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                CMB_TEX();
                gDPLoadTLUT_pal256(gdl++, D.p3B0->data);
                DRAW(xoff + 0x16E, yoff + 0x34, &D.p3B0[2], 3);
                DRAW(xoff + 0x149, yoff + 0xBE, &D.p3B0[2], 0x23);
                DRAW(xoff + 0xFE, yoff + 0x79, &D.p3B0[1], 0x43);
                DRAW(xoff + 0x1C1, yoff + 0x79, &D.p3B0[1], 3);
            }
            switch (D.f70) {
            case 0:
                strcpy(buf, "Custom");
                if (LVLDONE) {
                    for (i = 0; i < 3; i++) {
                        font_text_create(D.txt[i + 12], 0x40, 1, (i == D.f6C) ? 4 : 2, 0x21);
                    }
                    if (D_800CA9C8_8[*s->sel].n == 0) {
                        font_text_add(D.txt[12], D_800D9CDC);
                        font_draw_text(D.txt[12], xoff + 0x16E, yoff + 0x80);
                    } else {
                        font_text_add(D.txt[12], D_800D9A30);
                        font_draw_text(D.txt[12], xoff + 0x16E, yoff + 0x79);
                        font_text_add(D.txt[13], D_800D9A28);
                        font_draw_text(D.txt[13], xoff + 0x16E, yoff + 0x87);
                    }
                }
                break;
            case 1:
                strcpy(buf, "Select Slot");
                font_text_create(D.txt[12], 0x40, 5, 4, 0x20);
                if (D.f6C == D_800CA9C8_8[*s->sel].n) {
                    font_text_add(D.txt[12], D_800D9CF0);
                } else {
                    font_text_add(D.txt[12], D_800D9D14);
                }
                font_draw_text(D.txt[12], 0x2D, yoff + 0xCF);
                break;
            case 2:
                strcpy(buf, "Choose Track");
                font_text_create(D.txt[12], 0x40, 1, 4, 0x20);
                switch ((s16)(D.h134 / 5)) {
                case 0:
                    font_text_add(D.txt[12], D_800D9D54);
                    break;
                case 1:
                    font_text_add(D.txt[12], D_800D9D68);
                    break;
                case 2:
                    font_text_add(D.txt[12], D_800D9D7C);
                    break;
                case 3:
                    font_text_add(D.txt[12], D_800D9D90);
                    break;
                }
                font_draw_text(D.txt[12], 0x2D, yoff + 0xCF);
                break;
            }
        } else {
            switch (*s->sel) {
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
        font_text_create(D.txt[11], 0x40, 4, 2, 0x21);
        font_text_add(D.txt[11], buf);
        font_draw_text(D.txt[11], xoff + 0xBD, yoff + 0x34);
        if (s->state != 18 && D.f70 == 0) {
            font_text_create(D.txt[10], 0x40, 1, 2, 0x20);
            if (D.f410 < SAVE.b2C0 || (SAVE.b2C0 == D.f410 && ((SAVE.b2BF >> *s->sel) & 1))) {
                font_text_add(D.txt[10], D_800D9DE4);
            } else if ((SAVE.b2C0 > D.f410 || (SAVE.b2C0 == D.f410 && (*s->sel == 0 || ((SAVE.b2BF >> (*s->sel - 1)) & 1)))) || (CUSTOM5 && SAVE.b2C0 == D.f410 && (SAVE.b2BF & 8))) {
                s32 fl;

                fl = 0;
                switch (D.f410) {
                case 1:
                    if (*s->sel == 1) {
                        fl = 1;
                    }
                    break;
                case 2:
                    if (*s->sel == 2) {
                        fl = 1;
                    }
                    break;
                case 3:
                    if (*s->sel == 4) {
                        fl = 1;
                    }
                    break;
                }
                if (fl) {
                    if (D.f410 == 3) {
                        font_text_add(D.txt[10], D_800D9DF4);
                    } else {
                        font_text_add(D.txt[10], D_800D9E24);
                    }
                } else if (*s->sel == 5) {
                    if (D_800CA9C8_8[5].n != 0) {
                        if (D.f6C == 0) {
                            font_text_add(D.txt[10], D_800D9E50);
                        } else {
                            font_text_add(D.txt[10], D_800D9E6C);
                        }
                    } else {
                        font_text_add(D.txt[10], D_800D9E94);
                    }
                } else {
                    font_text_add(D.txt[10], D_800D9EC0);
                }
            } else if (CUSTOM5) {
                font_text_add(D.txt[10], D_800D9EEC);
            } else {
                font_text_add(D.txt[10], D_800D9F14);
            }
            font_draw_text(D.txt[10], xoff + 0x2D, yoff + 0xCF);
        }
        gdl = font_render(gdl);
        PSYNCK(u4);
        if (s->state == 18) {
            s32 dx, dy;

            trk = &gTracks_16[D.p44C[D.f450]];

            if (D.f2E8 == 13) {
                D_800CA80F = 0;
            } else {
                D_800CA80F = (0x3B - D.f750) * 0xFF / 0x3B;
            }
            font_reset_queue();
            if (trk->f50 & 0x400) {
                font_text_create(D.txt[0], 0x20, 1, 9, 0x21);
                font_text_add(D.txt[0], D_800D9F40);
                font_draw_text(D.txt[0], 0x175, 0x6D);
                for (j = 0; j < 3; j++) {
                    font_text_create(D.txt[0], 0x20, 1, 9, 0x20);
                    int_to_str(trk->f34[j], buf);
                    font_text_add(D.txt[0], buf);
                    font_draw_text(D.txt[0], 0x134 - j * 4, 0x7E + j * 13);
                    font_text_create(D.txt[0], 0x20, 1, 9, 0x22);
                    __builtin_memcpy(buf, trk->init[j], 3);
                    buf[3] = 0;
                    font_text_add(D.txt[0], buf);
                    font_draw_text(D.txt[0], 0x1AC - j * 4, 0x7E + j * 13);
                }
            } else {
                if (D.f450 == 0x15 || D.f450 == 0x17) {
                    dx = -4;
                    dy = 0x14;
                } else {
                    dx = 0;
                    dy = 0;
                }
                font_text_create(D.txt[0], 0x20, 1, 9, 0x21);
                font_text_add(D.txt[0], D_800D9F4C);
                font_draw_text(D.txt[0], dx + 0x17B, dy + 0x59);
                for (j = 0; j < 3; j++) {
                    font_text_create(D.txt[0], 0x20, 1, 9, 0x20);
                    event_format_time(trk->f34[j], buf);
                    font_text_add(D.txt[0], buf);
                    font_draw_text(D.txt[0], dx + (0x175 - j * 4) - 0x3C, (0x6A + j * 13) + dy);
                    font_text_create(D.txt[0], 0x20, 1, 9, 0x22);
                    __builtin_memcpy(buf, trk->init[j], 3);
                    buf[3] = 0;
                    font_text_add(D.txt[0], buf);
                    font_draw_text(D.txt[0], dx + (0x175 - j * 4) + 0x3C, (0x6A + j * 13) + dy);
                }
                if (D.f450 != 0x15 && D.f450 != 0x17) {
                    font_text_create(D.txt[0], 0x20, 1, 9, 0x21);
                    font_text_add(D.txt[0], D_800D9F58);
                    font_draw_text(D.txt[0], 0x168, 0x9C);
                    font_text_create(D.txt[0], 0x20, 1, 9, 0x20);
                    event_format_time(trk->f40, buf);
                    font_text_add(D.txt[0], buf);
                    font_draw_text(D.txt[0], 0x129, 0xAC);
                    font_text_create(D.txt[0], 0x20, 1, 9, 0x22);
                    __builtin_memcpy(buf, trk->init4D, 3);
                    buf[3] = 0;
                    font_text_add(D.txt[0], buf);
                    font_draw_text(D.txt[0], 0x1A1, 0xAC);
                }
            }
            gdl = font_render(gdl);
        } else if (D.flags & 0x200) {
            gdl = shell_draw_players_difficulty_popup(gdl, fb, 1);
        }
        break;
    }
    case 6: {
        s32 u5;

        font_reset_queue();
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        if (D.f2E8 == 14) {
            D.f2E8 = 15;
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        CMB_TEX();
        {
            Spr_2 *_t = &D.p3A0[1];

            gDPLoadTLUT_pal256(gdl++, D.p3A0->data);
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
                    if (j == D.h448) {
                        gDPSetPrimColor(gdl++, 0, 0, 0xFF, k * 0xFF / 29, 0, 0xFF);
                    } else {
                        gDPSetPrimColor(gdl++, 0, 0, 0x8C, k * 0x8C / 29, 0, 0xFF);
                    }
                } else {
                    gDPSetPrimColor(gdl++, 0, 0, 0x4B, 0x4B, 0x4B, 0xFF);
                }
                DRAW(x, y, &D.p390[1], 6);
            }
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        img = &D.p390[2];
        DRAW(0x100 - img->w * 4, 0x78 - (img->h >> 1), img, 6);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        gDPLoadTLUT_pal256(gdl++, D.p3AC + 4);
        img = &D.p3A4[D.f17C / 2 % 30 + 1];
        for (j = 0; j < 11; j++) {
            if (j < 3) {
                x = 0x52 - j * 7;
            } else {
                x = 0x52 - ((j - 3) / 2 + 3) * 7 + ((j - 3) & 1) * 194;
            }
            y = sOptionsRowY[j];
            if (j == 9 || j == 10) {
                font_text_create(D.txt[0], 0x40, 1, (j == D.h448) ? 4 : 2, 0x20000020);
            } else {
                font_text_create(D.txt[0], 0x40, 1, (j == D.h448) ? 4 : 2,
                              (gHasExpansionPak_2 == 0 && j == 7) ? 0x20000020 : 0x20);
            }
            font_text_add(D.txt[0], gOptionsLabels[j]);
            font_draw_text(D.txt[0], x + 0x24 + ((u16)y >> 16), y + 8);
            if (j == D.h448) {
                DRAW(x - 2, y, img, 3);
            }
            switch (j) {
            case 3:
                font_text_create(D.txt[0], 0x40, 1, (D.h448 == j) ? 6 : 2, 0x22);
                font_text_add(D.txt[0], gMonoStereoStrings[SAVE.b2C4 & 1]);
                font_draw_text(D.txt[0], x + 0xB4 + ((u16)x >> 16), y + 8);
                break;
            case 8:
                font_text_create(D.txt[0], 0x40, 1, (D.h448 == j) ? 6 : 2, 0x22);
                font_text_add(D.txt[0], sOffOnStrings[(SAVE.b2C4 & 0x10) == 0]);
                font_draw_text(D.txt[0], x + 0xB4, y + 8);
                break;
            case 7:
                tmp = 2;
                if (D.h448 == j) {
                    tmp = 4;
                    if (gHasExpansionPak_2 != 0) {
                        tmp = 6;
                    }
                }
                font_text_create(D.txt[0], 0x40, 1, tmp, (gHasExpansionPak_2 == 0) ? 0x20000022 : 0x22);
                font_text_add(D.txt[0], sOffOnStrings[(SAVE.b2C4 & 2) >> 1]);
                font_draw_text(D.txt[0], x + 0xB4, y + 8);
                break;
            }
        }
        font_text_create(D.txt[0], 0x40, 4, 2, 0x20);
        font_text_add(D.txt[0], D_800D9370);
        font_draw_text(D.txt[0], 100, 0x23);
        gdl = font_render(gdl);
        if (D.f2E8 == 6 || D.f2E8 == 15 || D.f2E8 == 14) {
            gdl = shell_draw_yes_no_dialog(gdl, fb, D_800D9F64);
        }
        break;
    }
    case 24:
    case 25:
        font_reset_queue();
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        img = &D.p3DC[1];
        gDPLoadTLUT_pal256(gdl++, D.p3DC->data);
        if (s->state == 25) {
            DRAW(0x106 - img->w * 4, 0x41, img, 3);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            CMB_PRIM();
            gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
            img = &D.p390[1];
            DRAWV((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, 6);
            font_text_create(D.txt[0], 0x40, 4, 2, 0x21);
            font_text_add(D.txt[0], D_800D9F7C);
            font_draw_text(D.txt[0], 0x106, 0x4C);
            for (i = 0; sBackupLostMsg[i] != NULL; i++) {
                font_text_create(D.txt[0], 0x40, 1, 4, 0x21);
                str_copy(buf, sBackupLostMsg[i]);
                font_text_add(D.txt[0], buf);
                font_draw_text(D.txt[0], 0x100 - i * 4, i * 13 + 0x6D);
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
            img = &D.p390[1];
            DRAWV((0x200 - img->w * 8) >> 1, (0xF0 - img->h) / 2, img, 6);
            font_text_create(D.txt[0], 0x40, 4, 2, 0x21);
            font_text_add(D.txt[0], D_800D9F84);
            font_draw_text(D.txt[0], 0x110, 0x28);
            font_text_create(D.txt[0], 0x40, 5, 2, 0x21);
            font_text_add(D.txt[0], D_800D9F94);
            font_draw_text(D.txt[0], 400, 0x25);
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
                font_text_create(D.txt[0], 0x40, 5, 2, 0x21);
                font_text_add(D.txt[0], buf3);
                font_draw_text(D.txt[0], 0x106 - line * 3, line * 11 + 0x46);
                line++;
                for (k2 = 0; pp[k2] != NULL; k2++) {
                    font_text_create(D.txt[0], 0x40, 5, col, 0x21);
                    font_text_add(D.txt[0], pp[k2]);
                    font_draw_text(D.txt[0], 0x106 - line * 3, line * 11 + 0x46);
                    line++;
                }
            }
        }
        gdl = font_render(gdl);
        break;
    case 9:
    case 20: {
        s32 u6;
        s32 plr;
        s32 cheat;

        font_reset_queue();
        cheat = (s->state == 9);
        if (!cheat) {
            plr = D.b758[D.f460];
        }
        gDPSetTexturePersp(gdl++, G_TP_NONE);
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        gDPLoadTLUT_pal256(gdl++, D.spr2F4[0]->data);
        DRAW(0x108, 0x47, &D.spr2F4[0][1], 3);
        TDRAWP(D.p3A0, 0x45, 0x2A, 3);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        img = &D.p390[1];
        DRAWV(0x100 - img->w * 4, 0x78 - (img->h >> 1), img, 6);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        img = &D.p39C[D.f3EC / 2 % 60 + 1];
        if (D.f3C >= 2 && !cheat) {
            gDPLoadTLUT_pal256(gdl++, D.p444 + plr * 0x200);
        } else {
            gDPLoadTLUT_pal256(gdl++, D.p39C->data);
        }
        DRAWK(0x18B, 0x2F, img, 3, u6);
        font_text_create(D.txt[0], 0x20, 4, 2, 0x20);
        if (cheat) {
            strcpy(buf, "Enter A Cheat Code");
            font_text_add(D.txt[0], buf);
            font_draw_text(D.txt[0], 0x55, 0x35);
        } else {
            strcpy(buf, "Player ");
            int_to_str(plr + 1, buf + strlen_2(buf));
            str_cat(buf, D_800DA014);
            font_text_add(D.txt[0], buf);
            font_draw_text(D.txt[0], 0x55, 0x35);
        }
        if (cheat && D.f2E8 == 17) {
            font_text_create(D.txt[0], 0x20, 1, 2, 0x21);
            font_text_add(D.txt[0], sCheatTable_2[D.b7A9].name);
            if (((1 << D.b7A9) == 4 && (D.flags & 0x20000000)) || ((sCheatMask >> D.b7A9) & 1)) {
                font_text_add(D.txt[0], D_800DA020);
            } else {
                font_text_add(D.txt[0], D_800DA02C);
            }
            font_draw_text(D.txt[0], 0xAC, 0x71);
            font_text_create(D.txt[0], 0x20, 1, 2, 0x21);
            font_text_add(D.txt[0], D_800DA038);
            font_draw_text(D.txt[0], 0xA8, 0x7F);
        } else if (cheat && D.f2E8 == 18) {
            font_text_create(D.txt[0], 0x20, 1, 2, 0x21);
            font_text_add(D.txt[0], D_800DA04C);
            font_draw_text(D.txt[0], 0xAC, 0x71);
            font_text_create(D.txt[0], 0x20, 1, 2, 0x21);
            font_text_add(D.txt[0], D_800DA038);
            font_draw_text(D.txt[0], 0xA8, 0x7F);
        } else {
            s32 xx;

            for (y = 0; y < 5; y++) {
                for (x = 0; x < 8; x++) {
                    xx = x * 26;
                    font_text_create(D.txt[0], 0x20, (y * 8 + x > 0x25) ? 2 : 1, 2,
                                  (D.h464 == ((cheat) ? 10 : 3) && y * 8 + x < 0x26) ? 0x20000021 : 0x21);
                    font_text_add(D.txt[0], gKeyboardKeys[y * 8 + x]);
                    font_draw_text(D.txt[0], xx + 0x54 - y * 4, y * 17 + 0x56);
                    if (y * 8 + x == D.h454) {
                        gDPSetRenderMode(gdl++, 0x00404240, 0);
                        gDPSetTextureLUT(gdl++, G_TT_NONE);
                        CMB_PRIM();
                        img = &D.p390[3];
                        DRAW(xx + 0x54 - y * 4 - img->w * 4 + 4, y * 17 - (img->h >> 1) + 0x57, img, 6);
                    }
                }
            }
        }
        font_text_create(D.txt[0], 0x20, 2, (j == D.h464) ? 4 : 2, 0x20);
        if (!cheat) {
            switch (D.f460) {
            case 0:
            case 1:
            case 2:
                if (D.f460 != 0) {
                    buf[0] = D.f460 + '1';
                    buf[1] = D.f460 + '{';
                    buf[2] = ' ';
                    buf[3] = '\0';
                    font_text_add(D.txt[0], buf);
                }
                if (gCurTrack_41->f50 & 0x400) {
                    font_text_add(D.txt[0], D_800DA060);
                } else {
                    font_text_add(D.txt[0], D_800DA06C);
                }
                if (D.b758[3] == plr || D.b7A8 == plr) {
                    font_text_add(D.txt[0], D_800DA078);
                }
                break;
            case 3:
                font_text_add(D.txt[0], D_800D9F58);
                break;
            }
            font_draw_text(D.txt[0], 0x2F, 0xB6);
        }
        for (j = 0; (cheat) ? (j < 10) : (j < 3); j++) {
            font_text_create(D.txt[0], 0x20, 1, (j == D.h464) ? 4 : 2, 0x21);
            font_text_add(D.txt[0], D_800DA084);
            font_draw_text(D.txt[0], (j - 1) * 20 + ((cheat) ? 0x50 : 0xE2), 0xB9);
            if (j < strlen_2(D.s466)) {
                font_text_create(D.txt[0], 0x20, 1, (j == D.h464) ? 4 : 2, 0x21);
                buf[0] = D.s466[j];
                buf[1] = '\0';
                font_text_add(D.txt[0], buf);
                font_draw_text(D.txt[0], (j - 1) * 20 + ((cheat) ? 0x50 : 0xE3), 0xB3);
            }
        }
        gdl = font_render(gdl);
        break;
    }
    case 19: {
        Glob_8 *gp = &gGame_78;

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
        img = &gp->p45C[1];
        gDPLoadTLUT_pal256(gdl++, gp->p45C->data);
        DRAW(0xFE - img->w * 4, 0x74 - (img->h >> 1), img, 3);
        TDRAWP(gp->p3A0, 0x4A, 0x20, 3);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        font_text_create(gp->txt[0], 0x40, 4, 2, 0x21);
        strcpy(buf, "Controller ");
        int_to_str(gp->h476 + 1, buf + strlen_2(buf));
        font_text_add(gp->txt[0], buf);
        font_draw_text(gp->txt[0], 0xBA, 0x29);
        font_text_create(gp->txt[0], 0x40, 4, 2, 0x21);
        strcpy(buf, "Configuration ");
        buf[strlen_2(buf) + 1] = '\0';
        buf[strlen_2(buf)] = gp->b50[gp->h476] + 'A';
        font_text_add(gp->txt[0], buf);
        font_draw_text(gp->txt[0], 0xE9, 0xBF);
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
            font_text_create(gp->txt[0], 0x40, 5, tmp, a);
            font_text_add(gp->txt[0], gControlActionLabels[gControlConfigs[(gp->b50 + gp->h476)[0]][j]]);
            font_draw_text(gp->txt[0], gControlLabelPos[j].x, gControlLabelPos[j].y);
            if (gControlConfigs[(gp->b50 + gp->h476)[0]][j] == 6) {
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
                font_text_create(gp->txt[0], 0x40, 5, tmp, a);
                font_text_add(gp->txt[0], D_800CDAC8_2);
                font_draw_text(gp->txt[0], gControlLabelPos[j].x, gControlLabelPos[j].y + 11);
            }
        }
        for (j = 0; j < 3; j++) {
            font_text_create(D.txt[0], 0x40, 5, (j == 2) ? 4 : 6, (j != 2) ? 0x40000020 : 0x20);
            font_text_add(D.txt[0], gCameraButtonLabels[j]);
            font_draw_text(D.txt[0], gCameraLabelPos[j].x, gCameraLabelPos[j].y);
        }
        for (j = 0; j < D.f3C; j++) {
        }
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        img = &D.p390[1];
        DRAWV(0x100 - img->w * 4, 0x78 - (img->h >> 1), img, 6);
        for (j = 0; j < 4; j++) {
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            CMB_TEX();
            img = &D.p39C[D.f3EC / 2 % 60 + 1];
            if (j == D.h476) {
                gDPLoadTLUT_pal256(gdl++, D.p444 + j * 0x200);
            } else {
                gDPLoadTLUT_pal256(gdl++, D.p13C);
            }
            DRAW(0x132 + j * 0x2C, 0x24, img, 3);
        }
        gdl = font_render(gdl);
        break;
    }
    case 7:
#define YR (0x46 + j * rowh + ry)
    case 13:
    case 14: {
        s32 x;
        s32 y;
        s32 px;

        font_reset_queue();
        rx = 0;
        if (s->state == 14) {
            if (CHAMP4) {
                rx = -8;
                ry = 0x1B;
            } else {
                ry = -7;
            }
        } else if (D.f4C == 8) {
            ry = 0x32;
            rx = -0xD;
        } else {
            ry = 0;
        }
        for (j = 0; j < 3; j++) {
            font_text_create(D.txt[j], 0x20, 4, 2, 0x20);
        }
        if (s->state == 14 && !CHAMP4) {
            font_text_add(D.txt[0], D_800DA0A4);
            font_text_add(D.txt[1], D_800DA0B8);
            font_draw_text(D.txt[1], 0x18F, ry + 0x28);
        } else {
            if (D.f4C == 8) {
                font_text_add(D.txt[0], D_800DA0BC);
            } else {
                font_text_add(D.txt[0], D_800DA0D0);
            }
            font_text_add(D.txt[2], D_800DA0E0);
            if (s->state == 7) {
                font_text_add(D.txt[1], D_800DA0B8);
                font_draw_text(D.txt[1], 0x147, ry + 0x28);
                font_draw_text(D.txt[2], 0x18D, ry + 0x28);
            } else if (s->state == 14 && CHAMP4) {
                font_draw_text(D.txt[2], 0x14D, ry + 0x2A);
            } else if (D.f4C == 8) {
                font_draw_text(D.txt[2], 0x178, ry + 0x28);
            } else {
                font_draw_text(D.txt[2], 0x184, ry + 0x28);
            }
        }
        if (s->state == 14 && CHAMP4) {
            font_draw_text(D.txt[0], 0x50, ry + 0x2A);
            font_text_create(D.txt[0], 0x20, 4, 2, 0x21);
            if (R.b50[0] == 0) {
                font_text_add(D.txt[0], D_800DA0E8);
            } else {
                font_text_add(D.txt[0], D_800DA0FC);
            }
            font_draw_text(D.txt[0], 0xEE, 0x97);
        } else {
            font_draw_text(D.txt[0], 0x55, ry + 0x28);
        }
        rowh = 0x19;
        if (s->state == 14 && CHAMP4) {
            rowh = 0x17;
        }
        for (j = 0; j < R.h22; j++) {
            if (s->state == 14 && !CHAMP4) {
                r = (*gSeason_23).order[j];
            } else {
                r = R.b50[j];
            }
            img = (Spr_2 *)((r < D.f3C || D.f4C == 8) ? ((D.f4C == 8) ? ((r == D_800C8CE0) ? 4 : 2) : 4) : 2);
            font_text_create(D.txt[j * 4 + 5], 0x20, 1, (s32)img, 0x20);
            pl = j;
            while (pl > 0 && s->state == 13 && gRiders_91[R.b50[pl]].f68C == gRiders_91[R.b50[pl - 1]].f68C) {
                s32 d0;

                pl--;
            }
            int_to_str(pl + 1, buf);
            font_text_add(D.txt[j * 4 + 5], buf);
            x = rx + 0x4E - j * 7;
            font_draw_text(D.txt[j * 4 + 5], (s->state == 14 && CHAMP4) ? x - 2 : x, YR);
            str_copy(buf, sRiderBios_3[D.f1C[r]].name);
            font_text_create(D.txt[j * 4 + 6], 0x20, 1, (s32)img, 0x20);
            font_text_add(D.txt[j * 4 + 6], buf);
            font_draw_text(D.txt[j * 4 + 6], rx - (j * 7 - 0x6A), YR);
            if (r < 4) {
                wid[r] = font_get_char_pos(D.txt[j * 4 + 6], 0x6A - j * 7, YR, strlen_2(buf) - 1) >> 16;
            }
            if (s->state == 7 && gRacePoints_3[j] != 0) {
                font_text_create(D.txt[j * 4 + 7], 0x20, 1, (s32)img, 0x20);
                int_to_str(gRacePoints_3[j], buf + 1);
                buf[0] = '+';
                font_text_add(D.txt[j * 4 + 7], buf);
                font_draw_text(D.txt[j * 4 + 7], 0x14C - j * 7, YR);
            }
            font_text_create(D.txt[j * 4 + 7], 0x20, 1, (s32)img, 0x20);
            if (s->state == 14 && !CHAMP4) {
                int_to_str((*gSeason_23).pts[r], buf);
            } else {
                event_format_time(gRiders_91[r].f68C, buf);
            }
            font_text_add(D.txt[j * 4 + 7], buf);
            switch (s->state) {
            case 7:
                font_draw_text(D.txt[j * 4 + 7], 0x186 - j * 7, YR);
                break;
            case 13:
                if (D.f4C == 8) {
                    font_draw_text(D.txt[j * 4 + 7], 0x170 - j * 7, YR);
                } else {
                    font_draw_text(D.txt[j * 4 + 7], 0x17C - j * 7, YR);
                }
                break;
            case 14:
                if (CHAMP4) {
                    font_draw_text(D.txt[j * 4 + 7], 0x146 - j * 7, YR);
                } else {
                    font_draw_text(D.txt[j * 4 + 7], 0x199 - j * 7, YR);
                }
                break;
            }
        }
        gDPSetRenderMode(gdl++, 0x00404240, 0);
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        TDRAW(D.p414, rx + 0x44, ry + 0x1F, 3);
        gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        CMB_PRIM();
        if (s->state == 14 && CHAMP4) {
            img = &D.p390[1];
            DRAWV(((0x200 - img->w * 8) >> 1) | 1, (0xF0 - img->h) / 2, img, 6);
        } else if (D.f4C == 8) {
            img = &D.p390[4];
            DRAWV(((0x200 - img->w * 8) >> 1) | 1, (0xF0 - img->h) / 2, img, 6);
        } else {
            img = &D.p390[(s->state == 7) ? 2 : 3];
            DRAWV(((0x200 - img->w * 8) >> 1) | 1, (0xF0 - img->h) / 2 + ry, img, 6);
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        CMB_TEX();
        img = &D.p39C[D.f3EC / 2 % 60 + 1];
        for (j = 0; j < D.f3C; j++) {
            if (D.f3C >= 2 && D.f4C != 8) {
                gDPLoadTLUT_pal256(gdl++, D.p444 + j * 0x200);
                font_text_create(D.txt[R.h22 * 4 + 5 + j], 0x20, 5, 6, 0x21);
                buf[0] = j + '1';
                buf[1] = '\0';
                font_text_add(D.txt[R.h22 * 4 + 5 + j], buf);
                font_draw_text(D.txt[R.h22 * 4 + 5 + j], wid[j] + 5 + img->w * 4,
                              (img->h >> 1) + 0x37 + gRiders_91[j].h668 * 25 + ((s->state != 14) ? 7 : 0));
            } else {
                gDPLoadTLUT_pal256(gdl++, D.p39C->data);
            }
            if (s->state == 14) {
                for (k = 0; k < R.h22; k++) {
                    if ((*gSeason_23).order[k] == j) {
                        break;
                    }
                }
            }
            px = 5 + wid[j + D_800C8CE0] + rx;
            if (s->state != 14 || CHAMP4) {
                y = 0x3E + rowh * gRiders_91[j + D_800C8CE0].h668 + ry;
            } else {
                y = 0x3E + rowh * k + ry;
            }
            DRAW(px, y, img, 3);
        }
        gdl = font_render(gdl);
        switch (D.f2E8) {
        case 7:
            gdl = shell_draw_yes_no_dialog(gdl, fb, D_800DA110);
            break;
        case 4:
            gdl = shell_draw_results_menu(gdl, fb, D.f400, sSeasonResultOptions);
            gdl = font_render(gdl);
            break;
        case 3:
            gdl = shell_draw_results_menu(gdl, fb, D.f400, (D.f4C != 8) ? ((gCurTrack_41->f50 & 2) ? sSpecialTrackResultOptions : sExhibitionResultOptions) : sTimeTrialResultOptions);
            gdl = font_render(gdl);
            break;
        }
        gDPPipeSync(gdl++);
        break;
    }
    case 0:
        if (D.f54 != 1) {
            render_draw_race(m, fb, &gdl);
            gDPSetPrimColor(gdl++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
            gDPSetTexturePersp(gdl++, G_TP_NONE);
            gDPSetTextureLUT(gdl++, G_TT_NONE);
            gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
            gDPSetRenderMode(gdl++, 0x00404240, 0);
            CMB_PRIM();
            DRAWV(0x43, 0x14, &D.p38C[1], 6);
            gDPSetPrimColor(gdl++, 0, 0, 0xE1, 0xD7, 0x1E, 0xFF);
            DRAWV(0x43, 0x14, &D.p38C[2], 6);
            gDPSetPrimColor(gdl++, 0, 0, 0xF5, 0xF5, 0xF5, 0xFA);
            DRAWV(0xFE - D.p390[1].w * 4, 200, &D.p390[1], 6);
        }
        font_reset_queue();
        gDPPipeSync(gdl++);
        if (D.flags & 0x8000000) {
            font_text_create(D.txt[0], 0, 1, 4, 0x11);
            font_text_add(D.txt[0], D_800DA120);
            font_draw_text(D.txt[0], gScreenWidth / 2, gScreenHeight * 0xB6 / 0xF0);
        } else if (gFrameCounter & 0x20) {
            font_text_create(D.txt[0], 0, 1, 2, 0x11);
            font_text_add(D.txt[0], D_800DA144);
            font_draw_text(D.txt[0], gScreenWidth / 2, gScreenHeight * 0xB6 / 0xF0);
        }
        gdl = font_render(gdl);
        gDPPipeSync(gdl++);
        break;
    case 2: {
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
        for (i = 0; i < D.f3C; i++) {
            s32 yo;
            s32 bx;
            s32 by;
            s32 x;
            s32 y;

            switch (D.b40C) {
            case 1:
                yo = (0x3E - D.h408) * 2;
                break;
            case 2:
                yo = 0x7C;
                break;
            case 3:
                yo = D.h408 * 2;
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
            if (D.f3C < 3) {
                if (!(D.flags & (0x400 << i))) {
                    gDPSetTexturePersp(gdl++, G_TP_NONE);
                    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                    CMB_TEX();
                    if (D.f3C == 1) {
                        gDPLoadTLUT_pal256(gdl++, D.p3DC->data);
                        DRAW(VP[gNumViews - 1][i].x + 0x10, VP[gNumViews - 1][i].y + 5 + yo, &D.p3DC[1], 3);
                        if ((gFrameCounter & 0x20) && D.b40C == 0) {
                            gDPLoadTLUT_pal256(gdl++, D.p74C->data);
                            DRAW(0x28, yo + 0x17, &D.p74C[1], 3);
                            DRAW(0xF5, yo + 0x17, &D.p74C[2], 3);
                        }
                    } else if (gFrameCounter & 0x20) {
                        if (gSplitVertical != 0 && i == 1) {
                            x = 0x120;
                            y = 0x3C;
                        } else {
                            x = 0x41;
                            y = i * 120 + 0x3C;
                        }
                        gDPLoadTLUT_pal256(gdl++, D.p74C->data);
                        DRAW(x, y, &D.p74C[1], 3);
                        DRAW(x | 0x8C, y, &D.p74C[2], 3);
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    gDPSetRenderMode(gdl++, 0x00404240, 0);
                    CMB_PRIM();
                    gDPSetPrimColor(gdl++, 0, 0, 0xD2, 0xD2, 0xD2, 0xFF);
                    img = &D.p390[8];
                    if (D.f3C == 2 && i == 0 && gSplitVertical == 0) {
                        s32 dx = yo2 + VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w - img->w * 8;
                        s32 dy = VP[gNumViews - 1][i].y;

                        if (D.flags & (0x400 << i)) {
                            dy += 600;
                        }
                        DRAWV(dx, dy, img, 6);
                    } else {
                        s32 dy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - img->h;
                        s32 dx = yo2 + VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w - img->w * 8;

                        if (D.flags & (0x400 << i)) {
                            dy += 600;
                        }
                        DRAWV(dx, dy, img, 6);
                    }
                    img = &D.p390[8];
                    bx = yo2 + VP[D.f3C - 1][i].x + VP[D.f3C - 1][i].w - img->w * 8 + 0x1F;
                    if (D.f3C == 2 && i == 0 && gSplitVertical == 0) {
                        by = VP[D.f3C - 1][i].y + 11;
                    } else {
                        by = VP[D.f3C - 1][i].y + VP[D.f3C - 1][i].h - img->h + 11;
                    }
                    for (j = 1; j < 17; j++) {
                        font_text_create(D.txt[j + i * 20], 0x40, 1, 2, 0x20);
                    }
                    for (; j < 20; j++) {
                        font_text_create(D.txt[j + i * 20], 0x40, 1, 2, 0x22);
                    }
                }
                font_text_create(D.txt[i * 20], 0x40, 5, 4, 0x21);
                font_text_add(D.txt[i * 20], sRiderBios_3[D.f1C[i]].f4);
                if (D.f3C == 1) {
                    if (D.flags & (0x400 << i)) {
                        font_draw_text(D.txt[i * 20], VP[0][0].x + VP[0][0].w / 2, VP[0][0].y + VP[0][0].h / 2 + 0x46);
                        font_text_create(D.txt[i * 20 + 1], 0x40, 5, 2, 0x21);
                        font_text_add(D.txt[i * 20 + 1], D_800DA150);
                        font_draw_text(D.txt[i * 20 + 1], VP[0][0].x + VP[0][0].w / 2, VP[0][0].y + VP[0][0].h / 2 + 0x53);
                    } else {
                        font_draw_text(D.txt[i * 20], VP[0][0].x + 0x7E, VP[0][0].y + 0xF + yo);
                    }
                } else if (gSplitVertical != 0) {
                    if (D.flags & (0x400 << i)) {
                        font_draw_text(D.txt[i * 20], VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 2,
                                      VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - 0x24);
                    } else {
                        font_draw_text(D.txt[i * 20], VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 2,
                                      VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2);
                    }
                } else {
                    s32 ty = i * 10 + 0x1C;

                    if (D.flags & (0x400 << i)) {
                        font_draw_text(D.txt[i * 20], VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 2,
                                      VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2 + ty);
                    } else {
                        font_draw_text(D.txt[i * 20], VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 4 - 10,
                                      VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2 + ty);
                    }
                }
                if (!(D.flags & (0x400 << i))) {
                    for (j = 1; j < 5; j++) {
                        font_text_add(D.txt[i * 20 + j], sRiderStatLabels[j - 1]);
                        font_draw_text(D.txt[i * 20 + j], bx - (j - 1) * 6, by + (j - 1) * 20);
                    }
                    if (D.f3C == 1) {
                        shell_draw_rider_bio(i, yo);
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    CMB_PRIM();
                    if (D.f3C == 1) {
                        j = 0;
                        DRAWV(VP[0][0].x, VP[0][0].y + yo, &D.p390[1], 6);
                        img = &D.p390[2];
                        DRAWV(VP[0][0].x, VP[0][0].y - img->h + yo, img, 6);
                        for (j = 0; j < 16; j++) {
                            if (str_icmp(sRiderBios_3[D.f1C[i]].f10, gRiderAnims_17[j].f18) == 0) {
                                break;
                            }
                        }
                        if (gRiderAnims_17[j].f14 != NULL) {
                            gdl = event_draw_glyph_string(gdl, fb, gRiderAnims_17[j].f14, 0x8C, yo + VP[0][0].y - 0x48);
                        }
                    }
                    gDPSetTextureLUT(gdl++, G_TT_NONE);
                    CMB_PRIM();
                    img = &D.p390[6];
                    for (j = 0; j < 4; j++) {
                        s32 ox;
                        s32 oy;

                        ox = j * 6 - 0x55;
                        n = (sRiderBios_3[D.f1C[i]].stats[j] + 1) * 2;
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
                if (!(D.flags & (0x400 << i))) {
                    img = &D.p390[7];
                    for (j = 0; j < 4; j++) {
                        nn = (sRiderBios_3[D.f1C[i]].stats[j] + 1) * 2;
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
                    img = &D.p390[10];
                    DRAWV(VP[gNumViews - 1][i].x, VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h - img->h, img, 6);
                }
                font_text_create(D.txt[i * 4], 0x40, 5, 4, 0x21);
                font_text_add(D.txt[i * 4], sRiderBios_3[D.f1C[i]].f4);
                {
                    s32 yy = VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2;
                    s32 xx = VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w / 2;
                    s32 ty2;

                    msk = 0x400 << i;
                    ty2 = yy + 0xA;
                    if (D.flags & msk) {
                        ty2 = yy + 0x23;
                    }
                    font_draw_text(D.txt[i * 4], xx, ty2);
                }
                gDPSetTextureLUT(gdl++, G_TT_RGBA16);
                CMB_TEX();
                if (!(D.flags & msk) && (gFrameCounter & 0x20)) {
                    gDPLoadTLUT_pal256(gdl++, D.p74C->data);
                    DRAWK(VP[gNumViews - 1][i].x + 5, VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2 + 3, &D.p74C[1], 3, u10);
                    DRAWK(VP[gNumViews - 1][i].x + VP[gNumViews - 1][i].w - 0x14,
                         VP[gNumViews - 1][i].y + VP[gNumViews - 1][i].h / 2 + 3, &D.p74C[2], 3, u11);
                }
            }
            if (D.f3C < 3) {
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
        if (D.f3C == 1) {
            if (D.f4C != 8) {
                font_text_create(D.txt[0], 0x40, 2, 2, 0x20);
                font_text_add(D.txt[0], D_800DA170);
                font_draw_text(D.txt[0], 0x16C, 0x25);
            }
            font_text_create(D.txt[1], 0x40, 2, 2, 0x20);
            font_text_add(D.txt[1], D_800DA184);
            font_draw_text(D.txt[1], 0x16C, 0x19);
            gDPSetTexturePersp(gdl++, G_TP_NONE);
            gDPSetTextureLUT(gdl++, G_TT_RGBA16);
            CMB_TEX();
            gDPSetRenderMode(gdl++, 0x00404240, 0);
            gDPLoadTLUT_pal256(gdl++, D.p3C8->data);
            if (D.f4C != 8) {
                DRAWK(0x140, 0x21, &D.p3C8[1], 3, u9);
                DRAW(0x154, 0x21, &D.p3C8[3], 3);
            }
            DRAWK(0x140, 0x13, &D.p3C8[4], 3, u7);
            DRAWK(0x154, 0x13, &D.p3C8[2], 3, u8);
        }
        gdl = font_render(gdl);
        break;
    }
    }
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    if (D.f54 != 3) {
        gdl = screen_draw_wipe(gdl, m, gGfxFrameIndex);
    }
    {
        Gfx *_g = gdl++;

        _g->words.w0 = 0xE7000000;
        _g->words.w1 = ((u16)u1 >> 16) + ((u16)u2 >> 16) + ((u16)u3 >> 16);
    }
    return gdl;
}

#define IMG(s) ((s)->type == 4 ? gGame_79.imgs[*(s)->idx] : (s)->img)

typedef struct {
    u8 pad0[0x54];
    s32 mode;           /* 0x54 */
    u16 fade;           /* 0x58 */
    s16 timer;          /* 0x5A */
    u8 pad5C[0x36C - 0x5C];
    Pic *imgs[1];       /* 0x36C */
} Glob_9;

typedef struct {
    u8 pad0[0xC];
    u8 *tbl;            /* 0x0C */
    u8 pad10[6];
    s16 type;           /* 0x16 */
    u8 pad18[4];
    s32 f1C;            /* 0x1C */
    s32 *idx;           /* 0x20 */
    Pic *img;           /* 0x24 */
} Obj_65;

typedef struct {
    s16 x, y, w, h;
} Rect_3;

extern Glob_9 gGame_79;
extern Rect_3 sTitleSceneRect;

void shell_draw(Mtx *m, s32 fb, Gfx **pgdl)
{
    u8 unk28[0x70];
    Mtx persp;
    u16 pn;
    Gfx *gdl;
    Obj_65 *s;
    u8 *tbl;
    u16 alpha;

    gdl = *pgdl;
    if (D_800EFDC4 == 0) {
        s = sPrevScreen;
    } else {
        s = sCurScreen;
    }
    if (s != NULL) {
        if (D_800EFDC4 == 0) {
            tbl = s->tbl + sPrevScreenCursor * 16;
        } else {
            tbl = s->tbl;
            if (s->idx != NULL) {
                tbl += *s->idx * 16;
            }
        }
    }
    alpha = 0xFF;
    if (gGame_79.mode == 1) goto c1;
    if (gGame_79.mode == 0) goto c0;
    if (gGame_79.mode == 2) goto c2;
    goto done;
c1:
    alpha = 0;
    goto done;
c0:
    alpha = gGame_79.fade;
    goto done;
c2:
    alpha = 0xFF - gGame_79.fade;
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
    if (D_800EFDC4 == 1 || s == NULL || s->f1C == 0) {
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
    if (gGame_79.mode == 1) {
        if (gGame_79.timer > 0 || gGame_79.timer == -1) {
            gGame_79.timer--;
        }
    } else {
        if (s->type == 4 || s->f1C != 0) {
            gDPLoadTLUT_pal256(gdl++, s->type == 4 ? gGame_79.imgs[*s->idx]->pal : s->img->pal);
            gdl = render_draw_image(0, 0, IMG(s)->img, gdl, fb, 1, 1, 3, 0);
        }
        if (s->type == 0) {
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
            gdl = shell_draw_screen(m, fb, gdl, tbl, 0);
        } else if (s != NULL) {
            gdl = shell_draw_screen(m, fb, gdl, tbl, 0);
            gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
            D_800CA80F_2[0] = alpha;
        }
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}

void shell_load_gui_res(void) {
    res_open_set("guires", gGuiRes_3);
}

void shell_free_gui_res(void)
{
    res_close_set(gGuiRes);
    gGuiRes = 0;
}

typedef struct {
    u32 w00;
    u8 pad[0x4C - 4];
    u32 w4C;
} Blk_19;

extern Blk_19 gGame_80;

void shell_cb_desert(s32 p) {
    if (p != 0) {
        gCurTrack = "Desert";
        gGame_80.w4C = 5;
        D_800CA924_5.b = 0;
        D_800CA924_5.a = 1;
        D_800CA924_5.c = 0;
        gGame_80.w00 &= 0xFFFFFF7F;
    }
}

void shell_cb_select_track(s32 a) {
    if (a != 0) {
        gCurTrack = gTracks_2 + gGame_4[4] * 0xA0;
        gGame_4[0] |= 0x80;
    }
}

void shell_cb_time_trials(s32 a0)
{
    u8 *base;

    if (a0 != 0) {
        base = gGame_2;
        *(s32 *)(base + 0x4C) = 8;
        *(s32 *)(base + 0x3C) = 1;
        *(s32 *)(base) = *(s32 *)(base) | 0x80;
    }
}

typedef struct {
    u32 flags;
    u8 pad0[0x18];
    s32 f1C;
    u8 pad1[0x1C];
    s32 f3C;
    u8 pad2[0xC];
    s32 f4C;
} Game_33;

typedef struct {
    u8 pad0[0x2C1];
    u8 track;
} Opts_2;

typedef struct {
    void *p0;
    u8 pad0[0x60];
    s16 f64;
} Obj_66;

extern Opts_2 gSaveData_14;
extern Game_33 gGame_81;
extern Obj_66 *gSeason_24;

void shell_cb_tutorial(s32 arg)
{
    s32 i;

    if (arg != 0) {
        gCurTrack = sTrackTutorial;
        gGame_81.f3C = 1;
        gGame_81.f4C = 9;
        D_800F0624 = 0;
        gGame_81.f1C = 4;
        gGame_81.flags &= ~0x80;
        gSeason_24->p0 = sTutorialRound;
        gSeason_24->f64 = 0;
        for (i = 0; i < 0x11; i++) {
            if (gTutorialLessons_2[i].id == gSaveData_14.track) {
                gSeason_24->f64 = i;
                return;
            }
        }
    }
}

void shell_cb_exhibition(s32 x)
{
    if (x != 0) {
        gGameMode = 0;
    }
}

typedef struct {
    u32 w00;
    u8 p04[0x38];
    u32 w3C;
    u8 p40[0x0C];
    u32 w4C;
    u8 p50[0x370];
    u32 w3C0;
    u8 p3C4[0x4C];
    u32 w410;
    u8 p414[0x28];
    u8 arr43C[6];
} Blk_20;

extern Blk_20 gGame_82;

void shell_cb_excite3d(s32 p) {
    s32 i;

    if (p != 0) {
        i = 0;
        do {
            if (i == 0) {
                gGame_82.arr43C[0] = (u8)gGame_82.w3C0;
            } else {
                gGame_82.arr43C[i] = i;
            }
            i++;
        } while (i < 6);
        gGame_82.w4C = 6;
        gCurTrack = "Excite 3D";
        gGame_82.w3C = 1;
        gGame_82.w410 = 2;
        gGame_82.w00 &= 0xFFFFFF7F;
    }
}

typedef struct {
    u32 flags;
    u8 pad04[0x38];
    s32 x3C;
    s32 x40;
    s32 x44;
    s32 x48;
    s32 x4C;
    u8 pad50[0x3C3 - 0x50];
    u8 x3C3;
    u8 pad3C4[0x43C - 0x3C4];
    u8 arr[6];
} G_4;

extern G_4 gGame_83;

void shell_cb_hill_climb(s32 p)
{
    s32 i;
    if (p != 0) {
        for (i = 0; i < 6; i++) {
            if (gGame_83.x3C < 2) {
                if (i == 0) {
                    gGame_83.arr[0] = gGame_83.x3C3;
                } else {
                    gGame_83.arr[i] = i;
                }
            } else {
                gGame_83.arr[i] = i;
            }
        }
        gGame_83.x44 = 0;
        gSplitVertical = 1;
        gGame_83.x4C = 7;
        gCurTrack = D_800CC070;
        gGame_83.flags &= ~0x80;
    }
}

void shell_cb_custom_tracks(s32 x)
{
    if (x != 0) {
        gGame = (gGame & ~0x80) | 0x20;
    }
}

void func_800880DC(s32 a0) {
    s32 i;
    s8 *b;
    u32 *w;
    if (a0 == 0) {
        return;
    }
    w = gGame_5;
    w[15] = 1;
    b = (s8 *)w;
    gCurTrack = D_800CC1B0;
    w[19] = 4;
    w[0] = w[0] & 0xFFFFFF7F;
    for (i = 0; i < 4; i++) {
        if (i == 0) {
            b[0x2C8] = 0;
        } else {
            b[i + 0x2C8] = -1;
        }
        if (i == 0) {
            b[0x2CC] = 0;
        } else {
            b[i + 0x2CC] = -1;
        }
    }
    D_800CA924[1] = 0;
    D_800CA924[0] = 1;
    D_800CA924[2] = 0;
}

void shell_cb_original_excitebike(void)
{
    gGame_4[19] = 12;
    gGame_4[0] = (gGame_4[0] & ~0x80) | 0x40;
}

void shell_cb_stunt_course(int param_1)
{
    if (param_1 != 0) {
        u32 *b = &gGame;
        b[0x13] = 10;
        gCurTrack = "Stunt Course";
        gGame = gGame & 0xffffff7f;
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

typedef struct {
    u8 p0[0x22];
    u16 h22;
    u8 p24[0x50 - 0x24];
    u8 b50[6];
} SrcA;

typedef struct {
    u8 p0[0x418];
    u16 h418;
    u8 b41A[6];
    u32 w420[6];
    u8 p438[0x764 - 0x438];
    u8 b764[6];
    u8 b76A[6];
} DstB;

typedef struct {
    u8 p0[0x669];
    u8 f669;
    u8 p66A[0x68C - 0x66A];
    u32 f68C;
    u8 pad[0x7E0 - 0x690];
} EntE;

extern SrcA gEvent_41;
extern DstB gGame_84;
extern EntE gRiders_92[];

void shell_backup_race_results(void)
{
    s32 i = 0;

    gGame_84.h418 = gEvent_41.h22;
    do {
        gGame_84.b41A[i] = gEvent_41.b50[i];
        gGame_84.w420[i] = gRiders_92[i].f68C;
        gGame_84.b76A[i] = gRiders_92[i].f669;
        if (gSeason != 0) {
            gGame_84.b764[i] = (gSeason + i)[0x6C];
        }
        i++;
    } while (i < 6);
}

typedef struct {
    u8 pad0[0x668];
    u16 h668;
    u8 pad1[0x68C - 0x66A];
    u32 w68C;
    u8 pad2[0x7E0 - 0x690];
} Elem_4;

typedef struct {
    u8 pad0[0x418];
    u16 h418;
    u8 b41A[6];
    u32 w420[6];
    u8 pad1[0x764 - 0x438];
    u8 b764[6];
    u8 b76A[6];
} Src_3;

typedef struct {
    u8 pad0[0x22];
    u16 h22;
    u8 pad1[0x50 - 0x24];
    u8 b50[6];
} Info_3;

typedef struct {
    u8 pad0[0x6C];
    u8 b6C[6];
} Out_4;

extern Info_3 gEvent_42;
extern Src_3 gGame_85;
extern Elem_4 gRiders_93[];
extern Out_4 *gSeason_25;

void shell_restore_race_results(void)
{
    s32 i;

    gEvent_42.h22 = gGame_85.h418;
    for (i = 0; i < 6; i++) {
        gEvent_42.b50[i] = gGame_85.b41A[i];
        gRiders_93[i].w68C = gGame_85.w420[i];
        gRiders_93[i].h668 = gGame_85.b76A[i];
        if (gSeason_25 != 0) {
            gSeason_25->b6C[i] = gGame_85.b764[i];
        }
    }
}

typedef struct {
    u8 pad0[0x4C];
    s32 f4C;
    u8 pad1[0x410 - 0x50];
    s32 f410;
} State_10;

struct Obj_67 {
    u8 pad0[0x14];
    s16 h14;
};

extern State_10 gGame_86;

s32 shell_is_round_screen_unlocked(Obj_67 *o)
{
    s32 mode;
    s32 t;

    if (gGame_86.f4C == 8 || gGame_86.f4C == 0) {
        mode = D_800F1BC0;
    } else {
        mode = gGame_86.f410;
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
    t = o->h14 < 10;
tail:
    if (t == 0) {
        goto ret0;
    }
ret1:
    return 1;
two:
    t = o->h14 < 0xF;
    goto tail;
ret0:
    return 0;
}

void shell_main_menu_or_back_input(u8 *p, s32 x, u32 *f, void *s)
{
    s32 n;
    void *q;

    if ((*f & 0x50) != 0) {
        shell_goto_screen(sScreenMainMenu, 0, 1, s);
        n = 1;
    } else {
        if ((*f & 0x20) == 0) {
            return;
        }
        q = *(void **)(p + 0x28);
        if (q == 0) {
            return;
        }
        shell_goto_screen(q, 0, 0, s);
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
