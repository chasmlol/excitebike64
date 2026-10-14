#include "game.h"

typedef struct { u8 pad[0x10]; void *p; } PodiumModel;

extern PodiumModel *sPodiumModel;
extern PodiumModel *sPodiumHelmetModel;

extern PodiumCameraClip sPodiumCameraClips[];

/* view 0's camera; every store is kept */
#define CAM0 (*(volatile Vec3i *)&gCamPos[0])

/* Places view 0's camera from the camera clips (the last frame held at their end), turned by sPodiumYaw around the
   podium. */
void podium_update_camera(void)
{
    s32 frame = sPodiumFrame;
    s32 clip = 0;
    PodiumCamFrame *cf;
    s16 *cosTable;
    s32 ofs;
    s32 ang, cosY, sinY, camX, camZ, rotX, rotZ, xCos, zCos, podZ, podX;

    while (frame >= sPodiumCameraClips[clip].clip->numFrames) {
        frame -= sPodiumCameraClips[clip].clip->numFrames;
        clip++;
        if (sPodiumCameraClips[clip].resId == 0) {
            clip--;
            frame = sPodiumCameraClips[clip].clip->numFrames - 1;
            break;
        }
    }

    ofs = frame * sizeof(PodiumCamFrame);
    cf = (PodiumCamFrame *)((u8 *)sPodiumCameraClips[clip].clip->frames + ofs);
    camX = cf->x;
    CAM0.x = camX;
    camZ = cf->z;
    camZ = (s16)camZ;
    CAM0.z = camZ;
    cosTable = gCosTable;
    ang = (s16)sPodiumYaw >> 4;
    cosY = cosTable[ang & 0xFFF];
    sinY = cosTable[(ang - 0x400) & 0xFFF];
    xCos = camX * cosY;
    zCos = camZ * cosY;
    rotZ = (zCos + camX * sinY) >> 14;
    rotX = (xCos - camZ * sinY) >> 14;
    CAM0.z = rotZ;
    podX = sPodiumPos_2.x;
    podZ = rotZ + ((volatile Vec *)&sPodiumPos_2)->z;
    CAM0.x = rotX;
    CAM0.x = rotX + podX;
    CAM0.z = podZ;
    CAM0.y = cf->y + sPodiumPos_2.y;
    gCamPitch[0] = cf->pitch;
    gCamRoll[0] = cf->roll;
    gCamYaw[0] = cf->yaw + sPodiumYaw;
}

extern PodiumRiderClip *sPodiumRiderClips[];
extern PodiumModel *sPodiumTexture;

/* Sets up the ceremony: where the podium stands (a node of the racing line, special cases for three tracks), its
   music, models and texture, and the clips. */
void podium_init(void)
{
    s32 i;
    PodiumRiderClip *rc;

    gRaceFrame = 0;
    sPodiumFrame = 0;
    sPodiumYaw = 0x8000;
    i = ((gCurTrack_6 - (s32)gTracks) * -0x33333333) >> 5; /* the track's index: / sizeof(Rec) */
    sPodiumTrackPoint = 0;
    if (i == TRACK_LAS_VEGAS) {
        sPodiumTrackPoint = 70;
    } else if (i == TRACK_DETROIT) {
        sPodiumTrackPoint = 100;
        sPodiumYaw = 24000;
    } else if (i == TRACK_BLIZZARD_BLITZ) {
        sPodiumTrackPoint = 20;
        sPodiumYaw = -2000;
    }
    i = 0;
    gDesaturation = 0;
    tunes_play_song(gPodiumSongResId);
    sPodiumTexture = res_load(gResourceSet, sPodiumTextureResId);
    sPodiumModel = res_load(gResourceSet, sPodiumModelResId);
    sPodiumMtxBuf = mem_alloc_dbg(0x200, i, sStrPodiumFile, 0xFE); /* two frames of four matrices */
    sPodiumHelmetModel = res_load(gResourceSet, sPodiumHelmetResId);

    for (i = 0; i < PODIUM_NUM_PLACES; i++) {
        for (rc = sPodiumRiderClips[i]; rc->poseResId != 0; rc++) {
            rc->pose = (PodiumClip *)res_load(gResourceSet, *rc->poseResId);
            rc->pose->frames = (u8 *)rc->pose + PODIUM_POSE_HEADER;
            if (rc->propResId != 0) {
                rc->prop = (PodiumClip *)res_load(gResourceSet, *rc->propResId);
                rc->prop->frames = (u8 *)rc->prop + PODIUM_CLIP_HEADER;
            }
        }
    }
    for (i = 0; sPodiumCameraClips[i].resId != 0; i++) {
        sPodiumCameraClips[i].clip = (PodiumClip *)res_load(gResourceSet, *sPodiumCameraClips[i].resId);
        sPodiumCameraClips[i].clip->frames = (u8 *)sPodiumCameraClips[i].clip + PODIUM_CLIP_HEADER;
    }
    palmod_register(sPodiumTexture, 0);
}

