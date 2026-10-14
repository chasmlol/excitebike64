#include "game.h"

extern GravelPitMover sGravelPitMovers[GRAVEL_PIT_NUM_MOVERS];

extern TrackHack *sTrackHacks[];

/* Finds the current track's hooks in sTrackHacks, allocates their buffers, loads their resources and runs their
   init. */
void hacks_init(void)
{
    s32 track;
    s32 i;

    track = gCurTrack_7 - gTracks_3;
    sCurTrackHack = NULL;
    for (i = 0; sTrackHacks[i] != NULL; i++) {
        if (sTrackHacks[i]->track == track) {
            sCurTrackHack = sTrackHacks[i];
            break;
        }
    }
    if (sCurTrackHack != NULL) {
        sHackBuffers[0] = NULL;
        sHackBuffers[1] = NULL;
        sHackResources = NULL;
        if (sCurTrackHack->numBufs != 0) {
            /* two banks of numBufs 0x40-byte buffers */
            sHackBuffers[0] = mem_alloc_dbg(sCurTrackHack->numBufs << 7, 0, sStrHacksFile, 0x9f);
            sHackBuffers[1] = (u8 *)sHackBuffers[0] + (sCurTrackHack->numBufs << 6);
        }
        if (sCurTrackHack->numRes != 0) {
            sHackResources = mem_alloc_dbg(sCurTrackHack->numRes << 2, 0, sStrHacksFile, 0xa4);
            for (i = 0; i < sCurTrackHack->numRes; i++) {
                sHackResources[i] = res_load(gResourceSet, *sCurTrackHack->resNames[i]);
            }
        }
        sCurTrackHack->init();
    }
}

typedef struct {
    s32 a, b, c;
    s32 data;
} ItemModelRes;

/* The Gravel Pit: lays each mover's line on the ground, spreads its items along it, and loads the four item models
   (their vertex data moved to the end of the Z buffer). */
void hack_gravel_pit_init(void)
{
    s32 i, j;
    s32 midX, midZ, size;
    GravelPitMover *mover;
    ItemModelRes *res;
    s32 len;
    s32 ofs, ofs2;

    gGravelPitMoverTimer = 0;
    for (i = 0; i < GRAVEL_PIT_NUM_MOVERS; i++) {
        mover = &sGravelPitMovers[i];
        math_to_polar(mover->lineX1 - mover->lineX0, mover->lineZ1 - mover->lineZ0, &mover->angle, &len);
        midX = (mover->lineX0 + mover->lineX1) / 2;
        midZ = (mover->lineZ0 + mover->lineZ1) / 2;
        len = mover->count * GRAVEL_PIT_ITEM_LENGTH; /* half the line's length */
        mover->start.x = midX + gCosTable[(((s16)mover->angle >> 4) - 0x400) & 0xFFF] * len / 0x4000;
        mover->start.z = midZ + gCosTable[mover->angle >> 4] * len / 0x4000;
        mover->end.x = midX - gCosTable[(((s16)mover->angle >> 4) - 0x400) & 0xFFF] * len / 0x4000;
        mover->end.z = midZ - gCosTable[mover->angle >> 4] * len / 0x4000;
        mover->start.y = height_get_ground(midX, 50000, midZ);
        mover->end.y = height_get_ground(midX, 50000, midZ);
        mover->section = gGroundArea;
        for (j = 0; j < mover->count; j++) {
            mover->progress[j] = (j << 8) / mover->count;
            mover->model[j] = j;
        }
        mover->mtx = mem_alloc_dbg(mover->count << 9, 0, "hacks.c", 0x116);
    }
    for (i = 0; i < GRAVEL_PIT_NUM_MODELS; i++) {
        res = res_load(gResourceSet, *sGravelPitItemResIds[i]);
        len = res->data - (s32)res; /* the header's size */
        size = mem_get_size(res);
        ofs = 0x1E000;
        memcpy(gZBuffer + ofs, res->data, size - len);
        ofs = 0;
        mem_realloc_dbg(res, len, ofs, "hacks.c", 0x124);
        ofs2 = 0x1E000;
        res->data = gZBuffer + ofs2;
        prim_alloc_dl(&sGravelPitModels[i], res, sObjectSegs, 0xC);
    }
}

/* The Gravel Pit: moves the items along the lines (a new random model each time one starts over). A human rider
   doing a trick over a mover's section switches an ambient sound for 300 frames (gGravelPitMoverTimer). */
