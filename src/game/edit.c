#include "game.h"

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
    EditPiece *n;

    if (gEditor->start == NULL) {
        n = gEditor->first;
        while (n != NULL) {
            if (n->type->flags & PIECE_START) {
                gEditor->start = n;
                break;
            }
            n = n->next;
            if (n == gEditor->first) {
                break;
            }
        }
    }
}

s32 edit_cam_step(void)
{
    sEditCamPosFixed.x += edit_ease_delta(1, sEditCamDelta.x, 0x23, sEditCamStep);
    sEditCamPosFixed.z += edit_ease_delta(1, sEditCamDelta.z, 0x23, sEditCamStep);
    sEditCamPosFixed.yaw += edit_ease_delta(2, sEditCamDelta.yaw, 0x23, sEditCamStep);
    sEditCamPos.x = sEditCamPosFixed.x >> 10;
    sEditCamPos.z = sEditCamPosFixed.z >> 10;
    sEditCamPos.yaw = (u16)(sEditCamPosFixed.yaw >> 10);
    sEditCamStep++;
    if (sEditCamStep == 0x23) {
        sEditCamMoving = 0;
        sEditCamPos.x = sEditCamTarget.x;
        sEditCamPos.z = sEditCamTarget.z;
        sEditCamPos.yaw = ((u32)(sEditCamPos.yaw + 0x800) >> 13) << 13;
    }
}

