/* The game's setup and the game states: gGame (the setup that carries across states, cleared by shell_init) and
   the state table of main.c (gGameStates, gCurGameState). */
#ifndef STRUCTS_GAME_H
#define STRUCTS_GAME_H

#include "common.h"
#include "structs/race.h"

/* flags: 0x1 a race run by the front end (title, rider select), 0x2 leaving the editor returns to the front end,
   0x4 run the same game state again, 0x10 rider select, 0x20 editor, 0x40 NES, 0x80 track chosen, 0x8000 replay
   playback, 0x40000 podium ceremony (gPodiumMode), 0x100000/0x400000/0x800000 unlock messages (0x800000: the next
   race is the credits run), 0x20000000 the Debug cheat.
   mode (gGameMode): 0 exhibition, 1 season, 2 soccer, 5 desert, 6 Excite 3D, 7 hill climb, 8 time trial, 9 tutorial,
   10 stunt course, 12 Original Excitebike, 13 and 14 the editor's test ride. */
typedef struct Game {
    /* 0x000 */ u32 flags;
    /* The front end's cursors (a screen's item list points at its own: sScreens +0x20) */
    /* 0x004 */ s32 mainMenuSel;
    /* 0x008 */ s32 specialTracksSel;
    /* 0x00C */ s32 cupSel;             /* season: the cup (D_800CA9C8) */
    /* 0x010 */ s32 trackSel;           /* track lists: the track */
    /* 0x014 */ s32 trackListSel;       /* track lists: the cursor on the page */
    /* 0x018 */ s32 unk_018;
    /* 0x01C */ s32 riders[8];          /* the rider of each racer */
    /* 0x03C */ s32 numPlayers;
    /* 0x040 */ s32 numLaps;
    /* 0x044 */ s32 numCpuRiders;
    /* 0x048 */ s16 unk_048;            /* a 10-frame animation of the front end */
    /* 0x04A */ s16 unk_04A;
    /* 0x04C */ u32 mode;               /* gGameMode */
    /* 0x050 */ u8 padConfig[4];        /* the button layout of each controller (A-D) */
    /* 0x054 */ u32 fadeState;          /* front end fade: 0 out, 1 swap, 2 in, 3 done */
    /* 0x058 */ s16 fadeLevel;
    /* 0x05A */ s16 fadeTimer;
    /* 0x05C */ u8 unk_05C[0x64 - 0x5C];
    /* 0x064 */ s32 cupPreviewTimer;    /* season menu: shows each track of the cup for 60 frames */
    /* 0x068 */ s32 cupPreviewTrack;
    /* 0x06C */ s32 roundSel;
    /* 0x070 */ u32 seasonMenuState;
    /* 0x074 */ s32 *texts[48];         /* text objects (font_text_create) */
    /* 0x134 */ s16 customTrackSel;     /* custom cup: the track picker */
    /* 0x136 */ s16 unk_136;
    /* 0x138 */ s16 customTrackPrev;
    /* 0x13A */ u8 unk_13A[2];
    /* 0x13C */ u16 *greyPalette;       /* shell_blend_palette_to_gray */
    /* 0x140 */ u8 unk_140[0x17C - 0x140];
    /* 0x17C */ s32 menuAnimTimer;      /* 0-59 */
    /* 0x180 */ u8 unk_180[0x29C - 0x180];
    /* 0x29C */ s32 unk_29C;
    /* 0x2A0 */ s32 unk_2A0;
    /* 0x2A4 */ u8 unk_2A4;
    /* 0x2A5 */ u8 unk_2A5;
    /* 0x2A6 */ u8 unk_2A6[2];
    /* 0x2A8 */ s32 riderSelectRot[4][2]; /* rider select: each player's turning rider (angle, unused) */
    /* 0x2C8 */ s8 playerPad[4];        /* the controller of each player */
    /* 0x2CC */ s8 padPlayer[4];        /* the player of each controller (-1 none) */
    /* 0x2D0 */ u8 unk_2D0[0x2E0 - 0x2D0];
    /* 0x2E0 */ s16 riderSelectTimer[4];
    /* 0x2E8 */ u32 screenState;        /* the current screen's own state */
    /* 0x2EC */ u8 unk_2EC[0x2F4 - 0x2EC];
    /* GUI image sets (res_load of gGuiRes): entry 0 holds the palette, the images follow */
    /* 0x2F4 */ Img *itemPics[24];      /* the pictures of the tracks or menu items */
    /* 0x354 */ u8 unk_354[0x36C - 0x354];
    /* 0x36C */ Pic *screenPics[7];
    /* 0x388 */ Img *unk_388;
    /* 0x38C */ Img *unk_38C;
    /* 0x390 */ Img *unk_390;
    /* 0x394 */ Img *unk_394;
    /* 0x398 */ Img *unk_398;
    /* 0x39C */ Img *unk_39C;
    /* 0x3A0 */ Img *unk_3A0;
    /* 0x3A4 */ Img *cursorImg;         /* 30 frames */
    /* 0x3A8 */ Img *unk_3A8;
    /* 0x3AC */ u8 *cursorPal;
    /* 0x3B0 */ Img *unk_3B0;
    /* 0x3B4 */ u8 unk_3B4[4];
    /* 0x3B8 */ u32 savedMode;          /* rider select: the mode and track to return to */
    /* 0x3BC */ void *savedTrack;
    /* 0x3C0 */ s32 riderSkinSel;
    /* 0x3C4 */ s32 bikeSkinSel;
    /* 0x3C8 */ Img *unk_3C8;
    /* 0x3CC */ u8 unk_3CC[0x3DC - 0x3CC];
    /* 0x3DC */ Img *unk_3DC;
    /* 0x3E0 */ u8 unk_3E0[4];
    /* 0x3E4 */ s32 pakMenuSel;
    /* 0x3E8 */ u8 *userTrack;          /* a packed user track (0x400 bytes) for the editor or the Controller Pak */
    /* 0x3EC */ s32 blinkTimer;         /* 0-119 */
    /* 0x3F0 */ u8 unk_3F0[0x3FC - 0x3F0];
    /* 0x3FC */ void *returnScreen;     /* the screen to return to after a race */
    /* 0x400 */ s32 menuSel;
    /* 0x404 */ Img *unk_404;
    /* 0x408 */ s16 riderSelectAnim[2];
    /* 0x40C */ u8 riderSelectState[4];
    /* 0x410 */ s32 difficulty;
    /* 0x414 */ Img *unk_414;
    /* The last race's results, kept for the results screens (shell_backup_race_results) */
    /* 0x418 */ u16 resultsNumRiders;
    /* 0x41A */ u8 resultsOrder[6];
    /* 0x420 */ s32 resultsTimes[6];
    /* 0x438 */ Img *unk_438;
    /* 0x43C */ u8 riderSkins[6];       /* the rider texture of each racer (render_load_rider) */
    /* 0x442 */ u8 unk_442[2];
    /* 0x444 */ u16 *playerPalettes;    /* 4 palettes of 0x100 colours */
    /* 0x448 */ s16 optionsSel;         /* 0x100: just entered */
    /* 0x44A */ s16 numRecordTracks;
    /* 0x44C */ s16 *recordTracks;      /* the tracks that keep records */
    /* 0x450 */ s32 recordsSel;
    /* Text entry (initials, cheat codes) */
    /* 0x454 */ s16 keyIndex;
    /* 0x456 */ s16 keyCursorX;
    /* 0x458 */ s16 keyCursorY;
    /* 0x45A */ u8 unk_45A[2];
    /* 0x45C */ Img *unk_45C;
    /* 0x460 */ s32 entryRecord;        /* the record whose initials are entered */
    /* 0x464 */ s16 nameLen;
    /* 0x466 */ char name[16];
    /* 0x476 */ s16 padConfigSel;       /* controller settings: the controller */
    /* 0x478 */ Img *unk_478;
    /* 0x47C */ s16 dialogSel;          /* yes/no */
    /* 0x47E */ u8 unk_47E[2];
    /* 0x480 */ Img *unk_480;
    /* 0x484 */ Img *unk_484;
    /* 0x488 */ s16 pakScreenMode;
    /* 0x48A */ u8 unk_48A[0x490 - 0x48A];
    /* 0x490 */ TrackRecord records[24]; /* the tracks that keep records, in order */
    /* 0x730 */ struct Font *font;      /* the race HUD's glyph font */
    /* 0x734 */ u8 playerInitials[4][3];
    /* 0x740 */ s32 idleFrames;         /* the front end times out after 0xE10 */
    /* 0x744 */ Img *unk_744;
    /* 0x748 */ Img *unk_748;
    /* 0x74C */ Img *unk_74C;
    /* 0x750 */ s32 screenTimer;
    /* 0x754 */ u32 newUnlocks;         /* save_update_unlocks */
    /* 0x758 */ s8 recordPlayer[4];     /* the player who set each record of the race (-1 none) */
    /* 0x75C */ s32 twoPlayerCpuRiders;
    /* 0x760 */ u8 padCamMode[4];       /* the camera mode of each controller */
    /* 0x764 */ u8 resultsSeasonOrder[6];
    /* 0x76A */ u8 resultsCelebration[6];
    /* 0x770 */ s32 repeatTimer;        /* options: key repeat */
    /* 0x774 */ s32 optionsSfx;
    /* 0x778 */ s32 savedRiders[6];     /* the riders and skins before the podium */
    /* 0x790 */ s32 savedRiderSkins[6];
    /* 0x7A8 */ u8 unk_7A8;
    /* 0x7A9 */ u8 cheatEntered;        /* sCheatTable */
    /* 0x7AA */ u8 bikeSkins[6];        /* the bike of each racer: bit 0 model, bits 1-2 palette */
} Game; /* size 0x7B0 */

/* An entry of gGameStates: 0 front end (shell.c), 1 race (min.c), 2 track editor (edit.c), 3 NES (nesn64.c). */
typedef struct GameState {
    /* 0x00 */ void (*init)(s32 arg);
    /* 0x04 */ void (*update)(u32 *pads);   /* gPadInput */
    /* 0x08 */ void (*draw)(void *gfxCtx, s32 fb, Gfx **pgdl);
    /* 0x0C */ void (*exit)(s32 arg);
    /* 0x10 */ s32 mode;                    /* 0 needs init, 2 running, 3 exit requested, 4 exiting, 5 waiting */
    /* 0x14 */ s32 dlSize;                  /* display list length in packets */
    /* 0x18 */ s32 wait;                    /* mode 5: frames to wait for the GPU */
} GameState; /* size 0x1C */

#endif