void hack_gravel_pit_update(void)
{
    s32 j;
    s32 i;
    GravelPitMover *mover;

    for (j = 0; j < GRAVEL_PIT_NUM_MOVERS; j++) {
        mover = &sGravelPitMovers[j];
        for (i = 0; i < mover->count; i++) {
            if (++mover->progress[i] >= 0x100) {
                mover->progress[i] = 0;
                mover->model[i] = random_byte() & 3;
            }
        }
    }

    for (i = 0; i < 8; i++) {
        if (gRiders[i].inactive == 0 && gRiders[i].isCpu == 0 && gRiders[i].anim != -1 && gRiders[i].anim < 16) {
            if (gRiderSection[i] == sGravelPitMovers[0].section || gRiderSection[i] == sGravelPitMovers[1].section) {
                gGravelPitMoverTimer = 300;
            }
        }
    }

    if (gGravelPitMoverTimer > 0) {
        gGravelPitMoverTimer--;
    }
}

/* The Gravel Pit: draws the items of the movers whose section is visible in this view. */
Gfx *hack_gravel_pit_draw_pass2(Gfx *gdl, s32 unused, s32 fb, s32 view)
{
    s32 i, k;
    s32 *cam;
    s32 *camPos;
    s32 visBit;
    GravelPitMover *mover;
    Mtx *m;
    Gfx **models;
    s32 x, y, z;

    visBit = view * 2;
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetRenderMode(gdl++, G_RM_AA_ZB_TEX_EDGE, G_RM_NOOP2);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);

    i = 0;
    camPos = gCamPos;
    cam = camPos + (visBit + view); /* this view's camera position */
    for (; i < GRAVEL_PIT_NUM_MOVERS; i++) {
        mover = &sGravelPitMovers[i];
        if ((gTrackSections->e[mover->section].mask >> visBit) & 1) {
            k = 0;
            if (mover->count > 0) {
                models = sGravelPitModels;
                do {
                    x = mover->start.x + (mover->end.x - mover->start.x) * mover->progress[k] / 256;
                    z = mover->start.z + (mover->end.z - mover->start.z) * mover->progress[k] / 256;
                    y = mover->start.y;
                    m = &mover->mtx[fb * (mover->count << 2) + view * mover->count + k];
                    mtx_from_angles_pos_scale(m, 0, 0, 0x8000 - (s16)mover->angle, x - cam[0], y - cam[1], z - cam[2],
                                              0x100);
                    gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPDisplayList(gdl++, models[mover->model[k]]);
                    k++;
                } while (k < mover->count);
            }
        }
    }
    return gdl;
}

/* Lefty's Mill: presses the planks down under the riders' wheels and bobs them on the water. */
void hack_mill_update(void)
{
    s32 cosA, sinA;
    s32 i, n;
    u32 angle;
    Vec3i *state;
    s32 dx, dz, front, rear, t, press, bob;

    angle = gInstanceItems[0].rot >> 4;
    cosA = gCosTable[angle | 0xC00];
    sinA = gCosTable[angle];
    for (n = 0; n < sMillObjCount; n++) {
        sMillObjState[n].z = 0;
    }
    for (n = 0; n < 8; n++) {
        if (gRiders[n].inactive != 0 || gRiders[n].rearWheel.airTicks >= 0x15 || gRiders[n].frontWheel.airTicks >= 0x15) continue;
        for (i = 0; i < sMillObjCount; i++) {
            /* each wheel's distance across the plank's axis */
            dx = (s32)(gRiders[n].frontWheel.contact.x * 1000.0f) - gInstanceItems[i].x;
            if ((u32)(dx + MILL_REACH) > 2 * MILL_REACH) continue;
            dz = (s32)(gRiders[n].frontWheel.contact.z * 1000.0f) - gInstanceItems[i].z;
            if ((u32)(dz + MILL_REACH) > 2 * MILL_REACH) continue;
            front = (dx * cosA + dz * sinA) >> 14;
            dx = (s32)(gRiders[n].rearWheel.contact.x * 1000.0f) - gInstanceItems[i].x;
            if ((u32)(dx + MILL_REACH) > 2 * MILL_REACH) continue;
            dz = (s32)(gRiders[n].rearWheel.contact.z * 1000.0f) - gInstanceItems[i].z;
            if ((u32)(dz + MILL_REACH) > 2 * MILL_REACH) continue;
            rear = (dx * cosA + dz * sinA) >> 14;
            if (front > 0 && rear < 0) {
                press = 0x100; /* the wheels straddle the plank */
            } else if (front < 0 && rear > 0) {
                press = 0x100;
            } else {
                /* the nearer wheel presses more the closer it is (rear is reused as a temporary) */
                if (front > 0) {
                    press = rear;
                    if (front < press) press = front;
                    rear = MILL_PRESS_DIST;
                    rear -= press;
                    press = (rear << 8) / MILL_PRESS_DIST;
                } else if (front < 0) {
                    press = rear;
                    if (press < front) press = front;
                    rear = press + MILL_PRESS_DIST;
                    press = (rear << 8) / MILL_PRESS_DIST;
                } else {
                    goto store;
                }
                if (press < 0) press = 0;
                if (press >= 0x100) press = 0xFF;
            }
store:
            if (sMillObjState[i].z < press) sMillObjState[i].z = press;
        }
    }
    i = 0;
    if (sMillObjCount > 0) {
        do {
            state = &sMillObjState[i];
            t = state->z;
            bob = (gCosTable[(((gRaceFrame << 9) + i * 0x1000) >> 4) & 0xFFF] * (0xFF - t)) >> 15;
            bob -= t;
            bob /= 2;
            bob -= 200;
            /* rises at once, sinks 16 a frame at most */
            if (bob < state->y - 16) {
                state->y = state->y - 16;
            } else {
                state->y = bob;
            }
            ((MtxS *)gInstanceMtx)[i].i[3][1] = (state->x + state->y) >> 4; /* the y translation */
            i++;
        } while (i < sMillObjCount);
    }
}

