#include "game.h"

/* The game's pad word (main_convert_pad_input): the stick's directions, the buttons, and the stick's magnitudes
   (0-15) in bits 24-27 (x) and 28-31 (y). */
#define PAD_STICK_UP    0x00000001
#define PAD_STICK_DOWN  0x00000002
#define PAD_STICK_LEFT  0x00000004
#define PAD_STICK_RIGHT 0x00000008
#define PAD_A           0x00000010
#define PAD_B           0x00000020
#define PAD_START       0x00000040
#define PAD_C_UP        0x00000080
#define PAD_C_DOWN      0x00000100
#define PAD_C_LEFT      0x00000200
#define PAD_C_RIGHT     0x00000400
#define PAD_D_UP        0x00000800
#define PAD_D_DOWN      0x00001000
#define PAD_D_LEFT      0x00002000
#define PAD_D_RIGHT     0x00004000
#define PAD_L           0x00008000
#define PAD_R           0x00010000
#define PAD_Z           0x00020000

/* snd_play_menu_sound */
#define MENU_SFX_MOVE   0
#define MENU_SFX_SELECT 1
#define MENU_SFX_BACK   2
#define MENU_SFX_ERROR  3

/* SaveData.options: a user track is saved on the Game Pak */
#define SAVE_HAS_USER_TRACK 8

extern EditCamPos sEditCamDelta;
extern EditCamPos sEditCamPosFixed; /* 1/1024 */
extern EditCamPos sEditCamPos;
extern EditCamPos sEditCamTarget;
extern GuiMenu sEditMainMenu;
extern GuiMenu sEditTrackMenu;
extern GuiMenuItem sEditMainMenuItems[];
extern GuiMenuItem sEditTrackMenuItems[];

/* Makes the first piece that can start the track the start piece, if there is none. */
static inline void edit_pick_start_piece(void) {
    EditPiece *piece;

    if (gEditor->start == NULL) {
        piece = gEditor->first;
        while (piece != NULL) {
            if (piece->type->flags & PIECE_START) {
                gEditor->start = piece;
                break;
            }
            piece = piece->next;
            if (piece == gEditor->first) {
                break;
            }
        }
    }
}

/* One frame of the camera's eased move to sEditCamTarget (in 1/1024 units); at the end it lands there, the yaw rounded
   to an eighth turn. Declared s32 with no return value, as the original's code needs. */
s32 edit_cam_step(void)
{
    sEditCamPosFixed.x += edit_ease_delta(1, sEditCamDelta.x, EDIT_CAM_STEPS, sEditCamStep);
    sEditCamPosFixed.z += edit_ease_delta(1, sEditCamDelta.z, EDIT_CAM_STEPS, sEditCamStep);
    sEditCamPosFixed.yaw += edit_ease_delta(2, sEditCamDelta.yaw, EDIT_CAM_STEPS, sEditCamStep);
    sEditCamPos.x = sEditCamPosFixed.x >> 10;
    sEditCamPos.z = sEditCamPosFixed.z >> 10;
    sEditCamPos.yaw = (u16)(sEditCamPosFixed.yaw >> 10);
    sEditCamStep++;
    if (sEditCamStep == EDIT_CAM_STEPS) {
        sEditCamMoving = 0;
        sEditCamPos.x = sEditCamTarget.x;
        sEditCamPos.z = sEditCamTarget.z;
        sEditCamPos.yaw = ((u32)(sEditCamPos.yaw + 0x800) >> 13) << 13;
    }
}

/* Aims the camera at the current piece (the piece next to a preview piece), its yaw along the piece's cursor angle
   (eighths of a turn, turned round when reversed or for a C button turn), and starts the move if anything changed. */
void edit_cam_update_target(void)
{
    EditPiece *cur;
    EditPiece *piece;
    s32 angle;
    EditCamPos *target;
    s32 flipped;

    cur = gEditor->cur;
    if (cur != NULL && (cur->flags & EDIT_PIECE_PREVIEW) && (cur->next != NULL || cur->prev != NULL) && gEditor->mode != EDIT_MODE_USER_PIECE) {
        piece = cur->next;
        if (piece == NULL) {
            piece = cur->prev;
        }
    } else {
        piece = gEditor->cur;
    }
    if (piece != NULL && (gEditor->mode == EDIT_MODE_USER_PIECE || !(piece->flags & EDIT_PIECE_PREVIEW))) {
        angle = piece->type->cursorAngle;
        angle -= piece->rot << 1;
        if (flipped = piece->flags & EDIT_PIECE_FLIPPED, target = &sEditCamTarget, flipped) {
            angle += 4;
        }
        if (gEditor->flags & EDIT_REVERSED) {
            angle += 4;
        }
        target->yaw = angle & 7;
    } else {
        sEditCamTarget.yaw = (gEditor->flags & EDIT_REVERSED) << 2;
    }
    sEditCamTarget.yaw = (sEditCamTarget.yaw - (sEditCamPos.yaw >> 13)) & 7;
    /* the shorter way round */
    switch (sEditCamTarget.yaw) {
    case 5:
        sEditCamTarget.yaw = -3;
        break;
    case 6:
        sEditCamTarget.yaw = -2;
        break;
    case 7:
        sEditCamTarget.yaw = -1;
        break;
    }
    if (gEditor->flags & EDIT_TURN_LEFT) {
        sEditCamTarget.yaw -= 4;
        gEditor->flags ^= EDIT_TURN_LEFT | EDIT_REVERSED;
    }
    if (gEditor->flags & EDIT_TURN_RIGHT) {
        sEditCamTarget.yaw += 4;
        gEditor->flags ^= EDIT_TURN_RIGHT | EDIT_REVERSED;
    }
    if (gEditor->cur != NULL) {
        sEditCamTarget.x = piece->x * CELL_MM + piece->type->dim[piece->rot & 1] * (CELL_MM / 2) - gEditor->gridSize[0] * (CELL_MM / 2);
        sEditCamTarget.z = piece->y * CELL_MM + piece->type->dim[(piece->rot + 1) & 1] * (CELL_MM / 2) - gEditor->gridSize[1] * (CELL_MM / 2);
    } else {
        sEditCamTarget.x = -4000;
        sEditCamTarget.z = -4000;
    }
    if (sEditCamTarget.x != sEditCamPos.x || sEditCamTarget.z != sEditCamPos.z || sEditCamTarget.yaw != 0) {
        sEditCamMoving = 1;
        sEditCamStep = 0;
        sEditCamDelta.x = sEditCamTarget.x - sEditCamPos.x;
        sEditCamDelta.z = sEditCamTarget.z - sEditCamPos.z;
        sEditCamDelta.yaw = sEditCamTarget.yaw << 13;
        sEditCamPosFixed.x = sEditCamPos.x << 10;
        sEditCamPosFixed.z = sEditCamPos.z << 10;
        sEditCamPosFixed.yaw = sEditCamPos.yaw << 10;
    }
}

/* Game state 2: allocates the editor (kept when back from the Controller Pak screen), loads the "Pieces" resources and
   the piece types' meshes, sets up the grid, the views, the cameras and the user pieces, and takes a track handed
   over in gGame.userTrack. */