void edit_cam_update_target(void)
{
    EditPiece *obj;
    EditPiece *o;
    s32 a;
    EditCamPos *p;
    s32 fl;

    obj = gEditor->cur;
    if (obj != NULL && (obj->flags & EDIT_PIECE_PREVIEW) && (obj->next != NULL || obj->prev != NULL) && gEditor->mode != EDIT_MODE_USER_PIECE) {
        o = obj->next;
        if (o == NULL) {
            o = obj->prev;
        }
    } else {
        o = gEditor->cur;
    }
    if (o != NULL && (gEditor->mode == EDIT_MODE_USER_PIECE || !(o->flags & 4))) {
        a = o->type->cursorAngle;
        a -= o->rot << 1;
        if (fl = o->flags & 2, p = &sEditCamTarget, fl) {
            a += 4;
        }
        if (gEditor->flags & EDIT_REVERSED) {
            a += 4;
        }
        p->yaw = a & 7;
    } else {
        sEditCamTarget.yaw = (gEditor->flags & EDIT_REVERSED) << 2;
    }
    sEditCamTarget.yaw = (sEditCamTarget.yaw - (sEditCamPos.yaw >> 13)) & 7;
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
        gEditor->flags ^= 3;
    }
    if (gEditor->flags & EDIT_TURN_RIGHT) {
        sEditCamTarget.yaw += 4;
        gEditor->flags ^= 5;
    }
    if (gEditor->cur != NULL) {
        sEditCamTarget.x = o->x * 8000 + o->type->dim[o->rot & 1] * 4000 - gEditor->gridSize[0] * 4000;
        sEditCamTarget.z = o->y * 8000 + o->type->dim[(o->rot + 1) & 1] * 4000 - gEditor->gridSize[1] * 4000;
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

void edit_init(void)
{
    s32 i, j;
    u32 k, n;
    PieceType *o;
    PieceMesh *vl;
    s32 v;
    s32 c, q, x, y, w, f, d;

    gCurTrack = &gTracks[TRACK_USER];
    if (gEditor == NULL || !(gEditor->flags & (EDIT_PAK_LOAD | EDIT_PAK_SAVE))) {
        gEditor = mem_alloc_dbg(0x1810, 0, sStrEditFile, 0x50A);
        mem_fill((u8 *)gEditor, 0, 0x1810);
    } else {
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
    for (c = 149; c >= 0; c--) {
        gEditor->pieces[c].used = 0;
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
    for (q = 0; q < 4; q++) {
        gEditor->views[q].vp.vscale[0] = sEditViewRects[q].w * 2;
        gEditor->views[q].vp.vscale[1] = sEditViewRects[q].h * 2;
        gEditor->views[q].vp.vtrans[0] = (sEditViewRects[q].x + sEditViewRects[q].w / 2) * 4;
        gEditor->views[q].vp.vtrans[1] = (sEditViewRects[q].y + sEditViewRects[q].h / 2) * 4;
    }
    gEditor->screenVp.vp.vscale[0] = 1000;
    gEditor->screenVp.vp.vscale[1] = 320;
    gEditor->screenVp.vp.vscale[2] = 0x1FF;
    gEditor->screenVp.vp.vtrans[0] = (gScreenWidth / 2) * 4;
    gEditor->screenVp.vp.vtrans[1] = (gScreenHeight / 2) * 4;
    gEditor->screenVp.vp.vtrans[2] = 0x1FF;
    if (!(gEditor->flags & (EDIT_PAK_LOAD | EDIT_PAK_SAVE))) {
        gEditor->slideState = 1;
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
    gEditor->page = 0;
    for (i = 0; i < 29; i++) {
        gPieceTypes[i].flags |= 0x10;
        if (gPieceTypes[i].flags & 0x100) {
            gPieceTypes[i].flags |= 0x20;
        } else {
            gPieceTypes[i].flags &= ~0x20;
        }
        if (gPieceTypes[i].meshAId == NULL) continue;
        gPieceTypes[i].meshA = (PieceMeshSet *)res_load(gEditor->pieceRes, *gPieceTypes[i].meshAId);
        if (gPieceTypes[i].meshBId != NULL) {
            gPieceTypes[i].meshB = (PieceMeshSet *)res_load(gEditor->pieceRes, *gPieceTypes[i].meshBId);
        } else {
            gPieceTypes[i].meshB = NULL;
        }
        for (k = 0; k < gPieceTypes[i].meshA->count; k++) {
            vl = gPieceTypes[i].meshA->cells[k].mesh;
            for (n = 0; n < vl->vtxCount; n++) {
                for (j = 0; j < 3; j += 2) {
                    v = (s16)vl->vtx[n].v.ob[j];
                    if (v < 0) { s32 w2 = -v; s32 d2 = w2 / 50; s32 f2 = w2 - d2 * 50 >= 15; v = d2 + f2; v = -(v * 50); }
                    else { d = v / 50; f = v - d * 50 >= 15; v = d + f; v = v * 50; }
                    vl->vtx[n].v.ob[j] = v;
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
    arned_set_viewport(sEditViewRects[1].x, sEditViewRects[1].y, sEditViewRects[1].w - 1, sEditViewRects[1].h - 1);
    gEditor->cam[0].dist = 90000;
    gEditor->cam[1].dist = 45000;
    sEditCamTarget.yaw = 0;
    sEditCamPos.yaw = 0;
    {
        s32 fl = gEditor->flags;
        s32 m6 = fl & 0x6000;
        if (m6 != 0) {
            gEditor->flags = m6;
            if (fl & 0x2000) {
                sEditMainMenu.choice = 2;
                sEditMainMenu.cursor = 2;
            } else {
                sEditMainMenu.choice = 3;
                sEditMainMenu.cursor = 3;
            }
        } else {
            sEditMainMenu.choice = 1;
            sEditMainMenu.cursor = 1;
            gEditor->flags = 0;
        }
    }
    edit_calc_track_cam_elevation();
    edit_calc_piece_cam_elevation();
    gEditor->userPieceCount = 8;
    gEditor->lineVtx[0] = mem_alloc_dbg(0xF00, 0, sStrEditFile, 0x5E1);
    gEditor->lineVtx[1] = mem_alloc_dbg(0xF00, 0, sStrEditFile, 0x5E2);
    gEditor->userPiece = 0;
    edit_init_user_pieces();
    edit_user_piece_reset_cursor(0);
    gEditor->userPieceCount = 8;
    gui_init();
    gEditor->warnFrame = res_load(gGuiRes, gWarnFrameResId);
    gEditor->warnIcon = res_load(gGuiRes, gWarnTitleBarResId);
    if (gGame.userTrack != NULL) {
        gEditor->flags &= ~(EDIT_PAK_LOAD | EDIT_PAK_SAVE);
        usrtrk_load_into_editor((u8 *)gGame.userTrack + 4);
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
    osViSetSpecialFeatures(0xAA);
}

void edit_exit(void)
{
    s32 iVar7;
    s32 n;

    if (gEditor->flags & EDIT_TEST_TRACK) {
        iVar7 = gUserTrackLayout->pieceCount;
    } else {
        iVar7 = -1;
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
    osViSetSpecialFeatures(0xAA);
    if (iVar7 >= 0) {
        __builtin_memcpy(gZBuffer, gGame.userTrack, 0x400);
        n = (iVar7 - 1) * 0xc + 0x110;
        memcpy(gZBuffer + 0x400, gUserTrackLayout, n);
        mem_free(gUserTrackLayout);
        mem_free(gGame.userTrack);
        gUserTrackLayout = mem_alloc_dbg(n, 0, sStrEditFile, 0x651);
        gGame.userTrack = mem_alloc_dbg(0x400, 0, sStrEditFile, 0x652);
        memcpy(gUserTrackLayout, gZBuffer + 0x400, n);
        __builtin_memcpy(gGame.userTrack, gZBuffer, 0x400);
    }
    font_unload_set();
}

#define PRESSED(b) ((gPadInput_3[0] & (b)) && !(gPadInputPrev & (b)))

typedef struct { u8 c[11]; } Name11;

void edit_update_name_entry(Editor *arg0, s32 arg1)
{
    s32 i;
    s32 old;
    s32 lo, hi, lo2, hi2;

    for (i = 0; i < 4; i++) {
        if ((((s32)gPadInput_3[i] >> 24) & 0xF) < 6) {
            gPadInput_3[i] &= ~0xC;
        }
        if ((s32)(gPadInput_3[i] >> 28) < 6) {
            gPadInput_3[i] &= ~3;
        }
    }

    if (PRESSED(0x10)) {
        if (gEditor->nameKey == 0x26 && gEditor->nameLen == 0) {
            snd_play_menu_sound(3);
        } else {
            snd_play_menu_sound(1);
        }
        {
            Editor *g = gEditor;
            if (g->nameKey == 0x26) {
                s32 n = g->nameLen;
                if (n != 0) {
                    g->nameBuf[n] = 0;
                    g->nameLen = n - 1;
                }
            } else if (g->nameKey == 0x27) {
                *(Name11 *)g->name = *(Name11 *)g->nameBuf;
                gEditor->mode = EDIT_MODE_IDLE;
                gEditor->menu->flags |= GUI_MENU_ACTIVE;
            } else {
                g->nameLen++;
                if (g->nameLen >= 10) {
                    g->nameCursorX = 799;
                    g->nameCursorY = 799;
                    g->nameKey = 0x27;
                }
            }
        }
        return;
    }
    if (PRESSED(0x20)) {
        if (gEditor->nameLen == 0) {
            snd_play_menu_sound(3);
            return;
        }
        snd_play_menu_sound(1);
        gEditor->nameBuf[gEditor->nameLen - 1] = gEditor->nameBuf[gEditor->nameLen];
        {
            Editor *g = gEditor;
            s32 n = g->nameLen;
            g->nameBuf[n--] = 0;
            g->nameLen = n;
        }
        return;
    }

    {
        s32 y;
        y = (s32)gEditor;
        old = ((Editor *)y)->nameKey;
        if (((Editor *)y)->nameLen < 10) {
            Editor *g;
            u32 v;
            s32 x, nx;
            lo = 0;
            lo2 = lo;
            hi = 800;
            v = gPadInput_3[0];
            g = (Editor *)y;
            hi2 = hi;
            x = g->nameCursorX;
            if (v & 0xC) {
                if (v & 8) {
                    nx = x + (((s32)v >> 24) & 0xF);
                } else {
                    nx = x - (((s32)v >> 24) & 0xF);
                }
            } else {
                nx = x;
            }
            g->nameCursorX = nx;
            if (PRESSED(0x2000)) {
                gEditor->nameCursorX -= 100;
            }
            if (PRESSED(0x4000)) {
                gEditor->nameCursorX += 100;
            }
            {
                Editor *h = gEditor;
                s32 c = h->nameCursorX;
                if (c >= hi) {
                    h->nameCursorX = lo;
                } else if (c < lo) {
                    h->nameCursorX = hi - 1;
                }
            }
            {
                Editor *h = gEditor;
                u32 v2 = gPadInput_3[0];
                s32 ny;
                y = h->nameCursorY;
                if (v2 & 3) {
                    if (v2 & 1) {
                        ny = y - ((s32)((v2 >> 28) * 3) >> 1);
                    } else {
                        ny = y - (-(s32)(v2 >> 28) * 3) / 2;
                    }
                } else {
                    ny = y;
                }
                h->nameCursorY = ny;
            }
            if (PRESSED(0x800)) {
                gEditor->nameCursorY -= 0xA0;
            }
            if (PRESSED(0x1000)) {
                gEditor->nameCursorY += 0xA0;
            }
            {
                Editor *h = gEditor;
                s32 c = h->nameCursorY;
                if (c >= hi2) {
                    h->nameCursorY = lo2;
                } else if (c < lo2) {
                    h->nameCursorY = hi2 - 1;
                }
            }
            gEditor->nameKey = gEditor->nameCursorX / 100 + (gEditor->nameCursorY / 0xA0) * 8;
        } else if (PRESSED(4) || PRESSED(0x2000) || PRESSED(8) || PRESSED(0x4000)) {
            Editor *h = gEditor;
            if (h->nameKey == 0x27) {
                h->nameCursorX = 600;
                h->nameCursorY = 799;
                h->nameKey = 0x26;
            } else {
                h->nameCursorX = 799;
                h->nameCursorY = 799;
                h->nameKey = 0x27;
            }
        }
    }

    if (gEditor->nameKey < 0x26) {
        if (strlen(gEditor->nameBuf) == 0) {
            gEditor->nameBuf[1] = 0;
        }
        gEditor->nameBuf[gEditor->nameLen] = *gKeyboardKeys[gEditor->nameKey];
    } else {
        gEditor->nameBuf[gEditor->nameLen] = 0;
    }
    if (old != gEditor->nameKey) {
        snd_play_menu_sound(0);
    }
}

typedef struct {
    u8 pad0[0x2C4];
    u8 f2C4;
    u8 pad2C5[0x22F];
    u8 f4F4[0x310];
} Save_7;

extern Save_7 gSaveData_16;

void edit_update_main_menu(void)
{
    Editor *p = gEditor;
    s32 flags = p->flags;
    s32 i, j, k, n;

    if (flags & 0x800) {
        if ((gPadInput_2[0] & 0x10) && !(gPadInputPrev_2[0] & 0x10)) {
            gPadInputPrev_2[0] |= 0x10;
            gEditor->flags &= ~EDIT_MESSAGE;
            gEditor->menu->flags |= GUI_MENU_ACTIVE;
            snd_play_menu_sound(1);
        }
    } else if (flags & 0x1000) {
        if ((gPadInput_2[0] & 0x20) && !(gPadInputPrev_2[0] & 0x20)) {
            gPadInputPrev_2[0] |= 0x20;
            gEditor->flags &= ~EDIT_DIALOG;
            gEditor->menu->flags |= GUI_MENU_ACTIVE;
            snd_play_menu_sound(2);
        } else if ((gPadInput_2[0] & 0x10) && !(gPadInputPrev_2[0] & 0x10)) {
            gPadInputPrev_2[0] |= 0x10;
            snd_play_menu_sound(1);
            if (gEditor->dialogChoice) {
                switch (gEditor->dialog) {
                case 7:
                    gEditor->dialog = 6;
                    if (gSaveData_16.f2C4 & 8) {
                        gEditor->dialogChoice = 1;
                    } else {
                        gEditor->dialogChoice = 0;
                    }
                    break;
                case 6:
                    if (gSaveData_16.f2C4 & 8) {
                        for (j = 0; j < gEditor->gridSize[1]; j++) {
                            for (i = 0; i < gEditor->gridSize[0]; i++) {
                                arned_set_cell(i, j, 0, 0, 0, 0, 0, 0, 0);
                            }
                        }
                        gEditor->userPieceCount = 8;
                        usrtrk_load_into_editor(gSaveData.userTrack);
                        edit_try_close_loop(gEditor->last);
                        gEditor->mode = EDIT_MODE_IDLE;
                        gEditor->menu->flags |= GUI_MENU_ACTIVE;
                        edit_find_start_piece();
                        arned_load_piece_textures(gEditor->dirtType, gEditor->objectTex, gEditor->wallTex);
                        gEditor->menu->flags |= GUI_MENU_ACTIVE;
                        gEditor->flags &= ~EDIT_DIALOG;
                    } else {
                        gEditor->message = 0;
                        gEditor->flags |= EDIT_MESSAGE;
                    }
                    break;
                case 8:
                    gGame.userTrack = mem_alloc_dbg(0x400, 0, (s32)sStrEditFile, 0x6FE);
                    usrtrk_pack_track_2(gGame.userTrack);
                    __builtin_memcpy(gSaveData.userTrack, gGame.userTrack, 0x310);
                    save_write(4, 0);
                    mem_free(gGame.userTrack);
                    gGame.userTrack = 0;
                    gEditor->menu->flags |= GUI_MENU_ACTIVE;
                    gEditor->flags &= ~EDIT_DIALOG;
                    break;
                case 5:
                    if (gSaveData_16.f2C4 & 8) {
                        gEditor->dialog = 8;
                        gEditor->dialogChoice = 0;
                    } else {
                        gGame.userTrack = mem_alloc_dbg(0x400, 0, (s32)sStrEditFile, 0x6FE);
                        usrtrk_pack_track_2(gGame.userTrack);
                        __builtin_memcpy(gSaveData_16.f4F4, gGame.userTrack, 0x310);
                        save_write(4, 0);
                        mem_free(gGame.userTrack);
                        gGame.userTrack = 0;
                        gEditor->menu->flags |= GUI_MENU_ACTIVE;
                        gEditor->flags &= ~EDIT_DIALOG;
                    }
                    break;
                case 3:
                    gGame.flags |= 2;
                    gEditor->exitTimer = 0;
                    gEditor->flags |= EDIT_EXIT;
                    gEditor->menu->flags |= GUI_MENU_ACTIVE;
                    gEditor->flags &= ~EDIT_DIALOG;
                    break;
                case 4:
                    gEditor->first = 0;
                    gEditor->last = 0;
                    gEditor->cur = 0;
                    gEditor->start = 0;
                    gEditor->scrollX = 0;
                    gEditor->scrollY = 0;
                    for (k = 0; k < 150; k++) {
                        gEditor->pieces[k].used = 0;
                    }
                    gEditor->userPieceCount = 8;
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
                switch (gEditor->dialog) {
                case 8:
                    gEditor->message = 1;
                    gEditor->flags = (gEditor->flags & ~EDIT_DIALOG) | 0x800;
                    break;
                case 6:
                    gGame.flags |= 2;
                    gEditor->exitTimer = 0;
                    gEditor->flags = (gEditor->flags | 0x2010) & ~0x4000;
                    gEditor->menu->flags |= GUI_MENU_ACTIVE;
                    gEditor->flags &= ~EDIT_DIALOG;
                    break;
                case 5:
                    gGame.flags |= 2;
                    gEditor->exitTimer = 0;
                    gEditor->flags = (gEditor->flags | 0x4010) & ~0x2000;
                    gGame.userTrack = mem_alloc_dbg(0x400, 0, (s32)sStrEditFile, 0x7B3);
                    for (n = usrtrk_pack_track_2(gGame.userTrack + 4) + 4; n < 0x400; n++) {
                        gGame.userTrack[n] = 0;
                    }
                    *(u32 *)gGame.userTrack = calc_checksum24(gGame.userTrack + 4, 0x3FC);
                default:
                    gEditor->menu->flags |= GUI_MENU_ACTIVE;
                    gEditor->flags &= ~EDIT_DIALOG;
                    break;
                }
            }
        } else if (((gPadInput_2[0] & 0x800) && !(gPadInputPrev_2[0] & 0x800)) ||
                   ((gPadInput_2[0] & 0x1000) && !(gPadInputPrev_2[0] & 0x1000))) {
            if ((gEditor->dialog != 6 || (gSaveData_16.f2C4 & 8)) && gEditor->dialog != 5) {
                gEditor->dialogChoice ^= 1;
                snd_play_menu_sound(0);
            }
        }
    } else {
        if (p->cur != NULL && p->mode != 7) {
            if (((gPadInput_2[0] & 0x2000) && !(gPadInputPrev_2[0] & 0x2000) && !(gEditor->flags & EDIT_REVERSED)) ||
                ((gPadInput_2[0] & 0x4000) && !(gPadInputPrev_2[0] & 0x4000) && (gEditor->flags & EDIT_REVERSED))) {
                if (gEditor->cur->prev) {
                    gEditor->cur = gEditor->cur->prev;
                }
            } else if (((gPadInput_2[0] & 0x4000) && !(gPadInputPrev_2[0] & 0x4000) && !(gEditor->flags & EDIT_REVERSED)) ||
                       ((gPadInput_2[0] & 0x2000) && !(gPadInputPrev_2[0] & 0x2000) && (gEditor->flags & EDIT_REVERSED))) {
                if (gEditor->cur->next) {
                    gEditor->cur = gEditor->cur->next;
                }
            }
        }
        if (gEditor->mode == EDIT_MODE_NAME || gEditor->mode == EDIT_MODE_NAME_8) {
            edit_update_name_entry(p, flags);
        } else if (gui_menu_input(gEditor->menu)) {
            switch (sEditMainMenu.choice) {
            case 1:
                gEditor->mode = EDIT_MODE_IDLE;
                if (gEditor->menu != &sEditTrackMenu) {
                    switch (gEditor->slideState) {
                    case 1:
                        gEditor->menu = &sEditTrackMenu;
                        break;
                    case 0:
                        gEditor->slideState = 1;
                        gEditor->nextMenu = &sEditTrackMenu;
                        break;
                    case 2:
                        gEditor->slideState = 3;
                        gEditor->nextMenu = &sEditTrackMenu;
                        break;
                    case 3:
                        gEditor->slideState = 1;
                        gEditor->menu = &sEditTrackMenu;
                        break;
                    }
                    gEditor->flags &= ~EDIT_A_HELD;
                }
                sEditTrackMenu.choice = 0;
                sEditTrackMenu.cursor = 0;
                gEditor->page = 1;
                break;
            case 0:
                gEditor->dialog = 4;
                gEditor->flags |= EDIT_DIALOG;
                gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                gEditor->dialogChoice = 0;
                break;
            case 2:
                gEditor->flags |= EDIT_DIALOG;
                if (gEditor->cur != NULL) {
                    gEditor->dialog = 7;
                    gEditor->dialogChoice = 0;
                } else {
                    gEditor->dialog = 6;
                    if (gSaveData_16.f2C4 & 8) {
                        gEditor->dialogChoice = 1;
                    } else {
                        gEditor->dialogChoice = 0;
                    }
                }
                gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                break;
            case 3:
                if (gEditor->name[0] == '<') {
                    gEditor->flags |= EDIT_MESSAGE;
                    gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                    gEditor->message = 4;
                    sEditMainMenu.choice = 6;
                    sEditMainMenu.cursor = 6;
                } else {
                    gEditor->dialog = 5;
                    gEditor->flags |= EDIT_DIALOG;
                    gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                    gEditor->dialogChoice = 0;
                }
                break;
            case 5:
                if (++gEditor->dirtType == 3) {
                    gEditor->dirtType = 0;
                }
                arned_load_piece_textures(gEditor->dirtType, gEditor->objectTex, gEditor->wallTex);
                break;
            case 7:
                gEditor->dialog = 3;
                gEditor->dialogChoice = 0;
                gEditor->flags = (gEditor->flags & ~(EDIT_PAK_LOAD | EDIT_PAK_SAVE)) | 0x1000;
                gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                break;
            case 6:
                gEditor->nameLen = 0;
                gEditor->nameKey = 0;
                gEditor->nameCursorX = 0;
                gEditor->nameCursorY = 0;
                mem_fill(gEditor->nameBuf, 0, 0xB);
                gEditor->mode = EDIT_MODE_NAME;
                gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                break;
            case 4:
                if (gGame.difficulty == 0) {
                    gGame.difficulty = 1;
                }
                if (gEditor->first->prev == gEditor->last) {
                    gGame.mode = 0xD;
                } else {
                    gGame.mode = 0xE;
                }
                gGame.numPlayers = 1;
                gGame.flags &= ~2;
                gEditor->flags &= ~(EDIT_PAK_LOAD | EDIT_PAK_SAVE);
                gGame.flags |= 0x4000000;
                gEditor->flags |= 0x410;
                gEditor->exitTimer = 0;
                gEditor->flags &= ~EDIT_DIALOG;
                if (gGame.userTrack == NULL) {
                    gGame.userTrack = mem_alloc_dbg(0x400, 0, (s32)sStrEditFile, 0x88C);
                    for (n = usrtrk_pack_track_2(gGame.userTrack + 4) + 4; n < 0x400; n++) {
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
#define DEL()                                                                          if (gEditor->cur != NULL) {                                                         if (gEditor->cur->prev == NULL) {                                                   n = gEditor->cur;                                                               gEditor->cur = n->next;                                                         edit_delete_piece(n);                                                                  edit_pick_start_piece();                                                                         } else if (gEditor->cur->next == NULL || gEditor->first->prev == gEditor->last) {             n = gEditor->cur;                                                               gEditor->cur = n->prev;                                                         edit_delete_piece(n);                                                                  edit_pick_start_piece();                                                                         }                                                                              }


static inline s32 chk(Editor *e, EditPiece *c) {
    EditPiece *n;
    s32 r;

    if (c == NULL || c != e->start || (c->prev == NULL && c->next == NULL)) {
        return 1;
    }
    r = 0;
    n = e->first;
    while (n != NULL) {
        if (n != c && (n->type->flags & PIECE_START)) {
            r = 1;
            break;
        }
        n = n->next;
        if (n == e->first) {
            break;
        }
    }
    return r;
}

static inline s32 chkg(void) {
    EditPiece *n;
    s32 r;

    if (gEditor->cur == NULL || gEditor->cur != gEditor->start) {
        return 1;
    }
    if (gEditor->cur->prev != NULL || gEditor->cur->next != NULL) {
        r = 0;
        n = gEditor->first;
        while (n != NULL) {
            if (n != gEditor->cur && (n->type->flags & PIECE_START)) {
                r = 1;
                break;
            }
            n = n->next;
            if (n == gEditor->first) {
                break;
            }
        }
        return r;
    }
    return 1;
}

void edit_update_track_menu(void) {
    s16 minx, miny, maxx, maxy;
    s16 d[2];
    EditPiece *n;
    s32 ok;
    s32 i, j;
    Editor *e;
    EditPiece *c;
    EditPiece *s;

    minx = 1000;
    maxx = -1000;
    miny = 1000;
    maxy = -1000;
    gEditor->flags &= ~0x1E0;
    if (gEditor->cur != NULL) {
        n = gEditor->first;
        do {
            if (n->x < minx) {
                minx = n->x;
            }
            if (n->y < miny) {
                miny = n->y;
            }
            if (n->x + n->w > maxx) {
                maxx = n->x + n->w;
            }
            if (n->y + n->h > maxy) {
                maxy = n->y + n->h;
            }
            n = n->next;
        } while (n != NULL && n != gEditor->first);
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
    if (gEditor->cur != NULL && (gEditor->cur->flags & 4)) {
        sEditTrackMenuItems[6].flags &= ~GUI_ITEM_ENABLED;
        sEditTrackMenuItems[4].flags &= ~GUI_ITEM_ENABLED;
        sEditTrackMenuItems[7].flags &= ~GUI_ITEM_ENABLED;
        sEditTrackMenuItems[5].flags &= ~GUI_ITEM_ENABLED;
        gEditor->flags &= ~EDIT_CAN_DELETE;
    } else {
        sEditTrackMenuItems[7].flags |= GUI_ITEM_ENABLED;
        if (gEditor->cur == NULL ||
            (gEditor->cur->next != NULL && gEditor->cur->prev != NULL && gEditor->first->prev == NULL) ||
            (e = gEditor, c = e->cur, !chk(e, c))) {
            sEditTrackMenuItems[6].flags &= ~GUI_ITEM_ENABLED;
            gEditor->flags &= ~EDIT_CAN_DELETE;
        } else {
            gEditor->flags |= EDIT_CAN_DELETE;
            sEditTrackMenuItems[6].flags |= GUI_ITEM_ENABLED;
        }
        if (gEditor->flags & 0x1E0) {
            sEditTrackMenuItems[4].flags |= GUI_ITEM_ENABLED;
        } else {
            sEditTrackMenuItems[4].flags &= ~GUI_ITEM_ENABLED;
        }
    }
    if (gEditor->menu != NULL) {
        gui_menu_fix_cursor(gEditor->menu);
    }
    if (gEditor->flags & EDIT_MESSAGE) {
        if (PRESSED_2(0x10)) {
            gPadInputPrev_3[0] |= 0x10;
            gEditor->flags &= ~EDIT_MESSAGE;
            gEditor->menu->flags |= GUI_MENU_ACTIVE;
        }
    } else {
        if (gEditor->mode == EDIT_MODE_WARN_MULTI || gEditor->mode == EDIT_MODE_WARN_LOCKED) {
            if (PRESSED_2(0x10)) {
                gEditor->mode = EDIT_MODE_USER_PIECE;
            } else if (PRESSED_2(0x20)) {
                gEditor->menu->flags |= GUI_MENU_ACTIVE;
                gEditor->mode = EDIT_MODE_PLACE;
                gEditor->flags &= ~EDIT_DIALOG;
            }
        } else if (gEditor->mode == EDIT_MODE_PLACE || gEditor->mode == EDIT_MODE_USER_PIECE) {
            if (PRESSED_2(0x20)) {
                if (gEditor->mode == EDIT_MODE_PLACE) {
                    edit_remove_preview_pieces(gEditor->cur);
                    gEditor->mode = EDIT_MODE_IDLE;
                    gEditor->flags &= ~EDIT_OPEN_END;
                }
            } else {
                edit_update_place_piece();
            }
        } else if (gEditor->mode == EDIT_MODE_MOVE_TRACK) {
            ok = 0;
            if (PRESSED_2(0x20)) {
                d[0] = gEditor->first->x - gEditor->moveFromX;
                d[1] = gEditor->first->y - gEditor->moveFromY;
                n = gEditor->first;
                do {
                    n->x -= d[0];
                    n->y -= d[1];
                    n = n->next;
                } while (n != NULL && n != gEditor->first);
                gEditor->mode = EDIT_MODE_IDLE;
                gEditor->menu->flags |= GUI_MENU_ACTIVE;
                ok = 1;
                snd_play_menu_sound(2);
            } else if (PRESSED_2(0x10)) {
                gEditor->mode = EDIT_MODE_IDLE;
                gEditor->menu->flags |= GUI_MENU_ACTIVE;
                snd_play_menu_sound(1);
            } else if ((gEditor->flags & EDIT_ROOM_X_NEG) && PRESSED_2(0x2000)) {
                n = gEditor->first;
                do {
                    n->x--;
                    n = n->next;
                } while (n != NULL && n != gEditor->first);
                snd_play_menu_sound(0);
                ok = 1;
            } else if ((gEditor->flags & EDIT_ROOM_X_POS) && PRESSED_2(0x4000)) {
                n = gEditor->first;
                do {
                    n->x++;
                    n = n->next;
                } while (n != NULL && n != gEditor->first);
                snd_play_menu_sound(0);
                ok = 1;
            } else if ((gEditor->flags & EDIT_ROOM_Y_POS) && PRESSED_2(0x800)) {
                n = gEditor->first;
                do {
                    n->y++;
                    n = n->next;
                } while (n != NULL && n != gEditor->first);
                snd_play_menu_sound(0);
                ok = 1;
            } else if ((gEditor->flags & EDIT_ROOM_Y_NEG) && PRESSED_2(0x1000)) {
                n = gEditor->first;
                do {
                    n->y--;
                    n = n->next;
                } while (n != NULL && n != gEditor->first);
                snd_play_menu_sound(0);
                ok = 1;
            }
            if (ok) {
                n = gEditor->first;
                do {
                    edit_update_piece_ports(gEditor->first);
                    n = n->next;
                } while (n != NULL && n != gEditor->first);
                for (i = 0; i < gEditor->gridSize[1]; i++) {
                    for (j = 0; j < gEditor->gridSize[0]; j++) {
                        arned_set_cell(j, i, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
        } else if (gui_menu_input(gEditor->menu)) {
            switch (sEditTrackMenu.choice) {
            case 0:
            case 1:
            case 2:
            case 3:
                if (gEditor->cur == NULL) {
                    gEditor->cur = edit_insert_piece(NULL, NULL, gEditor->selType);
                    gEditor->cur->flags |= 4;
                    gEditor->mode = EDIT_MODE_PLACE;
                    gEditor->first = gEditor->cur;
                    gEditor->last = gEditor->cur;
                } else if (gEditor->cur->next == NULL) {
                    if (gEditor->cur->prev == NULL) {
                        gEditor->cur = edit_insert_piece(gEditor->cur, NULL, gEditor->selType);
                        gEditor->cur->flags |= 4;
                        gEditor->mode = EDIT_MODE_IDLE;
                        gEditor->last = gEditor->cur;
                        gEditor->menu->flags |= GUI_MENU_ACTIVE;
                        gEditor->mode = EDIT_MODE_PLACE;
                        gEditor->flags |= EDIT_OPEN_END;
                    } else {
                        gEditor->cur = edit_insert_piece(gEditor->cur, NULL, gEditor->selType);
                        gEditor->cur->flags |= 4;
                        gEditor->mode = EDIT_MODE_PLACE;
                        gEditor->last = gEditor->cur;
                    }
                } else if (gEditor->cur->prev == NULL) {
                    gEditor->cur = edit_insert_piece(NULL, gEditor->cur, gEditor->selType);
                    gEditor->cur->flags |= 4;
                    gEditor->mode = EDIT_MODE_PLACE;
                    gEditor->first = gEditor->cur;
                }
                break;
            case 5:
                if (gEditor->cur != NULL) {
                    gEditor->start = gEditor->cur;
                }
                break;
            case 4:
                if (gEditor->cur != NULL) {
                    gEditor->moveFromX = gEditor->first->x;
                    gEditor->moveFromY = gEditor->first->y;
                    gEditor->mode = EDIT_MODE_MOVE_TRACK;
                    gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
                }
                break;
            case 7:
                snd_play_menu_sound(2);
                if (gEditor->menu != &sEditMainMenu) {
                    switch (gEditor->slideState) {
                    case 0:
                        gEditor->slideState = 1;
                        gEditor->nextMenu = &sEditMainMenu;
                        break;
                    case 1:
                        gEditor->menu = &sEditMainMenu;
                        break;
                    case 2:
                        gEditor->slideState = 3;
                        gEditor->nextMenu = &sEditMainMenu;
                        break;
                    case 3:
                        gEditor->slideState = 1;
                        gEditor->menu = &sEditMainMenu;
                        break;
                    }
                    gEditor->flags &= ~EDIT_A_HELD;
                }
                gEditor->page = 0;
                break;
            case 6:
                if (gEditor->cur != NULL && chkg()) {
                    DEL();
                }
                break;
            }
        } else if (PRESSED_2(0x20)) {
            e = gEditor;
            if (e->flags & EDIT_CAN_DELETE) {
                c = e->cur;
                if (chk(e, c)) {
                    DEL();
                }
            }
        }
        if (gEditor->cur != NULL) {
            s = gEditor->cur;
            if (s->flags & 4) {
                if (s->next == NULL) {
                    s = s->prev;
                } else {
                    s = s->next;
                }
            }
            if (s != NULL) {
                edit_update_fitting_pieces(s);
            } else {
                edit_set_start_piece_choices();
            }
        }
        if (gEditor->mode == EDIT_MODE_IDLE && gEditor->cur != NULL) {
            if ((PRESSED_2(0x2000) && !(gEditor->flags & EDIT_REVERSED)) || (PRESSED_2(0x4000) && (gEditor->flags & EDIT_REVERSED))) {
                if (gEditor->cur->prev != NULL) {
                    gEditor->cur = gEditor->cur->prev;
                }
            } else if ((PRESSED_2(0x4000) && !(gEditor->flags & EDIT_REVERSED)) ||
                       (PRESSED_2(0x2000) && (gEditor->flags & EDIT_REVERSED))) {
                if (gEditor->cur->next != NULL) {
                    gEditor->cur = gEditor->cur->next;
                }
            }
        }
    }
    if (gEditor->cur != NULL) {
        if (gEditor->cur->flags & 2) {
            gEditor->flags |= EDIT_FLIP;
        } else {
            gEditor->flags &= ~EDIT_FLIP;
        }
    }
}

#define PRESSED_3(m) ((gPadInput & (m)) && !(gPadInputPrev & (m)))
#define IDX (sEditTrackMenu.items[sEditTrackMenu.choice].value)

#define INSERT(B)                                                                          \
    if (gEditor->cur == NULL) {                                                         \
        gEditor->cur = edit_insert_piece(NULL, NULL, &gPieceTypes[(B) + IDX]);                              \
        gEditor->first = gEditor->cur;                                                \
        gEditor->last = gEditor->cur;                                                \
        gEditor->start = gEditor->cur;                                                \
    } else if (gEditor->cur->prev != NULL ||                                            \
               ((gEditor->flags & EDIT_OPEN_END) && gEditor->cur->next == NULL)) {          \
        gEditor->cur = edit_insert_piece(gEditor->cur, gEditor->cur->next, &gPieceTypes[(B) + IDX]);  \
        gEditor->last = gEditor->cur;                                                \
    } else {                                                                               \
        gEditor->cur = edit_insert_piece(NULL, gEditor->cur, &gPieceTypes[(B) + IDX]);                   \
        gEditor->first = gEditor->cur;                                                \
    }


void edit_update_place_piece(void) {
    EditPiece *cur;
    EditPiece *c;
    EditPiece *p;
    EditPiece *n;
    s32 i;
    s32 j;
    s32 ok;

    c = gEditor->cur;
    p = gEditor->markStart;
    if (c != NULL && c->prev == NULL && c->next == NULL) {
        if ((PRESSED_3(0x2000) && !(gEditor->flags & EDIT_REVERSED)) || (PRESSED_3(0x4000) && (gEditor->flags & EDIT_REVERSED))) {
            if (gEditor->scrollX >= -30) {
                gEditor->scrollX--;
            }
        }
        if ((PRESSED_3(0x4000) && !(gEditor->flags & EDIT_REVERSED)) || (PRESSED_3(0x2000) && (gEditor->flags & EDIT_REVERSED))) {
            if (gEditor->scrollX < 31) {
                gEditor->scrollX++;
            }
        }
        if ((PRESSED_3(0x1000) && !(gEditor->flags & EDIT_REVERSED)) || (PRESSED_3(0x800) && (gEditor->flags & EDIT_REVERSED))) {
            if (gEditor->scrollY >= -30) {
                gEditor->scrollY--;
            }
        }
        if ((PRESSED_3(0x800) && !(gEditor->flags & EDIT_REVERSED)) || (PRESSED_3(0x1000) && (gEditor->flags & EDIT_REVERSED))) {
            if (gEditor->scrollY < 31) {
                gEditor->scrollY++;
            }
        }
    }
    if (p != NULL) {
        do {
            p = p->next;
        } while (p->prev != gEditor->markEnd);
    }
    if (PRESSED_3(0x10000)) {
        if ((*(u32 *)&gEditor->cur->type->flags & 0x300000) == 0x300000 /* flags 0x30 tested as a word */) {
            gPadInput &= 0x10000;
            snd_play_menu_sound(0);
            for (i = 0; i < gEditor->cur->type->dim[(gEditor->cur->rot + 1) & 1]; i++) {
                for (j = 0; j < gEditor->cur->type->dim[gEditor->cur->rot & 1]; j++) {
                    if (gEditor->cur->x + j >= 0 && gEditor->cur->x + j < gEditor->gridSize[0] &&
                        gEditor->cur->y + i >= 0 && gEditor->cur->y + i < gEditor->gridSize[1]) {
                        arned_set_cell(gEditor->cur->x + j, gEditor->cur->y + i, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
            gEditor->flags ^= 8;
            if (gEditor->flags & EDIT_FLIP) {
                gEditor->cur->flags |= 2;
            } else {
                gEditor->cur->flags &= ~2;
            }
        }
        p = gEditor->first;
        if (p != NULL) {
            do {
                edit_update_piece_ports(p);
                p = p->next;
            } while (p != NULL && p != gEditor->first);
        }
    } else if (PRESSED_3(0x20000)) {
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
    n = gEditor->cur;
    if (n->flags & 4) {
        if (n->next == NULL) {
            n = n->prev;
        } else {
            n = n->next;
        }
    }
    if (n != NULL) {
        edit_update_fitting_pieces(n);
    } else {
        edit_set_start_piece_choices();
    }
    if (gEditor->mode == EDIT_MODE_USER_PIECE) {
        edit_update_user_piece();
        gEditor->flags |= EDIT_OPEN_END;
    } else if (gui_menu_input(gEditor->menu)) {
        ok = 0;
        switch (sEditTrackMenu.choice) {
        case 3:
            edit_remove_preview_pieces_2();
            gEditor->userPiece = IDX;
            INSERT(PIECE_TYPE_USER);
            ok = 1;
            break;
        case 0:
            edit_remove_preview_pieces_2();
            INSERT(PIECE_TYPE_STRAIGHT);
            ok = 1;
            break;
        case 1:
            edit_remove_preview_pieces_2();
            INSERT(PIECE_TYPE_TURN);
            ok = 1;
            break;
        case 2:
            edit_remove_preview_pieces_2();
            INSERT(PIECE_TYPE_JUMP);
            ok = 1;
            break;
        }
        gEditor->flags &= ~EDIT_OPEN_END;
        if (ok) {
            gEditor->cur->flags &= ~4;
            if (edit_update_fitting_pieces(gEditor->cur)) {
                cur = gEditor->cur;
                if (cur->prev == NULL) {
                    if (cur->next == NULL) {
                        gEditor->cur = edit_insert_piece(cur, NULL, gEditor->selType);
                        gEditor->cur->flags |= 4;
                        gEditor->last = gEditor->cur;
                        gEditor->flags |= EDIT_OPEN_END;
                    } else {
                        gEditor->cur = edit_insert_piece(NULL, cur, gEditor->selType);
                        gEditor->cur->flags |= 4;
                        gEditor->first = gEditor->cur;
                    }
                } else {
                    gEditor->cur = edit_insert_piece(cur, NULL, gEditor->selType);
                    gEditor->cur->flags |= 4;
                    gEditor->last = gEditor->cur;
                }
            } else {
                edit_try_close_loop(gEditor->cur);
                gEditor->mode = EDIT_MODE_IDLE;
                gEditor->flags |= EDIT_MESSAGE;
                if (gEditor->first->prev != NULL) {
                    gEditor->message = 2;
                    sEditTrackMenu.choice = 7;
                    sEditTrackMenu.cursor = 7;
                } else {
                    gEditor->message = 3;
                }
                gEditor->menu->flags &= ~GUI_MENU_ACTIVE;
            }
        }
    } else {
        switch (sEditTrackMenu.choice) {
        case 3:
            edit_remove_preview_pieces_2();
            gEditor->userPiece = IDX;
            INSERT(PIECE_TYPE_USER);
            gEditor->cur->flags |= 4;
            break;
        case 0:
            edit_remove_preview_pieces_2();
            INSERT(PIECE_TYPE_STRAIGHT);
            gEditor->cur->flags |= 4;
            break;
        case 1:
            edit_remove_preview_pieces_2();
            INSERT(PIECE_TYPE_TURN);
            gEditor->cur->flags |= 4;
            break;
        case 2:
            edit_remove_preview_pieces_2();
            INSERT(PIECE_TYPE_JUMP);
            gEditor->cur->flags |= 4;
            break;
        }
    }
    edit_update_piece_ports(gEditor->cur);
}

void edit_remove_preview_pieces(EditPiece *piece) {
    EditPiece *n;
    EditPiece *cur;
    s32 f;

    n = gEditor->cur;
    while (n != 0 && (n->flags & 4) != 0) {
        n = n->prev;
    }
    f = 1;
    if (n == 0) {
        n = gEditor->cur;
        while (n != 0) {
            if ((n->flags & 4) == 0) {
                break;
            }
            n = n->next;
        }
        f = 0;
    }
    gEditor->cur = n;
    if (n == 0) {
        n = gEditor->first;
        f = 1;
        gEditor->first = 0;
        gEditor->last = 0;
    } else if (f != 0) {
        gEditor->last = n;
    } else {
        gEditor->first = n;
    }
    while (n != 0) {
        cur = n;
        if (f != 0) {
            n = cur->next;
        } else {
            n = cur->prev;
        }
        if ((cur->flags & 4) != 0) {
            edit_delete_piece(cur);
        }
    }
}

void edit_try_close_loop(EditPiece *n)
{
    EditPiece *c;

    for (c = gEditor->first; c != NULL; c = c->next) {
        if (c->prev == gEditor->last) {
            return;
        }
        if (c->next != NULL && c->prev != NULL) {
            continue;
        }
        if (c == n) {
            continue;
        }
        if (c->prev == NULL) {
            if (n->next == NULL && ((n->dir[1] - c->dir[0]) & 3) == 2) {
                switch (n->dir[1]) {
                case 0:
                    if (n->y + n->h == c->y && n->x + n->off[1] == c->x + c->off[0]) {
                        n->next = c;
                        c->prev = n;
                        return;
                    }
                    break;
                case 2:
                    if (c->y + c->h == n->y && n->x + n->off[1] == c->x + c->off[0]) {
                        n->next = c;
                        c->prev = n;
                        return;
                    }
                    break;
                case 1:
                    if (n->x + n->w == c->x && n->y + n->off[1] == c->y + c->off[0]) {
                        n->next = c;
                        c->prev = n;
                        return;
                    }
                    break;
                case 3:
                    if (n->x == c->x + c->w && n->y + n->off[1] == c->y + c->off[0]) {
                        n->next = c;
                        c->prev = n;
                        return;
                    }
                    break;
                }
            }
        } else {
            if (n->prev == NULL && ((n->dir[0] - c->dir[1]) & 3) == 2) {
                switch (n->dir[0]) {
                case 0:
                    if (n->y + n->h == c->y && n->x + n->off[0] == c->x + c->off[1]) {
                        n->prev = c;
                        c->next = n;
                        return;
                    }
                    break;
                case 2:
                    if (c->y + c->h == n->y && n->x + n->off[0] == c->x + c->off[1]) {
                        n->prev = c;
                        c->next = n;
                        return;
                    }
                    break;
                case 1:
                    if (n->x + n->w == c->x && n->y + n->off[0] == c->y + c->off[1]) {
                        n->prev = c;
                        c->next = n;
                        return;
                    }
                    break;
                case 3:
                    if (n->x == c->x + c->w && n->y + n->off[0] == c->y + c->off[1]) {
                        n->prev = c;
                        c->next = n;
                        return;
                    }
                    break;
                }
            }
        }
    }
}

#define STKY(v) (((v) & 0xC) ? (((v) & 8) ? (((s32)(v) >> 24) & 0xF) : -(((s32)(v) >> 24) & 0xF)) : 0)
#define STKX(v) (((v) & 3) ? (((v) & 1) ? (s32)((v) >> 28) : -(s32)((v) >> 28)) : 0)
#define ABS(x) ((x) > 0 ? (x) : -(x))

#define CALC1(F, R, A, B, C, D)                                                          \
    {                                                                                    \
        f32 x = gEditor->F;                                                           \
        s32 t = (s32)((x - A) * B / C + D);                                              \
        gEditor->R = t;                                                               \
        fs = (f32)t * (f32)t + x * x;                                                    \
    }

#define CALC2(R, O)                                                                          {                                                                                            s32 s = (s32)__builtin_sqrtf(fs);                                                        gEditor->O = (u16)math_asini(((gEditor->R << 14) / s) & 0x3FFF);            }

void edit_update(void) {
    Editor *g;
    EditPiece *p;
    f32 fs;

    g = gEditor;
    if (g->lastCur != g->cur) {
        g->prevCur = g->lastCur;
        g->lastCur = g->cur;
    }
    if ((gPadInput & 4) && ABS(STKY(gPadInput)) >= 3) {
        gPadInput |= 0x2000;
    }
    if ((gPadInput & 8) && ABS(STKY(gPadInput)) >= 3) {
        gPadInput |= 0x4000;
    }
    if ((gPadInput & 1) && ABS(STKX(gPadInput)) >= 3) {
        gPadInput |= 0x800;
    }
    if ((gPadInput & 2) && ABS(STKX(gPadInput)) >= 3) {
        gPadInput |= 0x1000;
    }
    if ((gPadInput & 0x10) && !(gPadInputPrev & 0x10)) {
        gEditor->flags |= EDIT_A_HELD;
    } else if (gEditor->flags & EDIT_A_HELD) {
        if (((gPadInput & 0x20) && !(gPadInputPrev & 0x20)) || !(gPadInput & 0x10)) {
            gEditor->flags &= ~EDIT_A_HELD;
        }
    }
    if (gEditor->camZoomFrames != 0) {
        gEditor->cam[1].dist += gEditor->camZoomStep / 50;
        gEditor->camZoomFrames--;
        CALC1(cam[1].dist, cam[1].height, 20000.0f, 32000.0f, 100000.0f, 8000.0f);
        CALC2(cam[1].height, cam[1].pitch);
    } else if (gEditor->cur != 0) {
        if (gPadInput & 0x80) {
            if (gEditor->mode == EDIT_MODE_USER_PIECE) {
                gEditor->cam[1].dist -= 600;
                if (gEditor->cam[1].dist < 20000) {
                    gEditor->cam[1].dist = 20000;
                }
                CALC1(cam[1].dist, cam[1].height, 20000.0f, 32000.0f, 100000.0f, 8000.0f);
                CALC2(cam[1].height, cam[1].pitch);
            } else {
                gEditor->cam[0].dist -= 600;
                if (gEditor->cam[0].dist < 20000) {
                    gEditor->cam[0].dist = 20000;
                }
                CALC1(cam[0].dist, cam[0].height, 20000.0f, 38000.0f, 165000.0f, 40000.0f);
                CALC2(cam[0].height, cam[0].pitch);
            }
        }
        if (gPadInput & 0x100) {
            if (gEditor->mode == EDIT_MODE_USER_PIECE) {
                gEditor->cam[1].dist += 600;
                if (gEditor->cam[1].dist > 120000) {
                    gEditor->cam[1].dist = 120000;
                }
                CALC1(cam[1].dist, cam[1].height, 20000.0f, 32000.0f, 100000.0f, 8000.0f);
                CALC2(cam[1].height, cam[1].pitch);
            } else {
                gEditor->cam[0].dist += 600;
                if (gEditor->cam[0].dist > 185000) {
                    gEditor->cam[0].dist = 185000;
                }
                CALC1(cam[0].dist, cam[0].height, 20000.0f, 38000.0f, 165000.0f, 40000.0f);
                CALC2(cam[0].height, cam[0].pitch);
            }
        }
    }
    if (gEditor->cur != 0) {
        if ((gPadInput & 0x200) && !(gPadInputPrev & 0x200)) {
            if (gEditor->flags & EDIT_TURN_LEFT) {
                gEditor->flags &= ~EDIT_TURN_LEFT;
            } else if (gEditor->flags & EDIT_TURN_RIGHT) {
                gEditor->flags &= ~EDIT_TURN_RIGHT;
            } else {
                gEditor->flags |= EDIT_TURN_LEFT;
            }
        }
        if ((gPadInput & 0x400) && !(gPadInputPrev & 0x400)) {
            if (gEditor->flags & EDIT_TURN_RIGHT) {
                gEditor->flags &= ~EDIT_TURN_RIGHT;
            } else if (gEditor->flags & EDIT_TURN_LEFT) {
                gEditor->flags &= ~EDIT_TURN_LEFT;
            } else {
                gEditor->flags |= EDIT_TURN_RIGHT;
            }
        }
    }
    sEditTrackMenuItems[3].numValues = gEditor->userPieceCount;
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
        sEditMainMenuItems[0].flags &= ~GUI_ITEM_ENABLED;
        sEditMainMenuItems[6].flags &= ~GUI_ITEM_ENABLED;
    } else {
        sEditMainMenuItems[0].flags |= GUI_ITEM_ENABLED;
        sEditMainMenuItems[6].flags |= GUI_ITEM_ENABLED;
    }
    if (gEditor->cur != NULL && (gEditor->cur->type->flags & PIECE_START) && gEditor->start != NULL) {
        sEditTrackMenuItems[5].flags |= GUI_ITEM_ENABLED;
    } else {
        sEditTrackMenuItems[5].flags &= ~GUI_ITEM_ENABLED;
    }
    if (gEditor->start != 0) {
        sEditMainMenuItems[4].flags |= GUI_ITEM_ENABLED;
        sEditMainMenuItems[5].flags |= GUI_ITEM_ENABLED;
    } else {
        sEditMainMenuItems[4].flags &= ~GUI_ITEM_ENABLED;
        sEditMainMenuItems[5].flags &= ~GUI_ITEM_ENABLED;
    }
    if (gEditor->start != 0 && gEditor->first->prev == gEditor->last) {
        sEditMainMenuItems[3].flags |= GUI_ITEM_ENABLED;
    } else {
        sEditMainMenuItems[3].flags &= ~GUI_ITEM_ENABLED;
    }
    if (gEditor->menu != NULL) {
        gui_menu_fix_cursor(gEditor->menu);
    }
    if (gEditor->slideState == 3 && gEditor->nextMenu != NULL) {
        gEditor->slideState = 1;
        gEditor->menu = gEditor->nextMenu;
        gEditor->nextMenu = NULL;
    }
    switch (sEditTrackMenu.choice) {
    case 0:
        gEditor->selType = &gPieceTypes[sEditTrackMenu.items[0].value];
        break;
    case 1:
        gEditor->selType = &gPieceTypes[PIECE_TYPE_TURN + sEditTrackMenu.items[1].value];
        break;
    case 2:
        gEditor->selType = &gPieceTypes[PIECE_TYPE_JUMP + sEditTrackMenu.items[2].value];
        break;
    case 3:
        gEditor->selType = &gPieceTypes[PIECE_TYPE_USER + sEditTrackMenu.items[3].value];
        break;
    }
    if (gEditor->flags & EDIT_EXIT) {
        if (++gEditor->exitTimer >= 0) {
            if (gEditor->flags & EDIT_TEST_TRACK) {
                usrtrk_layout_from_editor();
            }
            gCurGameState->mode = 3;
        }
    } else {
        switch (gEditor->page) {
        case 0:
            edit_update_main_menu();
            break;
        case 1:
            edit_update_track_menu();
            break;
        }
    }
    p = gEditor->first;
    if (p != NULL) {
        do {
            edit_update_piece_ports(p);
            p = p->next;
        } while (p != NULL && p != gEditor->first);
    }
}

#define RAW(g, a, b) { Gfx *_g = (Gfx *)(g); _g->words.w0 = (a); _g->words.w1 = (b); }

Gfx *edit_draw_name_entry(Gfx *gdl, s32 arg1)
{
    s32 u, j, m, i, k, y0, xo, t, yoff, yy, idx, x, c, s, f;
    u8 *p;
    Editor *mp;
    u8 buf[8];

    RAW(gdl++, 0xE3000C00, 0);
    RAW(gdl++, 0xFA000000, 0xFAFAFAFF);
    RAW(gdl++, 0xFC119623, 0xFF2FFFFF);
    RAW(gdl++, 0xE3001001, 0);
    RAW(gdl++, 0xE200001C, 0x00404240);
    RAW(gdl++, 0xD7000002, 0x80008000);
    p = (u8 *)gGui->images + 0x48;
    gdl = render_draw_image(0xBC, 0x78 - (p[1] >> 1), p, gdl, arg1, 1, 1, 6, 0);
    font_text_create(gEditor->bigText, 0x40, 2, 2, 0x20);
    font_text_add(gEditor->bigText, sStrTrackNameLabel);
    font_draw_text(gEditor->bigText, 0xCC, 0xA7);
    c = -0xF2;
    for (i = 0; i < 5; i++) {
        for (k = 0, idx = i << 3; k < 8; k++) {
            mp = gEditor;
            s = 1;
            if (idx >= 0x26) s = 2;
            f = 2;
            if (idx == mp->nameKey) f = 7;
            if (mp->nameLen == 10 && idx < 0x26) font_text_create(mp->bigText, 0x20, s, f, 0x20000021);
            else font_text_create(mp->bigText, 0x20, s, f, 0x21);
            font_text_add(gEditor->bigText, gKeyboardKeys[idx]);
            x = k * 26 - (i * 5 + c);
            font_draw_text(gEditor->bigText, x, i * 17 + 0x48);
            if (idx == gEditor->nameKey) {
                p = (u8 *)gGui->images + 0x50;
                gdl = render_draw_image(x - p[0] * 4 + 4, i * 17 - (p[1] >> 1) + 0x49, p, gdl, arg1, 1, 1, 6, 0);
                do { } while (0);
                idx++;
            } else {
                idx++;
            }
        }
    }
    for (j = 0; j < 10; j++) {
        if (gEditor->nameBuf[j] == 0) break;
        f = 2;
        if (j == gEditor->nameLen) f = 7;
        font_text_create(gEditor->bigText, 0x20, 2, f, 0x21);
        buf[0] = gEditor->nameBuf[j];
        buf[1] = 0;
        font_text_add(gEditor->bigText, buf);
        font_draw_text(gEditor->bigText, j * 13 + 0x121, 0xA5);
    }
    for (j = 0; j < 10; j++) {
        f = 2;
        if (j == gEditor->nameLen) f = 7;
        font_text_create(gEditor->bigText, 0x20, 1, f, 0x21);
        font_text_add(gEditor->bigText, sStrNameUnderline);
        font_draw_text(gEditor->bigText, j * 13 + 0x120, 0xAB);
    }
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPPipeSync(gdl++);
    return gdl;
}

void edit_draw(s32 arg0, s32 arg1, Gfx **pgdl)
{
    char buf[0x68];
    char *s;
    Gfx *gdl;
    s32 i;
    u8 *p;
    u8 *img;
    char *str;
    Editor *m;
    PieceType *t;
    PieceType *e;

    sEditDrawCtx = arg0;
    switch (gEditor->slideState) {
    case 1:
        gEditor->slideOffset = 0;
        break;
    case 2:
        gEditor->slideOffset = 30 - gEditor->slideTimer;
        break;
    case 0:
        gEditor->slideOffset = gEditor->slideTimer;
        break;
    case 3:
        gEditor->slideOffset = 30;
        break;
    }
    gEditor->slideOffset *= 4;
    sEditViewRects[1].x = sEditViewRectsBase[1].x - gEditor->slideOffset;
    sEditViewRects[1].w = sEditViewRectsBase[1].w + gEditor->slideOffset;
    sEditViewRects[2].x = sEditViewRectsBase[2].x - gEditor->slideOffset;
    sEditViewRects[3].x = sEditViewRectsBase[3].x - gEditor->slideOffset;
    arned_set_viewport_stretch(sEditViewRects[1].x, sEditViewRects[1].y, sEditViewRects[1].w - 1, sEditViewRects[1].h - 1, gEditor->slideOffset);
    gEditor->views[1].vp.vscale[0] = sEditViewRectsBase[1].w * 2;
    gEditor->views[1].vp.vscale[1] = sEditViewRectsBase[1].h * 2;
    gEditor->views[1].vp.vtrans[0] = (sEditViewRectsBase[1].x + sEditViewRectsBase[1].w / 2) * 4;
    gEditor->views[1].vp.vtrans[1] = (sEditViewRectsBase[1].y + sEditViewRectsBase[1].h / 2) * 4;
    gEditor->views[2].vp.vscale[0] = sEditViewRects[2].w * 2;
    gEditor->views[2].vp.vscale[1] = sEditViewRects[2].h * 2;
    gEditor->views[2].vp.vtrans[0] = (sEditViewRects[2].x + sEditViewRects[2].w / 2) * 4;
    gEditor->views[2].vp.vtrans[1] = (sEditViewRects[2].y + sEditViewRects[2].h / 2) * 4;
    gEditor->views[3].vp.vscale[0] = sEditViewRects[3].w * 2;
    gEditor->views[3].vp.vscale[1] = sEditViewRects[3].h * 2;
    gEditor->views[3].vp.vtrans[0] = (sEditViewRects[3].x + sEditViewRects[3].w / 2) * 4;
    gEditor->views[3].vp.vtrans[1] = (sEditViewRects[3].y + sEditViewRects[3].h / 2) * 4;

    gdl = *pgdl;
    gdl = render_load_ucode(gdl, arg1, arg0, gF3dex2NonText, gF3dex2NonData);
    gdl = render_restore_state(gdl, arg1, arg0);
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
    gDPSetFillColor(gdl++, 0xFFFCFFFC);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gFramebuffers[arg1]);
    gDPSetFillColor(gdl++, 0);
    gDPFillRectangle(gdl++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    gDPPipeSync(gdl++);
    gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, gScreenWidth, gFramebuffers[arg1]);
    gDPSetDepthSource(gdl++, G_ZS_PIXEL);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, sEditViewRects[0].x, sEditViewRects[0].y,
                  sEditViewRects[0].x + sEditViewRects[0].w - 1, sEditViewRects[0].y + sEditViewRects[0].h - 1);
    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_FILL);
    gDPPipeSync(gdl++);
    gDPSetFillColor(gdl++, 0x00010001);
    gDPFillRectangle(gdl++, sEditViewRects[2].x, sEditViewRects[2].y,
                     sEditViewRects[2].x + sEditViewRects[2].w - 1, sEditViewRects[2].y + sEditViewRects[2].h - 1);
    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gdl = gui_draw_menu(arg1, gdl, gEditor->menu, 0x1A, 0x11);
    gdl = edit_draw_track_view(arg0, arg1, gdl);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetPrimColor(gdl++, 0, 0, 0xB4, 0xB4, 0xB4, 0xFF);

    switch ((u32)gEditor->mode) { /* compared unsigned here */
    case 10:
    case 11:
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
        gdl = render_draw_image(0xD0, 0x3D, img, gdl, arg1, 1, 1, 3, 0);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        gdl = render_draw_image(0xB6, 0x3A, gEditor->warnFrame->img, gdl, arg1, 1, 1, 6, 0);
        break;
    }

    if (gEditor->cur != NULL) {
        gDPPipeSync(gdl++);
        gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(gdl++, G_TT_NONE);
        gDPSetCombine(gdl++, 0x119623, 0xFF2FFFFF);
        p = (u8 *)gGui->images + 0x58;
        font_text_create(gEditor->text, 0x20, 5, 2, 0x21);
        font_text_add(gEditor->text, sStrTrackNameLabel);
        font_text_add(gEditor->text, gEditor->name);
        font_draw_text(gEditor->text, 0x140, 0x19);
        for (i = 0; i < (0x207 - sEditViewRects[1].x) / (p[0] * 8); i++) {
            gdl = render_draw_image(sEditViewRects[1].x + p[0] * (i << 3), 0x11, p, gdl, arg1, 1, 1, 6, 0);
        }
        if (gEditor->mode == EDIT_MODE_PLACE && (gFrameCounter & 0x20)) {
            gDPSetPrimColor(gdl++, 0, 0, 0xB4, 0xB4, 0x50, 0xFF);
        }
        for (i = 0; i < (0x207 - sEditViewRects[1].x) / (p[0] * 8); i++) {
            gdl = render_draw_image(sEditViewRects[1].x + p[0] * (i << 3), 0xCD, p, gdl, arg1, 1, 1, 6, 0);
        }
        gDPPipeSync(gdl++);
        font_text_create(gEditor->bigText, 0x40, 5, 2, 0x21);
        i = gEditor->cur->type - gPieceTypes;
        if (gEditor->mode == EDIT_MODE_PLACE && gEditor->cur != NULL &&
            (gEditor->cur->next != 0 || gEditor->cur->prev != 0)) {
            font_text_add(gEditor->bigText, "< ");
        }
        if (i < 2) {
            font_text_add(gEditor->bigText, "Straight ");
            i += 1;
        } else if (i < 6) {
            font_text_add(gEditor->bigText, "Turn ");
            i -= 1;
        } else if (i < 18) {
            font_text_add(gEditor->bigText, "Jump ");
            i -= 5;
        } else {
            i -= 17;
        }
        if (gEditor->cur->type - gPieceTypes < 18) {
            s = buf + 0x48;
            int_to_str(i, s);
            font_text_add(gEditor->bigText, s);
            font_text_add(gEditor->bigText, " : ");
        }
        font_text_add(gEditor->bigText, gEditor->cur->type->name);
        if (gEditor->cur->type - gPieceTypes >= 18) {
            if (gEditor->mode == EDIT_MODE_PLACE) {
                font_text_add(gEditor->bigText, " - Press Z Button To Edit");
            } else if (gEditor->mode == EDIT_MODE_USER_PIECE && gPieceTypes[gEditor->userPiece + 18].dim[1] >= 5 &&
                       !(gPieceTypes[gEditor->userPiece + 18].flags & PIECE_SIZE_LOCKED) &&
                       (gEditor->heightSel == 1 || gEditor->heightSel == gPieceTypes[gEditor->userPiece + 18].dim[1] * 2 - 1)) {
                font_text_add(gEditor->bigText, " - R Button Deletes Segment");
            }
        } else if (gEditor->mode == EDIT_MODE_PLACE && (*(u32 *)&gEditor->cur->type->flags & 0x300000) == 0x300000) {
            font_text_add(gEditor->bigText, " - R Button Flips");
        }
        if (gEditor->mode == EDIT_MODE_PLACE && gEditor->cur != NULL &&
            (gEditor->cur->next != 0 || gEditor->cur->prev != 0)) {
            font_text_add(gEditor->bigText, " >");
        }
        font_draw_text(gEditor->bigText, 0x142, 0xD5);
    }
    if (gEditor->mode == EDIT_MODE_NAME || gEditor->mode == EDIT_MODE_NAME_8) {
        gdl = edit_draw_name_entry(gdl, arg1);
    }
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, sEditViewRects[0].x, sEditViewRects[0].y,
                  sEditViewRects[0].x + sEditViewRects[0].w - 1, sEditViewRects[0].y + sEditViewRects[0].h - 1);
    gdl = font_render(gdl);
    gDPPipeSync(gdl++);
    *pgdl = gdl;
}

void edit_update_piece_ports(EditPiece *r)
{
    s32 a, b;

    if (r == NULL) return;
    if (r->flags & 2) {
        b = 0;
        a = 1;
    } else {
        a = 0;
        b = 1;
    }
    r->dir[0] = (r->type->dir[a] + r->rot) & 3;
    r->dir[1] = (r->type->dir[b] + r->rot) & 3;

    switch (r->dir[0]) {
    case 0:
        switch (r->type->dir[a]) {
        case 0: case 3: r->off[0] = r->type->off[a]; break;
        case 2: r->off[0] = r->type->dim[0] - r->type->off[a] - 2; break;
        case 1: r->off[0] = r->type->dim[1] - r->type->off[a] - 2; break;
        }
        break;
    case 2:
        switch (r->type->dir[a]) {
        case 1: case 2: r->off[0] = r->type->off[a]; break;
        case 0: r->off[0] = r->type->dim[0] - r->type->off[a] - 2; break;
        case 3: r->off[0] = r->type->dim[1] - r->type->off[a] - 2; break;
        }
        break;
    case 3:
        switch (r->type->dir[a]) {
        case 0: case 3: r->off[0] = r->type->off[a]; break;
        case 2: r->off[0] = r->type->dim[0] - r->type->off[a] - 2; break;
        case 1: r->off[0] = r->type->dim[1] - r->type->off[a] - 2; break;
        }
        break;
    case 1:
        switch (r->type->dir[a]) {
        case 1: case 2: r->off[0] = r->type->off[a]; break;
        case 0: r->off[0] = r->type->dim[0] - r->type->off[a] - 2; break;
        case 3: r->off[0] = r->type->dim[1] - r->type->off[a] - 2; break;
        }
        break;
    }

    switch (r->dir[1]) {
    case 0:
        switch (r->type->dir[b]) {
        case 0: case 3: r->off[1] = r->type->off[b]; break;
        case 2: r->off[1] = r->type->dim[0] - r->type->off[b] - 2; break;
        case 1: r->off[1] = r->type->dim[1] - r->type->off[b] - 2; break;
        }
        break;
    case 2:
        switch (r->type->dir[b]) {
        case 1: case 2: r->off[1] = r->type->off[b]; break;
        case 0: r->off[1] = r->type->dim[0] - r->type->off[b] - 2; break;
        case 3: r->off[1] = r->type->dim[1] - r->type->off[b] - 2; break;
        }
        break;
    case 3:
        switch (r->type->dir[b]) {
        case 0: case 3: r->off[1] = r->type->off[b]; break;
        case 2: r->off[1] = r->type->dim[0] - r->type->off[b] - 2; break;
        case 1: r->off[1] = r->type->dim[1] - r->type->off[b] - 2; break;
        }
        break;
    case 1:
        switch (r->type->dir[b]) {
        case 1: case 2: r->off[1] = r->type->off[b]; break;
        case 0: r->off[1] = r->type->dim[0] - r->type->off[b] - 2; break;
        case 3: r->off[1] = r->type->dim[1] - r->type->off[b] - 2; break;
        }
        break;
    }

    if (r->rot & 1) {
        r->w = r->type->dim[1];
        r->h = r->type->dim[0];
    } else {
        r->w = r->type->dim[0];
        r->h = r->type->dim[1];
    }
}

void edit_add_overlay_line(s32 idx, s32 x0, s32 y0, s16 z0, s32 x1, s32 y1, s32 z1, u8 r, u8 g, u8 b)
{
    s32 ax, ay, ox, oy;

    x0 = x0 - gScreenWidth / 2;
    x1 = x1 - gScreenWidth / 2;
    y0 = gScreenHeight / 2 - y0;
    y1 = -y1;
    y1 += gScreenHeight / 2;
    if (gEditor->lineCount >= 0x3C) {
        return;
    }
    ax = x1 - x0;
    if (ax <= 0) ax = x0 - x1;
    ay = y1 - y0;
    if ((ay <= 0 ? y0 - y1 : ay) < ax) {
        ox = 0;
        oy = 2;
    } else {
        ox = 3;
        oy = 0;
    }
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 0].v.ob[0] = x0;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 0].v.ob[1] = y0;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 0].v.ob[2] = z0;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 1].v.ob[0] = x0 + ox;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 1].v.ob[1] = y0 + oy;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 1].v.ob[2] = z0;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 2].v.ob[0] = x1;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 2].v.ob[1] = y1;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 2].v.ob[2] = z1;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 3].v.ob[0] = x1 + ox;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 3].v.ob[1] = y1 + oy;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 3].v.ob[2] = z1;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 0].v.cn[0] = r;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 0].v.cn[1] = g;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 0].v.cn[2] = b;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 1].v.cn[0] = r;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 1].v.cn[1] = g;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 1].v.cn[2] = b;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 2].v.cn[0] = r;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 2].v.cn[1] = g;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 2].v.cn[2] = b;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 3].v.cn[0] = r;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 3].v.cn[1] = g;
    gEditor->lineVtx[idx][gEditor->lineCount * 4 + 3].v.cn[2] = b;
    gEditor->lineCount++;
}