typedef struct {
    u8 pad[0x14];
    u32 palette;
    u8 pad2[0xA80 - 0x18];
} RiderGfxPalette;

extern RiderGfxPalette gRiderGfx_8[];

/* Draws the podium and each rider's helmet (with the rider's palette). */
Gfx *podium_draw(Gfx *gdl, s32 projMtx, s32 arg2)
{
    s32 i;

    /* the packets it writes are overwritten: the returned pointer is dropped (an original bug) */
    render_set_viewport(gdl, projMtx, 0, 1);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetRenderMode(gdl++, G_RM_ZB_OPA_SURF, G_RM_NOOP2);
    gDPSetCombineMode(gdl++, G_CC_DECALRGB, G_CC_DECALRGB);
    gDPLoadTLUT_pal256(gdl++, sPodiumTexture_2[1]);
    i = 0;
    render_make_world_mtx(sPodiumMtxBuf + gGfxFrameIndex * 0x100, 0, 0, sPodiumYaw, sPodiumPos[0], sPodiumPos[1],
                          sPodiumPos[2], 0x10, 0);
    gSPMatrix(gdl++, sPodiumMtxBuf + gGfxFrameIndex * 0x100, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gdl = prim_build_dl(0, sPodiumModel, &sPodiumTexture_2, gdl);
    for (; i < gEvent.numRiders; i++) {
        render_make_world_mtx((s32)&((Mtx *)(sPodiumMtxBuf + gGfxFrameIndex * 0x100))[i + 1], sPodiumHelmetRot[i][0],
                              sPodiumHelmetRot[i][1], sPodiumHelmetRot[i][2], sPodiumHelmetPos[i][0],
                              sPodiumHelmetPos[i][1], sPodiumHelmetPos[i][2], 0x10, 0);
        gSPMatrix(gdl++, (s32)&((Mtx *)(sPodiumMtxBuf + gGfxFrameIndex * 0x100))[i + 1],
                  G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gDPLoadTLUT_pal256(gdl++, gRiderGfx_8[i].palette);
        gdl = prim_build_dl(0, sPodiumHelmetModel, &gRiderGfx_8[i], gdl);
    }
    return gdl;
}

/* the low half of sPodiumYaw, through the yawHalves pointer */
#define YAW16 (yawHalves[1])

/* Turns (*px, *pz) by the 12-bit angle ang. */
static inline void rotate_xz(s32 *px, s32 *pz, s32 ang)
{
    s32 c = gCosTable[ang & 0xFFF];
    s32 s = gCosTable[(ang - 0x400) & 0xFFF];
    s32 t = (*px * c - *pz * s) >> 14;

    *pz = (*pz * c + *px * s) >> 14;
    *px = t;
}

/* Plays the ceremony one frame: the riders' poses from their clips, and their helmets, on the head during the
   first pose clip and then from the prop clips. The clips run 1.5 frames per tick, slow down while the scene greys
   (gDesaturation, started when a rider's clips end) and stop once it is grey. */
void podium_update(void)
{
    Mtx3 headRot;
    s32 vx, vz;
    u16 rx, ry, rz;
    s32 place, j, clip, frame, ang;
    RiderSkeleton *skel;
    PodiumPoseFrame *pose;
    u8 *rot;
    u16 *yawHalves;

    sPodiumPos[0] = (s32)((f32)(s32)gPathNodes[sPodiumTrackPoint].pos.x * 1000.0f);
    sPodiumPos[2] = (s32)((f32)(s32)gPathNodes[sPodiumTrackPoint].pos.z * 1000.0f);
    /* sPodiumPos[sPodiumTrackPoint] where sPodiumPos[0] was meant (an original bug) */
    sPodiumPos[1] = height_get_ground(sPodiumPos[sPodiumTrackPoint], 999999, sPodiumPos[2]);
    mtx3i_from_angles(sPodiumRotMtx, 0, 0, sPodiumYaw);
    if (gDesaturation != 0 && gDesaturation < 0x100) {
        gDesaturation++;
    }
    if (gDesaturation == 0 || (gDesaturation < 0x100 && !(gRaceFrame & 1))) {
        sPodiumFrame++;
    }
    if (gDesaturation != 0x100) {
        gRaceFrame++;
        if (gRaceFrame & 1) {
            sPodiumFrame++;
        }
    }
    for (place = 0; place < gEvent.numRiders; place++) {
        yawHalves = (u16 *)&sPodiumYaw;
        gRiders[place].inactive = 0;
        gRiders[place].crashState = 1;
        frame = sPodiumFrame;
        skel = gRiders[place].riderSkel;
        clip = 0;
        while (frame >= sPodiumRiderClips[place][clip].pose->numFrames) {
            frame -= sPodiumRiderClips[place][clip].pose->numFrames;
            clip++;
            if (sPodiumRiderClips[place][clip].poseResId == 0) {
                /* the last clip loops; the first time, the scene starts to grey */
                clip--;
                frame %= sPodiumRiderClips[place][clip].pose->numFrames;
                if (gDesaturation == 0) {
                    gDesaturation = 1;
                    tunes_queue_crowd(2);
                }
                break;
            }
        }
        {
            s32 off = frame * sizeof(PodiumPoseFrame);
            pose = (PodiumPoseFrame *)((u8 *)sPodiumRiderClips[place][clip].pose->frames + off);
        }
        {
            /* the bones' rotations; the root's is in the world, turned by the podium's yaw */
            Rgb *angles = pose->rot;
            for (j = 0; j < 15; j++) {
                s32 *dst = skel->bones[j].rot.m;
                s32 x, y, z;
                if (j == 0) dst = skel->bones[0].worldRot.m;
                x = angles[j].r << 8; y = angles[j].g << 8; z = angles[j].b << 8;
                if (j == 0) z += sPodiumYaw;
                mtx3i_from_angles(dst, x, y, z);
            }
        }
        ang = (s32)(YAW16 << 16) >> 20;
        skel->bones[0].pos.x = 0;
        skel->bones[0].pos.y = 0;
        skel->bones[0].pos.z = 0;
        vx = pose->x;
        vz = pose->z;
        rotate_xz(&vx, &vz, ang);
        gRiders[place].drawPos.x = gRiders[place].drawBodyPos.x = sPodiumPos[0] + vx;
        gRiders[place].drawPos.y = gRiders[place].drawBodyPos.y = sPodiumPos[1] + pose->y;
        gRiders[place].drawPos.z = gRiders[place].drawBodyPos.z = sPodiumPos[2] + vz;
        skel_update_world_transforms(skel->bones, 15);
        if (clip != 0) {
            /* the helmet's prop clips, counted from the end of the first pose clip; the winner's holds its last
               frame, the others loop */
            clip = 1;
            frame = sPodiumFrame;
            frame -= sPodiumRiderClips[place][0].pose->numFrames;
            while (frame >= sPodiumRiderClips[place][clip].prop->numFrames) {
                frame -= sPodiumRiderClips[place][clip].prop->numFrames;
                clip++;
                if (sPodiumRiderClips[place][clip].propResId == 0) {
                    clip--;
                    if (place == 0) {
                        frame = sPodiumRiderClips[place][clip].prop->numFrames - 1;
                    } else {
                        frame %= sPodiumRiderClips[place][clip].prop->numFrames;
                    }
                    break;
                }
            }
            {
                s32 off = frame * sizeof(PodiumPropFrame);
                pose = (PodiumPoseFrame *)((u8 *)sPodiumRiderClips[place][clip].prop->frames + off);
            }
            {
                s32 *px = &sPodiumHelmetPos[place][0], *pz = &sPodiumHelmetPos[place][2];
                s32 x = ((PodiumPropFrame *)pose)->x, z = ((PodiumPropFrame *)pose)->z;
                s32 a = (s32)(YAW16 << 16) >> 20;
                s32 c = gCosTable[a & 0xFFF];
                s32 s = gCosTable[(a - 0x400) & 0xFFF];
                s32 zs, xs, zc, xc;

                xc = x * c;
                zc = z * c;
                xs = x * s;
                zs = z * s;
                *pz = (zc + xs) >> 14;
                *px = (xc - zs) >> 14;
            }
            sPodiumHelmetPos[place][1] = ((PodiumPropFrame *)pose)->y;
            sPodiumHelmetPos[place][0] += sPodiumPos[0];
            sPodiumHelmetPos[place][1] += sPodiumPos[1];
            sPodiumHelmetPos[place][2] += sPodiumPos[2];
            rot = (u8 *)((PodiumPropFrame *)pose)->rot;
            sPodiumHelmetRot[place][0] = rot[0];
            sPodiumHelmetRot[place][1] = rot[1];
            sPodiumHelmetRot[place][2] = rot[2] + YAW16;
        } else {
            /* the helmet on the head: 200 up and 50 forward of the head bone, turned with it */
            s32 *q = sPodiumHelmetPos[place];
            q[0] = skel->bones[PODIUM_HEAD_BONE].worldPos.x + gRiders[place].drawBodyPos.x;
            q[1] = skel->bones[PODIUM_HEAD_BONE].worldPos.y + gRiders[place].drawBodyPos.y;
            q[2] = skel->bones[PODIUM_HEAD_BONE].worldPos.z + gRiders[place].drawBodyPos.z;
            q[0] += (skel->bones[PODIUM_HEAD_BONE].worldRot.m[3] * 200) >> 15;
            q[1] += (skel->bones[PODIUM_HEAD_BONE].worldRot.m[4] * 200) >> 15;
            q[2] += (skel->bones[PODIUM_HEAD_BONE].worldRot.m[5] * 200) >> 15;
            q[0] += (skel->bones[PODIUM_HEAD_BONE].worldRot.m[6] * 50) >> 15;
            q[1] += (skel->bones[PODIUM_HEAD_BONE].worldRot.m[7] * 50) >> 15;
            q[2] += (skel->bones[PODIUM_HEAD_BONE].worldRot.m[8] * 50) >> 15;
            headRot = skel->bones[PODIUM_HEAD_BONE].worldRot;
            mtx3i_to_angles(headRot.m, &rx, &ry, &rz);
            sPodiumHelmetRot[place][0] = rx;
            sPodiumHelmetRot[place][1] = ry;
            sPodiumHelmetRot[place][2] = rz;
        }
    }
}

/* Frees the ceremony's matrices and stops its music. */
void podium_exit(void) {
    mem_free(sPodiumMtxBuf);
    tunes_stop_song();
}

/* Turns (x, z) by the 16-bit angle into (*outX, *outZ). Not called. */
void podium_rotate_xz(s32 angle, s32 x, s32 z, s32 *outX, s32 *outZ) {
    s16 *cosTable = gCosTable;
    s32 a = ((s32)(angle << 16)) >> 20;
    s32 cosA = cosTable[a & 0xFFF];
    s32 xCos = x * cosA;
    s32 zCos = z * cosA;
    s32 sinA = cosTable[(a - 0x400) & 0xFFF];
    s32 xSin = x * sinA;
    s32 zSin = z * sinA;

    *outZ = (zCos + xSin) >> 14;
    *outX = (xCos - zSin) >> 14;
}