void edit_init(void)
{
    s32 i, j;
    u32 cell, vtx;
    PieceMesh *mesh;
    s32 v;
    s32 idx, view, x, y, roundUp, quot;

    gCurTrack = &gTracks[TRACK_USER];
    if (gEditor == NULL || !(gEditor->flags & (EDIT_PAK_LOAD | EDIT_PAK_SAVE))) {
        gEditor = mem_alloc_dbg(sizeof(Editor), 0, sStrEditFile, 0x50A);
        mem_fill((u8 *)gEditor, 0, sizeof(Editor));
    } else {
        /* back from the Controller Pak screen */
        gGame.flags &= ~2;
        gEditor->flags &= ~EDIT_EXIT;
    }
    shell_load_gui_res();
    prim_init();
    if (res_open_set(sStrEditResource, &gResourceSet) != 0) {
        fatal_halt(1);
    }
    font_load_menu_set(gResourceSet);
    gEditor->first = 0;
    gEditor->last = 0;
    gEditor->cur = 0;
    gEditor->start = 0;
    gEditor->scrollX = 0;
    gEditor->scrollY = 0;
    for (idx = EDIT_MAX_PIECES - 1; idx >= 0; idx--) {
        gEditor->pieces[idx].used = 0;
    }
    strcpy(gEditor->name, "<NONE>");
    res_register_type(sStrEditArn, arn_relocate);
    res_register_type(sStrEditN64, prim_n64_relocate);
    res_open_set(sStrEditPieces, &gEditor->pieceRes);
    gEditor->cursorTex = res_load(gEditor->pieceRes, sEditCursorTexResId);
    gEditor->cursorModel = res_load(gEditor->pieceRes, sEditCursorModelResId);
    gEditor->objectTex = res_load(gEditor->pieceRes, gPieceObjectTexResId);
    gEditor->wallTex = res_load(gEditor->pieceRes, *gEditArena.wallTexId);
    gEditor->res170C = res_load(gEditor->pieceRes, D_800D1274);
    if (!(gEditor->flags & (EDIT_PAK_LOAD | EDIT_PAK_SAVE))) {
        if (gUserTrackLayout != NULL) {
            mem_free(gUserTrackLayout);
            gUserTrackLayout = NULL;
        }
    }
    for (view = 0; view < 4; view++) {
        gEditor->views[view].vp.vscale[0] = sEditViewRects[view].w * 2;
        gEditor->views[view].vp.vscale[1] = sEditViewRects[view].h * 2;
        gEditor->views[view].vp.vtrans[0] = (sEditViewRects[view].x + sEditViewRects[view].w / 2) * 4;
        gEditor->views[view].vp.vtrans[1] = (sEditViewRects[view].y + sEditViewRects[view].h / 2) * 4;
    }
    gEditor->screenVp.vp.vscale[0] = 1000;
    gEditor->screenVp.vp.vscale[1] = 320;
    gEditor->screenVp.vp.vscale[2] = G_MAXZ / 2;
    gEditor->screenVp.vp.vtrans[0] = (gScreenWidth / 2) * 4;
    gEditor->screenVp.vp.vtrans[1] = (gScreenHeight / 2) * 4;
    gEditor->screenVp.vp.vtrans[2] = G_MAXZ / 2;
    if (!(gEditor->flags & (EDIT_PAK_LOAD | EDIT_PAK_SAVE))) {
        gEditor->slideState = EDIT_SLIDE_SHOWN;
        gEditor->slideTimer = 0;
        gEditor->menu = &sEditMainMenu;
        gEditor->mode = EDIT_MODE_IDLE;
    }
    gEditor->gridSize = &gEditArena.w;
    gEditor->cur = 0;
    gEditor->selType = gPieceTypes;
    gEditor->markStart = 0;
    gEditor->markEnd = 0;
    gEditor->start = 0;
    gEditor->first = gEditor->cur;
    gEditor->last = gEditor->cur;
    gEditor->bigText = font_text_create(0, 0x40, 2, 2, 0x21);
    gEditor->text = font_text_create(0, 0x20, 2, 2, 0x21);
    edit_update_piece_ports(gEditor->cur);
    gEditor->page = EDIT_PAGE_MAIN;
    for (i = 0; i < PIECE_TYPE_COUNT; i++) {
        gPieceTypes[i].flags |= PIECE_FITS;
        if (gPieceTypes[i].flags & PIECE_FLIPPABLE) {
            gPieceTypes[i].flags |= PIECE_FITS_FLIPPED;
        } else {
            gPieceTypes[i].flags &= ~PIECE_FITS_FLIPPED;
        }
        if (gPieceTypes[i].meshAId == NULL) continue;
        gPieceTypes[i].meshA = (PieceMeshSet *)res_load(gEditor->pieceRes, *gPieceTypes[i].meshAId);
        if (gPieceTypes[i].meshBId != NULL) {
            gPieceTypes[i].meshB = (PieceMeshSet *)res_load(gEditor->pieceRes, *gPieceTypes[i].meshBId);
        } else {
            gPieceTypes[i].meshB = NULL;
        }
        /* the ground's vertices snapped to multiples of 50, rounding up from a remainder of 15 */
        for (cell = 0; cell < gPieceTypes[i].meshA->count; cell++) {
            mesh = gPieceTypes[i].meshA->cells[cell].mesh;
            for (vtx = 0; vtx < mesh->vtxCount; vtx++) {
                for (j = 0; j < 3; j += 2) {
                    v = (s16)mesh->vtx[vtx].v.ob[j];
                    if (v < 0) { s32 neg = -v; s32 quot2 = neg / 50; s32 roundUp2 = neg - quot2 * 50 >= 15; v = quot2 + roundUp2; v = -(v * 50); }
                    else { quot = v / 50; roundUp = v - quot * 50 >= 15; v = quot + roundUp; v = v * 50; }
                    mesh->vtx[vtx].v.ob[j] = v;
                }
            }
        }
    }
    arned_init_grid(gEditor->gridSize[0], gEditor->gridSize[1]);
    for (y = 0; y < gEditor->gridSize[1]; y++) {
        for (x = 0; x < gEditor->gridSize[0]; x++) {
            arned_set_cell(x, y, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    arned_set_viewport(sEditViewRects[EDIT_VIEW_TRACK].x, sEditViewRects[EDIT_VIEW_TRACK].y, sEditViewRects[EDIT_VIEW_TRACK].w - 1,
                       sEditViewRects[EDIT_VIEW_TRACK].h - 1);
    gEditor->cam[0].dist = 90000;
    gEditor->cam[1].dist = 45000;
    sEditCamTarget.yaw = 0;
    sEditCamPos.yaw = 0;
    {
        s32 fl = gEditor->flags;
        s32 pak = fl & (EDIT_PAK_LOAD | EDIT_PAK_SAVE);
        if (pak != 0) {
            gEditor->flags = pak;
            if (fl & EDIT_PAK_LOAD) {
                sEditMainMenu.choice = EDIT_MAIN_LOAD;
                sEditMainMenu.cursor = EDIT_MAIN_LOAD;
            } else {
                sEditMainMenu.choice = EDIT_MAIN_SAVE;
                sEditMainMenu.cursor = EDIT_MAIN_SAVE;
            }
        } else {
            sEditMainMenu.choice = EDIT_MAIN_EDIT;
            sEditMainMenu.cursor = EDIT_MAIN_EDIT;
            gEditor->flags = 0;
        }
    }
    edit_calc_track_cam_elevation();
    edit_calc_piece_cam_elevation();
    gEditor->userPieceCount = USER_PIECE_COUNT;
    gEditor->lineVtx[0] = mem_alloc_dbg(EDIT_MAX_LINES * 4 * sizeof(Vtx), 0, sStrEditFile, 0x5E1);
    gEditor->lineVtx[1] = mem_alloc_dbg(EDIT_MAX_LINES * 4 * sizeof(Vtx), 0, sStrEditFile, 0x5E2);
    gEditor->userPiece = 0;
    edit_init_user_pieces();
    edit_user_piece_reset_cursor(0);
    gEditor->userPieceCount = USER_PIECE_COUNT;
    gui_init();
    gEditor->warnFrame = res_load(gGuiRes, gWarnFrameResId);
    gEditor->warnIcon = res_load(gGuiRes, gWarnTitleBarResId);
    if (gGame.userTrack != NULL) {
        gEditor->flags &= ~(EDIT_PAK_LOAD | EDIT_PAK_SAVE);
        usrtrk_load_into_editor((u8 *)gGame.userTrack + 4); /* after the checksum */
        mem_free(gGame.userTrack);
        gGame.userTrack = NULL;
        edit_try_close_loop(gEditor->last);
    }
    edit_cam_update_target();
    sEditCamPos.x = sEditCamTarget.x;
    sEditCamPos.z = sEditCamTarget.z;
    sEditCamPos.yaw = ((u32)(sEditCamPos.yaw + 0x800) >> 13) << 13;
    sEditCamMoving = 0;
    arned_load_piece_textures(gEditor->dirtType, gEditor->objectTex, gEditor->wallTex);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF | OS_VI_GAMMA_DITHER_OFF | OS_VI_DIVOT_OFF | OS_VI_DITHER_FILTER_OFF);
}

/* Frees the editor's resources (the editor itself unless it is coming back from the Controller Pak screen); when
   leaving to test the track, keeps its layout and packed copy for the race. */
void edit_exit(void)
{
    s32 testPieces;
    s32 layoutSize;

    if (gEditor->flags & EDIT_TEST_TRACK) {
        testPieces = gUserTrackLayout->pieceCount;
    } else {
        testPieces = -1;
    }
    res_close_set(gResourceSet);
    prim_exit();
    arned_free_grid();
    mem_free(gEditor->lineVtx[0]);
    mem_free(gEditor->lineVtx[1]);
    res_close_set(gEditor->pieceRes);
    mem_free(gEditor->text);
    mem_free(gEditor->bigText);
    usrtrk_free_user_piece_meshes(1);
    if (!(gEditor->flags & (EDIT_PAK_LOAD | EDIT_PAK_SAVE))) {
        gRaceExitCode = 6;
        mem_free(gEditor);
        gEditor = 0;
    }
    shell_free_gui_res();
    gui_free();
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF | OS_VI_GAMMA_DITHER_OFF | OS_VI_DIVOT_OFF | OS_VI_DITHER_FILTER_OFF);
    if (testPieces >= 0) {
        /* the packed track and the layout move through the z-buffer to fresh allocations */
        __builtin_memcpy(gZBuffer, gGame.userTrack, USER_TRACK_BUF_SIZE);
        layoutSize = (testPieces - 1) * 0xc + 0x110; /* sizeof(UserTrackLayout) with testPieces pieces */
        memcpy(gZBuffer + 0x400, gUserTrackLayout, layoutSize);
        mem_free(gUserTrackLayout);
        mem_free(gGame.userTrack);
        gUserTrackLayout = mem_alloc_dbg(layoutSize, 0, sStrEditFile, 0x651);
        gGame.userTrack = mem_alloc_dbg(USER_TRACK_BUF_SIZE, 0, sStrEditFile, 0x652);
        memcpy(gUserTrackLayout, gZBuffer + 0x400, layoutSize);
        __builtin_memcpy(gGame.userTrack, gZBuffer, USER_TRACK_BUF_SIZE);
    }
    font_unload_set();
}

/* Newly pressed this frame (the different PRESSED macros compare different copies of the pad word) */
#define PRESSED(b) ((gPadInput_3[0] & (b)) && !(gPadInputPrev & (b)))

/* The name with its terminator, copied as one block */
typedef struct { u8 c[EDIT_NAME_LEN + 1]; } NameBuf;

/* Name entry: the cursor moves over the keyboard (stick, D-pad); A types the key (or deletes, or takes the name with
   OK), B deletes the last letter. */
void edit_update_name_entry(Editor *editor, s32 flags)
{
    s32 i;
    s32 oldKey;
    s32 lo, hi, lo2, hi2;

    /* the stick moves the cursor only when pushed past 6 */
    for (i = 0; i < 4; i++) {
        if ((((s32)gPadInput_3[i] >> 24) & 0xF) < 6) {
            gPadInput_3[i] &= ~(PAD_STICK_LEFT | PAD_STICK_RIGHT);
        }
        if ((s32)(gPadInput_3[i] >> 28) < 6) {
            gPadInput_3[i] &= ~(PAD_STICK_UP | PAD_STICK_DOWN);
        }
    }

    if (PRESSED(PAD_A)) {
        if (gEditor->nameKey == EDIT_KEY_DEL && gEditor->nameLen == 0) {
            snd_play_menu_sound(MENU_SFX_ERROR);
        } else {
            snd_play_menu_sound(MENU_SFX_SELECT);
        }
        {
            Editor *ed = gEditor;
            if (ed->nameKey == EDIT_KEY_DEL) {
                s32 n = ed->nameLen;
                if (n != 0) {
                    ed->nameBuf[n] = 0;
                    ed->nameLen = n - 1;
                }
            } else if (ed->nameKey == EDIT_KEY_OK) {
                *(NameBuf *)ed->name = *(NameBuf *)ed->nameBuf;
                gEditor->mode = EDIT_MODE_IDLE;
                gEditor->menu->flags |= GUI_MENU_ACTIVE;
            } else {
                ed->nameLen++;
                if (ed->nameLen >= EDIT_NAME_LEN) {
                    ed->nameCursorX = EDIT_KEY_AREA - 1;
                    ed->nameCursorY = EDIT_KEY_AREA - 1;
                    ed->nameKey = EDIT_KEY_OK;
                }
            }
        }
        return;
    }
    if (PRESSED(PAD_B)) {
        if (gEditor->nameLen == 0) {
            snd_play_menu_sound(MENU_SFX_ERROR);
            return;
        }
        snd_play_menu_sound(MENU_SFX_SELECT);
        gEditor->nameBuf[gEditor->nameLen - 1] = gEditor->nameBuf[gEditor->nameLen];
        {
            Editor *ed = gEditor;
            s32 n = ed->nameLen;
            ed->nameBuf[n--] = 0;
            ed->nameLen = n;
        }
        return;
    }

    {
        s32 y;
        /* y holds gEditor first, then the cursor's y */
        y = (s32)gEditor;
        oldKey = ((Editor *)y)->nameKey;
        if (((Editor *)y)->nameLen < EDIT_NAME_LEN) {
            Editor *ed;
            u32 pad;
            s32 x, newX;
            lo = 0;
            lo2 = lo;
            hi = EDIT_KEY_AREA;
            pad = gPadInput_3[0];
            ed = (Editor *)y;
            hi2 = hi;
            x = ed->nameCursorX;
            if (pad & (PAD_STICK_LEFT | PAD_STICK_RIGHT)) {
                if (pad & PAD_STICK_RIGHT) {
                    newX = x + (((s32)pad >> 24) & 0xF);
                } else {
                    newX = x - (((s32)pad >> 24) & 0xF);
                }
            } else {
                newX = x;
            }
            ed->nameCursorX = newX;
            if (PRESSED(PAD_D_LEFT)) {
                gEditor->nameCursorX -= EDIT_KEY_W;
            }
            if (PRESSED(PAD_D_RIGHT)) {
                gEditor->nameCursorX += EDIT_KEY_W;
            }
            {
                Editor *e = gEditor;
                s32 c = e->nameCursorX;
                if (c >= hi) {
                    e->nameCursorX = lo;
                } else if (c < lo) {
                    e->nameCursorX = hi - 1;
                }
            }
            {
                Editor *e = gEditor;
                u32 pad2 = gPadInput_3[0];
                s32 newY;
                y = e->nameCursorY;
                if (pad2 & (PAD_STICK_UP | PAD_STICK_DOWN)) {
                    if (pad2 & PAD_STICK_UP) {
                        newY = y - ((s32)((pad2 >> 28) * 3) >> 1);
                    } else {
                        newY = y - (-(s32)(pad2 >> 28) * 3) / 2;
                    }
                } else {
                    newY = y;
                }
                e->nameCursorY = newY;
            }
            if (PRESSED(PAD_D_UP)) {
                gEditor->nameCursorY -= EDIT_KEY_H;
            }
            if (PRESSED(PAD_D_DOWN)) {
                gEditor->nameCursorY += EDIT_KEY_H;
            }
            {
                Editor *e = gEditor;
                s32 c = e->nameCursorY;
                if (c >= hi2) {
                    e->nameCursorY = lo2;
                } else if (c < lo2) {
                    e->nameCursorY = hi2 - 1;
                }
            }
            gEditor->nameKey = gEditor->nameCursorX / EDIT_KEY_W + (gEditor->nameCursorY / EDIT_KEY_H) * EDIT_KEYS_PER_ROW;
        } else if (PRESSED(PAD_STICK_LEFT) || PRESSED(PAD_D_LEFT) || PRESSED(PAD_STICK_RIGHT) || PRESSED(PAD_D_RIGHT)) {
            /* the name is full: left and right swap between Del and OK */
            Editor *e = gEditor;
            if (e->nameKey == EDIT_KEY_OK) {
                e->nameCursorX = 600;
                e->nameCursorY = EDIT_KEY_AREA - 1;
                e->nameKey = EDIT_KEY_DEL;
            } else {
                e->nameCursorX = EDIT_KEY_AREA - 1;
                e->nameCursorY = EDIT_KEY_AREA - 1;
                e->nameKey = EDIT_KEY_OK;
            }
        }
    }

    /* the key under the cursor shows at the end of the name */
    if (gEditor->nameKey < EDIT_KEY_DEL) {
        if (strlen(gEditor->nameBuf) == 0) {
            gEditor->nameBuf[1] = 0;
        }
        gEditor->nameBuf[gEditor->nameLen] = *gKeyboardKeys[gEditor->nameKey];
    } else {
        gEditor->nameBuf[gEditor->nameLen] = 0;
    }
    if (oldKey != gEditor->nameKey) {
        snd_play_menu_sound(MENU_SFX_MOVE);
    }
}

/* gSaveData as edit.c reads it */
typedef struct {
    u8 pad0[0x2C4];
    u8 options;
    u8 pad2C5[0x22F];
    u8 userTrack[USER_TRACK_MAX_SIZE];
} SaveDataView;

extern SaveDataView gSaveData_16;

/* The main menu page: the message and OK/Cancel boxes (load, save, clear, exit), stepping along the pieces, name
   entry and the menu's choices. */
void edit_update_main_menu(void)
{
    Editor *ed = gEditor;
    s32 flags = ed->flags;
    s32 i, j, k, n;

    if (flags & EDIT_MESSAGE) {
        if ((gPadInput_2[0] & PAD_A) && !(gPadInputPrev_2[0] & PAD_A)) {
            gPadInputPrev_2[0] |= PAD_A;
            gEditor->flags &= ~EDIT_MESSAGE;
            gEditor->menu->flags |= GUI_MENU_ACTIVE;
            snd_play_menu_sound(MENU_SFX_SELECT);
        }
    } else if (flags & EDIT_DIALOG) {
        if ((gPadInput_2[0] & PAD_B) && !(gPadInputPrev_2[0] & PAD_B)) {
            gPadInputPrev_2[0] |= PAD_B;
            gEditor->flags &= ~EDIT_DIALOG;
            gEditor->menu->flags |= GUI_MENU_ACTIVE;
            snd_play_menu_sound(MENU_SFX_BACK);
        } else if ((gPadInput_2[0] & PAD_A) && !(gPadInputPrev_2[0] & PAD_A)) {
            gPadInputPrev_2[0] |= PAD_A;
            snd_play_menu_sound(MENU_SFX_SELECT);
            if (gEditor->dialogChoice) {
                /* OK, or the left answer */
                switch (gEditor->dialog) {
                case EDIT_ASK_LOAD_DISCARD:
                    gEditor->dialog = EDIT_ASK_LOAD;
                    if (gSaveData_16.options & SAVE_HAS_USER_TRACK) {
                        gEditor->dialogChoice = 1;
                    } else {
                        gEditor->dialogChoice = 0;
                    }
                    break;
                case EDIT_ASK_LOAD:
                    /* from the Game Pak */
                    if (gSaveData_16.options & SAVE_HAS_USER_TRACK) {
                        for (j = 0; j < gEditor->gridSize[1]; j++) {
                            for (i = 0; i < gEditor->gridSize[0]; i++) {
                                arned_set_cell(i, j, 0, 0, 0, 0, 0, 0, 0);
                            }
                        }
                        gEditor->userPieceCount = USER_PIECE_COUNT;
                        usrtrk_load_into_editor(gSaveData.userTrack);
                        edit_try_close_loop(gEditor->last);
                        gEditor->mode = EDIT_MODE_IDLE;
                        gEditor->menu->flags |= GUI_MENU_ACTIVE;
                        edit_find_start_piece();
                        arned_load_piece_textures(gEditor->dirtType, gEditor->objectTex, gEditor->wallTex);
                        gEditor->menu->flags |= GUI_MENU_ACTIVE;
                        gEditor->flags &= ~EDIT_DIALOG;
                    } else {
                        gEditor->message = EDIT_MSG_NO_TRACK;
                        gEditor->flags |= EDIT_MESSAGE;
                    }
                    break;
                case EDIT_ASK_OVERWRITE:
                    gGame.userTrack = mem_alloc_dbg(USER_TRACK_BUF_SIZE, 0, (s32)sStrEditFile, 0x6FE);
                    usrtrk_pack_track_2(gGame.userTrack);
                    __builtin_memcpy(gSaveData.userTrack, gGame.userTrack, USER_TRACK_MAX_SIZE);
                    save_write(4, 0); /* the user track's section */
                    mem_free(gGame.userTrack);
                    gGame.userTrack = 0;
                    gEditor->menu->flags |= GUI_MENU_ACTIVE;
                    gEditor->flags &= ~EDIT_DIALOG;
                    break;
                case EDIT_ASK_SAVE:
                    /* to the Game Pak */
                    if (gSaveData_16.options & SAVE_HAS_USER_TRACK) {
                        gEditor->dialog = EDIT_ASK_OVERWRITE;
                        gEditor->dialogChoice = 0;
                    } else {
                        gGame.userTrack = mem_alloc_dbg(USER_TRACK_BUF_SIZE, 0, (s32)sStrEditFile, 0x6FE);
                        usrtrk_pack_track_2(gGame.userTrack);
                        __builtin_memcpy(gSaveData_16.userTrack, gGame.userTrack, USER_TRACK_MAX_SIZE);
                        save_write(4, 0);
                        mem_free(gGame.userTrack);
                        gGame.userTrack = 0;
                        gEditor->menu->flags |= GUI_MENU_ACTIVE;
                        gEditor->flags &= ~EDIT_DIALOG;
                    }
                    break;
                case EDIT_ASK_EXIT:
                    gGame.flags |= 2; /* back to the front end */
                    gEditor->exitTimer = 0;
                    gEditor->flags |= EDIT_EXIT;
                    gEditor->menu->flags |= GUI_MENU_ACTIVE;
                    gEditor->flags &= ~EDIT_DIALOG;
                    break;
                case EDIT_ASK_CLEAR:
                    gEditor->first = 0;
                    gEditor->last = 0;
                    gEditor->cur = 0;
                    gEditor->start = 0;
                    gEditor->scrollX = 0;
                    gEditor->scrollY = 0;
                    for (k = 0; k < EDIT_MAX_PIECES; k++) {
                        gEditor->pieces[k].used = 0;
                    }
                    gEditor->userPieceCount = USER_PIECE_COUNT;
                    __builtin_strcpy(gEditor->name, "<NONE>");
                    for (j = 0; j < gEditor->gridSize[1]; j++) {
                        for (i = 0; i < gEditor->gridSize[0]; i++) {
                            arned_set_cell(i, j, 0, 0, 0, 0, 0, 0, 0);
                        }
                    }
                    gEditor->cur = 0;
                    gEditor->first = 0;
                    gEditor->last = gEditor->cur;
                    edit_update_piece_ports(gEditor->cur);
                    gEditor->menu->flags |= GUI_MENU_ACTIVE;
                    gEditor->dirtType = 0;
                    gEditor->flags &= ~EDIT_DIALOG;
                    arned_load_piece_textures(0, gEditor->objectTex, gEditor->wallTex);
                    edit_cam_update_target();
                    sEditCamMoving = 0;
                    sEditCamPos.x = sEditCamTarget.x;
                    sEditCamPos.z = sEditCamTarget.z;
                    sEditCamPos.yaw = ((sEditCamPos.yaw + 0x800) >> 13) << 13;
                    break;
                }
            } else {
                /* Cancel, or the right answer */
                switch (gEditor->dialog) {
                case EDIT_ASK_OVERWRITE:
                    gEditor->message = EDIT_MSG_SAVE_CANCELED;
                    gEditor->flags = (gEditor->flags & ~EDIT_DIALOG) | EDIT_MESSAGE;
                    break;
                case EDIT_ASK_LOAD:
                    /* from the Controller Pak: leave for its screen */
                    gGame.flags |= 2;
                    gEditor->exitTimer = 0;
                    gEditor->flags = (gEditor->flags | (EDIT_PAK_LOAD | EDIT_EXIT)) & ~EDIT_PAK_SAVE;
                    gEditor->menu->flags |= GUI_MENU_ACTIVE;
                    gEditor->flags &= ~EDIT_DIALOG;
                    break;
                case EDIT_ASK_SAVE:
                    /* to the Controller Pak: leave for its screen with the packed track */
                    gGame.flags |= 2;
                    gEditor->exitTimer = 0;
                    gEditor->flags = (gEditor->flags | (EDIT_PAK_SAVE | EDIT_EXIT)) & ~EDIT_PAK_LOAD;
                    gGame.userTrack = mem_alloc_dbg(USER_TRACK_BUF_SIZE, 0, (s32)sStrEditFile, 0x7B3);
                    for (n = usrtrk_pack_track_2(gGame.userTrack + 4) + 4; n < USER_TRACK_BUF_SIZE; n++) {
                        gGame.userTrack[n] = 0;
                    }
                    *(u32 *)gGame.userTrack = calc_checksum24(gGame.userTrack + 4, USER_TRACK_BUF_SIZE - 4);
                default:
                    gEditor->menu->flags |= GUI_MENU_ACTIVE;
                    gEditor->flags &= ~EDIT_DIALOG;
                    break;
                }
            }
        } else if (((gPadInput_2[0] & PAD_D_UP) && !(gPadInputPrev_2[0] & PAD_D_UP)) ||
                   ((gPadInput_2[0] & PAD_D_DOWN) && !(gPadInputPrev_2[0] & PAD_D_DOWN))) {
            /* the Game Pak answers are locked: loading needs a saved track, saving goes to the Controller Pak */
            if ((gEditor->dialog != EDIT_ASK_LOAD || (gSaveData_16.options & SAVE_HAS_USER_TRACK)) && gEditor->dialog != EDIT_ASK_SAVE) {
                gEditor->dialogChoice ^= 1;
                snd_play_menu_sound(MENU_SFX_MOVE);
            }
        }
    } else {
        /* D-pad left and right step along the pieces (swapped when the view is reversed) */
        if (ed->cur != NULL && ed->mode != EDIT_MODE_NAME) {
            if (((gPadInput_2[0] & PAD_D_LEFT) && !(gPadInputPrev_2[0] & PAD_D_LEFT) && !(gEditor->flags & EDIT_REVERSED)) ||
                ((gPadInput_2[0] & PAD_D_RIGHT) && !(gPadInputPrev_2[0] & PAD_D_RIGHT) && (gEditor->flags & EDIT_REVERSED))) {
                if (gEditor->cur->prev) {
                    gEditor->cur = gEditor->cur->prev;
                }
            } else if (((gPadInput_2[0] & PAD_D_RIGHT) && !(gPadInputPrev_2[0] & PAD_D_RIGHT) && !(gEditor->flags & EDIT_REVERSED)) ||
                       ((gPadInput_2[0] & PAD_D_LEFT) && !(gPadInputPrev_2[0] & PAD_D_LEFT) && (gEditor->flags & EDIT_REVERSED))) {
                if (gEditor->cur->next) {
                    gEditor->cur = gEditor->cur->next;
                }
            }
        }
        if (gEditor->mode == EDIT_MODE_NAME || gEditor->mode == EDIT_MODE_NAME_8) {
            edit_update_name_entry(ed, flags);
        } else if (gui_menu_input(gEditor->menu)) {
            switch (sEditMainMenu.choice) {
            case EDIT_MAIN_EDIT:
                gEditor->mode = EDIT_MODE_IDLE;
                if (gEditor->menu != &sEditTrackMenu) {
                    switch (gEditor->slideState) {
                    case EDIT_SLIDE_SHOWN:
                        gEditor->menu = &sEditTrackMenu;
                        break;
                    case EDIT_SLIDE_IN:
                        gEditor->slideState = EDIT_SLIDE_SHOWN;
                        gEditor->nextMenu = &sEditTrackMenu;
                        break;
                    case EDIT_SLIDE_OUT:
                        gEditor->slideState = EDIT_SLIDE_SWAP;
                        gEditor->nextMenu = &sEditTrackMenu;
                        break;
                    case EDIT_SLIDE_SWAP:
                        gEditor->slideState = EDIT_SLIDE_SHOWN;
                        gEditor->menu = &sEditTrackMenu;
                        break;
                    }
                    gEditor->flags &= ~EDIT_A_HELD;
                }
                sEditTrackMenu.choice = EDIT_ITEM_STRAIGHT;
                sEditTrackMenu.cursor = EDIT_ITEM_STRAIGHT;
                gEditor->page = EDIT_PAGE_TRACK;
                break;
            case EDIT_MAIN_CLEAR:
                gEditor->dialog = EDIT_ASK_CLEAR;
                gEditor->flags |= EDIT_DIALOG;
                gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                gEditor->dialogChoice = 0;
                break;
            case EDIT_MAIN_LOAD:
                gEditor->flags |= EDIT_DIALOG;
                if (gEditor->cur != NULL) {
                    gEditor->dialog = EDIT_ASK_LOAD_DISCARD;
                    gEditor->dialogChoice = 0;
                } else {
                    gEditor->dialog = EDIT_ASK_LOAD;
                    if (gSaveData_16.options & SAVE_HAS_USER_TRACK) {
                        gEditor->dialogChoice = 1;
                    } else {
                        gEditor->dialogChoice = 0;
                    }
                }
                gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                break;
            case EDIT_MAIN_SAVE:
                if (gEditor->name[0] == '<' /* "<NONE>" */) {
                    gEditor->flags |= EDIT_MESSAGE;
                    gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                    gEditor->message = EDIT_MSG_NAME_FIRST;
                    sEditMainMenu.choice = EDIT_MAIN_NAME;
                    sEditMainMenu.cursor = EDIT_MAIN_NAME;
                } else {
                    gEditor->dialog = EDIT_ASK_SAVE;
                    gEditor->flags |= EDIT_DIALOG;
                    gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                    gEditor->dialogChoice = 0;
                }
                break;
            case EDIT_MAIN_DIRT:
                if (++gEditor->dirtType == EDIT_DIRT_TYPES) {
                    gEditor->dirtType = 0;
                }
                arned_load_piece_textures(gEditor->dirtType, gEditor->objectTex, gEditor->wallTex);
                break;
            case EDIT_MAIN_EXIT:
                gEditor->dialog = EDIT_ASK_EXIT;
                gEditor->dialogChoice = 0;
                gEditor->flags = (gEditor->flags & ~(EDIT_PAK_LOAD | EDIT_PAK_SAVE)) | EDIT_DIALOG;
                gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                break;
            case EDIT_MAIN_NAME:
                gEditor->nameLen = 0;
                gEditor->nameKey = 0;
                gEditor->nameCursorX = 0;
                gEditor->nameCursorY = 0;
                mem_fill(gEditor->nameBuf, 0, EDIT_NAME_LEN + 1);
                gEditor->mode = EDIT_MODE_NAME;
                gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                break;
            case EDIT_MAIN_TEST:
                if (gGame.difficulty == 0) {
                    gGame.difficulty = 1;
                }
                /* the test ride: mode 13 round a closed track, 14 along an open one */
                if (gEditor->first->prev == gEditor->last) {
                    gGame.mode = 0xD;
                } else {
                    gGame.mode = 0xE;
                }
                gGame.numPlayers = 1;
                gGame.flags &= ~2;
                gEditor->flags &= ~(EDIT_PAK_LOAD | EDIT_PAK_SAVE);
                gGame.flags |= 0x4000000;
                gEditor->flags |= (EDIT_TEST_TRACK | EDIT_EXIT);
                gEditor->exitTimer = 0;
                gEditor->flags &= ~EDIT_DIALOG;
                if (gGame.userTrack == NULL) {
                    gGame.userTrack = mem_alloc_dbg(USER_TRACK_BUF_SIZE, 0, (s32)sStrEditFile, 0x88C);
                    for (n = usrtrk_pack_track_2(gGame.userTrack + 4) + 4; n < USER_TRACK_BUF_SIZE; n++) {
                        gGame.userTrack[n] = 0;
                    }
                }
                break;
            }
        }
    }
    if (gEditor->cur != NULL) {
        if (gEditor->cur->flags & EDIT_PIECE_FLIPPED) {
            gEditor->flags |= EDIT_FLIP;
        } else {
            gEditor->flags &= ~EDIT_FLIP;
        }
    }
}

#define PRESSED_2(m) ((gPadInput & (m)) && !(gPadInputPrev_3[0] & (m)))
/* Delete: the current piece, at an end of the track (or anywhere in a closed one); the cursor moves to its
   neighbour and a new start piece is picked if it was the start. */
#define DELETE_CUR_PIECE()                                                              \
    if (gEditor->cur != NULL) {                                                         \
        if (gEditor->cur->prev == NULL) {                                               \
            piece = gEditor->cur;                                                       \
            gEditor->cur = piece->next;                                                 \
            edit_delete_piece(piece);                                                   \
            edit_pick_start_piece();                                                    \
        } else if (gEditor->cur->next == NULL || gEditor->first->prev == gEditor->last) { \
            piece = gEditor->cur;                                                       \
            gEditor->cur = piece->prev;                                                 \
            edit_delete_piece(piece);                                                   \
            edit_pick_start_piece();                                                    \
        }                                                                               \
    }


/* Whether piece may be deleted: not the start piece of a track, or another piece can start it. */
static inline s32 can_delete(Editor *ed, EditPiece *piece) {
    EditPiece *other;
    s32 ok;

    if (piece == NULL || piece != ed->start || (piece->prev == NULL && piece->next == NULL)) {
        return 1;
    }
    ok = 0;
    other = ed->first;
    while (other != NULL) {
        if (other != piece && (other->type->flags & PIECE_START)) {
            ok = 1;
            break;
        }
        other = other->next;
        if (other == ed->first) {
            break;
        }
    }
    return ok;
}

/* The same for the current piece. */
static inline s32 can_delete_cur(void) {
    EditPiece *other;
    s32 ok;

    if (gEditor->cur == NULL || gEditor->cur != gEditor->start) {
        return 1;
    }
    if (gEditor->cur->prev != NULL || gEditor->cur->next != NULL) {
        ok = 0;
        other = gEditor->first;
        while (other != NULL) {
            if (other != gEditor->cur && (other->type->flags & PIECE_START)) {
                ok = 1;
                break;
            }
            other = other->next;
            if (other == gEditor->first) {
                break;
            }
        }
        return ok;
    }
    return 1;
}

/* The edit menu page: which items are enabled, the message box, the warnings, placing pieces, moving the track, the
   menu's choices, B to delete, and stepping along the pieces. */
void edit_update_track_menu(void) {
    s16 minx, miny, maxx, maxy;
    s16 delta[2];
    EditPiece *piece;
    s32 moved;
    s32 i, j;
    Editor *ed;
    EditPiece *cur;
    EditPiece *fitFrom;

    minx = 1000;
    maxx = -1000;
    miny = 1000;
    maxy = -1000;
    gEditor->flags &= ~EDIT_ROOM_ANY;
    if (gEditor->cur != NULL) {
        piece = gEditor->first;
        do {
            if (piece->x < minx) {
                minx = piece->x;
            }
            if (piece->y < miny) {
                miny = piece->y;
            }
            if (piece->x + piece->w > maxx) {
                maxx = piece->x + piece->w;
            }
            if (piece->y + piece->h > maxy) {
                maxy = piece->y + piece->h;
            }
            piece = piece->next;
        } while (piece != NULL && piece != gEditor->first);
        if (minx > 0) {
            gEditor->flags |= EDIT_ROOM_X_NEG;
        }
        if (miny > 0) {
            gEditor->flags |= EDIT_ROOM_Y_NEG;
        }
        if (maxx < gEditor->gridSize[0]) {
            gEditor->flags |= EDIT_ROOM_X_POS;
        }
        if (maxy < gEditor->gridSize[1]) {
            gEditor->flags |= EDIT_ROOM_Y_POS;
        }
    }
    if (gEditor->cur != NULL && (gEditor->cur->flags & EDIT_PIECE_PREVIEW)) {
        sEditTrackMenuItems[EDIT_ITEM_DELETE].flags &= ~GUI_ITEM_ENABLED;
        sEditTrackMenuItems[EDIT_ITEM_MOVE_TRACK].flags &= ~GUI_ITEM_ENABLED;
        sEditTrackMenuItems[EDIT_ITEM_MAIN_MENU].flags &= ~GUI_ITEM_ENABLED;
        sEditTrackMenuItems[EDIT_ITEM_MOVE_START].flags &= ~GUI_ITEM_ENABLED;
        gEditor->flags &= ~EDIT_CAN_DELETE;
    } else {
        sEditTrackMenuItems[EDIT_ITEM_MAIN_MENU].flags |= GUI_ITEM_ENABLED;
        if (gEditor->cur == NULL ||
            (gEditor->cur->next != NULL && gEditor->cur->prev != NULL && gEditor->first->prev == NULL) ||
            (ed = gEditor, cur = ed->cur, !can_delete(ed, cur))) {
            sEditTrackMenuItems[EDIT_ITEM_DELETE].flags &= ~GUI_ITEM_ENABLED;
            gEditor->flags &= ~EDIT_CAN_DELETE;
        } else {
            gEditor->flags |= EDIT_CAN_DELETE;
            sEditTrackMenuItems[EDIT_ITEM_DELETE].flags |= GUI_ITEM_ENABLED;
        }
        if (gEditor->flags & EDIT_ROOM_ANY) {
            sEditTrackMenuItems[EDIT_ITEM_MOVE_TRACK].flags |= GUI_ITEM_ENABLED;
        } else {
            sEditTrackMenuItems[EDIT_ITEM_MOVE_TRACK].flags &= ~GUI_ITEM_ENABLED;
        }
    }
    if (gEditor->menu != NULL) {
        gui_menu_fix_cursor(gEditor->menu);
    }
    if (gEditor->flags & EDIT_MESSAGE) {
        if (PRESSED_2(PAD_A)) {
            gPadInputPrev_3[0] |= PAD_A;
            gEditor->flags &= ~EDIT_MESSAGE;
            gEditor->menu->flags |= GUI_MENU_ACTIVE;
        }
    } else {
        if (gEditor->mode == EDIT_MODE_WARN_MULTI || gEditor->mode == EDIT_MODE_WARN_LOCKED) {
            if (PRESSED_2(PAD_A)) {
                gEditor->mode = EDIT_MODE_USER_PIECE;
            } else if (PRESSED_2(PAD_B)) {
                gEditor->menu->flags |= GUI_MENU_ACTIVE;
                gEditor->mode = EDIT_MODE_PLACE;
                gEditor->flags &= ~EDIT_DIALOG;
            }
        } else if (gEditor->mode == EDIT_MODE_PLACE || gEditor->mode == EDIT_MODE_USER_PIECE) {
            if (PRESSED_2(PAD_B)) {
                if (gEditor->mode == EDIT_MODE_PLACE) {
                    edit_remove_preview_pieces(gEditor->cur);
                    gEditor->mode = EDIT_MODE_IDLE;
                    gEditor->flags &= ~EDIT_OPEN_END;
                }
            } else {
                edit_update_place_piece();
            }
        } else if (gEditor->mode == EDIT_MODE_MOVE_TRACK) {
            /* B puts the track back, A keeps it, the D-pad moves it a cell where there is room */
            moved = 0;
            if (PRESSED_2(PAD_B)) {
                delta[0] = gEditor->first->x - gEditor->moveFromX;
                delta[1] = gEditor->first->y - gEditor->moveFromY;
                piece = gEditor->first;
                do {
                    piece->x -= delta[0];
                    piece->y -= delta[1];
                    piece = piece->next;
                } while (piece != NULL && piece != gEditor->first);
                gEditor->mode = EDIT_MODE_IDLE;
                gEditor->menu->flags |= GUI_MENU_ACTIVE;
                moved = 1;
                snd_play_menu_sound(MENU_SFX_BACK);
            } else if (PRESSED_2(PAD_A)) {
                gEditor->mode = EDIT_MODE_IDLE;
                gEditor->menu->flags |= GUI_MENU_ACTIVE;
                snd_play_menu_sound(MENU_SFX_SELECT);
            } else if ((gEditor->flags & EDIT_ROOM_X_NEG) && PRESSED_2(PAD_D_LEFT)) {
                piece = gEditor->first;
                do {
                    piece->x--;
                    piece = piece->next;
                } while (piece != NULL && piece != gEditor->first);
                snd_play_menu_sound(MENU_SFX_MOVE);
                moved = 1;
            } else if ((gEditor->flags & EDIT_ROOM_X_POS) && PRESSED_2(PAD_D_RIGHT)) {
                piece = gEditor->first;
                do {
                    piece->x++;
                    piece = piece->next;
                } while (piece != NULL && piece != gEditor->first);
                snd_play_menu_sound(MENU_SFX_MOVE);
                moved = 1;
            } else if ((gEditor->flags & EDIT_ROOM_Y_POS) && PRESSED_2(PAD_D_UP)) {
                piece = gEditor->first;
                do {
                    piece->y++;
                    piece = piece->next;
                } while (piece != NULL && piece != gEditor->first);
                snd_play_menu_sound(MENU_SFX_MOVE);
                moved = 1;
            } else if ((gEditor->flags & EDIT_ROOM_Y_NEG) && PRESSED_2(PAD_D_DOWN)) {
                piece = gEditor->first;
                do {
                    piece->y--;
                    piece = piece->next;
                } while (piece != NULL && piece != gEditor->first);
                snd_play_menu_sound(MENU_SFX_MOVE);
                moved = 1;
            }
            if (moved) {
                piece = gEditor->first;
                do {
                    edit_update_piece_ports(gEditor->first); /* the first piece each time, as in the original */
                    piece = piece->next;
                } while (piece != NULL && piece != gEditor->first);
                for (i = 0; i < gEditor->gridSize[1]; i++) {
                    for (j = 0; j < gEditor->gridSize[0]; j++) {
                        arned_set_cell(j, i, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
        } else if (gui_menu_input(gEditor->menu)) {
            switch (sEditTrackMenu.choice) {
            case EDIT_ITEM_STRAIGHT:
            case EDIT_ITEM_TURN:
            case EDIT_ITEM_JUMP:
            case EDIT_ITEM_USER:
                /* a preview piece of the selected type at the open end */
                if (gEditor->cur == NULL) {
                    gEditor->cur = edit_insert_piece(NULL, NULL, gEditor->selType);
                    gEditor->cur->flags |= EDIT_PIECE_PREVIEW;
                    gEditor->mode = EDIT_MODE_PLACE;
                    gEditor->first = gEditor->cur;
                    gEditor->last = gEditor->cur;
                } else if (gEditor->cur->next == NULL) {
                    if (gEditor->cur->prev == NULL) {
                        gEditor->cur = edit_insert_piece(gEditor->cur, NULL, gEditor->selType);
                        gEditor->cur->flags |= EDIT_PIECE_PREVIEW;
                        gEditor->mode = EDIT_MODE_IDLE;
                        gEditor->last = gEditor->cur;
                        gEditor->menu->flags |= GUI_MENU_ACTIVE;
                        gEditor->mode = EDIT_MODE_PLACE;
                        gEditor->flags |= EDIT_OPEN_END;
                    } else {
                        gEditor->cur = edit_insert_piece(gEditor->cur, NULL, gEditor->selType);
                        gEditor->cur->flags |= EDIT_PIECE_PREVIEW;
                        gEditor->mode = EDIT_MODE_PLACE;
                        gEditor->last = gEditor->cur;
                    }
                } else if (gEditor->cur->prev == NULL) {
                    gEditor->cur = edit_insert_piece(NULL, gEditor->cur, gEditor->selType);
                    gEditor->cur->flags |= EDIT_PIECE_PREVIEW;
                    gEditor->mode = EDIT_MODE_PLACE;
                    gEditor->first = gEditor->cur;
                }
                break;
            case EDIT_ITEM_MOVE_START:
                if (gEditor->cur != NULL) {
                    gEditor->start = gEditor->cur;
                }
                break;
            case EDIT_ITEM_MOVE_TRACK:
                if (gEditor->cur != NULL) {
                    gEditor->moveFromX = gEditor->first->x;
                    gEditor->moveFromY = gEditor->first->y;
                    gEditor->mode = EDIT_MODE_MOVE_TRACK;
                    gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                }
                break;
            case EDIT_ITEM_MAIN_MENU:
                snd_play_menu_sound(MENU_SFX_BACK);
                if (gEditor->menu != &sEditMainMenu) {
                    switch (gEditor->slideState) {
                    case EDIT_SLIDE_IN:
                        gEditor->slideState = EDIT_SLIDE_SHOWN;
                        gEditor->nextMenu = &sEditMainMenu;
                        break;
                    case EDIT_SLIDE_SHOWN:
                        gEditor->menu = &sEditMainMenu;
                        break;
                    case EDIT_SLIDE_OUT:
                        gEditor->slideState = EDIT_SLIDE_SWAP;
                        gEditor->nextMenu = &sEditMainMenu;
                        break;
                    case EDIT_SLIDE_SWAP:
                        gEditor->slideState = EDIT_SLIDE_SHOWN;
                        gEditor->menu = &sEditMainMenu;
                        break;
                    }
                    gEditor->flags &= ~EDIT_A_HELD;
                }
                gEditor->page = EDIT_PAGE_MAIN;
                break;
            case EDIT_ITEM_DELETE:
                if (gEditor->cur != NULL && can_delete_cur()) {
                    DELETE_CUR_PIECE();
                }
                break;
            }
        } else if (PRESSED_2(PAD_B)) {
            /* B deletes too */
            ed = gEditor;
            if (ed->flags & EDIT_CAN_DELETE) {
                cur = ed->cur;
                if (can_delete(ed, cur)) {
                    DELETE_CUR_PIECE();
                }
            }
        }
        if (gEditor->cur != NULL) {
            fitFrom = gEditor->cur;
            if (fitFrom->flags & EDIT_PIECE_PREVIEW) {
                if (fitFrom->next == NULL) {
                    fitFrom = fitFrom->prev;
                } else {
                    fitFrom = fitFrom->next;
                }
            }
            if (fitFrom != NULL) {
                edit_update_fitting_pieces(fitFrom);
            } else {
                edit_set_start_piece_choices();
            }
        }
        if (gEditor->mode == EDIT_MODE_IDLE && gEditor->cur != NULL) {
            if ((PRESSED_2(PAD_D_LEFT) && !(gEditor->flags & EDIT_REVERSED)) || (PRESSED_2(PAD_D_RIGHT) && (gEditor->flags & EDIT_REVERSED))) {
                if (gEditor->cur->prev != NULL) {
                    gEditor->cur = gEditor->cur->prev;
                }
            } else if ((PRESSED_2(PAD_D_RIGHT) && !(gEditor->flags & EDIT_REVERSED)) ||
                       (PRESSED_2(PAD_D_LEFT) && (gEditor->flags & EDIT_REVERSED))) {
                if (gEditor->cur->next != NULL) {
                    gEditor->cur = gEditor->cur->next;
                }
            }
        }
    }
    if (gEditor->cur != NULL) {
        if (gEditor->cur->flags & EDIT_PIECE_FLIPPED) {
            gEditor->flags |= EDIT_FLIP;
        } else {
            gEditor->flags &= ~EDIT_FLIP;
        }
    }
}

#define PRESSED_3(m) ((gPadInput & (m)) && !(gPadInputPrev & (m)))
/* The value chosen in the selected item's row */
#define MENU_VALUE (sEditTrackMenu.items[sEditTrackMenu.choice].value)

/* Inserts a piece of type base + MENU_VALUE: the first piece, or at the open end */
#define INSERT_PIECE(base)                                                                         \
    if (gEditor->cur == NULL) {                                                                    \
        gEditor->cur = edit_insert_piece(NULL, NULL, &gPieceTypes[(base) + MENU_VALUE]);           \
        gEditor->first = gEditor->cur;                                                             \
        gEditor->last = gEditor->cur;                                                              \
        gEditor->start = gEditor->cur;                                                             \
    } else if (gEditor->cur->prev != NULL ||                                                       \
               ((gEditor->flags & EDIT_OPEN_END) && gEditor->cur->next == NULL)) {                 \
        gEditor->cur = edit_insert_piece(gEditor->cur, gEditor->cur->next, &gPieceTypes[(base) + MENU_VALUE]); \
        gEditor->last = gEditor->cur;                                                              \
    } else {                                                                                       \
        gEditor->cur = edit_insert_piece(NULL, gEditor->cur, &gPieceTypes[(base) + MENU_VALUE]);   \
        gEditor->first = gEditor->cur;                                                             \
    }


/* Placing pieces (EDIT_MODE_PLACE): moving a lone piece, R to flip, Z to edit a user piece, A to place the previewed
   piece, and the preview following the menu. */
void edit_update_place_piece(void) {
    EditPiece *placed;
    EditPiece *cur;
    EditPiece *mark;
    EditPiece *fitFrom;
    s32 i;
    s32 j;
    s32 inserted;

    cur = gEditor->cur;
    mark = gEditor->markStart;
    /* a lone piece: the D-pad moves where it goes on the grid */
    if (cur != NULL && cur->prev == NULL && cur->next == NULL) {
        if ((PRESSED_3(PAD_D_LEFT) && !(gEditor->flags & EDIT_REVERSED)) || (PRESSED_3(PAD_D_RIGHT) && (gEditor->flags & EDIT_REVERSED))) {
            if (gEditor->scrollX >= -30) {
                gEditor->scrollX--;
            }
        }
        if ((PRESSED_3(PAD_D_RIGHT) && !(gEditor->flags & EDIT_REVERSED)) || (PRESSED_3(PAD_D_LEFT) && (gEditor->flags & EDIT_REVERSED))) {
            if (gEditor->scrollX < 31) {
                gEditor->scrollX++;
            }
        }
        if ((PRESSED_3(PAD_D_DOWN) && !(gEditor->flags & EDIT_REVERSED)) || (PRESSED_3(PAD_D_UP) && (gEditor->flags & EDIT_REVERSED))) {
            if (gEditor->scrollY >= -30) {
                gEditor->scrollY--;
            }
        }
        if ((PRESSED_3(PAD_D_UP) && !(gEditor->flags & EDIT_REVERSED)) || (PRESSED_3(PAD_D_DOWN) && (gEditor->flags & EDIT_REVERSED))) {
            if (gEditor->scrollY < 31) {
                gEditor->scrollY++;
            }
        }
    }
    /* walks to the end of the marked range (the result is not used) */
    if (mark != NULL) {
        do {
            mark = mark->next;
        } while (mark->prev != gEditor->markEnd);
    }
    if (PRESSED_3(PAD_R)) {
        /* R flips the piece if it fits both ways (PIECE_FITS | PIECE_FITS_FLIPPED, the flags tested as a word) */
        if ((*(u32 *)&gEditor->cur->type->flags & ((PIECE_FITS | PIECE_FITS_FLIPPED) << 16)) == ((PIECE_FITS | PIECE_FITS_FLIPPED) << 16)) {
            gPadInput &= PAD_R; /* clears all but R */
            snd_play_menu_sound(MENU_SFX_MOVE);
            for (i = 0; i < gEditor->cur->type->dim[(gEditor->cur->rot + 1) & 1]; i++) {
                for (j = 0; j < gEditor->cur->type->dim[gEditor->cur->rot & 1]; j++) {
                    if (gEditor->cur->x + j >= 0 && gEditor->cur->x + j < gEditor->gridSize[0] &&
                        gEditor->cur->y + i >= 0 && gEditor->cur->y + i < gEditor->gridSize[1]) {
                        arned_set_cell(gEditor->cur->x + j, gEditor->cur->y + i, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
            gEditor->flags ^= EDIT_FLIP;
            if (gEditor->flags & EDIT_FLIP) {
                gEditor->cur->flags |= EDIT_PIECE_FLIPPED;
            } else {
                gEditor->cur->flags &= ~EDIT_PIECE_FLIPPED;
            }
        }
        mark = gEditor->first;
        if (mark != NULL) {
            do {
                edit_update_piece_ports(mark);
                mark = mark->next;
            } while (mark != NULL && mark != gEditor->first);
        }
    } else if (PRESSED_3(PAD_Z)) {
        /* Z edits a user piece's heights (after a warning when it is used twice or locked) */
        if (gEditor->cur != NULL && (gEditor->cur->type->flags & PIECE_USER)) {
            gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
            edit_update_user_piece_usage();
            edit_user_piece_reset_cursor(0);
            if (gEditor->cur->type->flags & PIECE_USED_TWICE) {
                gEditor->mode = EDIT_MODE_WARN_MULTI;
            } else if (gEditor->cur->type->flags & PIECE_SIZE_LOCKED) {
                gEditor->mode = EDIT_MODE_WARN_LOCKED;
            } else {
                gEditor->mode = EDIT_MODE_USER_PIECE;
            }
        }
    }
    fitFrom = gEditor->cur;
    if (fitFrom->flags & EDIT_PIECE_PREVIEW) {
        if (fitFrom->next == NULL) {
            fitFrom = fitFrom->prev;
        } else {
            fitFrom = fitFrom->next;
        }
    }
    if (fitFrom != NULL) {
        edit_update_fitting_pieces(fitFrom);
    } else {
        edit_set_start_piece_choices();
    }
    if (gEditor->mode == EDIT_MODE_USER_PIECE) {
        edit_update_user_piece();
        gEditor->flags |= EDIT_OPEN_END;
    } else if (gui_menu_input(gEditor->menu)) {
        inserted = 0;
        /* A places the piece */
        switch (sEditTrackMenu.choice) {
        case EDIT_ITEM_USER:
            edit_remove_preview_pieces_2();
            gEditor->userPiece = MENU_VALUE;
            INSERT_PIECE(PIECE_TYPE_USER);
            inserted = 1;
            break;
        case EDIT_ITEM_STRAIGHT:
            edit_remove_preview_pieces_2();
            INSERT_PIECE(PIECE_TYPE_STRAIGHT);
            inserted = 1;
            break;
        case EDIT_ITEM_TURN:
            edit_remove_preview_pieces_2();
            INSERT_PIECE(PIECE_TYPE_TURN);
            inserted = 1;
            break;
        case EDIT_ITEM_JUMP:
            edit_remove_preview_pieces_2();
            INSERT_PIECE(PIECE_TYPE_JUMP);
            inserted = 1;
            break;
        }
        gEditor->flags &= ~EDIT_OPEN_END;
        if (inserted) {
            gEditor->cur->flags &= ~EDIT_PIECE_PREVIEW;
            /* a new preview piece after it, or the track is closed or out of room */
            if (edit_update_fitting_pieces(gEditor->cur)) {
                placed = gEditor->cur;
                if (placed->prev == NULL) {
                    if (placed->next == NULL) {
                        gEditor->cur = edit_insert_piece(placed, NULL, gEditor->selType);
                        gEditor->cur->flags |= EDIT_PIECE_PREVIEW;
                        gEditor->last = gEditor->cur;
                        gEditor->flags |= EDIT_OPEN_END;
                    } else {
                        gEditor->cur = edit_insert_piece(NULL, placed, gEditor->selType);
                        gEditor->cur->flags |= EDIT_PIECE_PREVIEW;
                        gEditor->first = gEditor->cur;
                    }
                } else {
                    gEditor->cur = edit_insert_piece(placed, NULL, gEditor->selType);
                    gEditor->cur->flags |= EDIT_PIECE_PREVIEW;
                    gEditor->last = gEditor->cur;
                }
            } else {
                edit_try_close_loop(gEditor->cur);
                gEditor->mode = EDIT_MODE_IDLE;
                gEditor->flags |= EDIT_MESSAGE;
                if (gEditor->first->prev != NULL) {
                    gEditor->message = EDIT_MSG_TRACK_COMPLETE;
                    sEditTrackMenu.choice = EDIT_ITEM_MAIN_MENU;
                    sEditTrackMenu.cursor = EDIT_ITEM_MAIN_MENU;
                } else {
                    gEditor->message = EDIT_MSG_OUT_OF_ROOM;
                }
                gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
            }
        }
    } else {
        /* the preview follows the menu's selection */
        switch (sEditTrackMenu.choice) {
        case EDIT_ITEM_USER:
            edit_remove_preview_pieces_2();
            gEditor->userPiece = MENU_VALUE;
            INSERT_PIECE(PIECE_TYPE_USER);
            gEditor->cur->flags |= EDIT_PIECE_PREVIEW;
            break;
        case EDIT_ITEM_STRAIGHT:
            edit_remove_preview_pieces_2();
            INSERT_PIECE(PIECE_TYPE_STRAIGHT);
            gEditor->cur->flags |= EDIT_PIECE_PREVIEW;
            break;
        case EDIT_ITEM_TURN:
            edit_remove_preview_pieces_2();
            INSERT_PIECE(PIECE_TYPE_TURN);
            gEditor->cur->flags |= EDIT_PIECE_PREVIEW;
            break;
        case EDIT_ITEM_JUMP:
            edit_remove_preview_pieces_2();
            INSERT_PIECE(PIECE_TYPE_JUMP);
            gEditor->cur->flags |= EDIT_PIECE_PREVIEW;
            break;
        }
    }
    edit_update_piece_ports(gEditor->cur);
}

/* Deletes the preview pieces at the open end; the current piece becomes the last placed one (or the first). */
void edit_remove_preview_pieces(EditPiece *piece) {
    EditPiece *p;
    EditPiece *del;
    s32 forward;

    p = gEditor->cur;
    while (p != 0 && (p->flags & EDIT_PIECE_PREVIEW) != 0) {
        p = p->prev;
    }
    forward = 1;
    if (p == 0) {
        p = gEditor->cur;
        while (p != 0) {
            if ((p->flags & EDIT_PIECE_PREVIEW) == 0) {
                break;
            }
            p = p->next;
        }
        forward = 0;
    }
    gEditor->cur = p;
    if (p == 0) {
        p = gEditor->first;
        forward = 1;
        gEditor->first = 0;
        gEditor->last = 0;
    } else if (forward != 0) {
        gEditor->last = p;
    } else {
        gEditor->first = p;
    }
    while (p != 0) {
        del = p;
        if (forward != 0) {
            p = del->next;
        } else {
            p = del->prev;
        }
        if ((del->flags & EDIT_PIECE_PREVIEW) != 0) {
            edit_delete_piece(del);
        }
    }
}

/* Closes the circuit if piece `end`'s open connector meets the other open end of the track. */
void edit_try_close_loop(EditPiece *end)
{
    EditPiece *piece;

    for (piece = gEditor->first; piece != NULL; piece = piece->next) {
        if (piece->prev == gEditor->last) {
            return;
        }
        if (piece->next != NULL && piece->prev != NULL) {
            continue;
        }
        if (piece == end) {
            continue;
        }
        if (piece->prev == NULL) {
            if (end->next == NULL && ((end->dir[1] - piece->dir[0]) & 3) == 2) {
                switch (end->dir[1]) {
                case SIDE_POS_Y:
                    if (end->y + end->h == piece->y && end->x + end->off[1] == piece->x + piece->off[0]) {
                        end->next = piece;
                        piece->prev = end;
                        return;
                    }
                    break;
                case SIDE_NEG_Y:
                    if (piece->y + piece->h == end->y && end->x + end->off[1] == piece->x + piece->off[0]) {
                        end->next = piece;
                        piece->prev = end;
                        return;
                    }
                    break;
                case SIDE_POS_X:
                    if (end->x + end->w == piece->x && end->y + end->off[1] == piece->y + piece->off[0]) {
                        end->next = piece;
                        piece->prev = end;
                        return;
                    }
                    break;
                case SIDE_NEG_X:
                    if (end->x == piece->x + piece->w && end->y + end->off[1] == piece->y + piece->off[0]) {
                        end->next = piece;
                        piece->prev = end;
                        return;
                    }
                    break;
                }
            }
        } else {
            if (end->prev == NULL && ((end->dir[0] - piece->dir[1]) & 3) == 2) {
                switch (end->dir[0]) {
                case SIDE_POS_Y:
                    if (end->y + end->h == piece->y && end->x + end->off[0] == piece->x + piece->off[1]) {
                        end->prev = piece;
                        piece->next = end;
                        return;
                    }
                    break;
                case SIDE_NEG_Y:
                    if (piece->y + piece->h == end->y && end->x + end->off[0] == piece->x + piece->off[1]) {
                        end->prev = piece;
                        piece->next = end;
                        return;
                    }
                    break;
                case SIDE_POS_X:
                    if (end->x + end->w == piece->x && end->y + end->off[0] == piece->y + piece->off[1]) {
                        end->prev = piece;
                        piece->next = end;
                        return;
                    }
                    break;
                case SIDE_NEG_X:
                    if (end->x == piece->x + piece->w && end->y + end->off[0] == piece->y + piece->off[1]) {
                        end->prev = piece;
                        piece->next = end;
                        return;
                    }
                    break;
                }
            }
        }
    }
}

/* The stick's signed position in a pad word */
#define STICK_X(v) (((v) & 0xC) ? (((v) & 8) ? (((s32)(v) >> 24) & 0xF) : -(((s32)(v) >> 24) & 0xF)) : 0)
#define STICK_Y(v) (((v) & 3) ? (((v) & 1) ? (s32)((v) >> 28) : -(s32)((v) >> 28)) : 0)
#define ABS(x) ((x) > 0 ? (x) : -(x))

/* An orbit camera's height from its distance (a line through (DIST0, HEIGHT0) of slope SCALE / RANGE); leaves the
   squared distance to the target in distSq */
#define CAM_CALC_HEIGHT(DIST, HEIGHT, DIST0, SCALE, RANGE, HEIGHT0)                    \
    {                                                                                    \
        f32 x = gEditor->DIST;                                                           \
        s32 t = (s32)((x - DIST0) * SCALE / RANGE + HEIGHT0);                            \
        gEditor->HEIGHT = t;                                                             \
        distSq = (f32)t * (f32)t + x * x;                                                \
    }

/* ... and its pitch: asin(height / distance) */
#define CAM_CALC_PITCH(HEIGHT, PITCH)                                                    \
    {                                                                                    \
        s32 s = (s32)__builtin_sqrtf(distSq);                                            \
        gEditor->PITCH = (u16)math_asini(((gEditor->HEIGHT << 14) / s) & 0x3FFF);        \
    }

/* Game state 2's update: the stick as the D-pad, the cameras (zoom, turn round), which menu items are enabled, the
   menu panel's slide, the menu page's input, leaving after EDIT_EXIT, and every piece's connectors. */
void edit_update(void) {
    Editor *ed;
    EditPiece *piece;
    f32 distSq;

    ed = gEditor;
    if (ed->lastCur != ed->cur) {
        ed->prevCur = ed->lastCur;
        ed->lastCur = ed->cur;
    }
    /* the stick pushed far enough counts as the D-pad */
    if ((gPadInput & PAD_STICK_LEFT) && ABS(STICK_X(gPadInput)) >= 3) {
        gPadInput |= PAD_D_LEFT;
    }
    if ((gPadInput & PAD_STICK_RIGHT) && ABS(STICK_X(gPadInput)) >= 3) {
        gPadInput |= PAD_D_RIGHT;
    }
    if ((gPadInput & PAD_STICK_UP) && ABS(STICK_Y(gPadInput)) >= 3) {
        gPadInput |= PAD_D_UP;
    }
    if ((gPadInput & PAD_STICK_DOWN) && ABS(STICK_Y(gPadInput)) >= 3) {
        gPadInput |= PAD_D_DOWN;
    }
    if ((gPadInput & PAD_A) && !(gPadInputPrev & PAD_A)) {
        gEditor->flags |= EDIT_A_HELD;
    } else if (gEditor->flags & EDIT_A_HELD) {
        if (((gPadInput & PAD_B) && !(gPadInputPrev & PAD_B)) || !(gPadInput & PAD_A)) {
            gEditor->flags &= ~EDIT_A_HELD;
        }
    }
    /* the camera: a zoom under way, or C up and down */
    if (gEditor->camZoomFrames != 0) {
        gEditor->cam[1].dist += gEditor->camZoomStep / 50;
        gEditor->camZoomFrames--;
        CAM_CALC_HEIGHT(cam[1].dist, cam[1].height, 20000.0f, 32000.0f, 100000.0f, 8000.0f);
        CAM_CALC_PITCH(cam[1].height, cam[1].pitch);
    } else if (gEditor->cur != 0) {
        if (gPadInput & PAD_C_UP) {
            if (gEditor->mode == EDIT_MODE_USER_PIECE) {
                gEditor->cam[1].dist -= 600;
                if (gEditor->cam[1].dist < 20000) {
                    gEditor->cam[1].dist = 20000;
                }
                CAM_CALC_HEIGHT(cam[1].dist, cam[1].height, 20000.0f, 32000.0f, 100000.0f, 8000.0f);
                CAM_CALC_PITCH(cam[1].height, cam[1].pitch);
            } else {
                gEditor->cam[0].dist -= 600;
                if (gEditor->cam[0].dist < 20000) {
                    gEditor->cam[0].dist = 20000;
                }
                CAM_CALC_HEIGHT(cam[0].dist, cam[0].height, 20000.0f, 38000.0f, 165000.0f, 40000.0f);
                CAM_CALC_PITCH(cam[0].height, cam[0].pitch);
            }
        }
        if (gPadInput & PAD_C_DOWN) {
            if (gEditor->mode == EDIT_MODE_USER_PIECE) {
                gEditor->cam[1].dist += 600;
                if (gEditor->cam[1].dist > 120000) {
                    gEditor->cam[1].dist = 120000;
                }
                CAM_CALC_HEIGHT(cam[1].dist, cam[1].height, 20000.0f, 32000.0f, 100000.0f, 8000.0f);
                CAM_CALC_PITCH(cam[1].height, cam[1].pitch);
            } else {
                gEditor->cam[0].dist += 600;
                if (gEditor->cam[0].dist > 185000) {
                    gEditor->cam[0].dist = 185000;
                }
                CAM_CALC_HEIGHT(cam[0].dist, cam[0].height, 20000.0f, 38000.0f, 165000.0f, 40000.0f);
                CAM_CALC_PITCH(cam[0].height, cam[0].pitch);
            }
        }
    }
    if (gEditor->cur != 0) {
        /* C left and right turn the view round */
        if ((gPadInput & PAD_C_LEFT) && !(gPadInputPrev & PAD_C_LEFT)) {
            if (gEditor->flags & EDIT_TURN_LEFT) {
                gEditor->flags &= ~EDIT_TURN_LEFT;
            } else if (gEditor->flags & EDIT_TURN_RIGHT) {
                gEditor->flags &= ~EDIT_TURN_RIGHT;
            } else {
                gEditor->flags |= EDIT_TURN_LEFT;
            }
        }
        if ((gPadInput & PAD_C_RIGHT) && !(gPadInputPrev & PAD_C_RIGHT)) {
            if (gEditor->flags & EDIT_TURN_RIGHT) {
                gEditor->flags &= ~EDIT_TURN_RIGHT;
            } else if (gEditor->flags & EDIT_TURN_LEFT) {
                gEditor->flags &= ~EDIT_TURN_LEFT;
            } else {
                gEditor->flags |= EDIT_TURN_RIGHT;
            }
        }
    }
    sEditTrackMenuItems[EDIT_ITEM_USER].numValues = gEditor->userPieceCount;
    if (gEditor->menu != NULL) {
        gui_menu_fix_cursor(gEditor->menu);
    }
    if (sEditCamMoving == 0) {
        edit_cam_update_target();
    }
    if (sEditCamMoving != 0) {
        edit_cam_step();
    }
    if (gEditor->cur == NULL) {
        edit_set_start_piece_choices();
    }
    if (gEditor->cur == NULL) {
        sEditMainMenuItems[EDIT_MAIN_CLEAR].flags &= ~GUI_ITEM_ENABLED;
        sEditMainMenuItems[EDIT_MAIN_NAME].flags &= ~GUI_ITEM_ENABLED;
    } else {
        sEditMainMenuItems[EDIT_MAIN_CLEAR].flags |= GUI_ITEM_ENABLED;
        sEditMainMenuItems[EDIT_MAIN_NAME].flags |= GUI_ITEM_ENABLED;
    }
    if (gEditor->cur != NULL && (gEditor->cur->type->flags & PIECE_START) && gEditor->start != NULL) {
        sEditTrackMenuItems[EDIT_ITEM_MOVE_START].flags |= GUI_ITEM_ENABLED;
    } else {
        sEditTrackMenuItems[EDIT_ITEM_MOVE_START].flags &= ~GUI_ITEM_ENABLED;
    }
    if (gEditor->start != 0) {
        sEditMainMenuItems[EDIT_MAIN_TEST].flags |= GUI_ITEM_ENABLED;
        sEditMainMenuItems[EDIT_MAIN_DIRT].flags |= GUI_ITEM_ENABLED;
    } else {
        sEditMainMenuItems[EDIT_MAIN_TEST].flags &= ~GUI_ITEM_ENABLED;
        sEditMainMenuItems[EDIT_MAIN_DIRT].flags &= ~GUI_ITEM_ENABLED;
    }
    /* only a closed track can be saved */
    if (gEditor->start != 0 && gEditor->first->prev == gEditor->last) {
        sEditMainMenuItems[EDIT_MAIN_SAVE].flags |= GUI_ITEM_ENABLED;
    } else {
        sEditMainMenuItems[EDIT_MAIN_SAVE].flags &= ~GUI_ITEM_ENABLED;
    }
    if (gEditor->menu != NULL) {
        gui_menu_fix_cursor(gEditor->menu);
    }
    if (gEditor->slideState == EDIT_SLIDE_SWAP && gEditor->nextMenu != NULL) {
        gEditor->slideState = EDIT_SLIDE_SHOWN;
        gEditor->menu = gEditor->nextMenu;
        gEditor->nextMenu = NULL;
    }
    switch (sEditTrackMenu.choice) {
    case EDIT_ITEM_STRAIGHT:
        gEditor->selType = &gPieceTypes[sEditTrackMenu.items[EDIT_ITEM_STRAIGHT].value];
        break;
    case EDIT_ITEM_TURN:
        gEditor->selType = &gPieceTypes[PIECE_TYPE_TURN + sEditTrackMenu.items[EDIT_ITEM_TURN].value];
        break;
    case EDIT_ITEM_JUMP:
        gEditor->selType = &gPieceTypes[PIECE_TYPE_JUMP + sEditTrackMenu.items[EDIT_ITEM_JUMP].value];
        break;
    case EDIT_ITEM_USER:
        gEditor->selType = &gPieceTypes[PIECE_TYPE_USER + sEditTrackMenu.items[EDIT_ITEM_USER].value];
        break;
    }
    if (gEditor->flags & EDIT_EXIT) {
        if (++gEditor->exitTimer >= 0) {
            if (gEditor->flags & EDIT_TEST_TRACK) {
                usrtrk_layout_from_editor();
            }
            gCurGameState->mode = 3; /* exit requested */
        }
    } else {
        switch (gEditor->page) {
        case EDIT_PAGE_MAIN:
            edit_update_main_menu();
            break;
        case EDIT_PAGE_TRACK:
            edit_update_track_menu();
            break;
        }
    }
    piece = gEditor->first;
    if (piece != NULL) {
        do {
            edit_update_piece_ports(piece);
            piece = piece->next;
        } while (piece != NULL && piece != gEditor->first);
    }
}

/* Draws name entry: the keyboard with the cursor on the selected key, and the name. */
Gfx *edit_draw_name_entry(Gfx *gdl, s32 fb)
{
    s32 j, row, col, key, x, rowShift, font, color;
    u8 *img;
    Editor *ed;
    u8 letter[8];

    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetPrimColor(gdl++, 0, 0, 0xFA, 0xFA, 0xFA, 0xFF);
    gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetRenderMode(gdl++, 0x00404240, 0);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    img = (u8 *)gGui->images + 0x48;
    gdl = render_draw_image(0xBC, 0x78 - (img[1] >> 1), img, gdl, fb, 1, 1, 6, 0);
    font_text_create(gEditor->bigText, 0x40, 2, 2, 0x20);
    font_text_add(gEditor->bigText, sStrTrackNameLabel);
    font_draw_text(gEditor->bigText, 0xCC, 0xA7);
    rowShift = -0xF2;
    /* the keyboard, 5 rows of 8 keys, each row set 5 pixels left of the one above */
    for (row = 0; row < 5; row++) {
        for (col = 0, key = row << 3; col < EDIT_KEYS_PER_ROW; col++) {
            ed = gEditor;
            font = 1;
            if (key >= EDIT_KEY_DEL) font = 2;
            color = 2;
            if (key == ed->nameKey) color = 7;
            if (ed->nameLen == EDIT_NAME_LEN && key < EDIT_KEY_DEL) font_text_create(ed->bigText, 0x20, font, color, 0x20000021);
            else font_text_create(ed->bigText, 0x20, font, color, 0x21);
            font_text_add(gEditor->bigText, gKeyboardKeys[key]);
            x = col * 26 - (row * 5 + rowShift);
            font_draw_text(gEditor->bigText, x, row * 17 + 0x48);
            if (key == gEditor->nameKey) {
                img = (u8 *)gGui->images + 0x50;
                gdl = render_draw_image(x - img[0] * 4 + 4, row * 17 - (img[1] >> 1) + 0x49, img, gdl, fb, 1, 1, 6, 0);
                do { } while (0);
                key++;
            } else {
                key++;
            }
        }
    }
    /* the name so far, then the underlines */
    for (j = 0; j < EDIT_NAME_LEN; j++) {
        if (gEditor->nameBuf[j] == 0) break;
        color = 2;
        if (j == gEditor->nameLen) color = 7;
        font_text_create(gEditor->bigText, 0x20, 2, color, 0x21);
        letter[0] = gEditor->nameBuf[j];
        letter[1] = 0;
        font_text_add(gEditor->bigText, letter);
        font_draw_text(gEditor->bigText, j * 13 + 0x121, 0xA5);
    }
    for (j = 0; j < EDIT_NAME_LEN; j++) {
        color = 2;
        if (j == gEditor->nameLen) color = 7;
        font_text_create(gEditor->bigText, 0x20, 1, color, 0x21);
        font_text_add(gEditor->bigText, sStrNameUnderline);
        font_draw_text(gEditor->bigText, j * 13 + 0x120, 0xAB);
    }
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPPipeSync(gdl++);
    return gdl;
}

/* Game state 2's draw: clears the screen, then the menu, the 3D view, the warnings, the track's name, the bars and
   the current piece's name, and name entry. */
void edit_draw(s32 ctx, s32 fb, Gfx **pgdl)
{
    char buf[0x68];
    char *numStr;
    Gfx *gdl;
    s32 i;
    u8 *bar;
    u8 *img;

    sEditDrawCtx = ctx;
    /* the menu panel slides out to the left over 30 frames, the 3D view widening with it */
    switch (gEditor->slideState) {
    case EDIT_SLIDE_SHOWN:
        gEditor->slideOffset = 0;
        break;
    case EDIT_SLIDE_OUT:
        gEditor->slideOffset = 30 - gEditor->slideTimer;
        break;
    case EDIT_SLIDE_IN:
        gEditor->slideOffset = gEditor->slideTimer;
        break;
    case EDIT_SLIDE_SWAP:
        gEditor->slideOffset = 30;
        break;
    }
    gEditor->slideOffset *= 4;
    sEditViewRects[EDIT_VIEW_TRACK].x = sEditViewRectsBase[EDIT_VIEW_TRACK].x - gEditor->slideOffset;
    sEditViewRects[EDIT_VIEW_TRACK].w = sEditViewRectsBase[EDIT_VIEW_TRACK].w + gEditor->slideOffset;
    sEditViewRects[EDIT_VIEW_MENU].x = sEditViewRectsBase[EDIT_VIEW_MENU].x - gEditor->slideOffset;
    sEditViewRects[3].x = sEditViewRectsBase[3].x - gEditor->slideOffset;
    arned_set_viewport_stretch(sEditViewRects[EDIT_VIEW_TRACK].x, sEditViewRects[EDIT_VIEW_TRACK].y, sEditViewRects[EDIT_VIEW_TRACK].w - 1,
                               sEditViewRects[EDIT_VIEW_TRACK].h - 1, gEditor->slideOffset);
    gEditor->views[EDIT_VIEW_TRACK].vp.vscale[0] = sEditViewRectsBase[EDIT_VIEW_TRACK].w * 2;
    gEditor->views[EDIT_VIEW_TRACK].vp.vscale[1] = sEditViewRectsBase[EDIT_VIEW_TRACK].h * 2;
    gEditor->views[EDIT_VIEW_TRACK].vp.vtrans[0] = (sEditViewRectsBase[EDIT_VIEW_TRACK].x + sEditViewRectsBase[EDIT_VIEW_TRACK].w / 2) * 4;
    gEditor->views[EDIT_VIEW_TRACK].vp.vtrans[1] = (sEditViewRectsBase[EDIT_VIEW_TRACK].y + sEditViewRectsBase[EDIT_VIEW_TRACK].h / 2) * 4;
    gEditor->views[EDIT_VIEW_MENU].vp.vscale[0] = sEditViewRects[EDIT_VIEW_MENU].w * 2;
    gEditor->views[EDIT_VIEW_MENU].vp.vscale[1] = sEditViewRects[EDIT_VIEW_MENU].h * 2;
    gEditor->views[EDIT_VIEW_MENU].vp.vtrans[0] = (sEditViewRects[EDIT_VIEW_MENU].x + sEditViewRects[EDIT_VIEW_MENU].w / 2) * 4;
    gEditor->views[EDIT_VIEW_MENU].vp.vtrans[1] = (sEditViewRects[EDIT_VIEW_MENU].y + sEditViewRects[EDIT_VIEW_MENU].h / 2) * 4;
    gEditor->views[3].vp.vscale[0] = sEditViewRects[3].w * 2;
    gEditor->views[3].vp.vscale[1] = sEditViewRects[3].h * 2;
    gEditor->views[3].vp.vtrans[0] = (sEditViewRects[3].x + sEditViewRects[3].w / 2) * 4;
    gEditor->views[3].vp.vtrans[1] = (sEditViewRects[3].y + sEditViewRects[3].h / 2) * 4;

    gdl = *pgdl;
    gdl = render_load_ucode(gdl, fb, ctx, gF3dex2NonText, gF3dex2NonData);
    gdl = render_restore_state(gdl, fb, ctx);
    gDPSetTextureFilter(gdl++, G_TF_AVERAGE);
    gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
    gSPClipRatio(gdl++, FRUSTRATIO_2);
    gSPViewport(gdl++, &gEditor->screenVp);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, gScreenWidth, gScreenHeight);
    gDPPipeSync(gdl++);
    gDPSetDepthImage(gdl++, gZBuffer);
    gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gZBuffer);
    gDPSetFillColor(gdl++, 0xFFFCFFFC); /* the z-buffer cleared to the far plane */
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gFramebuffers[fb]);
    gDPSetFillColor(gdl++, 0);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gFramebuffers[fb]);
    gDPSetDepthSource(gdl++, G_ZS_PIXEL);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, sEditViewRects[EDIT_VIEW_SCREEN].x, sEditViewRects[EDIT_VIEW_SCREEN].y,
                  sEditViewRects[EDIT_VIEW_SCREEN].x + sEditViewRects[EDIT_VIEW_SCREEN].w - 1,
                  sEditViewRects[EDIT_VIEW_SCREEN].y + sEditViewRects[EDIT_VIEW_SCREEN].h - 1);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPPipeSync(gdl++);
    gDPSetFillColor(gdl++, 0x00010001);
    gDPFillRectangle(gdl++, sEditViewRects[EDIT_VIEW_MENU].x, sEditViewRects[EDIT_VIEW_MENU].y,
                     sEditViewRects[EDIT_VIEW_MENU].x + sEditViewRects[EDIT_VIEW_MENU].w - 1,
                     sEditViewRects[EDIT_VIEW_MENU].y + sEditViewRects[EDIT_VIEW_MENU].h - 1);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gdl = gui_draw_menu(fb, gdl, gEditor->menu, 0x1A, 0x11);
    gdl = edit_draw_track_view(ctx, fb, gdl);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetPrimColor(gdl++, 0, 0, 0xB4, 0xB4, 0xB4, 0xFF);

    switch ((u32)gEditor->mode) { /* compared unsigned here */
    case EDIT_MODE_WARN_MULTI:
    case EDIT_MODE_WARN_LOCKED:
        font_text_create(gEditor->text, 0x20, 4, 2, 0x21);
        font_text_add(gEditor->text, "Warning");
        font_draw_text(gEditor->text, 0x14C, 0x48);
        for (i = 0;; i++) {
            if (*(gEditor->mode == EDIT_MODE_WARN_MULTI ? &sEditMultiUseWarning[i] : &sEditSizeLockWarning[i]) == NULL) break;
            font_text_create(gEditor->text, 0x20, 5, 2, 0x21);
            font_text_add(gEditor->text, *(gEditor->mode == EDIT_MODE_WARN_MULTI ? &sEditMultiUseWarning[i] : &sEditSizeLockWarning[i]));
            font_draw_text(gEditor->text, 0x144 - i * 4, i * 12 + 0x61);
        }
        gDPSetTextureLUT(gdl++, G_TT_RGBA16);
        gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
        img = gEditor->warnIcon->img;
        gDPLoadTLUT_pal256(gdl++, gEditor->warnIcon->pal);
        gdl = render_draw_image(0xD0, 0x3D, img, gdl, fb, 1, 1, 3, 0);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        gdl = render_draw_image(0xB6, 0x3A, gEditor->warnFrame->img, gdl, fb, 1, 1, 6, 0);
        break;
    }

    if (gEditor->cur != NULL) {
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        bar = (u8 *)gGui->images + 0x58;
        font_text_create(gEditor->text, 0x20, 5, 2, 0x21);
        font_text_add(gEditor->text, sStrTrackNameLabel);
        font_text_add(gEditor->text, gEditor->name);
        font_draw_text(gEditor->text, 0x140, 0x19);
        /* the bars above and below the 3D view (the lower one blinking while placing) */
        for (i = 0; i < (0x207 - sEditViewRects[EDIT_VIEW_TRACK].x) / (bar[0] * 8); i++) {
            gdl = render_draw_image(sEditViewRects[EDIT_VIEW_TRACK].x + bar[0] * (i << 3), 0x11, bar, gdl, fb, 1, 1, 6, 0);
        }
        if (gEditor->mode == EDIT_MODE_PLACE && (gFrameCounter & 0x20)) {
            gDPSetPrimColor(gdl++, 0, 0, 0xB4, 0xB4, 0x50, 0xFF);
        }
        for (i = 0; i < (0x207 - sEditViewRects[EDIT_VIEW_TRACK].x) / (bar[0] * 8); i++) {
            gdl = render_draw_image(sEditViewRects[EDIT_VIEW_TRACK].x + bar[0] * (i << 3), 0xCD, bar, gdl, fb, 1, 1, 6, 0);
        }
        gDPPipeSync(gdl++);
        font_text_create(gEditor->bigText, 0x40, 5, 2, 0x21);
        /* the current piece's name: "Straight 1 : ...", the user pieces by their own name */
        i = gEditor->cur->type - gPieceTypes;
        if (gEditor->mode == EDIT_MODE_PLACE && gEditor->cur != NULL &&
            (gEditor->cur->next != 0 || gEditor->cur->prev != 0)) {
            font_text_add(gEditor->bigText, "< ");
        }
        if (i < PIECE_TYPE_TURN) {
            font_text_add(gEditor->bigText, "Straight ");
            i += 1;
        } else if (i < PIECE_TYPE_JUMP) {
            font_text_add(gEditor->bigText, "Turn ");
            i -= 1;
        } else if (i < PIECE_TYPE_USER) {
            font_text_add(gEditor->bigText, "Jump ");
            i -= 5;
        } else {
            i -= 17;
        }
        if (gEditor->cur->type - gPieceTypes < PIECE_TYPE_USER) {
            numStr = buf + 0x48;
            int_to_str(i, numStr);
            font_text_add(gEditor->bigText, numStr);
            font_text_add(gEditor->bigText, " : ");
        }
        font_text_add(gEditor->bigText, gEditor->cur->type->name);
        if (gEditor->cur->type - gPieceTypes >= PIECE_TYPE_USER) {
            if (gEditor->mode == EDIT_MODE_PLACE) {
                font_text_add(gEditor->bigText, " - Press Z Button To Edit");
            } else if (gEditor->mode == EDIT_MODE_USER_PIECE && gPieceTypes[gEditor->userPiece + PIECE_TYPE_USER].dim[1] >= 5 &&
                       !(gPieceTypes[gEditor->userPiece + PIECE_TYPE_USER].flags & PIECE_SIZE_LOCKED) &&
                       (gEditor->heightSel == 1 || gEditor->heightSel == gPieceTypes[gEditor->userPiece + PIECE_TYPE_USER].dim[1] * 2 - 1)) {
                font_text_add(gEditor->bigText, " - R Button Deletes Segment");
            }
        } else if (gEditor->mode == EDIT_MODE_PLACE && (*(u32 *)&gEditor->cur->type->flags & ((PIECE_FITS | PIECE_FITS_FLIPPED) << 16)) == ((PIECE_FITS | PIECE_FITS_FLIPPED) << 16)) {
            font_text_add(gEditor->bigText, " - R Button Flips");
        }
        if (gEditor->mode == EDIT_MODE_PLACE && gEditor->cur != NULL &&
            (gEditor->cur->next != 0 || gEditor->cur->prev != 0)) {
            font_text_add(gEditor->bigText, " >");
        }
        font_draw_text(gEditor->bigText, 0x142, 0xD5);
    }
    if (gEditor->mode == EDIT_MODE_NAME || gEditor->mode == EDIT_MODE_NAME_8) {
        gdl = edit_draw_name_entry(gdl, fb);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, sEditViewRects[EDIT_VIEW_SCREEN].x, sEditViewRects[EDIT_VIEW_SCREEN].y,
                  sEditViewRects[EDIT_VIEW_SCREEN].x + sEditViewRects[EDIT_VIEW_SCREEN].w - 1,
                  sEditViewRects[EDIT_VIEW_SCREEN].y + sEditViewRects[EDIT_VIEW_SCREEN].h - 1);
    gdl = font_render(gdl);
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}

/* Sets a piece's connectors (dir: the sides they face after rotation, off: where along them) and its size from its
   type, rotation and flip. */
void edit_update_piece_ports(EditPiece *piece)
{
    s32 entryEnd, exitEnd;

    if (piece == NULL) return;
    /* a flipped piece is entered at its type's exit */
    if (piece->flags & EDIT_PIECE_FLIPPED) {
        exitEnd = 0;
        entryEnd = 1;
    } else {
        entryEnd = 0;
        exitEnd = 1;
    }
    piece->dir[0] = (piece->type->dir[entryEnd] + piece->rot) & 3;
    piece->dir[1] = (piece->type->dir[exitEnd] + piece->rot) & 3;

    switch (piece->dir[0]) {
    case SIDE_POS_Y:
        switch (piece->type->dir[entryEnd]) {
        case SIDE_POS_Y: case SIDE_NEG_X: piece->off[0] = piece->type->off[entryEnd]; break;
        case SIDE_NEG_Y: piece->off[0] = piece->type->dim[0] - piece->type->off[entryEnd] - 2; break;
        case SIDE_POS_X: piece->off[0] = piece->type->dim[1] - piece->type->off[entryEnd] - 2; break;
        }
        break;
    case SIDE_NEG_Y:
        switch (piece->type->dir[entryEnd]) {
        case SIDE_POS_X: case SIDE_NEG_Y: piece->off[0] = piece->type->off[entryEnd]; break;
        case SIDE_POS_Y: piece->off[0] = piece->type->dim[0] - piece->type->off[entryEnd] - 2; break;
        case SIDE_NEG_X: piece->off[0] = piece->type->dim[1] - piece->type->off[entryEnd] - 2; break;
        }
        break;
    case SIDE_NEG_X:
        switch (piece->type->dir[entryEnd]) {
        case SIDE_POS_Y: case SIDE_NEG_X: piece->off[0] = piece->type->off[entryEnd]; break;
        case SIDE_NEG_Y: piece->off[0] = piece->type->dim[0] - piece->type->off[entryEnd] - 2; break;
        case SIDE_POS_X: piece->off[0] = piece->type->dim[1] - piece->type->off[entryEnd] - 2; break;
        }
        break;
    case SIDE_POS_X:
        switch (piece->type->dir[entryEnd]) {
        case SIDE_POS_X: case SIDE_NEG_Y: piece->off[0] = piece->type->off[entryEnd]; break;
        case SIDE_POS_Y: piece->off[0] = piece->type->dim[0] - piece->type->off[entryEnd] - 2; break;
        case SIDE_NEG_X: piece->off[0] = piece->type->dim[1] - piece->type->off[entryEnd] - 2; break;
        }
        break;
    }

    switch (piece->dir[1]) {
    case SIDE_POS_Y:
        switch (piece->type->dir[exitEnd]) {
        case SIDE_POS_Y: case SIDE_NEG_X: piece->off[1] = piece->type->off[exitEnd]; break;
        case SIDE_NEG_Y: piece->off[1] = piece->type->dim[0] - piece->type->off[exitEnd] - 2; break;
        case SIDE_POS_X: piece->off[1] = piece->type->dim[1] - piece->type->off[exitEnd] - 2; break;
        }
        break;
    case SIDE_NEG_Y:
        switch (piece->type->dir[exitEnd]) {
        case SIDE_POS_X: case SIDE_NEG_Y: piece->off[1] = piece->type->off[exitEnd]; break;
        case SIDE_POS_Y: piece->off[1] = piece->type->dim[0] - piece->type->off[exitEnd] - 2; break;
        case SIDE_NEG_X: piece->off[1] = piece->type->dim[1] - piece->type->off[exitEnd] - 2; break;
        }
        break;
    case SIDE_NEG_X:
        switch (piece->type->dir[exitEnd]) {
        case SIDE_POS_Y: case SIDE_NEG_X: piece->off[1] = piece->type->off[exitEnd]; break;
        case SIDE_NEG_Y: piece->off[1] = piece->type->dim[0] - piece->type->off[exitEnd] - 2; break;
        case SIDE_POS_X: piece->off[1] = piece->type->dim[1] - piece->type->off[exitEnd] - 2; break;
        }
        break;
    case SIDE_POS_X:
        switch (piece->type->dir[exitEnd]) {
        case SIDE_POS_X: case SIDE_NEG_Y: piece->off[1] = piece->type->off[exitEnd]; break;
        case SIDE_POS_Y: piece->off[1] = piece->type->dim[0] - piece->type->off[exitEnd] - 2; break;
        case SIDE_NEG_X: piece->off[1] = piece->type->dim[1] - piece->type->off[exitEnd] - 2; break;
        }
        break;
    }

    if (piece->rot & 1) {
        piece->w = piece->type->dim[1];
        piece->h = piece->type->dim[0];
    } else {
        piece->w = piece->type->dim[0];
        piece->h = piece->type->dim[1];
    }
}

/* Adds a line from screen point (x0, y0) to (x1, y1) at depths z0, z1, in colour r g b, to frame buffer frame's line
   vertices (centred on the screen, y up). */
void edit_add_overlay_line(s32 frame, s32 x0, s32 y0, s16 z0, s32 x1, s32 y1, s32 z1, u8 r, u8 g, u8 b)
{
    s32 dx, dy, thickX, thickY;

    x0 = x0 - gScreenWidth / 2;
    x1 = x1 - gScreenWidth / 2;
    y0 = gScreenHeight / 2 - y0;
    y1 = -y1;
    y1 += gScreenHeight / 2;
    if (gEditor->lineCount >= EDIT_MAX_LINES) {
        return;
    }
    dx = x1 - x0;
    if (dx <= 0) dx = x0 - x1;
    dy = y1 - y0;
    /* a quad 2 pixels high for a flat line, 3 wide for a steep one */
    if ((dy <= 0 ? y0 - y1 : dy) < dx) {
        thickX = 0;
        thickY = 2;
    } else {
        thickX = 3;
        thickY = 0;
    }
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 0].v.ob[0] = x0;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 0].v.ob[1] = y0;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 0].v.ob[2] = z0;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 1].v.ob[0] = x0 + thickX;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 1].v.ob[1] = y0 + thickY;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 1].v.ob[2] = z0;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 2].v.ob[0] = x1;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 2].v.ob[1] = y1;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 2].v.ob[2] = z1;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 3].v.ob[0] = x1 + thickX;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 3].v.ob[1] = y1 + thickY;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 3].v.ob[2] = z1;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 0].v.cn[0] = r;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 0].v.cn[1] = g;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 0].v.cn[2] = b;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 1].v.cn[0] = r;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 1].v.cn[1] = g;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 1].v.cn[2] = b;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 2].v.cn[0] = r;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 2].v.cn[1] = g;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 2].v.cn[2] = b;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 3].v.cn[0] = r;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 3].v.cn[1] = g;
    gEditor->lineVtx[frame][gEditor->lineCount * 4 + 3].v.cn[2] = b;
    gEditor->lineCount++;
}