Gfx *edit_draw_overlay_lines(u8 *a0, s32 arg1, Gfx *gdl)
{
    Mtx *m;
    s32 i;

    m = (Mtx *)(a0 + 0x300);
    guOrtho(m, -gScreenWidth / 2, gScreenWidth / 2, -gScreenHeight / 2, gScreenHeight / 2, 50.0f,
                  gFarClip, 1.0f);
    gSPMatrix(gdl++, &gIdentityMtx, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gdl++, m, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gdl++);
    gSPViewport(gdl++, &gScreenViewport);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, sEditViewRects[1].x, sEditViewRects[1].y,
                  sEditViewRects[1].x + sEditViewRects[1].w - 1, sEditViewRects[1].y + sEditViewRects[1].h - 1);
    gSPClearGeometryMode(gdl++, 0x3F0604);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPSetGeometryMode(gdl++, G_SHADE);
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    gDPSetCombineMode(gdl++, G_CC_SHADE, G_CC_SHADE);
    gDPSetRenderMode(gdl++, 0x00442078, 0);
    for (i = 0; i < gEditor->lineCount; i++) {
        gSPVertex(gdl++, gEditor->lineVtx[arg1] + i * 4, 4, 0);
        gDPPipeSync(gdl++);
        gSP2Triangles(gdl++, 0, 1, 3, 0, 0, 3, 2, 0);
        gDPPipeSync(gdl++);
    }
    return gdl;
}