void hacks_update(void)
{
    if (sCurTrackHack != 0) {
        sCurTrackHack->update();
    }
}

/* Frees the current track hook's resources and buffers and runs its exit. */
void hacks_exit(void) {
    if (sCurTrackHack != NULL) {
        if (sCurTrackHack->numRes != 0) {
            mem_free((void *)sHackResources);
        }
        if (sCurTrackHack->numBufs != 0) {
            mem_free(sHackBuffers);
        }
        sCurTrackHack->exit();
    }
}

/* render.c's hooks: the current track's draw passes, which take and return the display list pointer. */
s32 hacks_draw_pass2(s32 gdl, Dyn *dyn, s32 fb, s32 view)
{
    if (sCurTrackHack != 0) {
        gdl = sCurTrackHack->drawPass2(gdl);
    }
    return gdl;
}

s32 hacks_draw_pass1(s32 gdl, Dyn *dyn, s32 fb, s32 view)
{
    if (sCurTrackHack != 0) {
        gdl = sCurTrackHack->drawPass1(gdl);
    }
    return gdl;
}

typedef struct {
    s32 restY;
    s32 offset;
    s32 press;
} MillState;

/* Lefty's Mill: one state per instance, resting at the instance's height. */
void hack_mill_init(void)
{
    s32 i;
    s32 n;
    s32 cnt;
    MillState *out;
    Instance *inst;

    sMillObjCount = gInstanceHeader->numItems;
    sMillObjState = (s32 *)mem_alloc_dbg(sMillObjCount * 12, 0, sStrHacksFile, 0x1B1);
    i = 0;
    n = sMillObjCount;
    if (n > 0) {
        cnt = n;
        out = (MillState *)sMillObjState;
        inst = gInstanceItems;
        do {
            s32 y = inst->y;
            inst++;
            i++;
            out->offset = 0;
            out->restY = y;
            out++;
        } while (i < cnt);
    }
}

void hack_mill_exit(void)
{
    mem_free(sMillObjState);
}

s32 hack_mill_draw_pass1(s32 gdl)
{
    return gdl;
}

void *hack_mill_draw_pass2(void *gdl)
{
    return gdl;
}

/* The Gravel Pit: frees the movers' matrices and the item models. */
void hack_gravel_pit_exit(void) {
    s32 i = 0;
    GravelPitMover *mover = sGravelPitMovers;
    u32 *models;

    do {
        Mtx *mtx = mover->mtx;
        mover++;
        i++;
        mem_free(mtx);
    } while (i < GRAVEL_PIT_NUM_MOVERS);

    i = 0;
    models = sGravelPitModels;
    do {
        u32 model = *models;
        models++;
        i++;
        mem_free(model);
    } while (i < GRAVEL_PIT_NUM_MODELS);
}

s32 hack_gravel_pit_draw_pass1(s32 gdl)
{
    return gdl;
}