/* Draws the frame's lines in an orthographic projection of the screen. */
Gfx *edit_draw_overlay_lines(u8 *ctx, s32 frame, Gfx *gdl)
{
    Mtx *proj;
    s32 i;

    proj = (Mtx *)(ctx + 0x300); /* a matrix of the frame's graphics context */
    guOrtho(proj, -gScreenWidth / 2, gScreenWidth / 2, -gScreenHeight / 2, gScreenHeight / 2, 50.0f,
                  gFarClip, 1.0f);
    gSPMatrix(gdl++, &gIdentityMtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, proj, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gSPViewport(gdl++, &gScreenViewport);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, sEditViewRects[EDIT_VIEW_TRACK].x, sEditViewRects[EDIT_VIEW_TRACK].y,
                  sEditViewRects[EDIT_VIEW_TRACK].x + sEditViewRects[EDIT_VIEW_TRACK].w - 1,
                  sEditViewRects[EDIT_VIEW_TRACK].y + sEditViewRects[EDIT_VIEW_TRACK].h - 1);
    gSPClearGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                                G_TEXTURE_GEN_LINEAR | G_LOD);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, G_SHADE);
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gDPSetCombineMode(gdl++, G_CC_SHADE, G_CC_SHADE);
    gDPSetRenderMode(gdl++, 0x00442078, 0);
    for (i = 0; i < gEditor->lineCount; i++) {
        gSPVertex(gdl++, gEditor->lineVtx[frame] + i * 4, 4, 0);
        gDPPipeSync(gdl++);
        gSP2Triangles(gdl++, 0, 1, 3, 0, 0, 3, 2, 0);
        gDPPipeSync(gdl++);
    }
    return gdl;
}