s32 edit_project_to_screen(s32 x, s32 y, s32 z, s32 *sx, s32 *sy, s32 *sz, s32 *sw, s32 arg7, s32 arg8) {
    f32 hx;
    f32 hy;
    f32 tx;
    f32 ty;
    f32 px;
    f32 py;
    f32 pz;
    f32 pw;

    hx = gArnedViewport_2.vscale[0] * 0.25f;
    hy = gArnedViewport_2.vscale[1] * 0.25f;
    x *= 0.0625f;
    y *= 0.0625f;
    z *= 0.0625f;
    px = gEditor->camMtx[0][0] * x + gEditor->camMtx[1][0] * y + gEditor->camMtx[2][0] * z + gEditor->camMtx[3][0];
    py = gEditor->camMtx[0][1] * x + gEditor->camMtx[1][1] * y + gEditor->camMtx[2][1] * z + gEditor->camMtx[3][1];
    pz = gEditor->camMtx[0][2] * x + gEditor->camMtx[1][2] * y + gEditor->camMtx[2][2] * z + gEditor->camMtx[3][2];
    pw = gEditor->camMtx[0][3] * x + gEditor->camMtx[1][3] * y + gEditor->camMtx[2][3] * z + gEditor->camMtx[3][3];
    tx = gArnedViewport_2.vtrans[0] * 0.25f;
    ty = gArnedViewport_2.vtrans[1] * 0.25f;
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
        rx = px * hx + tx;
        ry = py * -hy + ty;
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

#define gDPRaw(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }
#define SINT(i) ((s16)gCosTable[(i) & 0xFFF] >> 4)
/* The mesh sets advanced by k cells: ->cells[0] is cells[k] */
#define CellAt(d, k) ((PieceMeshSet *)((u8 *)(d)->meshA + ((k) << 5)))
#define EntAt(d, k) ((PieceMeshSet *)((u8 *)(d)->meshB + ((k) << 5)))
#define W (o->type->dim[0])
#define H (o->type->dim[1])
#define K0 (j * W + i)
#define K1 (i * W + W - 1 - j)
#define K2 ((H - 1 - j) * W + W - 1 - i)
#define K3 ((H - 1 - i) * W + j)
#define XB(a, b) (gEditor->cur->x * 8000 - gEditor->gridSize[a] * 4000)
#define ZB(a) (gEditor->cur->y * 8000 - gEditor->gridSize[a] * 4000)

Gfx *edit_draw_track_view(s32 arg0, s32 arg1, Gfx *gdl) {
    s32 p0[3];
    s32 p1[3];
    s32 p2[3];
    s32 p3[3];
    EditPiece *o;
    s32 flag;
    s32 sel;
    s32 i;
    s32 j;
    s32 k;
    s32 n;
    s32 x;
    s32 y;
    s16 rot;
    s32 fl;
    u32 a;
    s32 r;

    gDPPipeSync(gdl++);
    gDPSetCycleType(gdl++, G_CYC_1CYCLE);
    gDPPipeSync(gdl++);
    gDPSetScissor(gdl++, G_SC_NON_INTERLACE, sEditViewRects[1].x, sEditViewRects[1].y, sEditViewRects[1].x + sEditViewRects[1].w - 1,
                  sEditViewRects[1].y + sEditViewRects[1].h - 1);
    gSPViewport(gdl++, &gEditor->views[1]);
    gSPClearGeometryMode(gdl, G_ZBUFFER);
    gDPRaw(gdl++, 0xE200001C, 0x0C084000);
    gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFDF6FB);
    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, 1);
    gDPFillRectangle(gdl++, sEditViewRects[1].x, sEditViewRects[1].y, sEditViewRects[1].x + sEditViewRects[1].w - 1,
                     sEditViewRects[1].y + sEditViewRects[1].h - 1);
    gDPPipeSync(gdl++);
    gDPRaw(gdl++, 0xFCFFFFFF, 0xFFFCF279);
    gDPRaw(gdl++, 0xE200001C, 0x004041C8);
    gSPClearGeometryMode(gdl++, G_ZBUFFER);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);

    if (gEditor->mode == EDIT_MODE_MOVE_TRACK) {
        arned_set_camera_2(arg1, 0, 250000, 0, 0, 0x4000);
    } else if (gEditor->mode == EDIT_MODE_USER_PIECE) {
        a = sEditCamPos.yaw;
        arned_set_camera_2(arg1, sEditCamPos.x + ((SINT((a >> 4) - 0x400) * gEditor->cam[1].dist) >> 10), gEditor->cam[1].height,
                      sEditCamPos.z - ((SINT(a >> 4) * gEditor->cam[1].dist) >> 10), a, gEditor->cam[1].pitch);
    } else {
        a = sEditCamPos.yaw;
        arned_set_camera_2(arg1, sEditCamPos.x + ((SINT((a >> 4) - 0x400) * gEditor->cam[0].dist) >> 10), gEditor->cam[0].height,
                      sEditCamPos.z - ((SINT(a >> 4) * gEditor->cam[0].dist) >> 10), a, gEditor->cam[0].pitch);
    }

    o = gEditor->first;
    if (o != NULL) {
        do {
            if (gEditor->cur != NULL && gEditor->mode == EDIT_MODE_USER_PIECE && (gEditor->cur->type->flags & PIECE_USER)) {
                fl = o->flags & ~8;
            } else {
                fl = o->flags | 8;
            }
            o->flags = fl;
            o = o->next;
        } while (o != NULL && o != gEditor->first);
    }
    if (gEditor->cur != NULL && gEditor->mode == EDIT_MODE_USER_PIECE && (gEditor->cur->type->flags & PIECE_USER)) {
        for (o = gEditor->cur->next; o != NULL; o = o->next) {
            o->flags |= 8;
            if (o->type->flags & PIECE_CORNER) {
                break;
            }
        }
        for (o = gEditor->cur; o != NULL; o = o->prev) {
            o->flags |= 8;
            if (o->type->flags & PIECE_CORNER) {
                break;
            }
        }
    }

    o = gEditor->first;
    flag = 0;
    if (o != NULL) {
        do {
            if (gEditor->markStart != NULL && gEditor->markEnd != NULL) {
                if (flag != 0 && o->prev == gEditor->markEnd) {
                    flag = 0;
                }
                if (o == gEditor->markStart) {
                    flag = 1;
                }
            }
            sel = 0;
            if ((o->flags & 4) || o == gEditor->cur) {
                sel = 1;
            }
            sel |= 2;
            for (j = 0; j < o->type->dim[(o->rot + 1) & 1]; j++) {
                for (i = 0; i < o->type->dim[(rot = o->rot) & 1]; i++) {
                    if (o->x + i < 0 || o->x + i >= gEditor->gridSize[0]) {
                        continue;
                    }
                    if (o->y + j < 0 || o->y + j >= gEditor->gridSize[1]) {
                        continue;
                    }
                    if (o->flags & 8) {
                        switch (rot) {
                        case 0:
                            arned_set_cell(o->x + i, o->y + j, CellAt(o->type, K0)->cells[0].mesh, rot,
                                          o->type->meshB ? EntAt(o->type, K0)->cells[0].mesh : 0, o->rot, sel,
                                          o->type->meshB ? (EntAt(o->type, K0)->cells[0].x0 + EntAt(o->type, K0)->cells[0].x1) / 2 - 250 - i * 500 : 0,
                                          o->type->meshB ? (EntAt(o->type, K0)->cells[0].y0 + EntAt(o->type, K0)->cells[0].y1) / 2 - 250 - j * 500 : 0);
                            break;
                        case 1:
                            arned_set_cell(o->x + i, o->y + j, CellAt(o->type, K1)->cells[0].mesh, rot,
                                          o->type->meshB ? EntAt(o->type, K1)->cells[0].mesh : 0, o->rot, sel,
                                          o->type->meshB ? (EntAt(o->type, K1)->cells[0].y0 + EntAt(o->type, K1)->cells[0].y1) / 2 - 250 - i * 500 : 0,
                                          o->type->meshB ? (o->h - j - 1) * 500 + (250 - (EntAt(o->type, K1)->cells[0].x0 + EntAt(o->type, K1)->cells[0].x1) / 2) : 0);
                            break;
                        case 2:
                            arned_set_cell(o->x + i, o->y + j, CellAt(o->type, K2)->cells[0].mesh, rot,
                                          o->type->meshB ? EntAt(o->type, K2)->cells[0].mesh : 0, o->rot, sel,
                                          o->type->meshB ? (o->w - i - 1) * 500 + (250 - (EntAt(o->type, K2)->cells[0].x0 + EntAt(o->type, K2)->cells[0].x1) / 2) : 0,
                                          o->type->meshB ? (o->h - j - 1) * 500 + (250 - (EntAt(o->type, K2)->cells[0].y0 + EntAt(o->type, K2)->cells[0].y1) / 2) : 0);
                            break;
                        case 3:
                            arned_set_cell(o->x + i, o->y + j, CellAt(o->type, K3)->cells[0].mesh, rot,
                                          o->type->meshB ? EntAt(o->type, K3)->cells[0].mesh : 0, o->rot, sel,
                                          o->type->meshB ? (o->w - i - 1) * 500 + (250 - (EntAt(o->type, K3)->cells[0].y0 + EntAt(o->type, K3)->cells[0].y1) / 2) : 0,
                                          o->type->meshB ? (EntAt(o->type, K3)->cells[0].x0 + EntAt(o->type, K3)->cells[0].x1) / 2 - 250 - j * 500 : 0);
                            break;
                        }
                    } else {
                        arned_set_cell(o->x + i, o->y + j, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
            o = o->next;
        } while (o != NULL && o != gEditor->first);
    }

    arned_set_scissor(0, 0, gScreenWidth, gScreenHeight);
    if (gEditor->start != NULL) {
        r = ((-gEditor->start->rot * 2 - gEditor->start->type->cursorAngle) & 7) << 13;
        gdl = arned_draw(arg1, gdl, gEditor->cur->x, gEditor->cur->y, gEditor->cur->w, gEditor->cur->h,
                            (gEditor->start->x * 2 + gEditor->start->w) * 4000 - 60000, (gEditor->start->y * 2 + gEditor->start->h) * 4000 - 60000,
                            (gEditor->start->type->flags & PIECE_START) ? r : -1, 1);
        if (gEditor->mode == EDIT_MODE_USER_PIECE) {
            gEditor->lineCount = 0;
            guMtxL2F(gEditor->camMtx, &gArnedCamMtx[arg1]);
            mem_fill(gEditor->lineVtx[arg1], 0, 0xF00);
            if (gEditor->cur != NULL && (gEditor->cur->type->flags & PIECE_USER)) {
                switch (gEditor->cur->rot) {
                case 0:
                    for (k = 0; k < gEditor->cur->type->dim[1] * 2 + 1; k++) {
                        edit_project_to_screen(XB(0, 0) + 2000, k != 0 ? gUserPieceHeights[gEditor->userPiece][k - 1] * 16 : 0,
                                      ZB(1) + k * 4000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                        edit_project_to_screen(XB(0, 0) + 14000, k != 0 ? gUserPieceHeights[gEditor->userPiece][k - 1] * 16 : 0,
                                      ZB(1) + k * 4000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                        if (gEditor->heightSel == k) {
                            switch (gEditor->heightState) {
                            case 0:
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case 1:
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case 2:
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (k > 0) {
                            edit_project_to_screen(XB(0, 0) + 2000, k != 1 ? gUserPieceHeights[gEditor->userPiece][k - 2] * 16 : 0,
                                          ZB(1) + (k - 1) * 4000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                            edit_project_to_screen(XB(0, 0) + 2000, gUserPieceHeights[gEditor->userPiece][k - 1] * 16,
                                          ZB(1) + k * 4000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                            edit_project_to_screen(XB(0, 0) + 14000, k != 1 ? gUserPieceHeights[gEditor->userPiece][k - 2] * 16 : 0,
                                          ZB(1) + (k - 1) * 4000, &p2[0], &p2[1], &p2[2], NULL, arg0, arg1);
                            edit_project_to_screen(XB(0, 0) + 14000, gUserPieceHeights[gEditor->userPiece][k - 1] * 16,
                                          ZB(1) + k * 4000, &p3[0], &p3[1], &p3[2], NULL, arg0, arg1);
                            if (gEditor->heightSel == k || gEditor->heightSel == k - 1) {
                                switch (gEditor->heightState) {
                                case 0:
                                    edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case 1:
                                    edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case 2:
                                    edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                case 1:
                    for (k = 0; k < gEditor->cur->type->dim[1] * 2 + 1; k++) {
                        edit_project_to_screen(XB(1, 0) + k * 4000, k != 0 ? gUserPieceHeights[gEditor->userPiece][k - 1] * 16 : 0,
                                      ZB(0) + 2000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                        edit_project_to_screen(XB(1, 0) + k * 4000, k != 0 ? gUserPieceHeights[gEditor->userPiece][k - 1] * 16 : 0,
                                      ZB(0) + 14000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                        if (gEditor->heightSel == k) {
                            switch (gEditor->heightState) {
                            case 0:
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case 1:
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case 2:
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (k > 0) {
                            edit_project_to_screen(XB(1, 0) + (k - 1) * 4000, k != 1 ? gUserPieceHeights[gEditor->userPiece][k - 2] * 16 : 0,
                                          ZB(0) + 2000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                            edit_project_to_screen(XB(1, 0) + k * 4000, gUserPieceHeights[gEditor->userPiece][k - 1] * 16,
                                          ZB(0) + 2000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                            edit_project_to_screen(XB(1, 0) + (k - 1) * 4000, k != 1 ? gUserPieceHeights[gEditor->userPiece][k - 2] * 16 : 0,
                                          ZB(0) + 14000, &p2[0], &p2[1], &p2[2], NULL, arg0, arg1);
                            edit_project_to_screen(XB(1, 0) + k * 4000, gUserPieceHeights[gEditor->userPiece][k - 1] * 16,
                                          ZB(0) + 14000, &p3[0], &p3[1], &p3[2], NULL, arg0, arg1);
                            if (gEditor->heightSel == k || gEditor->heightSel == k - 1) {
                                switch (gEditor->heightState) {
                                case 0:
                                    edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case 1:
                                    edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case 2:
                                    edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                case 2:
                    for (k = 0; k < gEditor->cur->type->dim[1] * 2 + 1; k++) {
                        n = gEditor->cur->type->dim[1] * 2 - k;
                        edit_project_to_screen(XB(0, 0) + 2000, n != 0 ? gUserPieceHeights[gEditor->userPiece][n - 1] * 16 : 0,
                                      ZB(1) + k * 4000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                        edit_project_to_screen(XB(0, 0) + 14000, n != 0 ? gUserPieceHeights[gEditor->userPiece][n - 1] * 16 : 0,
                                      ZB(1) + k * 4000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                        if (gEditor->heightSel == n) {
                            switch (gEditor->heightState) {
                            case 0:
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case 1:
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case 2:
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (n < gEditor->cur->type->dim[1] * 2) {
                            edit_project_to_screen(XB(0, 0) + 2000, n != 0 ? gUserPieceHeights[gEditor->userPiece][n - 1] * 16 : 0,
                                          ZB(1) + k * 4000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                            edit_project_to_screen(XB(0, 0) + 2000, gUserPieceHeights[gEditor->userPiece][n] * 16,
                                          ZB(1) + (k - 1) * 4000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                            edit_project_to_screen(XB(0, 0) + 14000, n != 0 ? gUserPieceHeights[gEditor->userPiece][n - 1] * 16 : 0,
                                          ZB(1) + k * 4000, &p2[0], &p2[1], &p2[2], NULL, arg0, arg1);
                            edit_project_to_screen(XB(0, 0) + 14000, gUserPieceHeights[gEditor->userPiece][n] * 16,
                                          ZB(1) + (k - 1) * 4000, &p3[0], &p3[1], &p3[2], NULL, arg0, arg1);
                            if (gEditor->heightSel == n || gEditor->heightSel == n + 1) {
                                switch (gEditor->heightState) {
                                case 0:
                                    edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case 1:
                                    edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case 2:
                                    edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                case 3:
                    for (k = 0; k < gEditor->cur->type->dim[1] * 2 + 1; k++) {
                        n = gEditor->cur->type->dim[1] * 2 - k;
                        edit_project_to_screen(XB(1, 0) + k * 4000, n != 0 ? gUserPieceHeights[gEditor->userPiece][n - 1] * 16 : 0,
                                      ZB(0) + 2000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                        edit_project_to_screen(XB(1, 0) + k * 4000, n != 0 ? gUserPieceHeights[gEditor->userPiece][n - 1] * 16 : 0,
                                      ZB(0) + 14000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                        if (gEditor->heightSel == n) {
                            switch (gEditor->heightState) {
                            case 0:
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0, 0);
                                break;
                            case 1:
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0xE1, 0x8C, 0);
                                break;
                            case 2:
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0xFF, 0);
                                break;
                            }
                        } else {
                            edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                        }
                        if (n < gEditor->cur->type->dim[1] * 2) {
                            edit_project_to_screen(XB(1, 0) + k * 4000, n != 0 ? gUserPieceHeights[gEditor->userPiece][n - 1] * 16 : 0,
                                          ZB(0) + 2000, &p0[0], &p0[1], &p0[2], NULL, arg0, arg1);
                            edit_project_to_screen(XB(1, 0) + (k - 1) * 4000, gUserPieceHeights[gEditor->userPiece][n] * 16,
                                          ZB(0) + 2000, &p1[0], &p1[1], &p1[2], NULL, arg0, arg1);
                            edit_project_to_screen(XB(1, 0) + k * 4000, n != 0 ? gUserPieceHeights[gEditor->userPiece][n - 1] * 16 : 0,
                                          ZB(0) + 14000, &p2[0], &p2[1], &p2[2], NULL, arg0, arg1);
                            edit_project_to_screen(XB(1, 0) + (k - 1) * 4000, gUserPieceHeights[gEditor->userPiece][n] * 16,
                                          ZB(0) + 14000, &p3[0], &p3[1], &p3[2], NULL, arg0, arg1);
                            if (gEditor->heightSel == n || gEditor->heightSel == n + 1) {
                                switch (gEditor->heightState) {
                                case 0:
                                    edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0, 0);
                                    edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0, 0);
                                    break;
                                case 1:
                                    edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 100, 0x46, 0);
                                    edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 100, 0x46, 0);
                                    break;
                                case 2:
                                    edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 100, 0);
                                    edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 100, 0);
                                    break;
                                }
                            } else {
                                edit_add_overlay_line_2(arg1, p0[0], p0[1], p0[2], p1[0], p1[1], p1[2], 0, 0, 0);
                                edit_add_overlay_line_2(arg1, p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], 0, 0, 0);
                            }
                        }
                    }
                    break;
                }
            }
            gdl = edit_draw_overlay_lines(arg0, arg1, gdl);
        }
    } else {
        gdl = arned_draw(arg1, gdl, 0, 0, 0, 0, 0, 0, -1, 1);
    }

    if (gEditor->flags & EDIT_MESSAGE) {
        switch (gEditor->message) {
        case 0:
            gdl = gui_draw_message(gdl, arg0, arg1, "No Track Found On Game Pak");
            break;
        case 1:
            gdl = gui_draw_message(gdl, arg0, arg1, "Save Canceled");
            break;
        case 2:
            gdl = gui_draw_message(gdl, arg0, arg1, "Track Complete!");
            break;
        case 3:
            gdl = gui_draw_message(gdl, arg0, arg1, "You Ran Out Of Room!");
            break;
        case 4:
            gdl = gui_draw_message(gdl, arg0, arg1, "Name Your Track Before Saving!");
            break;
        }
    } else if (gEditor->flags & EDIT_DIALOG) {
        switch (gEditor->dialog) {
        case 0:
            gdl = gui_draw_prompt(gdl, arg0, arg1, "Add Segment?", "OK", "Cancel", 1, 1);
            break;
        case 1:
            gdl = gui_draw_prompt(gdl, arg0, arg1, "Delete Will Reset The Piece", "OK", "Cancel", 1, 1);
            break;
        case 3:
        case 7:
            gdl = gui_draw_prompt(gdl, arg0, arg1, "Lose Changes?", "OK", "Cancel", 1, 1);
            break;
        case 4:
            gdl = gui_draw_prompt(gdl, arg0, arg1, "Really Clear Track?", "OK", "Cancel", 1, 1);
            break;
        case 5:
            gdl = gui_draw_prompt(gdl, arg0, arg1, "Save The Track To...", "Game Pak", "Controller Pak", 0, 1);
            break;
        case 6:
            gdl = gui_draw_prompt(gdl, arg0, arg1, "Load A Track From...", "Game Pak", "Controller Pak",
                                (gSaveData.options >> 3) & 1, 1);
            break;
        case 8:
            gdl = gui_draw_prompt(gdl, arg0, arg1, "Overwrite Track On Game Pak?", "OK", "Cancel", 1, 1);
            break;
        }
    }
    return gdl;
}

EditPiece *edit_insert_piece(EditPiece *a, EditPiece *b, PieceType *type)
{
    EditPiece *n;
    s32 i;
    s32 idx;
    Editor *g;

    n = gEditor->pieces;
    for (i = 0; i < 150; i++, n++) {
        if (n->used == 0) break;
    }
    g = gEditor;
    n->used = 1;
    if (g->flags & EDIT_FLIP) {
        if (type->flags & 0x20) {
            n->flags = 2;
        } else {
            n->flags = 0;
            g->flags &= ~EDIT_FLIP;
        }
    } else if (type->flags & 0x10) {
        n->flags = 0;
    } else {
        n->flags = 2;
        g->flags |= EDIT_FLIP;
    }
    n->next = b;
    n->prev = a;
    n->type = type;
    if (b == NULL && a == NULL) {
        gEditor->first = n;
        gEditor->last = n;
        idx = type->rot;
        if (gEditor->flags & EDIT_FLIP) idx += 2;
        n->rot = idx;
        n->x = gEditor->gridSize[0] / 2 - type->dim[idx & 1] / 2;
        n->y = gEditor->gridSize[1] / 2 - type->dim[(n->rot + 1) & 1] / 2;
        n->x += gEditor->scrollX;
        n->y += gEditor->scrollY;
        if (n->x < 0) {
            n->x = 0;
            gEditor->scrollX = type->dim[n->rot & 1] / 2 - gEditor->gridSize[0] / 2;
        }
        if (n->x + n->type->dim[n->rot & 1] > gEditor->gridSize[0]) {
            n->x = gEditor->gridSize[0] - n->type->dim[n->rot & 1];
            gEditor->scrollX = n->x - (gEditor->gridSize[0] / 2 - type->dim[n->rot & 1] / 2);
        }
        if (n->y < 0) {
            n->y = 0;
            gEditor->scrollY = type->dim[~n->rot & 1] / 2 - gEditor->gridSize[1] / 2;
        }
        if (n->y + n->type->dim[~n->rot & 1] > gEditor->gridSize[1]) {
            n->y = gEditor->gridSize[1] - n->type->dim[~n->rot & 1];
            gEditor->scrollY = n->y - (gEditor->gridSize[1] / 2 - type->dim[~n->rot & 1] / 2);
        }
    } else {
        if (a != NULL) {
            gEditor->last = n;
            a->next = n;
            edit_place_piece_after(a, n);
        }
        if (b != NULL) {
            gEditor->first = n;
            b->prev = n;
            edit_place_piece_before(b, n);
        }
    }
    return n;
}

void edit_delete_piece(EditPiece *p)
{
    s16 *grid;
    s32 ix;
    s32 iy;
    s32 x;
    s32 y;

    if (p->next != NULL) {
        if (p->prev != NULL) {
            p->prev->next = NULL;
            p->next->prev = NULL;
            if ((p->flags & EDIT_PIECE_PREVIEW) == 0) {
                gEditor->first = p->next;
                gEditor->last = p->prev;
            }
            goto L_d4;
        }
        goto L_c;
    }
    if (p->prev != NULL) {
        p->prev->next = NULL;
        goto L_8c;
    }
L_c:
    if (p->next != NULL) {
        p->next->prev = p->prev;
    }
L_8c:
    if ((p->flags & EDIT_PIECE_PREVIEW) == 0) {
        if (gEditor->first == p) {
            gEditor->first = p->next;
        }
        if (gEditor->last == p) {
            gEditor->last = p->prev;
        }
    }
L_d4:
    {
        Editor *ed = gEditor;
        p->used = 0;
        if (ed->start == p) {
            ed->start = NULL;
        }
    }
    for (iy = 0; iy < p->type->dim[(p->rot + 1) & 1]; iy++) {
        for (ix = 0; ix < p->type->dim[p->rot & 1]; ix++) {
            x = p->x + ix;
            if (x >= 0) {
                grid = gEditor->gridSize;
                if (x < grid[0]) {
                    y = p->y + iy;
                    if (y >= 0 && y < grid[1]) {
                        arned_set_cell(x, y, 0, 0, 0, 0, 0, 0, 0);
                    }
                }
            }
        }
    }
}

/* Where a piece's connector is along an x side of the piece (0 when it is on a y side). P[s], P[s + 2] and
   P[s + 4] are the type's dim[s], dir[s] and off[s]. */
s32 edit_get_port_x(EditPiece *b, s32 s)
{
    s16 *P = b->type->dim;
    s16 *Q = &P[s];
    s32 t = Q[2];

    switch ((t + b->rot) & 3) {
    case 1:
    case 3:
        return 0;
    case 0:
        switch (t) {
        case 0:
        case 3:
            return Q[4];
        case 2:
            return P[0] - Q[4] - 2;
        case 1:
            return P[1] - Q[4] - 2;
        }
    case 2: {
        s16 *R = b->type->dim;
        s16 *Q2 = &R[s];

        switch (Q2[2]) {
        case 0:
            return R[0] - Q2[4] - 2;
        case 1:
            return Q2[4];
        case 2:
            return Q2[4];
        case 3:
            return R[1] - Q2[4] - 2;
        }
    }
    }
}

/* ... along a y side. */
s32 edit_get_port_y(EditPiece *p, s32 k)
{
    s16 *P = p->type->dim;
    s16 *Q = P + k;
    s32 a = Q[2];

    switch ((a + p->rot) & 3) {
    case 0:
        return 0;
    case 2:
        return 0;
    case 3:
        switch (a) {
        case 2:
            return P[0] - Q[4] - 2;
        case 0:
        case 3:
            return Q[4];
        case 1:
            return P[1] - Q[4] - 2;
        }
    case 1: {
        s16 *R = p->type->dim;
        s16 *S = R + k;

        switch (S[2]) {
        case 2:
            return S[4];
        case 0:
            return R[0] - S[4] - 2;
        case 3:
            return R[1] - S[4] - 2;
        case 1:
            return S[4];
        }
    }
    }
}

#define SX16(v) ((s32)((u32)(v) << 16))
#define HALF(v, s) ((s32)((SX16(v) >> 16) + (s32)((u32)SX16(v) >> 31)) >> (s))

/* Puts c after p: c's entry connector against p's exit (with no p, c goes in the middle of the grid). Some reads
   are unsigned or volatile as the original has them. */
void edit_place_piece_after(EditPiece *p, EditPiece *c)
{
    EditPiece *q = p;
    s32 k = 1;
    s32 m = 0;
    s32 t, t2, r, v, ka, ma;

    if (q == 0) {
        u16 *tg = (u16 *)gEditor->gridSize;
        c->x = HALF(tg[0], k) - HALF((u16)c->type->dim[0], k);
        tg = (u16 *)gEditor->gridSize;
        c->y = HALF(tg[1], k) - HALF((u16)c->type->dim[1], k);
        return;
    }
    if (q->flags & EDIT_PIECE_FLIPPED) k = 0;
    if (c->flags & EDIT_PIECE_FLIPPED) m = 1;
    c->x = q->x;
    *(volatile s16 *)&c->y = q->y;
    t = (u16)q->rot + ((u16)q->type->dir[k] - (u16)(*(PieceType *volatile *)&c->type)->dir[m]) + 2;
    t2 = t & 3;
    c->rot = t2;
    r = (q->type->dir[k] + q->rot) & 3;
    switch (r) {
    case 0:
        c->y += q->type->dim[(q->rot + 1) & 1];
        v = edit_get_port_x(q, k);
        c->x += v;
        v = edit_get_port_x(c, m);
        c->x -= v;
        break;
    case 2:
        c->y -= c->type->dim[(t2 + 1) & 1];
        v = edit_get_port_x(q, k);
        c->x += v;
        v = edit_get_port_x(c, m);
        c->x -= v;
        break;
    case 3:
        c->x -= c->type->dim[t & 1];
        v = edit_get_port_y(q, k);
        c->y += v;
        v = edit_get_port_y(c, m);
        c->y -= v;
        break;
    case 1:
        c->x += q->type->dim[q->rot & 1];
        v = edit_get_port_y(q, k);
        c->y += v;
        v = edit_get_port_y(c, m);
        c->y -= v;
        break;
    }
}

/* Puts b before a: b's exit connector against a's entry. */
void edit_place_piece_before(EditPiece *a, EditPiece *b)
{
    s32 fa, fb;
    u16 n;
    s32 f; u32 g;

    f = a->flags & EDIT_PIECE_FLIPPED; fa = f != 0;
    b->x = a->x;
    b->y = a->y;
    g = b->flags & EDIT_PIECE_FLIPPED; fb = g < 1;
    n = a->rot + (a->type->dir[fa] - b->type->dir[fb]) + 2;
    b->rot = n & 3;
    switch ((a->type->dir[fa] + a->rot) & 3) {
    case 0:
        b->y += a->type->dim[(a->rot + 1) & 1];
        b->x += edit_get_port_x(a, fa);
        b->x -= edit_get_port_x(b, fb);
        break;
    case 2:
        b->y -= b->type->dim[(b->rot + 1) & 1];
        b->x += edit_get_port_x(a, fa);
        b->x -= edit_get_port_x(b, fb);
        break;
    case 3:
        b->x -= b->type->dim[b->rot & 1];
        b->y += edit_get_port_y(a, fa);
        b->y -= edit_get_port_y(b, fb);
        break;
    case 1:
        b->x += a->type->dim[a->rot & 1];
        b->y += edit_get_port_y(a, fa);
        b->y -= edit_get_port_y(b, fb);
        break;
    }
}

void edit_set_start_piece_choices(void)
{
    s32 i;
    s32 k;
    s32 t0;
    s32 v;
    PieceType *p;
    s32 w;
    s32 r;

    for (k = 0; k < 0x1D; k++) {
        p = &gPieceTypes[k];
        v = (u16)p->flags; /* read unsigned here */
        if (v & 2) {
            w = v | 0x10;
            p->flags = w;
            if (w & 0x100) {
                r = w | 0x20;
            } else {
                r = w & ~0x20;
            }
        } else {
            r = v & ~0x10;
            r &= ~0x20;
        }
        p->flags = r;
    }
    for (i = 0; i < 4; i++) {
        switch (i) {
        case 0:
            t0 = 0;
            break;
        case 1:
            t0 = 2;
            break;
        case 2:
            t0 = 6;
            break;
        case 3:
            t0 = 0x12;
            break;
        }
        sEditTrackMenu.items[i].valueMask = 0;
        for (k = 0; k < sEditTrackMenu.items[i].numValues; k++) {
            if (gPieceTypes[t0 + k].flags & (PIECE_FITS | PIECE_FITS_FLIPPED)) {
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

typedef struct Grid_3 {
    u8 g[30][30];
} Grid_3;

typedef struct Rect_4 {
    s32 x0, y0, x1, y1;
} Rect_4;

extern Grid_3 sEditFitGrid;

s32 edit_update_fitting_pieces(EditPiece *o)
{
    Grid_3 grid;
    Rect_4 r;
    s16 k;
    s16 other;
    s32 i, j;
    s32 n, m, lo, hi, best;
    s16 s;
    s32 ret;
    EditPiece *p;
    PieceType *e;

    k = 1;
    if (o->next == NULL || (o->next->flags & 4)) {
        other = 0;
    } else {
        k = 0;
        other = 1;
    }
    switch (o->dir[k]) {
    case 0:
        {
            s32 t, u;
            t = o->x + o->off[k];
            r.x0 = t - 14;
            u = o->y + o->h;
            r.x1 = t + 16;
            r.y0 = u - 15;
            r.y1 = u + 15;
        }
        break;
    case 1:
        {
            s32 t, u;
            t = o->x + o->w;
            r.x0 = t - 15;
            u = o->y + o->off[k];
            r.x1 = t + 15;
            r.y0 = u - 14;
            r.y1 = u + 16;
        }
        break;
    case 2:
        {
            s32 t, u;
            t = o->x + o->off[k];
            r.x0 = t - 14;
            u = o->y;
            r.x1 = t + 16;
            r.y0 = u - 15;
            r.y1 = u + 15;
        }
        break;
    case 3:
        {
            s32 t, u;
            t = o->x;
            r.x0 = t - 15;
            u = o->y + o->off[k];
            r.x1 = t + 15;
            r.y0 = u - 14;
            r.y1 = u + 16;
        }
        break;
    }
    mem_fill((u8 *)&grid, 0, 0x384);
    for (j = 0; j < r.y1 - r.y0; j++) {
        for (i = 0; i < r.x1 - r.x0; i++) {
            if (i + r.x0 < 0 || i + r.x0 >= gEditor->gridSize[0]
                || j + r.y0 < 0 || j + r.y0 >= gEditor->gridSize[1]) {
                switch (o->dir[k]) {
                case 0:
                    grid.g[i][j] = 1;
                    break;
                case 2:
                    grid.g[29 - i][29 - j] = 1;
                    break;
                case 3:
                    grid.g[j][29 - i] = 1;
                    break;
                case 1:
                    grid.g[29 - j][i] = 1;
                    break;
                }
            }
        }
    }
    p = gEditor->first;
    if (p != NULL) {
        do {
            if (!(p->flags & 4) && p->x < r.x1 && r.x0 < p->x + p->w
                && p->y < r.y1 && r.y0 < p->y + p->h) {
                switch (o->dir[k]) {
                case 0:
                    for (j = p->y - r.y0; j < p->y - r.y0 + p->h; j++) {
                        for (i = p->x - r.x0; i < p->x - r.x0 + p->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[i][j] = 1;
                            }
                        }
                    }
                    break;
                case 2:
                    for (j = p->y - r.y0; j < p->y - r.y0 + p->h; j++) {
                        for (i = p->x - r.x0; i < p->x - r.x0 + p->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[29 - i][29 - j] = 1;
                            }
                        }
                    }
                    break;
                case 3:
                    for (j = p->y - r.y0; j < p->y - r.y0 + p->h; j++) {
                        for (i = p->x - r.x0; i < p->x - r.x0 + p->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[j][29 - i] = 1;
                            }
                        }
                    }
                    break;
                case 1:
                    for (j = p->y - r.y0; j < p->y - r.y0 + p->h; j++) {
                        for (i = p->x - r.x0; i < p->x - r.x0 + p->w; i++) {
                            if (i >= 0 && j >= 0 && i < 30 && j < 30) {
                                grid.g[29 - j][i] = 1;
                            }
                        }
                    }
                    break;
                }
            }
            p = p->next;
        } while (p != NULL && p != gEditor->first);
    }
    sEditFitGrid = grid;

    for (n = 0; n < 29; n++) {
        e = &gPieceTypes[n];
        if (!(e->flags & PIECE_IN_MENU)) {
            continue;
        }
        best = 0;
        if (e->flags & PIECE_VAR_LENGTH) {
            lo = 1;
            hi = 8;
        } else {
            hi = e->dim[(e->dir[k] + 1) & 1];
            lo = hi;
        }
        for (m = lo; m <= hi; m++) {
            if ((u32)(n - 18) < 8 && n - 18 >= gEditor->userPieceCount) {
                e->flags &= ~(PIECE_FITS | PIECE_FITS_FLIPPED);
            } else {
                switch (e->dir[other]) {
                case 0:
                    s = e->dim[0] - e->off[other] - 2;
                    break;
                case 3:
                    s = e->dim[1] - e->off[other] - 2;
                    break;
                case 1:
                    s = e->off[other];
                    break;
                case 2:
                    s = e->off[other];
                    break;
                }
                e->flags |= PIECE_FITS;
                for (j = 0; j < m; j++) {
                    for (i = 0; i < e->dim[e->dir[k] & 1]; i++) {
                        if (grid.g[i + (14 - s)][j + 15]) {
                            e->flags &= ~PIECE_FITS;
                            break;
                        }
                    }
                    if (!(e->flags & PIECE_FITS)) {
                        break;
                    }
                }
                if (e->flags & PIECE_FLIPPABLE) {
                    switch (e->dir[k]) {
                    case 0:
                        s = e->dim[0] - e->off[k] - 2;
                        break;
                    case 3:
                        s = e->dim[1] - e->off[k] - 2;
                        break;
                    case 1:
                        s = e->off[k];
                        break;
                    case 2:
                        s = e->off[k];
                        break;
                    }
                    e->flags |= PIECE_FITS_FLIPPED;
                    j = 0;
                    if (m > 0) {
                        do {
                            for (i = 0; i < e->dim[e->dir[k] & 1]; i++) {
                                if (grid.g[i + (14 - s)][j + 15]) {
                                    e->flags &= ~PIECE_FITS_FLIPPED;
                                    break;
                                }
                            }
                            j++;
                        } while ((e->flags & PIECE_FITS_FLIPPED) && j < m);
                    }
                } else {
                    e->flags &= ~PIECE_FITS_FLIPPED;
                }
            }
            if (e->flags & PIECE_FITS) {
                best = m;
            } else if (e->flags & PIECE_VAR_LENGTH) {
                if (m != lo) {
                    e->flags |= PIECE_FITS;
                }
                break;
            }
        }
        if (e->flags & PIECE_VAR_LENGTH) {
            if (gEditor->mode != EDIT_MODE_USER_PIECE) {
                e->dim[1] = (best < e->dim[1]) ? best : e->dim[1];
            }
            gEditor->maxSegments = best;
        }
    }

    ret = 0;
    for (i = 0; i < 6; i++) {
        switch (i) {
        case 0:
            j = 0;
            break;
        case 1:
            j = 2;
            break;
        case 2:
            j = 6;
            break;
        case 3:
            j = 0x12;
            break;
        default:
            continue;
        }
        sEditTrackMenu.items[i].valueMask = 0;
        for (n = 0; n < sEditTrackMenu.items[i].numValues; n++) {
            if (gPieceTypes[j + n].flags & 0x30) {
                sEditTrackMenu.items[i].valueMask |= 1 << n;
            }
        }
        if (sEditTrackMenu.items[i].valueMask == 0) {
            sEditTrackMenu.items[i].flags &= ~GUI_ITEM_ENABLED;
        } else {
            ret = 1;
            sEditTrackMenu.items[i].flags |= GUI_ITEM_ENABLED;
        }
    }
    gui_menu_fix_cursor(&sEditTrackMenu);
    return ret;
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