/* Projects world point (x, y, z) (mm) with the editor's camera matrix to the screen (sx, sy, depth sz, w sw);
   returns 1 when it is behind the camera or out of the depth range. */
s32 edit_project_to_screen(s32 x, s32 y, s32 z, s32 *sx, s32 *sy, s32 *sz, s32 *sw, s32 ctx, s32 frame) {
    f32 halfW;
    f32 halfH;
    f32 centreX;
    f32 centreY;
    f32 px;
    f32 py;
    f32 pz;
    f32 pw;

    halfW = gArnedViewport_2.vscale[0] * 0.25f;
    halfH = gArnedViewport_2.vscale[1] * 0.25f;
    /* mm to the camera matrix's 16 mm units */
    x *= 0.0625f;
    y *= 0.0625f;
    z *= 0.0625f;
    px = gEditor->camMtx[0][0] * x + gEditor->camMtx[1][0] * y + gEditor->camMtx[2][0] * z + gEditor->camMtx[3][0];
    py = gEditor->camMtx[0][1] * x + gEditor->camMtx[1][1] * y + gEditor->camMtx[2][1] * z + gEditor->camMtx[3][1];
    pz = gEditor->camMtx[0][2] * x + gEditor->camMtx[1][2] * y + gEditor->camMtx[2][2] * z + gEditor->camMtx[3][2];
    pw = gEditor->camMtx[0][3] * x + gEditor->camMtx[1][3] * y + gEditor->camMtx[2][3] * z + gEditor->camMtx[3][3];
    centreX = gArnedViewport_2.vtrans[0] * 0.25f;
    centreY = gArnedViewport_2.vtrans[1] * 0.25f;
    if (pw == 0.0f) {
        return 1;
    }
    px /= pw;
    py /= pw;
    pz /= pw;
    if (pz <= -1.0f || pz >= 1.0f) {
        return 1;
    }
    {
        f32 rx, ry, rz;
        rx = px * halfW + centreX;
        ry = py * -halfH + centreY;
        rz = pz * (gFarClip - 50.0f) + 50.0f;
        *sx = rx;
        *sy = ry;
        *sz = rz;
    }
    if (sw != NULL) {
        *sw = pw;
    }
    return 0;
}

/* cos(i / 0x1000 turns) in 1/1024 */
#define COS_1024(i) ((s16)gCosTable[(i) & 0xFFF] >> 4)
/* The mesh sets advanced by k cells: ->cells[0] is cells[k] */
#define MESH_A_AT(type, k) ((PieceMeshSet *)((u8 *)(type)->meshA + ((k) << 5)))
#define MESH_B_AT(type, k) ((PieceMeshSet *)((u8 *)(type)->meshB + ((k) << 5)))
#define TYPE_W (piece->type->dim[0])
#define TYPE_H (piece->type->dim[1])
/* The cell of the piece at (col, row) of the grid, for each rotation */
#define CELL_ROT0 (row * TYPE_W + col)
#define CELL_ROT1 (col * TYPE_W + TYPE_W - 1 - row)
#define CELL_ROT2 ((TYPE_H - 1 - row) * TYPE_W + TYPE_W - 1 - col)
#define CELL_ROT3 ((TYPE_H - 1 - col) * TYPE_W + row)
/* The current piece's corner on the grid in mm; the turned cases take gridSize[1] for x and gridSize[0] for z */
#define CUR_X0(axis) (gEditor->cur->x * CELL_MM - gEditor->gridSize[axis] * (CELL_MM / 2))
#define CUR_Z0(axis) (gEditor->cur->y * CELL_MM - gEditor->gridSize[axis] * (CELL_MM / 2))

/* Draws the 3D view: lays every piece's cells on the grid (the view's camera follows the mode), draws it with the
   start cursor and, in the height editor, the user piece's height lines; then the message or OK/Cancel box. */
Gfx *edit_draw_track_view(s32 ctx, s32 fb, Gfx *gdl) {
    s32 p0[3];
    s32 p1[3];
    s32 p2[3];
    s32 p3[3];
    EditPiece *piece;
    s32 inMark;
    s32 cellFlags;
    s32 col;
    s32 row;
    s32 point;
    s32 revPoint;
    s16 rot;
    s32 flags;
    u32 yaw;
    s32 cursorYaw;

    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPPipeSync(gdl++);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, sEditViewRects[EDIT_VIEW_TRACK].x, sEditViewRects[EDIT_VIEW_TRACK].y,
                  sEditViewRects[EDIT_VIEW_TRACK].x + sEditViewRects[EDIT_VIEW_TRACK].w - 1,
                  sEditViewRects[EDIT_VIEW_TRACK].y + sEditViewRects[EDIT_VIEW_TRACK].h - 1);
    gSPViewport(gdl++, &gEditor->views[EDIT_VIEW_TRACK]);
    gSPClearGeometryMode(gdl, G_ZBUFFER); /* gdl not advanced: the next packet overwrites it, as in the original */
    gDPSetRenderMode(gdl++, 0x0C084000, 0);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFDF6FB);
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 1);
    gDPFillRectangle(gdl++, sEditViewRects[EDIT_VIEW_TRACK].x, sEditViewRects[EDIT_VIEW_TRACK].y,
                     sEditViewRects[EDIT_VIEW_TRACK].x + sEditViewRects[EDIT_VIEW_TRACK].w - 1,
                     sEditViewRects[EDIT_VIEW_TRACK].y + sEditViewRects[EDIT_VIEW_TRACK].h - 1);
    gDPPipeSync(gdl++);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF279);
    gDPSetRenderMode(gdl++, 0x004041C8, 0);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);

    /* the camera: straight down while moving the track, else orbiting the target at its distance */
    if (gEditor->mode == EDIT_MODE_MOVE_TRACK) {
        arned_set_camera_2(fb, 0, 250000, 0, 0, 0x4000);
    } else if (gEditor->mode == EDIT_MODE_USER_PIECE) {
        yaw = sEditCamPos.yaw;
        arned_set_camera_2(fb, sEditCamPos.x + ((COS_1024((yaw >> 4) - 0x400) * gEditor->cam[1].dist) >> 10), gEditor->cam[1].height,
                      sEditCamPos.z - ((COS_1024(yaw >> 4) * gEditor->cam[1].dist) >> 10), yaw, gEditor->cam[1].pitch);
    } else {
        yaw = sEditCamPos.yaw;
        arned_set_camera_2(fb, sEditCamPos.x + ((COS_1024((yaw >> 4) - 0x400) * gEditor->cam[0].dist) >> 10), gEditor->cam[0].height,
                      sEditCamPos.z - ((COS_1024(yaw >> 4) * gEditor->cam[0].dist) >> 10), yaw, gEditor->cam[0].pitch);
    }

    /* editing a user piece shows only the pieces round it up to the corners on either side */
    piece = gEditor->first;
    if (piece != NULL) {
        do {
            if (gEditor->cur != NULL && gEditor->mode == EDIT_MODE_USER_PIECE && (gEditor->cur->type->flags & PIECE_USER)) {
                flags = piece->flags & ~EDIT_PIECE_VISIBLE;
            } else {
                flags = piece->flags | EDIT_PIECE_VISIBLE;
            }
            piece->flags = flags;
            piece = piece->next;
        } while (piece != NULL && piece != gEditor->first);
    }
    if (gEditor->cur != NULL && gEditor->mode == EDIT_MODE_USER_PIECE && (gEditor->cur->type->flags & PIECE_USER)) {
        for (piece = gEditor->cur->next; piece != NULL; piece = piece->next) {
            piece->flags |= EDIT_PIECE_VISIBLE;
            if (piece->type->flags & PIECE_CORNER) {
                break;
            }
        }
        for (piece = gEditor->cur; piece != NULL; piece = piece->prev) {
            piece->flags |= EDIT_PIECE_VISIBLE;
            if (piece->type->flags & PIECE_CORNER) {
                break;
            }
        }
    }

    /* each piece's cells into the 3D view's grid */
    piece = gEditor->first;
    inMark = 0;
    if (piece != NULL) {
        do {
            if (gEditor->markStart != NULL && gEditor->markEnd != NULL) {
                if (inMark != 0 && piece->prev == gEditor->markEnd) {
                    inMark = 0;
                }
                if (piece == gEditor->markStart) {
                    inMark = 1;
                }
            }
            cellFlags = 0;
            if ((piece->flags & EDIT_PIECE_PREVIEW) || piece == gEditor->cur) {
                cellFlags = ARNED_CELL_SELECTED;
            }
            cellFlags |= ARNED_CELL_REBUILD;
            for (row = 0; row < piece->type->dim[(piece->rot + 1) & 1]; row++) {
                for (col = 0; col < piece->type->dim[(rot = piece->rot) & 1]; col++) {
                    if (piece->x + col < 0 || piece->x + col >= gEditor->gridSize[0]) {
                        continue;
                    }
                    if (piece->y + row < 0 || piece->y + row >= gEditor->gridSize[1]) {
                        continue;
                    }
                    if (piece->flags & EDIT_PIECE_VISIBLE) {
                        switch (rot) {
                        case 0:
                            arned_set_cell(piece->x + col, piece->y + row, MESH_A_AT(piece->type, CELL_ROT0)->cells[0].mesh, rot,
                                          piece->type->meshB ? MESH_B_AT(piece->type, CELL_ROT0)->cells[0].mesh : 0, piece->rot, cellFlags,
                                          piece->type->meshB ? (MESH_B_AT(piece->type, CELL_ROT0)->cells[0].x0 + MESH_B_AT(piece->type, CELL_ROT0)->cells[0].x1) / 2 - 250 - col * 500 : 0,
                                          piece->type->meshB ? (MESH_B_AT(piece->type, CELL_ROT0)->cells[0].y0 + MESH_B_AT(piece->type, CELL_ROT0)->cells[0].y1) / 2 - 250 - row * 500 : 0);
                            break;
                        case 1:
                            arned_set_cell(piece->x + col, piece->y + row, MESH_A_AT(piece->type, CELL_ROT1)->cells[0].mesh, rot,
                                          piece->type->meshB ? MESH_B_AT(piece->type, CELL_ROT1)->cells[0].mesh : 0, piece->rot, cellFlags,
                                          piece->type->meshB ? (MESH_B_AT(piece->type, CELL_ROT1)->cells[0].y0 + MESH_B_AT(piece->type, CELL_ROT1)->cells[0].y1) / 2 - 250 - col * 500 : 0,
                                          piece->type->meshB ? (piece->h - row - 1) * 500 + (250 - (MESH_B_AT(piece->type, CELL_ROT1)->cells[0].x0 + MESH_B_AT(piece->type, CELL_ROT1)->cells[0].x1) / 2) : 0);
                            break;
                        case 2:
                            arned_set_cell(piece->x + col, piece->y + row, MESH_A_AT(piece->type, CELL_ROT2)->cells[0].mesh, rot,
                                          piece->type->meshB ? MESH_B_AT(piece->type, CELL_ROT2)->cells[0].mesh : 0, piece->rot, cellFlags,
                                          piece->type->meshB ? (piece->w - col - 1) * 500 + (250 - (MESH_B_AT(piece->type, CELL_ROT2)->cells[0].x0 + MESH_B_AT(piece->type, CELL_ROT2)->cells[0].x1) / 2) : 0,
                                          piece->type->meshB ? (piece->h - row - 1) * 500 + (250 - (MESH_B_AT(piece->type, CELL_ROT2)->cells[0].y0 + MESH_B_AT(piece->type, CELL_ROT2)->cells[0].y1) / 2) : 0);
                            break;
                        case 3:
                            arned_set_cell(piece->x + col, piece->y + row, MESH_A_AT(piece->type, CELL_ROT3)->cells[0].mesh, rot,
                                          piece->type->meshB ? MESH_B_AT(piece->type, CELL_ROT3)->cells[0].mesh : 0, piece->rot, cellFlags,
                                          piece->type->meshB ? (piece->w - col - 1) * 500 + (250 - (MESH_B_AT(piece->type, CELL_ROT3)->cells[0].y0 + MESH_B_AT(piece->type, CELL_ROT3)->cells[0].y1) / 2) : 0,
                                          piece->type->meshB ? (MESH_B_AT(piece->type, CELL_ROT3)->cells[0].x0 + MESH_B_AT(piece->type, CELL_ROT3)->cells[0].x1) / 2 - 250 - row * 500 : 0);
                            break;
                        }
                    } else {
                        arned_set_cell(piece->x + col, piece->y + row, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
            piece = piece->next;
        } while (piece != NULL && piece != gEditor->first);
    }

    arned_set_scissor(0, 0, gScreenWidth, gScreenHeight);
    /* the 3D view with the start cursor; in the height editor, the user piece's height lines over it */
    if (gEditor->start != NULL) {
        cursorYaw = ((-gEditor->start->rot * 2 - gEditor->start->type->cursorAngle) & 7) << 13;
        gdl = arned_draw(fb, gdl, gEditor->cur->x, gEditor->cur->y, gEditor->cur->w, gEditor->cur->h,
                            (gEditor->start->x * 2 + gEditor->start->w) * 4000 - 60000, (gEditor->start->y * 2 + gEditor->start->h) * 4000 - 60000,
                            (gEditor->start->type->flags & PIECE_START) ? cursorYaw : -1, 1);
        if (gEditor->mode == EDIT_MODE_USER_PIECE) {
            gEditor->lineCount = 0;
            guMtxL2F(gEditor->camMtx, &gArnedCamMtx[fb]);
            mem_fill(gEditor->lineVtx[fb], 0, EDIT_MAX_LINES * 4 * sizeof(Vtx));
            if (gEditor->cur != NULL && (gEditor->cur->type->flags & PIECE_USER)) {
                /* a line across the piece at each height point, and lines along it between them */
                switch (gEditor->cur->rot) {
                case 0:
                    for (point = 0; point < gEditor->cur->type->dim[1] * 2 + 1; point++) {
                        edit_project_to_screen(CUR_X0(0) + 2000, point != 0 ? gUserPieceHeights[gEditor->userPiece][point - 1] * 16 : 0,
                                      CUR_Z0(1) + point * 4000, &p0[0], &p0[1], &p0[2], NULL, ctx, fb);
                        edit_project_to_screen(CUR_X0(0) + 14000, point != 0 ? gUserPieceHeights[gEditor->userPiece][point - 1] * 16 : 0,
                                      CUR_Z0(1) + point * 4000, &p1[0], &p1[1], &p1[2], NULL, ctx, fb);
                        if (gEditor->heightSel == point) {
                            switch (gEditor->heightState) {
                            case EDIT_HEIGHT_AT_LIMIT:
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case EDIT_HEIGHT_NEAR_LIMIT:
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case EDIT_HEIGHT_FREE:
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (point > 0) {
                            edit_project_to_screen(CUR_X0(0) + 2000, point != 1 ? gUserPieceHeights[gEditor->userPiece][point - 2] * 16 : 0,
                                          CUR_Z0(1) + (point - 1) * 4000, &p0[0], &p0[1], &p0[2], NULL, ctx, fb);
                            edit_project_to_screen(CUR_X0(0) + 2000, gUserPieceHeights[gEditor->userPiece][point - 1] * 16,
                                          CUR_Z0(1) + point * 4000, &p1[0], &p1[1], &p1[2], NULL, ctx, fb);
                            edit_project_to_screen(CUR_X0(0) + 14000, point != 1 ? gUserPieceHeights[gEditor->userPiece][point - 2] * 16 : 0,
                                          CUR_Z0(1) + (point - 1) * 4000, &p2[0], &p2[1], &p2[2], NULL, ctx, fb);
                            edit_project_to_screen(CUR_X0(0) + 14000, gUserPieceHeights[gEditor->userPiece][point - 1] * 16,
                                          CUR_Z0(1) + point * 4000, &p3[0], &p3[1], &p3[2], NULL, ctx, fb);
                            if (gEditor->heightSel == point || gEditor->heightSel == point - 1) {
                                switch (gEditor->heightState) {
                                case EDIT_HEIGHT_AT_LIMIT:
                                    edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case EDIT_HEIGHT_NEAR_LIMIT:
                                    edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case EDIT_HEIGHT_FREE:
                                    edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                case 1:
                    for (point = 0; point < gEditor->cur->type->dim[1] * 2 + 1; point++) {
                        edit_project_to_screen(CUR_X0(1) + point * 4000, point != 0 ? gUserPieceHeights[gEditor->userPiece][point - 1] * 16 : 0,
                                      CUR_Z0(0) + 2000, &p0[0], &p0[1], &p0[2], NULL, ctx, fb);
                        edit_project_to_screen(CUR_X0(1) + point * 4000, point != 0 ? gUserPieceHeights[gEditor->userPiece][point - 1] * 16 : 0,
                                      CUR_Z0(0) + 14000, &p1[0], &p1[1], &p1[2], NULL, ctx, fb);
                        if (gEditor->heightSel == point) {
                            switch (gEditor->heightState) {
                            case EDIT_HEIGHT_AT_LIMIT:
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case EDIT_HEIGHT_NEAR_LIMIT:
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case EDIT_HEIGHT_FREE:
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (point > 0) {
                            edit_project_to_screen(CUR_X0(1) + (point - 1) * 4000, point != 1 ? gUserPieceHeights[gEditor->userPiece][point - 2] * 16 : 0,
                                          CUR_Z0(0) + 2000, &p0[0], &p0[1], &p0[2], NULL, ctx, fb);
                            edit_project_to_screen(CUR_X0(1) + point * 4000, gUserPieceHeights[gEditor->userPiece][point - 1] * 16,
                                          CUR_Z0(0) + 2000, &p1[0], &p1[1], &p1[2], NULL, ctx, fb);
                            edit_project_to_screen(CUR_X0(1) + (point - 1) * 4000, point != 1 ? gUserPieceHeights[gEditor->userPiece][point - 2] * 16 : 0,
                                          CUR_Z0(0) + 14000, &p2[0], &p2[1], &p2[2], NULL, ctx, fb);
                            edit_project_to_screen(CUR_X0(1) + point * 4000, gUserPieceHeights[gEditor->userPiece][point - 1] * 16,
                                          CUR_Z0(0) + 14000, &p3[0], &p3[1], &p3[2], NULL, ctx, fb);
                            if (gEditor->heightSel == point || gEditor->heightSel == point - 1) {
                                switch (gEditor->heightState) {
                                case EDIT_HEIGHT_AT_LIMIT:
                                    edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case EDIT_HEIGHT_NEAR_LIMIT:
                                    edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case EDIT_HEIGHT_FREE:
                                    edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                case 2:
                    for (point = 0; point < gEditor->cur->type->dim[1] * 2 + 1; point++) {
                        revPoint = gEditor->cur->type->dim[1] * 2 - point;
                        edit_project_to_screen(CUR_X0(0) + 2000, revPoint != 0 ? gUserPieceHeights[gEditor->userPiece][revPoint - 1] * 16 : 0,
                                      CUR_Z0(1) + point * 4000, &p0[0], &p0[1], &p0[2], NULL, ctx, fb);
                        edit_project_to_screen(CUR_X0(0) + 14000, revPoint != 0 ? gUserPieceHeights[gEditor->userPiece][revPoint - 1] * 16 : 0,
                                      CUR_Z0(1) + point * 4000, &p1[0], &p1[1], &p1[2], NULL, ctx, fb);
                        if (gEditor->heightSel == revPoint) {
                            switch (gEditor->heightState) {
                            case EDIT_HEIGHT_AT_LIMIT:
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case EDIT_HEIGHT_NEAR_LIMIT:
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case EDIT_HEIGHT_FREE:
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (revPoint < gEditor->cur->type->dim[1] * 2) {
                            edit_project_to_screen(CUR_X0(0) + 2000, revPoint != 0 ? gUserPieceHeights[gEditor->userPiece][revPoint - 1] * 16 : 0,
                                          CUR_Z0(1) + point * 4000, &p0[0], &p0[1], &p0[2], NULL, ctx, fb);
                            edit_project_to_screen(CUR_X0(0) + 2000, gUserPieceHeights[gEditor->userPiece][revPoint] * 16,
                                          CUR_Z0(1) + (point - 1) * 4000, &p1[0], &p1[1], &p1[2], NULL, ctx, fb);
                            edit_project_to_screen(CUR_X0(0) + 14000, revPoint != 0 ? gUserPieceHeights[gEditor->userPiece][revPoint - 1] * 16 : 0,
                                          CUR_Z0(1) + point * 4000, &p2[0], &p2[1], &p2[2], NULL, ctx, fb);
                            edit_project_to_screen(CUR_X0(0) + 14000, gUserPieceHeights[gEditor->userPiece][revPoint] * 16,
                                          CUR_Z0(1) + (point - 1) * 4000, &p3[0], &p3[1], &p3[2], NULL, ctx, fb);
                            if (gEditor->heightSel == revPoint || gEditor->heightSel == revPoint + 1) {
                                switch (gEditor->heightState) {
                                case EDIT_HEIGHT_AT_LIMIT:
                                    edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case EDIT_HEIGHT_NEAR_LIMIT:
                                    edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case EDIT_HEIGHT_FREE:
                                    edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                case 3:
                    for (point = 0; point < gEditor->cur->type->dim[1] * 2 + 1; point++) {
                        revPoint = gEditor->cur->type->dim[1] * 2 - point;
                        edit_project_to_screen(CUR_X0(1) + point * 4000, revPoint != 0 ? gUserPieceHeights[gEditor->userPiece][revPoint - 1] * 16 : 0,
                                      CUR_Z0(0) + 2000, &p0[0], &p0[1], &p0[2], NULL, ctx, fb);
                        edit_project_to_screen(CUR_X0(1) + point * 4000, revPoint != 0 ? gUserPieceHeights[gEditor->userPiece][revPoint - 1] * 16 : 0,
                                      CUR_Z0(0) + 14000, &p1[0], &p1[1], &p1[2], NULL, ctx, fb);
                        if (gEditor->heightSel == revPoint) {
                            switch (gEditor->heightState) {
                            case EDIT_HEIGHT_AT_LIMIT:
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case EDIT_HEIGHT_NEAR_LIMIT:
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case EDIT_HEIGHT_FREE:
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (revPoint < gEditor->cur->type->dim[1] * 2) {
                            edit_project_to_screen(CUR_X0(1) + point * 4000, revPoint != 0 ? gUserPieceHeights[gEditor->userPiece][revPoint - 1] * 16 : 0,
                                          CUR_Z0(0) + 2000, &p0[0], &p0[1], &p0[2], NULL, ctx, fb);
                            edit_project_to_screen(CUR_X0(1) + (point - 1) * 4000, gUserPieceHeights[gEditor->userPiece][revPoint] * 16,
                                          CUR_Z0(0) + 2000, &p1[0], &p1[1], &p1[2], NULL, ctx, fb);
                            edit_project_to_screen(CUR_X0(1) + point * 4000, revPoint != 0 ? gUserPieceHeights[gEditor->userPiece][revPoint - 1] * 16 : 0,
                                          CUR_Z0(0) + 14000, &p2[0], &p2[1], &p2[2], NULL, ctx, fb);
                            edit_project_to_screen(CUR_X0(1) + (point - 1) * 4000, gUserPieceHeights[gEditor->userPiece][revPoint] * 16,
                                          CUR_Z0(0) + 14000, &p3[0], &p3[1], &p3[2], NULL, ctx, fb);
                            if (gEditor->heightSel == revPoint || gEditor->heightSel == revPoint + 1) {
                                switch (gEditor->heightState) {
                                case EDIT_HEIGHT_AT_LIMIT:
                                    edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case EDIT_HEIGHT_NEAR_LIMIT:
                                    edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case EDIT_HEIGHT_FREE:
                                    edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                edit_add_overlay_line_2(fb, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                edit_add_overlay_line_2(fb, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                }
            }
            gdl = edit_draw_overlay_lines(ctx, fb, gdl);
        }
    } else {
        gdl = arned_draw(fb, gdl, 0, 0, 0, 0, 0, 0, -1, 1);
    }

    if (gEditor->flags & EDIT_MESSAGE) {
        switch (gEditor->message) {
        case EDIT_MSG_NO_TRACK:
            gdl = gui_draw_message(gdl, ctx, fb, "No Track Found On Game Pak");
            break;
        case EDIT_MSG_SAVE_CANCELED:
            gdl = gui_draw_message(gdl, ctx, fb, "Save Canceled");
            break;
        case EDIT_MSG_TRACK_COMPLETE:
            gdl = gui_draw_message(gdl, ctx, fb, "Track Complete!");
            break;
        case EDIT_MSG_OUT_OF_ROOM:
            gdl = gui_draw_message(gdl, ctx, fb, "You Ran Out Of Room!");
            break;
        case EDIT_MSG_NAME_FIRST:
            gdl = gui_draw_message(gdl, ctx, fb, "Name Your Track Before Saving!");
            break;
        }
    } else if (gEditor->flags & EDIT_DIALOG) {
        switch (gEditor->dialog) {
        case EDIT_ASK_ADD_SEGMENT:
            gdl = gui_draw_prompt(gdl, ctx, fb, "Add Segment?", "OK", "Cancel", 1, 1);
            break;
        case EDIT_ASK_DELETE_SEGMENT:
            gdl = gui_draw_prompt(gdl, ctx, fb, "Delete Will Reset The Piece", "OK", "Cancel", 1, 1);
            break;
        case EDIT_ASK_EXIT:
        case EDIT_ASK_LOAD_DISCARD:
            gdl = gui_draw_prompt(gdl, ctx, fb, "Lose Changes?", "OK", "Cancel", 1, 1);
            break;
        case EDIT_ASK_CLEAR:
            gdl = gui_draw_prompt(gdl, ctx, fb, "Really Clear Track?", "OK", "Cancel", 1, 1);
            break;
        case EDIT_ASK_SAVE:
            /* Game Pak greyed out */
            gdl = gui_draw_prompt(gdl, ctx, fb, "Save The Track To...", "Game Pak", "Controller Pak", 0, 1);
            break;
        case EDIT_ASK_LOAD:
            /* Game Pak enabled when a track is saved there (SAVE_HAS_USER_TRACK) */
            gdl = gui_draw_prompt(gdl, ctx, fb, "Load A Track From...", "Game Pak", "Controller Pak",
                                (gSaveData.options >> 3) & 1, 1);
            break;
        case EDIT_ASK_OVERWRITE:
            gdl = gui_draw_prompt(gdl, ctx, fb, "Overwrite Track On Game Pak?", "OK", "Cancel", 1, 1);
            break;
        }
    }
    return gdl;
}

/* Takes a free piece node of the given type and links it after prev and before next (either may be NULL), placed
   against them. */
EditPiece *edit_insert_piece(EditPiece *prev, EditPiece *next, PieceType *type)
{
    EditPiece *piece;
    s32 i;
    s32 rot;
    Editor *ed;

    piece = gEditor->pieces;
    for (i = 0; i < EDIT_MAX_PIECES; i++, piece++) {
        if (piece->used == 0) break;
    }
    ed = gEditor;
    piece->used = 1;
    /* flipped as the last piece was, if the type fits that way */
    if (ed->flags & EDIT_FLIP) {
        if (type->flags & PIECE_FITS_FLIPPED) {
            piece->flags = EDIT_PIECE_FLIPPED;
        } else {
            piece->flags = 0;
            ed->flags &= ~EDIT_FLIP;
        }
    } else if (type->flags & PIECE_FITS) {
        piece->flags = 0;
    } else {
        piece->flags = EDIT_PIECE_FLIPPED;
        ed->flags |= EDIT_FLIP;
    }
    piece->next = next;
    piece->prev = prev;
    piece->type = type;
    if (next == NULL && prev == NULL) {
        /* the first piece: in the middle of the grid, moved by the scroll, which is held to the grid */
        gEditor->first = piece;
        gEditor->last = piece;
        rot = type->rot;
        if (gEditor->flags & EDIT_FLIP) rot += 2;
        piece->rot = rot;
        piece->x = gEditor->gridSize[0] / 2 - type->dim[rot & 1] / 2;
        piece->y = gEditor->gridSize[1] / 2 - type->dim[(piece->rot + 1) & 1] / 2;
        piece->x += gEditor->scrollX;
        piece->y += gEditor->scrollY;
        if (piece->x < 0) {
            piece->x = 0;
            gEditor->scrollX = type->dim[piece->rot & 1] / 2 - gEditor->gridSize[0] / 2;
        }
        if (piece->x + piece->type->dim[piece->rot & 1] > gEditor->gridSize[0]) {
            piece->x = gEditor->gridSize[0] - piece->type->dim[piece->rot & 1];
            gEditor->scrollX = piece->x - (gEditor->gridSize[0] / 2 - type->dim[piece->rot & 1] / 2);
        }
        if (piece->y < 0) {
            piece->y = 0;
            gEditor->scrollY = type->dim[~piece->rot & 1] / 2 - gEditor->gridSize[1] / 2;
        }
        if (piece->y + piece->type->dim[~piece->rot & 1] > gEditor->gridSize[1]) {
            piece->y = gEditor->gridSize[1] - piece->type->dim[~piece->rot & 1];
            gEditor->scrollY = piece->y - (gEditor->gridSize[1] / 2 - type->dim[~piece->rot & 1] / 2);
        }
    } else {
        if (prev != NULL) {
            gEditor->last = piece;
            prev->next = piece;
            edit_place_piece_after(prev, piece);
        }
        if (next != NULL) {
            gEditor->first = piece;
            next->prev = piece;
            edit_place_piece_before(next, piece);
        }
    }
    return piece;
}

/* Unlinks a piece (opening a closed track), frees its node and clears its cells in the 3D view. */
void edit_delete_piece(EditPiece *piece)
{
    s16 *size;
    s32 col;
    s32 row;
    s32 x;
    s32 y;

    if (piece->next != NULL) {
        if (piece->prev != NULL) {
            /* from the middle of a closed track: it opens there */
            piece->prev->next = NULL;
            piece->next->prev = NULL;
            if ((piece->flags & EDIT_PIECE_PREVIEW) == 0) {
                gEditor->first = piece->next;
                gEditor->last = piece->prev;
            }
            goto free_node;
        }
        goto unlink_next;
    }
    if (piece->prev != NULL) {
        piece->prev->next = NULL;
        goto update_ends;
    }
unlink_next:
    if (piece->next != NULL) {
        piece->next->prev = piece->prev;
    }
update_ends:
    if ((piece->flags & EDIT_PIECE_PREVIEW) == 0) {
        if (gEditor->first == piece) {
            gEditor->first = piece->next;
        }
        if (gEditor->last == piece) {
            gEditor->last = piece->prev;
        }
    }
free_node:
    {
        Editor *ed = gEditor;
        piece->used = 0;
        if (ed->start == piece) {
            ed->start = NULL;
        }
    }
    for (row = 0; row < piece->type->dim[(piece->rot + 1) & 1]; row++) {
        for (col = 0; col < piece->type->dim[piece->rot & 1]; col++) {
            x = piece->x + col;
            if (x >= 0) {
                size = gEditor->gridSize;
                if (x < size[0]) {
                    y = piece->y + row;
                    if (y >= 0 && y < size[1]) {
                        arned_set_cell(x, y, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
        }
    }
}

/* Where a piece's connector `end` (0 entry, 1 exit) is along an x side of the piece (0 when it is on a y side).
   fields[end], fields[end + 2] and fields[end + 4] are the type's dim[end], dir[end] and off[end]. */
s32 edit_get_port_x(EditPiece *piece, s32 end)
{
    s16 *fields = piece->type->dim;
    s16 *port = &fields[end];
    s32 side = port[2];

    switch ((side + piece->rot) & 3) {
    case SIDE_POS_X:
    case SIDE_NEG_X:
        return 0;
    case SIDE_POS_Y:
        switch (side) {
        case SIDE_POS_Y:
        case SIDE_NEG_X:
            return port[4];
        case SIDE_NEG_Y:
            return fields[0] - port[4] - 2;
        case SIDE_POS_X:
            return fields[1] - port[4] - 2;
        }
    case SIDE_NEG_Y: {
        s16 *fields2 = piece->type->dim;
        s16 *port2 = &fields2[end];

        switch (port2[2]) {
        case SIDE_POS_Y:
            return fields2[0] - port2[4] - 2;
        case SIDE_POS_X:
            return port2[4];
        case SIDE_NEG_Y:
            return port2[4];
        case SIDE_NEG_X:
            return fields2[1] - port2[4] - 2;
        }
    }
    }
}

/* ... along a y side. */
s32 edit_get_port_y(EditPiece *piece, s32 end)
{
    s16 *fields = piece->type->dim;
    s16 *port = fields + end;
    s32 side = port[2];

    switch ((side + piece->rot) & 3) {
    case SIDE_POS_Y:
        return 0;
    case SIDE_NEG_Y:
        return 0;
    case SIDE_NEG_X:
        switch (side) {
        case SIDE_NEG_Y:
            return fields[0] - port[4] - 2;
        case SIDE_POS_Y:
        case SIDE_NEG_X:
            return port[4];
        case SIDE_POS_X:
            return fields[1] - port[4] - 2;
        }
    case SIDE_POS_X: {
        s16 *fields2 = piece->type->dim;
        s16 *port2 = fields2 + end;

        switch (port2[2]) {
        case SIDE_NEG_Y:
            return port2[4];
        case SIDE_POS_Y:
            return fields2[0] - port2[4] - 2;
        case SIDE_NEG_X:
            return fields2[1] - port2[4] - 2;
        case SIDE_POS_X:
            return port2[4];
        }
    }
    }
}

#define SX16(v) ((s32)((u32)(v) << 16))
#define HALF(v, s) ((s32)((SX16(v) >> 16) + (s32)((u32)SX16(v) >> 31)) >> (s))

/* Puts piece after prev: its entry connector against prev's exit (with no prev, in the middle of the grid). Some
   reads are unsigned or volatile as the original has them. */
void edit_place_piece_after(EditPiece *prev, EditPiece *piece)
{
    EditPiece *prevPiece = prev;
    s32 exitEnd = 1;
    s32 entryEnd = 0;
    s32 rot, rotMasked, side, ofs;

    if (prevPiece == 0) {
        u16 *grid = (u16 *)gEditor->gridSize;
        piece->x = HALF(grid[0], exitEnd) - HALF((u16)piece->type->dim[0], exitEnd);
        grid = (u16 *)gEditor->gridSize;
        piece->y = HALF(grid[1], exitEnd) - HALF((u16)piece->type->dim[1], exitEnd);
        return;
    }
    if (prevPiece->flags & EDIT_PIECE_FLIPPED) exitEnd = 0;
    if (piece->flags & EDIT_PIECE_FLIPPED) entryEnd = 1;
    piece->x = prevPiece->x;
    *(volatile s16 *)&piece->y = prevPiece->y;
    rot = (u16)prevPiece->rot + ((u16)prevPiece->type->dir[exitEnd] - (u16)(*(PieceType *volatile *)&piece->type)->dir[entryEnd]) + 2;
    rotMasked = rot & 3;
    piece->rot = rotMasked;
    side = (prevPiece->type->dir[exitEnd] + prevPiece->rot) & 3;
    switch (side) {
    case SIDE_POS_Y:
        piece->y += prevPiece->type->dim[(prevPiece->rot + 1) & 1];
        ofs = edit_get_port_x(prevPiece, exitEnd);
        piece->x += ofs;
        ofs = edit_get_port_x(piece, entryEnd);
        piece->x -= ofs;
        break;
    case SIDE_NEG_Y:
        piece->y -= piece->type->dim[(rotMasked + 1) & 1];
        ofs = edit_get_port_x(prevPiece, exitEnd);
        piece->x += ofs;
        ofs = edit_get_port_x(piece, entryEnd);
        piece->x -= ofs;
        break;
    case SIDE_NEG_X:
        piece->x -= piece->type->dim[rot & 1];
        ofs = edit_get_port_y(prevPiece, exitEnd);
        piece->y += ofs;
        ofs = edit_get_port_y(piece, entryEnd);
        piece->y -= ofs;
        break;
    case SIDE_POS_X:
        piece->x += prevPiece->type->dim[prevPiece->rot & 1];
        ofs = edit_get_port_y(prevPiece, exitEnd);
        piece->y += ofs;
        ofs = edit_get_port_y(piece, entryEnd);
        piece->y -= ofs;
        break;
    }
}

/* Puts piece before next: its exit connector against next's entry. */
void edit_place_piece_before(EditPiece *next, EditPiece *piece)
{
    s32 entryEnd, exitEnd;
    u16 rot;
    s32 flipA; u32 flipB;

    flipA = next->flags & EDIT_PIECE_FLIPPED; entryEnd = flipA != 0;
    piece->x = next->x;
    piece->y = next->y;
    flipB = piece->flags & EDIT_PIECE_FLIPPED; exitEnd = flipB < 1;
    rot = next->rot + (next->type->dir[entryEnd] - piece->type->dir[exitEnd]) + 2;
    piece->rot = rot & 3;
    switch ((next->type->dir[entryEnd] + next->rot) & 3) {
    case SIDE_POS_Y:
        piece->y += next->type->dim[(next->rot + 1) & 1];
        piece->x += edit_get_port_x(next, entryEnd);
        piece->x -= edit_get_port_x(piece, exitEnd);
        break;
    case SIDE_NEG_Y:
        piece->y -= piece->type->dim[(piece->rot + 1) & 1];
        piece->x += edit_get_port_x(next, entryEnd);
        piece->x -= edit_get_port_x(piece, exitEnd);
        break;
    case SIDE_NEG_X:
        piece->x -= piece->type->dim[piece->rot & 1];
        piece->y += edit_get_port_y(next, entryEnd);
        piece->y -= edit_get_port_y(piece, exitEnd);
        break;
    case SIDE_POS_X:
        piece->x += next->type->dim[next->rot & 1];
        piece->y += edit_get_port_y(next, entryEnd);
        piece->y -= edit_get_port_y(piece, exitEnd);
        break;
    }
}

/* With no track yet: only the types that can start a track fit; the add items offer those. */
void edit_set_start_piece_choices(void)
{
    s32 i;
    s32 k;
    s32 base;
    s32 flags;
    PieceType *type;
    s32 withFit;
    s32 newFlags;

    for (k = 0; k < PIECE_TYPE_COUNT; k++) {
        type = &gPieceTypes[k];
        flags = (u16)type->flags; /* read unsigned here */
        if (flags & PIECE_START) {
            withFit = flags | PIECE_FITS;
            type->flags = withFit;
            if (withFit & PIECE_FLIPPABLE) {
                newFlags = withFit | PIECE_FITS_FLIPPED;
            } else {
                newFlags = withFit & ~PIECE_FITS_FLIPPED;
            }
        } else {
            newFlags = flags & ~PIECE_FITS;
            newFlags &= ~PIECE_FITS_FLIPPED;
        }
        type->flags = newFlags;
    }
    for (i = 0; i < 4; i++) {
        switch (i) {
        case EDIT_ITEM_STRAIGHT:
            base = PIECE_TYPE_STRAIGHT;
            break;
        case EDIT_ITEM_TURN:
            base = PIECE_TYPE_TURN;
            break;
        case EDIT_ITEM_JUMP:
            base = PIECE_TYPE_JUMP;
            break;
        case EDIT_ITEM_USER:
            base = PIECE_TYPE_USER;
            break;
        }
        sEditTrackMenu.items[i].valueMask = 0;
        for (k = 0; k < sEditTrackMenu.items[i].numValues; k++) {
            if (gPieceTypes[base + k].flags & (PIECE_FITS | PIECE_FITS_FLIPPED)) {
                sEditTrackMenu.items[i].valueMask |= 1 << k;
            }
        }
        if (sEditTrackMenu.items[i].valueMask == 0) {
            sEditTrackMenu.items[i].flags &= ~GUI_ITEM_ENABLED;
        } else {
            sEditTrackMenu.items[i].flags |= GUI_ITEM_ENABLED;
        }
    }
    gui_menu_fix_cursor(&sEditTrackMenu);
}

/* The 30 x 30 cells round an open end, turned so that the end faces +y (g[x][y]): 1 taken or off the grid */
typedef struct FitGrid {
    u8 g[30][30];
} FitGrid;

typedef struct FitRect {
    s32 x0, y0, x1, y1;
} FitRect;

extern FitGrid sEditFitGrid;

/* Works out which piece types fit at the track's open end next to piece `end` (PIECE_FITS, PIECE_FITS_FLIPPED) and
   sets the add items' choices; returns whether anything fits. */
s32 edit_update_fitting_pieces(EditPiece *end)
{
    FitGrid grid;
    FitRect area;
    s16 openEnd;
    s16 otherEnd;
    s32 i, j;
    s32 typeIdx, len, lo, hi, bestLen;
    s16 ofs;
    s32 anyFit;
    EditPiece *piece;
    PieceType *type;

    /* the open end: after the piece, or before it */
    openEnd = 1;
    if (end->next == NULL || (end->next->flags & EDIT_PIECE_PREVIEW)) {
        otherEnd = 0;
    } else {
        openEnd = 0;
        otherEnd = 1;
    }
    switch (end->dir[openEnd]) {
    case SIDE_POS_Y:
        {
            s32 t, u;
            t = end->x + end->off[openEnd];
            area.x0 = t - 14;
            u = end->y + end->h;
            area.x1 = t + 16;
            area.y0 = u - 15;
            area.y1 = u + 15;
        }
        break;
    case SIDE_POS_X:
        {
            s32 t, u;
            t = end->x + end->w;
            area.x0 = t - 15;
            u = end->y + end->off[openEnd];
            area.x1 = t + 15;
            area.y0 = u - 14;
            area.y1 = u + 16;
        }
        break;
    case SIDE_NEG_Y:
        {
            s32 t, u;
            t = end->x + end->off[openEnd];
            area.x0 = t - 14;
            u = end->y;
            area.x1 = t + 16;
            area.y0 = u - 15;
            area.y1 = u + 15;
        }
        break;
    case SIDE_NEG_X:
        {
            s32 t, u;
            t = end->x;
            area.x0 = t - 15;
            u = end->y + end->off[openEnd];
            area.x1 = t + 15;
            area.y0 = u - 14;
            area.y1 = u + 16;
        }
        break;
    }
    mem_fill((u8 *)&grid, 0, sizeof(FitGrid));
    for (j = 0; j < area.y1 - area.y0; j++) {
        for (i = 0; i < area.x1 - area.x0; i++) {
            if (i + area.x0 < 0 || i + area.x0 >= gEditor->gridSize[0]
                || j + area.y0 < 0 || j + area.y0 >= gEditor->gridSize[1]) {
                switch (end->dir[openEnd]) {
                case SIDE_POS_Y:
                    grid.g[i][j] = 1;
                    break;
                case SIDE_NEG_Y:
                    grid.g[29 - i][29 - j] = 1;
                    break;
                case SIDE_NEG_X:
                    grid.g[j][29 - i] = 1;
                    break;
                case SIDE_POS_X:
                    grid.g[29 - j][i] = 1;
                    break;
                }
            }
        }
    }
    piece = gEditor->first;
    if (piece != NULL) {
        do {
            if (!(piece->flags & EDIT_PIECE_PREVIEW) && piece->x < area.x1 && area.x0 < piece->x + piece->w
                && piece->y < area.y1 && area.y0 < piece->y + piece->h) {
                switch (end->dir[openEnd]) {
                case SIDE_POS_Y:
                    for (j = piece->y - area.y0; j < piece->y - area.y0 + piece->h; j++) {
                        for (i = piece->x - area.x0; i < piece->x - area.x0 + piece->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[i][j] = 1;
                            }
                        }
                    }
                    break;
                case SIDE_NEG_Y:
                    for (j = piece->y - area.y0; j < piece->y - area.y0 + piece->h; j++) {
                        for (i = piece->x - area.x0; i < piece->x - area.x0 + piece->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[29 - i][29 - j] = 1;
                            }
                        }
                    }
                    break;
                case SIDE_NEG_X:
                    for (j = piece->y - area.y0; j < piece->y - area.y0 + piece->h; j++) {
                        for (i = piece->x - area.x0; i < piece->x - area.x0 + piece->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[j][29 - i] = 1;
                            }
                        }
                    }
                    break;
                case SIDE_POS_X:
                    for (j = piece->y - area.y0; j < piece->y - area.y0 + piece->h; j++) {
                        for (i = piece->x - area.x0; i < piece->x - area.x0 + piece->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[29 - j][i] = 1;
                            }
                        }
                    }
                    break;
                }
            }
            piece = piece->next;
        } while (piece != NULL && piece != gEditor->first);
    }
    sEditFitGrid = grid;

    /* each type in the menus fits if its cells are free (flipped too, if it can flip); a New User Piece takes the
       longest length that fits */
    for (typeIdx = 0; typeIdx < PIECE_TYPE_COUNT; typeIdx++) {
        type = &gPieceTypes[typeIdx];
        if (!(type->flags & PIECE_IN_MENU)) {
            continue;
        }
        bestLen = 0;
        if (type->flags & PIECE_VAR_LENGTH) {
            lo = 1;
            hi = USER_PIECE_MAX_SEGMENTS;
        } else {
            hi = type->dim[(type->dir[openEnd] + 1) & 1];
            lo = hi;
        }
        for (len = lo; len <= hi; len++) {
            if ((u32)(typeIdx - PIECE_TYPE_USER) < USER_PIECE_COUNT && typeIdx - PIECE_TYPE_USER >= gEditor->userPieceCount) {
                type->flags &= ~(PIECE_FITS | PIECE_FITS_FLIPPED);
            } else {
                switch (type->dir[otherEnd]) {
                case SIDE_POS_Y:
                    ofs = type->dim[0] - type->off[otherEnd] - 2;
                    break;
                case SIDE_NEG_X:
                    ofs = type->dim[1] - type->off[otherEnd] - 2;
                    break;
                case SIDE_POS_X:
                    ofs = type->off[otherEnd];
                    break;
                case SIDE_NEG_Y:
                    ofs = type->off[otherEnd];
                    break;
                }
                type->flags |= PIECE_FITS;
                for (j = 0; j < len; j++) {
                    for (i = 0; i < type->dim[type->dir[openEnd] & 1]; i++) {
                        if (grid.g[i + (14 - ofs)][j + 15]) {
                            type->flags &= ~PIECE_FITS;
                            break;
                        }
                    }
                    if (!(type->flags & PIECE_FITS)) {
                        break;
                    }
                }
                if (type->flags & PIECE_FLIPPABLE) {
                    switch (type->dir[openEnd]) {
                    case SIDE_POS_Y:
                        ofs = type->dim[0] - type->off[openEnd] - 2;
                        break;
                    case SIDE_NEG_X:
                        ofs = type->dim[1] - type->off[openEnd] - 2;
                        break;
                    case SIDE_POS_X:
                        ofs = type->off[openEnd];
                        break;
                    case SIDE_NEG_Y:
                        ofs = type->off[openEnd];
                        break;
                    }
                    type->flags |= PIECE_FITS_FLIPPED;
                    j = 0;
                    if (len > 0) {
                        do {
                            for (i = 0; i < type->dim[type->dir[openEnd] & 1]; i++) {
                                if (grid.g[i + (14 - ofs)][j + 15]) {
                                    type->flags &= ~PIECE_FITS_FLIPPED;
                                    break;
                                }
                            }
                            j++;
                        } while ((type->flags & PIECE_FITS_FLIPPED) && j < len);
                    }
                } else {
                    type->flags &= ~PIECE_FITS_FLIPPED;
                }
            }
            if (type->flags & PIECE_FITS) {
                bestLen = len;
            } else if (type->flags & PIECE_VAR_LENGTH) {
                if (len != lo) {
                    type->flags |= PIECE_FITS;
                }
                break;
            }
        }
        if (type->flags & PIECE_VAR_LENGTH) {
            if (gEditor->mode != EDIT_MODE_USER_PIECE) {
                type->dim[1] = (bestLen < type->dim[1]) ? bestLen : type->dim[1];
            }
            gEditor->maxSegments = bestLen;
        }
    }

    /* the add items offer the types that fit */
    anyFit = 0;
    for (i = 0; i < 6; i++) {
        switch (i) {
        case EDIT_ITEM_STRAIGHT:
            j = PIECE_TYPE_STRAIGHT;
            break;
        case EDIT_ITEM_TURN:
            j = PIECE_TYPE_TURN;
            break;
        case EDIT_ITEM_JUMP:
            j = PIECE_TYPE_JUMP;
            break;
        case EDIT_ITEM_USER:
            j = PIECE_TYPE_USER;
            break;
        default:
            continue;
        }
        sEditTrackMenu.items[i].valueMask = 0;
        for (typeIdx = 0; typeIdx < sEditTrackMenu.items[i].numValues; typeIdx++) {
            if (gPieceTypes[j + typeIdx].flags & (PIECE_FITS | PIECE_FITS_FLIPPED)) {
                sEditTrackMenu.items[i].valueMask |= 1 << typeIdx;
            }
        }
        if (sEditTrackMenu.items[i].valueMask == 0) {
            sEditTrackMenu.items[i].flags &= ~GUI_ITEM_ENABLED;
        } else {
            anyFit = 1;
            sEditTrackMenu.items[i].flags |= GUI_ITEM_ENABLED;
        }
    }
    gui_menu_fix_cursor(&sEditTrackMenu);
    return anyFit;
}

void edit_init_user_pieces(void)
{
    s32 i, j;
    PieceMeshSet *tmplA = gPieceTypes[PIECE_TYPE_TEMPLATE].meshA;
    PieceMeshSet *tmplB = gPieceTypes[PIECE_TYPE_TEMPLATE].meshB;

    gEditor->heightSel = 1;
    gEditor->userPiece = 0;
    gEditor->maxSegments = 8;
    for (i = 0; i < 8; i++) {
        gPieceTypes[i + 0x12].dim[1] = 4;
        gPieceTypes[i + 0x12].meshA = mem_alloc_dbg((tmplA->count << 8) | 4, 0, sStrEditFile, 0x1D73);
        gPieceTypes[i + 0x12].meshB = mem_alloc_dbg((tmplB->count << 8) | 4, 0, sStrEditFile, 0x1D74);
        usrtrk_clone_piece_meshes(gPieceTypes[i + 0x12].meshA, gPieceTypes[i + 0x12].meshB, 8);
        for (j = 0; j < 15; j++) {
            gUserPieceHeights[i][j] = 0;
        }
        usrtrk_apply_heights(gPieceTypes[i + 0x12].meshA, gPieceTypes[i + 0x12].meshB, gUserPieceHeights[i], 8);
    }
    {
        Editor *ea;
        u8 *rb;
        PieceType *r;
        u8 *tb;
        i = 0;
        tb = (u8 *)gUserPieceHeights;
        if (tb) j = 3; else j = 4;
        ea = gEditor;
        rb = (u8 *)gPieceTypes;
        r = (PieceType *)(rb + ((ea->userPiece + 0x12) << 6));
        *(volatile u16 *)&r->dim[1];
        if (r) j = 1; else j = 2;
        for (; i < 15; i++) {
            *(u16 *)(tb + (ea->userPiece * 15 + i) * 2) = 0;
        }
    }
}

void edit_user_piece_remove_first_segment(void)
{
    PieceType *t;
    s32 x;
    s32 y;
    s32 i;
    s16 sv;
    EditPiece *sub;
    Editor *g;

    t = &gPieceTypes[gEditor->userPiece + 18];
    for (y = 0; y < t->dim[(gEditor->cur->rot + 1) & 1]; y++) {
        for (x = 0; x < t->dim[gEditor->cur->rot & 1]; x++) {
            arned_set_cell(gEditor->cur->x + x, gEditor->cur->y + y, 0, 0, 0, 0, 0, 0, 0);
        }
    }

    for (i = 0; i < gPieceTypes[gEditor->userPiece + 18].dim[1]; i++) {
        __builtin_memcpy((u8 *)gUserPieceHeights + gEditor->userPiece * 30 + i * 4, (u8 *)gUserPieceHeights + gEditor->userPiece * 30 + i * 4 + 4, 4);
    }
    g = gEditor;
    gPieceTypes[g->userPiece + 18].dim[1]--;
    {
        s32 k = gPieceTypes[g->userPiece + 18].dim[1] * 4 - 2;
        k += g->userPiece * 30;
        *(s16 *)((u8 *)gUserPieceHeights + k) = 0;
    }
    usrtrk_apply_heights(gPieceTypes[g->userPiece + 18].meshA, gPieceTypes[g->userPiece + 18].meshB,
                  (u8 *)gUserPieceHeights + g->userPiece * 30, gPieceTypes[g->userPiece + 18].dim[1]);

    sub = gEditor->cur;
    sv = sub->dir[sub->prev == 0];
    if (sv != 0) {
        if (sv == 1) {
            sub->x++;
        }
    } else {
        sub->y++;
    }
    if (gEditor->cam[1].dist > 45000) {
        gEditor->camZoomStep = -10000;
        gEditor->camZoomFrames = 50;
    }
}

void edit_user_piece_remove_last_segment(void)
{
    s32 x, y, i, j;
    PieceType *tb;
    EditPiece *pl;
    s16 s;

    tb = &gPieceTypes[PIECE_TYPE_USER + gEditor->userPiece];
    for (y = 0; y < tb->dim[(gEditor->cur->rot + 1) & 1]; y++) {
        for (x = 0; x < tb->dim[gEditor->cur->rot & 1]; x++) {
            arned_set_cell(gEditor->cur->x + x, gEditor->cur->y + y, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    gPieceTypes[gEditor->userPiece + 18].dim[1]--;
    for (i = 0; i < 2; i++) {
        j = i - 1;
        gUserPieceHeights[gEditor->userPiece][gPieceTypes[gEditor->userPiece + 18].dim[1] * 2 + j] = 0;
    }
    usrtrk_apply_heights(gPieceTypes[gEditor->userPiece + 18].meshA, gPieceTypes[gEditor->userPiece + 18].meshB,
                  gUserPieceHeights[gEditor->userPiece], gPieceTypes[gEditor->userPiece + 18].dim[1]);
    gEditor->heightSel -= 2;
    pl = gEditor->cur;
    s = pl->dir[pl->prev == 0];
    if (s != 0) {
        if (s == 1) {
            pl->x++;
        }
    } else {
        pl->y++;
    }
    if (gEditor->cam[1].dist > 45000) {
        gEditor->camZoomStep = -10000;
        gEditor->camZoomFrames = 50;
    }
}

typedef struct U4 {
    u32 w;
} __attribute__((packed)) U4;

void edit_user_piece_prepend_segment(void) {
    PieceType *t1;
    PieceType *t2;
    PieceType *b2;
    PieceType *bp;
    PieceType *bq;
    PieceType *tq;
    PieceType *tr;
    PieceType *ts;
    Editor *g;
    u8 *hb;
    Editor *gt;
    u8 *ub;
    u8 *sb;
    EditPiece *sub;
    s32 x;
    s32 y;
    s32 i;
    s32 off;
    s16 sv;

    t1 = &gPieceTypes[PIECE_TYPE_USER + gEditor->userPiece];
    for (y = 0; y < t1->dim[(gEditor->cur->rot + 1) & 1]; y++) {
        for (x = 0; x < t1->dim[gEditor->cur->rot & 1]; x++) {
            arned_set_cell(gEditor->cur->x + x, gEditor->cur->y + y, 0, 0, 0, 0, 0, 0, 0);
        }
    }

    bp = gPieceTypes;
    tq = (PieceType *)((u8 *)bp + ((gEditor->userPiece + 0x12) << 6));
    i = tq->dim[1] - 1;
    if (i >= 0) {
        do {
            off = gEditor->userPiece * 30 + i * 4;
            ub = D_800F55BC; /* gUserPieceHeights + 4 */
            sb = ub - 4;
            *(U4 *)(ub + off) = *(U4 *)(sb + off);
            i--;
        } while (i >= 0);
    }

    mem_fill((u8 *)gUserPieceHeights + gEditor->userPiece * 30, 0, 4);

    g = gEditor;
    bq = gPieceTypes;
    tr = (PieceType *)((u8 *)bq + ((g->userPiece + 0x12) << 6));
    tr->dim[1] = tr->dim[1] + 1;
    bq = gPieceTypes;
    ts = (PieceType *)((u8 *)bq + ((g->userPiece + 0x12) << 6));
    usrtrk_apply_heights(ts->meshA, ts->meshB, (u8 *)gUserPieceHeights + g->userPiece * 30, ts->dim[1]);

    gEditor->heightSel = gEditor->heightSel + 1;
    sub = gEditor->cur;
    sv = sub->dir[sub->prev == 0];
    if (sv != 0) {
        if (sv == 1) {
            sub->x = sub->x - 1;
        }
    } else {
        sub->y = sub->y - 1;
    }
    gEditor->camZoomStep = 10000;
    gEditor->camZoomFrames = 50;
}

void edit_user_piece_append_segment(void)
{
    s32 x, y, i;
    PieceType *tb;
    EditPiece *pl;
    s16 s;

    tb = &gPieceTypes[PIECE_TYPE_USER + gEditor->userPiece];
    for (y = 0; y < tb->dim[(gEditor->cur->rot + 1) & 1]; y++) {
        for (x = 0; x < tb->dim[gEditor->cur->rot & 1]; x++) {
            arned_set_cell(gEditor->cur->x + x, gEditor->cur->y + y, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    for (i = 0; i < 2; i++) {
        gUserPieceHeights[gEditor->userPiece][gPieceTypes[gEditor->userPiece + 18].dim[1] * 2 + i] = 0;
    }
    gPieceTypes[gEditor->userPiece + 18].dim[1]++;
    usrtrk_apply_heights(gPieceTypes[gEditor->userPiece + 18].meshA, gPieceTypes[gEditor->userPiece + 18].meshB,
                  gUserPieceHeights[gEditor->userPiece], gPieceTypes[gEditor->userPiece + 18].dim[1]);
    gEditor->heightSel += 1;
    pl = gEditor->cur;
    s = pl->dir[pl->prev == 0];
    if (s != 0) {
        if (s == 1) {
            pl->x--;
        }
    } else {
        pl->y--;
    }
    gEditor->camZoomStep = 10000;
    gEditor->camZoomFrames = 50;
}

#define CEA_IDX (gEditor->userPiece)
#define LIM ((s32)gPieceTypes[CEA_IDX + 0x12].dim[1] << 1)

int edit_user_piece_height_max_prev(int param_1, u16 *param_2)
{
    int t0;
    int q;
    int quo;
    int res;
    int n;
    u8 *tb;
    u8 *tb2;
    s16 sv;
    u16 uv;

    if (param_1 - 1 > 0 && param_1 - 1 < LIM) {
        s16 *gb = gUserPieceHeights;
        s32 k = param_1 - 2;
        t0 = gb[CEA_IDX * 15 + k] + 0xFA;
    } else {
        t0 = 0xFA;
    }
    tb = gCosTable;
    if (param_1 - 1 > 0) {
        q = (((u32)param_2[param_1 - 2] + 0x1555) >> 3) & 0x1ffe;
    } else {
        q = 0x2AA;
    }
    sv = *(s16 *)(tb + q);
    uv = *(u16 *)(tb + q);
    if (sv == 0) {
        uv = 1;
    }
    tb2 = gCosTable;
    if (param_1 - 1 > 0) {
        q = ((((param_2[param_1 - 2] + 0x1555) >> 4) - 0x400) & 0xfff) << 1;
    } else {
        q = 0x1AAA;
    }
    quo = (*(s16 *)(tb2 + q) * 0xFA) / (s16)uv;
    if (param_1 - 1 > 0 && param_1 - 1 < LIM) {
        s16 *gb = gUserPieceHeights;
        s32 k = param_1 - 2;
        quo += gb[CEA_IDX * 15 + k];
    }
    res = quo;
    if (t0 < res) {
        res = t0;
    }
    return res;
}

#define CE_ROW() (gEditor->userPiece)
#define CE_LIM() ((gPieceTypes[CE_ROW() + 0x12].dim[1]) << 1)
#define GETA(i) (((i) + 1 > 0 && (i) + 1 < CE_LIM()) ? gUserPieceHeights[CE_ROW()][i] : 0)
#define ANG(p, i) (((i) + 1 < CE_LIM()) ? 0x2000 - (p)[(i) + 1] : 0x2000)
#define SINS(x) gCosTable[((x) >> 4) & 0xFFF]
#define COSS(x) gCosTable[(((x) >> 4) - 0x400) & 0xFFF]

s32 edit_user_piece_height_max_next(s32 p1, s16 *p2)
{
    s32 t0;
    s32 q;
    s16 s;

    t0 = GETA(p1) + 500;
    s = SINS(ANG(p2, p1));
    if (s == 0) {
        s = 1;
    }
    q = COSS(ANG(p2, p1)) * 250 / s;
    q += GETA(p1);
    return (t0 < q) ? t0 : q;
}

#define OFS(n) (((n) - 1 > 0 && (n) - 1 < gPieceTypes[gEditor->userPiece + 0x12].dim[1] * 2) ? gUserPieceHeights[gEditor->userPiece][(n) - 2] : 0)
#define ANG_2(n, p) ((n) - 1 > 0 ? (p)[(n) - 2] : 0)

int edit_user_piece_height_min_prev(int n, u16 *p)
{
    s32 lo;
    s16 sv;
    s32 res;

    lo = OFS(n) - 500;
    sv = gCosTable[((ANG_2(n, p) - 0x2000) >> 4) & 0xFFF];
    if (sv == 0) {
        sv = 1;
    }
    res = gCosTable[(((ANG_2(n, p) - 0x2000) >> 4) - 0x400) & 0xFFF] * 250 / sv;
    res += OFS(n);
    return res < lo ? lo : res;
}

#define OFS_2(i) (((i) > 0 && (i) < gPieceTypes[gEditor->userPiece + 0x12].dim[1] * 2) ? gUserPieceHeights[gEditor->userPiece][(i) - 1] : 0)
#define ANG_3(i, p) ((i) < gPieceTypes[gEditor->userPiece + 0x12].dim[1] * 2 ? (p)[i] : 0)

int edit_user_piece_height_min_next(int n, s16 *p)
{
    s32 lo;
    s16 sv;
    s32 res;

    lo = OFS_2(n + 1) - 250;
    sv = gCosTable[((-0x2000 - ANG_3(n + 1, p)) >> 4) & 0xFFF];
    if (sv == 0) {
        sv = 1;
    }
    res = gCosTable[(((-0x2000 - ANG_3(n + 1, p)) >> 4) - 0x400) & 0xFFF] * 250 / sv;
    res += OFS_2(n + 1);
    return res < lo ? lo : res;
}

#define LIM_2 (gPieceTypes[gEditor->userPiece + 0x12].dim[1] * 2)
#define T1_2 ((s16 *)gUserPieceHeights)

static inline s16 *At(s16 *b, s32 c, s32 i)
{
    return &b[c * 15 + i];
}

s32 edit_user_piece_find_height_max(s32 n)
{
    u16 r1;
    s32 dummy;
    u16 r2;
    s32 a, cnt, v;

    if (n > 0 && n < LIM_2)
        a = *At(T1_2, gEditor->userPiece, n - 1);
    else
        a = 0;
    cnt = a;
    for (;;) {
        gUserPieceHeights[gEditor->userPiece][n - 1]++;
        if (n > 0 && n < LIM_2)
            v = gUserPieceHeights[gEditor->userPiece][n - 1];
        else
            v = 0;
        if (n - 1 > 0 && n - 1 < LIM_2)
            v -= gUserPieceHeights[gEditor->userPiece][n - 2];
        math_to_polar(4000, v << 4, &r1, &dummy);
        r1 = 0x4000 - r1;
        if (n + 1 > 0 && n + 1 < LIM_2)
            v = T1_2[gEditor->userPiece * 15 + n];
        else
            v = 0;
        if (n > 0 && n < LIM_2)
            v -= gUserPieceHeights[gEditor->userPiece][n - 1];
        math_to_polar(4000, v << 4, &r2, &dummy);
        r2 = 0x4000 - r2;
        if ((s16)(r2 - r1) < -0x2000)
            break;
        cnt++;
    }
    {
        s16 *b = T1_2;
        *At(b, gEditor->userPiece, n - 1) = a;
    }
    return cnt;
}

#define LV() (gEditor->userPiece)
#define CE2() (gPieceTypes[LV() + 0x12].dim[1] << 1)

s32 edit_user_piece_find_height_min(s32 param_1)
{
    s32 iVar2;
    s32 iVar3;
    s32 iVar4;
    s32 iVar5;
    s32 w38;
    s32 iStack_34;
    s32 w30;
    u32 uVar1;

    if (param_1 > 0 && param_1 < CE2()) {
        iVar5 = gUserPieceHeights[LV()][param_1 - 1];
    } else {
        iVar5 = 0;
    }
    iVar4 = iVar5;
    iVar3 = param_1 - 1;
    while (1) {
        gUserPieceHeights[LV()][iVar3] = (s16)((u16)gUserPieceHeights[LV()][iVar3] - 1);
        if (param_1 > 0 && param_1 < CE2()) {
            iVar2 = gUserPieceHeights[LV()][iVar3];
        } else {
            iVar2 = 0;
        }
        if (iVar3 > 0 && iVar3 < CE2()) {
            iVar2 = iVar2 - gUserPieceHeights[LV()][param_1 - 2];
        }
        math_to_polar(4000, iVar2 << 4, (u16 *)&w38, &iStack_34);
        ((u16 *)&w38)[0] = 0x4000 - ((u16 *)&w38)[0];
        if (param_1 + 1 > 0 && param_1 + 1 < CE2()) {
            iVar2 = gUserPieceHeights[LV()][param_1];
        } else {
            iVar2 = 0;
        }
        if (param_1 > 0 && param_1 < CE2()) {
            iVar2 = iVar2 - gUserPieceHeights[LV()][iVar3];
        }
        math_to_polar(4000, iVar2 << 4, (u16 *)&w30, &iStack_34);
        uVar1 = ((u16 *)&w30)[0];
        ((u16 *)&w30)[0] = 0x4000 - uVar1;
        if (0x1555 < (s16)((0x4000 - uVar1) - ((u16 *)&w38)[0])) {
            break;
        }
        iVar4 = iVar4 - 1;
    }
    gUserPieceHeights[LV()][param_1 - 1] = (s16)iVar5;
    return iVar4;
}

#define A_2 (gEditor->heightSel)
#define B_2 (gEditor->userPiece)
#define LIM_3 (gPieceTypes[B_2 + 0x12].dim[1] * 2)
#define H_2(c) ((c) > 0 && (c) < LIM_3 ? gUserPieceHeights[B_2][(c) - 1] : 0)
#define MIN_2(x, y) ((x) < (y) ? (x) : (y))
#define MAX_2(x, y) ((x) > (y) ? (x) : (y))
#define CELL (({ s16 *_tb = (s16 *)gUserPieceHeights; _tb; })[({ s32 _c = A_2 - 1; B_2 * 15 + _c; })])

void edit_user_piece_adjust_height(void)
{
    volatile s32 pad[8];
    s32 t1;
    s32 hi, lo;
    s32 flag;
    s32 v, x;
    u32 F;

    t1 = gPadInput >> 17;
    t1 &= 1;
    hi = MIN_2(H_2(A_2 + 1) + 0x8C, H_2(A_2 - 1) + 0x8C);
    lo = MAX_2(H_2(A_2 - 1) - 0x8C, H_2(A_2 + 1) - 0x8C);
    if (hi > 0x1C2) {
        hi = 0x1C2;
    }
    if (lo < -0x1C2) {
        lo = -0x1C2;
    }
    *(volatile s16 *)&gEditor->heightState = 2;
    if (gPadInput & 1) {
        goto body;
    }
    if (gPadInput & 2) {
        goto body;
    }
    if ((gPadInput & 0x800) && !(gPadInputPrev & 0x800)) {
        goto body;
    }
    if ((gPadInput & 0x1000) && !(gPadInputPrev & 0x1000)) {
    body:
        if ((gPadInput & 0x800) && !(gPadInputPrev & 0x800)) {
            *(u16 *)&CELL += 0x14;
        } else if ((gPadInput & 0x1000) && !(gPadInputPrev & 0x1000)) {
            *(u16 *)&CELL -= 0x14;
        } else {
            s16 *p = &CELL;
            s32 w = *p;
            if (gPadInput & 3) {
                if (gPadInput & 1) {
                    x = w + (gPadInput >> 28);
                } else {
                    x = w - (gPadInput >> 28);
                }
            } else {
                x = w;
            }
            *p = x;
        }
        if (t1 != 0) {
            v = CELL;
            if (hi < v) {
                if (flag != 0) {
                    gEditor->heightState = 1;
                }
                CELL = hi;
            } else if (v < lo) {
                CELL = lo;
                gEditor->heightState = 1;
            }
        } else {
            v = CELL;
            if (hi < v) {
                gEditor->heightState = 0;
                CELL = hi;
            } else if (v < lo) {
                CELL = lo;
                gEditor->heightState = 0;
            }
        }
    }
}

#define PRESSED_4(b) ((gPadInput & (b)) && !(gPadInputPrev_3[0] & (b)))

void edit_update_user_piece(void)
{
    s32 i;

    if (gEditor->flags & EDIT_DIALOG) {
        if (PRESSED_4(0x20)) {
            gPadInputPrev_3[0] |= 0x20;
            gEditor->menu->flags |= GUI_MENU_ACTIVE;
            gEditor->flags &= ~EDIT_DIALOG;
        } else if (PRESSED_4(0x10)) {
            gPadInputPrev_3[0] |= 0x10;
            if (gEditor->dialogChoice != 0) {
                switch (gEditor->dialog) {
                case 0:
                    if (gEditor->heightSel == 1) {
                        edit_user_piece_prepend_segment();
                    } else {
                        edit_user_piece_append_segment();
                    }
                    break;
                case 1:
                    for (i = 0; i < 15; i++) {
                        gUserPieceHeights[gEditor->userPiece][i] = 0;
                    }
                    if (gEditor->heightSel == 1) {
                        edit_user_piece_remove_first_segment();
                    } else {
                        edit_user_piece_remove_last_segment();
                    }
                    break;
                }
            }
            gEditor->menu->flags |= GUI_MENU_ACTIVE;
            gEditor->flags &= ~EDIT_DIALOG;
        } else if (PRESSED_4(0x800) || PRESSED_4(0x1000)) {
            gEditor->dialogChoice ^= 1;
            snd_play_menu_sound(0);
        }
    } else if (PRESSED_4(0x10)) {
        gEditor->mode = EDIT_MODE_PLACE;
        gEditor->menu->flags |= GUI_MENU_ACTIVE;
        sEditTrackMenu.choice = 3;
        sEditTrackMenu.cursor = 3;
        sEditTrackMenu.items[3].flags |= GUI_ITEM_ENABLED;
        sEditTrackMenu.items[3].valueMask |= 1 << (gEditor->userPieceCount - 1);
    } else {
        if (PRESSED_4(0x10000)) {
            PieceType *b = gPieceTypes;
            s32 k = gEditor->userPiece + 0x12;
            PieceType *t;
            t = &b[k];
            if (t->dim[1] >= 5 && !(t->flags & 0x200)) {
                if (gEditor->heightSel == 1 || gEditor->heightSel == t->dim[1] * 2 - 1) {
                    gEditor->dialog = 1;
                    gEditor->flags |= EDIT_DIALOG;
                    gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                    gEditor->dialogChoice = 0;
                    goto end;
                }
            }
        }
        if ((PRESSED_4(0x4000) && (gEditor->flags & EDIT_REVERSED)) || (PRESSED_4(0x2000) && !(gEditor->flags & EDIT_REVERSED))) {
            if (gEditor->heightSel >= 2) {
                gEditor->heightSel--;
                snd_play_menu_sound(0);
            } else {
                PieceType *b = gPieceTypes;
            s32 k = gEditor->userPiece + 0x12;
                PieceType *t;
            t = &b[k];
                if (t->dim[1] < gEditor->maxSegments && !(t->flags & 0x200)) {
                    gEditor->dialog = 0;
                    gEditor->flags |= EDIT_DIALOG;
                    gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                    gEditor->dialogChoice = 0;
                }
            }
        }
        if ((PRESSED_4(0x4000) && !(gEditor->flags & EDIT_REVERSED)) || (PRESSED_4(0x2000) && (gEditor->flags & EDIT_REVERSED))) {
            PieceType *b = gPieceTypes;
            s32 k = gEditor->userPiece + 0x12;
            PieceType *t;
            t = &b[k];
            if (gEditor->heightSel < t->dim[1] * 2 - 1) {
                gEditor->heightSel++;
                snd_play_menu_sound(0);
            } else if (t->dim[1] < gEditor->maxSegments && !(t->flags & 0x200)) {
                gEditor->dialog = 0;
                gEditor->flags |= EDIT_DIALOG;
                gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                gEditor->dialogChoice = 0;
            }
        }
        edit_user_piece_adjust_height();
    }
end:
    {
        PieceType *b = gPieceTypes;
            s32 k = gEditor->userPiece + 0x12;
        PieceType *t;
            t = &b[k];
        usrtrk_apply_heights(t->meshA, t->meshB, gUserPieceHeights[gEditor->userPiece], t->dim[1]);
    }
}

s32 func_8009EB00(s32 a, s32 b)
{
    return b;
}

/* Unused: whether c's prev can be reached from its next (1 also when either is missing). */
s32 edit_can_unlink_piece(EditPiece *c)
{
    EditPiece *n = c->next;
    if (n != NULL && c->prev != NULL) {
        do {
            if (n == c->prev) {
                return 1;
            }
            n = n->next;
        } while (n != NULL);
        return 0;
    }
    return 1;
}

void edit_find_start_piece(void) {
    Editor *p = gEditor;
    EditPiece *n;
    if (p->start != NULL) {
        return;
    }
    n = p->first;
    if (n == NULL) {
        return;
    }
    do {
        if (n->type->flags & PIECE_START) {
            p->start = n;
            return;
        }
        n = n->next;
        if (n == p->first) {
            return;
        }
    } while (n != NULL);
}

/* Puts the height cursor at the user piece's first point, or its last when the view is reversed. */
void edit_user_piece_reset_cursor(s32 arg0)
{
    Editor *p = gEditor;
    if (p->flags & EDIT_REVERSED) {
        s32 idx = p->userPiece;
        p->heightSel = gPieceTypes[idx + 0x12].dim[1] * 2 - 1;
        return;
    }
    p->heightSel = 1;
}

void edit_calc_track_cam_elevation(void)
{
    f32 x, y;
    s32 d;

    x = gEditor->cam[0].dist;
    gEditor->cam[0].height = ((x - 20000.0f) * 38000.0f) / 165000.0f + 40000.0f;
    y = gEditor->cam[0].height;
    d = __builtin_sqrtf(y * y + x * x);
    gEditor->cam[0].pitch = math_asini((gEditor->cam[0].height << 14) / d & 0x3FFF) & 0xFFFF;
}

void edit_calc_piece_cam_elevation(void) {
    Editor *g;
    f32 x;
    s32 n;
    f32 hyp;

    g = gEditor;
    x = (f32)g->cam[1].dist;
    n = (s32)((x - 20000.0f) * 32000.0f / 100000.0f + 8000.0f);
    g->cam[1].height = n;
    hyp = __builtin_sqrtf((f32)n * (f32)n + x * x);
    gEditor->cam[1].pitch = math_asini((gEditor->cam[1].height << 14) / (s32)hyp & 0x3FFF) & 0xFFFF;
}

s32 edit_ease_delta(s32 mode, s32 v, s32 d, s32 f)
{
    s32 iv1;
    s32 iv2;
    s32 q;
    s32 num;
    s32 r;
    s32 res;
    s32 ta;
    s32 tb;
    volatile s32 pad[2];

    res = 0;
    if (v != 0) {
        if (mode == 1) {
            goto m1;
        }
        if (mode == 0) {
            goto m0;
        }
        if (mode == 2) {
            goto m2;
        }
        if (mode == 3) {
            goto m3;
        }
        iv1 = 0;
        iv2 = 0;
        goto dv;
    m0:
        iv1 = 0x4000;
        iv2 = 0x4000;
        v = v << 1;
        goto common;
    m1:
        iv1 = 0x4000;
        iv2 = 0x8000;
        v = v << 1;
        goto common;
    m2:
        iv1 = 0x8000;
        iv2 = 0x4000;
        goto common;
    m3:
        num = v << 10;
        q = num / d;
        r = num % d;
        if (f == 0) {
            q = q + r;
        }
        return q;
    dv:
        v = iv1;
    common:
        q = (iv1 << 10) / d;
        ta = (s32)gCosTable[(((q * f) >> 10) + iv2 >> 4) - 0x400 & 0xFFF];
        f = f + 1;
        tb = (s32)gCosTable[(((q * f) >> 10) + iv2 >> 4) - 0x400 & 0xFFF];
        r = ta - tb;
        res = (r * v) >> 5;
    }
    return res;
}

void edit_set_menu(GuiMenu *a0)
{
    Editor *st = gEditor;
    if (st->menu != a0) {
        s32 s = st->slideState;
        if (s == 1) {
            st->menu = a0;
            goto tail;
        }
        if (s == 0) goto c0;
        if (s == 2) goto c2;
        if (s == 3) goto c3;
        goto tail;
c0:
        st->slideState = 1;
        st->nextMenu = a0;
        goto tail;
c2:
        st->slideState = 3;
        st->nextMenu = a0;
        goto tail;
c3:
        st->slideState = 1;
        st->menu = a0;
tail:
        gEditor->flags &= 0xFFFEFFFF; /* ~EDIT_A_HELD */
    }
}

void edit_cam_snap(void)
{
    EditCamPos *dst = &sEditCamPos;
    EditCamPos *src = &sEditCamTarget;
    u32 a, b, c;

    a = src->x;
    b = src->z;
    c = dst->yaw;
    sEditCamMoving = 0;
    dst->x = a;
    dst->z = b;
    dst->yaw = ((c + 0x800) >> 13) << 13;
}

void edit_clear_track(s32 a0) {
    s32 i;

    gEditor->first = NULL;
    gEditor->last = NULL;
    gEditor->cur = NULL;
    gEditor->start = NULL;
    gEditor->scrollX = 0;
    gEditor->scrollY = 0;
    for (i = 149; i >= 0; i--) {
        gEditor->pieces[i].used = 0;
    }
    if (a0 != 0) {
        gEditor->userPieceCount = 8;
    }
    strcpy(gEditor->name, "<NONE>");
}

typedef struct { u8 b[0x310]; } Blk_24;

void edit_save_track_to_game_pak(void)
{
    gGame.userTrack = mem_alloc_dbg(0x400, 0, sStrEditFile, 0x6FE);
    usrtrk_pack_track(gGame.userTrack);
    *(Blk_24 *)gSaveData.userTrack = *(Blk_24 *)gGame.userTrack;
    save_write(4, 0);
    mem_free(gGame.userTrack);
    gGame.userTrack = 0;
}

s32 edit_can_delete_piece(void) {
    Editor *o = gEditor;
    EditPiece *a = o->cur;
    EditPiece *n;
    s32 r;
    if (a == NULL || a != o->start || (a->prev == 0 && a->next == NULL)) {
        goto ret1;
    }
    n = o->first;
    r = 0;
    while (n != NULL) {
        if (n != a && (n->type->flags & PIECE_START) != 0) {
            r = 1;
            break;
        }
        n = n->next;
        if (n == o->first) {
            break;
        }
    }
    return r;
ret1:
    return 1;
}

void edit_reset_overlay_lines(s32 param_1) {
    s32 p = param_1;
    gEditor->lineCount = 0;
    guMtxL2F(gEditor->camMtx, gArnedCamMtx_3 + (p << 6));
    mem_fill((u8 *)gEditor->lineVtx[p], 0, 0xF00);
}

void edit_update_user_piece_usage(void)
{
    Editor *obj;
    PieceType *blk;
    EditPiece *n;
    s32 i;
    s32 cnt, flag;
    s32 m;
    i = 0;
    obj = gEditor;
    m = -0x601;
    for (; i < 8; i++) {
        u32 c;
        blk = &gPieceTypes[PIECE_TYPE_USER + i];
        c = (u16)blk->flags; /* read unsigned here */
        n = obj->first;
        blk->flags = c & m;
        cnt = 0;
        flag = cnt;
        if (n != 0) {
            do {
                if (n->type == blk) {
                    cnt++;
                    if (n->prev != 0 && n->next != 0) {
                        flag = 1;
                    }
                }
                n = n->next;
            } while (n != obj->first && n != 0);
        }
        if (flag) {
            blk->flags = blk->flags | 0x200;
        }
        if (cnt >= 2) {
            blk->flags = blk->flags | 0x600;
        }
    }
}

void edit_free_user_piece_meshes(s32 n)
{
    u32 *wp;
    s32 i, j;
    u32 k;
    PieceMeshSet *a;
    PieceMeshSet *b;

    wp = &gPieceTypes[PIECE_TYPE_TEMPLATE].meshA->count;
    for (i = 0; i < n; i++) {
        a = gPieceTypes[i + 18].meshA;
        b = gPieceTypes[i + 18].meshB;
        for (j = 0; j < gPieceTypes[i + 18].dim[1]; j++) {
            for (k = 0; k < *wp; k++) {
                mem_free(a->cells[j * *wp + k].mesh);
                mem_free(b->cells[j * *wp + k].mesh);
            }
        }
        mem_free(a);
        mem_free(b);
    }
}
