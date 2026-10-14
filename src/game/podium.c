#include "game.h"

typedef struct { u8 pad[0x10]; void *p; } Obj_3;

extern Obj_3 *sPodiumModel;
extern Obj_3 *sPodiumHelmetModel;

extern PodiumCameraClip sPodiumCameraClips[];

/* view 0's camera; every store is kept */
#define CAM0 (*(volatile Vec3i *)&gCamPos[0])

void podium_update_camera(void)
{
    s32 t = sPodiumFrame;
    s32 n = 0;
    PodiumCamFrame *p;
    s16 *tp;
    s32 o;
    u16 *ap;
    s32 ang, s, c, A, B, X, Y, as, bs, y2, x2;

    while (t >= sPodiumCameraClips[n].clip->numFrames) {
        t -= sPodiumCameraClips[n].clip->numFrames;
        n++;
        if (sPodiumCameraClips[n].resId == 0) {
            n--;
            t = sPodiumCameraClips[n].clip->numFrames - 1;
            break;
        }
    }

    o = t * 12;
    p = (PodiumCamFrame *)((u8 *)sPodiumCameraClips[n].clip->frames + o);
    A = p->x;
    CAM0.x = A;
    B = p->z;
    B = (s16)B;
    CAM0.z = B;
    tp = gCosTable;
    ang = (s16)sPodiumYaw >> 4;
    s = tp[ang & 0xFFF];
    c = tp[(ang - 0x400) & 0xFFF];
    as = A * s;
    bs = B * s;
    Y = (bs + A * c) >> 14;
    X = (as - B * c) >> 14;
    CAM0.z = Y;
    x2 = sPodiumPos_2.x;
    y2 = Y + ((volatile Vec *)&sPodiumPos_2)->z;
    CAM0.x = X;
    CAM0.x = X + x2;
    CAM0.z = y2;
    CAM0.y = p->y + sPodiumPos_2.y;
    gCamPitch[0] = p->pitch;
    gCamRoll[0] = p->roll;
    gCamYaw[0] = p->yaw + sPodiumYaw;
}

extern PodiumRiderClip *sPodiumRiderClips[];
extern Obj_3 *sPodiumTexture;

void podium_init(void)
{
    s32 i;
    PodiumRiderClip *r;
    Obj_3 *o;

    gRaceFrame = 0;
    sPodiumFrame = 0;
    sPodiumYaw = 0x8000;
    i = ((gCurTrack_6 - (s32)gTracks) * -0x33333333) >> 5;
    sPodiumTrackPoint = 0;
    if (i == 9) {
        sPodiumTrackPoint = 0x46;
    } else if (i == 0xe) {
        sPodiumTrackPoint = 100;
        sPodiumYaw = 24000;
    } else if (i == 0x13) {
        sPodiumTrackPoint = 0x14;
        sPodiumYaw = -2000;
    }
    i = 0;
    gDesaturation = 0;
    tunes_play_song(gPodiumSongResId);
    sPodiumTexture = res_load(gResourceSet, sPodiumTextureResId);
    sPodiumModel = res_load(gResourceSet, sPodiumModelResId);
    sPodiumMtxBuf = mem_alloc_dbg(0x200, i, sStrPodiumFile, 0xFE);
    sPodiumHelmetModel = res_load(gResourceSet, sPodiumHelmetResId);

    for (i = 0; i < 3; i++) {
        for (r = sPodiumRiderClips[i]; r->poseResId != 0; r++) {
            r->pose = (PodiumClip *)res_load(gResourceSet, *r->poseResId);
            r->pose->frames = (u8 *)r->pose + 0x14;
            if (r->propResId != 0) {
                r->prop = (PodiumClip *)res_load(gResourceSet, *r->propResId);
                r->prop->frames = (u8 *)r->prop + 0x1C;
            }
        }
    }
    for (i = 0; sPodiumCameraClips[i].resId != 0; i++) {
        sPodiumCameraClips[i].clip = (PodiumClip *)res_load(gResourceSet, *sPodiumCameraClips[i].resId);
        sPodiumCameraClips[i].clip->frames = (u8 *)sPodiumCameraClips[i].clip + 0x1C;
    }
    palmod_register(sPodiumTexture, 0);
}

#define W2(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

typedef struct {
    u8 pad[0x14];
    u32 tex;
    u8 pad2[0xA80 - 0x18];
} Plr_3;

extern Plr_3 gRiderGfx_8[];

Gfx *podium_draw(Gfx *gdl, s32 x, s32 arg2)
{
    s32 i;
    s32 off;
    Plr_3 *pl;
    Plr_3 *cur;
    s32 *pc;
    s16 *ps;

    render_set_viewport(gdl, x, 0, 1);
    W2(gdl++, 0xE3001001, 0x8000);
    W2(gdl++, 0xE200001C, 0x442230);
    W2(gdl++, 0xFCFFFFFF, 0xFFFCF87C);
    W2(gdl++, 0xFD100000, sPodiumTexture_2[1]);
    W2(gdl++, 0xE8000000, 0);
    W2(gdl++, 0xF5000100, 0x07000000);
    W2(gdl++, 0xE6000000, 0);
    W2(gdl++, 0xF0000000, 0x073FC000);
    W2(gdl++, 0xE7000000, 0);
    i = 0;
    render_make_world_mtx(sPodiumMtxBuf + gGfxFrameIndex * 0x100, 0, 0, sPodiumYaw, sPodiumPos[0], sPodiumPos[1], sPodiumPos[2], 0x10, 0);
    W2(gdl++, 0xDA380003, sPodiumMtxBuf + gGfxFrameIndex * 0x100);
    gdl = prim_build_dl(0, sPodiumModel, &sPodiumTexture_2, gdl);
    for (; i < gEvent.numRiders; i++) {
        render_make_world_mtx((s32)&((Mtx *)(sPodiumMtxBuf + gGfxFrameIndex * 0x100))[i + 1], sPodiumHelmetRot[i][0], sPodiumHelmetRot[i][1], sPodiumHelmetRot[i][2], sPodiumHelmetPos[i][0], sPodiumHelmetPos[i][1], sPodiumHelmetPos[i][2], 0x10, 0);
        W2(gdl++, 0xDA380003, (s32)&((Mtx *)(sPodiumMtxBuf + gGfxFrameIndex * 0x100))[i + 1]);
        W2(gdl++, 0xFD100000, gRiderGfx_8[i].tex);
        W2(gdl++, 0xE8000000, 0);
        W2(gdl++, 0xF5000100, 0x07000000);
        W2(gdl++, 0xE6000000, 0);
        W2(gdl++, 0xF0000000, 0x073FC000);
        W2(gdl++, 0xE7000000, 0);
        gdl = prim_build_dl(0, sPodiumHelmetModel, &gRiderGfx_8[i], gdl);
    }
    return gdl;
}

#define LO9C (cam[1])

static inline void rot(s32 *px, s32 *pz, s32 ang)
{
    s32 c = gCosTable[ang & 0xFFF];
    s32 s = gCosTable[(ang - 0x400) & 0xFFF];
    s32 t = (*px * c - *pz * s) >> 14;

    *pz = (*pz * c + *px * s) >> 14;
    *px = t;
}

void podium_update(void)
{
    Mtx3 m;
    s32 vx, vz;
    u16 o0, o1, o2;
    s32 i, j, k, t, ang, c, s;
    PathNode *e;
    Rider *p;
    RiderSkeleton *b;
    PodiumPoseFrame *r;
    u8 *cp;
    u16 *cam;

    sPodiumPos[0] = (s32)((f32)(s32)gPathNodes[sPodiumTrackPoint].pos.x * 1000.0f);
    sPodiumPos[2] = (s32)((f32)(s32)gPathNodes[sPodiumTrackPoint].pos.z * 1000.0f);
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
    for (i = 0; i < gEvent.numRiders; i++) {
        cam = (u16 *)&sPodiumYaw;
        gRiders[i].inactive = 0;
        gRiders[i].crashState = 1;
        t = sPodiumFrame;
        b = gRiders[i].riderSkel;
        k = 0;
        while (t >= sPodiumRiderClips[i][k].pose->numFrames) {
            t -= sPodiumRiderClips[i][k].pose->numFrames;
            k++;
            if (sPodiumRiderClips[i][k].poseResId == 0) {
                k--;
                t %= sPodiumRiderClips[i][k].pose->numFrames;
                if (gDesaturation == 0) {
                    gDesaturation = 1;
                    tunes_queue_crowd(2);
                }
                break;
            }
        }
        { s32 off = t * sizeof(PodiumPoseFrame); r = (PodiumPoseFrame *)((u8 *)sPodiumRiderClips[i][k].pose->frames + off); }
        { Rgb *cc = r->rot; for (j = 0; j < 15; j++) {
            s32 *dst = b->bones[j].rot.m;
            s32 x, y, z;
            if (j == 0) dst = b->bones[0].worldRot.m;
            x = cc[j].r << 8; y = cc[j].g << 8; z = cc[j].b << 8;
            if (j == 0) z += sPodiumYaw;
            mtx3i_from_angles(dst, x, y, z);
        } }
        ang = (s32)(LO9C << 16) >> 20;
        b->bones[0].pos.x = 0;
        b->bones[0].pos.y = 0;
        b->bones[0].pos.z = 0;
        vx = r->x;
        vz = r->z;
        rot(&vx, &vz, ang);
        gRiders[i].drawPos.x = gRiders[i].drawBodyPos.x = sPodiumPos[0] + vx;
        gRiders[i].drawPos.y = gRiders[i].drawBodyPos.y = sPodiumPos[1] + r->y;
        gRiders[i].drawPos.z = gRiders[i].drawBodyPos.z = sPodiumPos[2] + vz;
        skel_update_world_transforms(b->bones, 15);
        if (k != 0) {
            k = 1;
            t = sPodiumFrame;
            t -= sPodiumRiderClips[i][0].pose->numFrames;
            while (t >= sPodiumRiderClips[i][k].prop->numFrames) {
                t -= sPodiumRiderClips[i][k].prop->numFrames;
                k++;
                if (sPodiumRiderClips[i][k].propResId == 0) {
                    k--;
                    if (i == 0) {
                        t = sPodiumRiderClips[i][k].prop->numFrames - 1;
                    } else {
                        t %= sPodiumRiderClips[i][k].prop->numFrames;
                    }
                    break;
                }
            }
            { s32 off = t * sizeof(PodiumPropFrame); r = (PodiumPoseFrame *)((u8 *)sPodiumRiderClips[i][k].prop->frames + off); }
            {
                s32 *px = &sPodiumHelmetPos[i][0], *pz = &sPodiumHelmetPos[i][2];
                s32 x = ((PodiumPropFrame *)r)->x, z = ((PodiumPropFrame *)r)->z;
                s32 a = (s32)(LO9C << 16) >> 20;
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
            sPodiumHelmetPos[i][1] = ((PodiumPropFrame *)r)->y;
            sPodiumHelmetPos[i][0] += sPodiumPos[0];
            sPodiumHelmetPos[i][1] += sPodiumPos[1];
            sPodiumHelmetPos[i][2] += sPodiumPos[2];
            cp = (u8 *)((PodiumPropFrame *)r)->rot;
            sPodiumHelmetRot[i][0] = cp[0];
            sPodiumHelmetRot[i][1] = cp[1];
            sPodiumHelmetRot[i][2] = cp[2] + LO9C;
        } else {
            s32 *q = sPodiumHelmetPos[i];
            q[0] = b->bones[2].worldPos.x + gRiders[i].drawBodyPos.x;
            q[1] = b->bones[2].worldPos.y + gRiders[i].drawBodyPos.y;
            q[2] = b->bones[2].worldPos.z + gRiders[i].drawBodyPos.z;
            q[0] += (b->bones[2].worldRot.m[3] * 200) >> 15;
            q[1] += (b->bones[2].worldRot.m[4] * 200) >> 15;
            q[2] += (b->bones[2].worldRot.m[5] * 200) >> 15;
            q[0] += (b->bones[2].worldRot.m[6] * 50) >> 15;
            q[1] += (b->bones[2].worldRot.m[7] * 50) >> 15;
            q[2] += (b->bones[2].worldRot.m[8] * 50) >> 15;
            m = b->bones[2].worldRot;
            mtx3i_to_angles(m.m, &o0, &o1, &o2);
            sPodiumHelmetRot[i][0] = o0;
            sPodiumHelmetRot[i][1] = o1;
            sPodiumHelmetRot[i][2] = o2;
        }
    }
}

void podium_exit(void) {
    mem_free(sPodiumMtxBuf);
    tunes_stop_song();
}

void podium_rotate_xz(s32 a0, s32 a1, s32 a2, s32 *p4, s32 *p5) {
    s16 *tab = gCosTable;
    s32 idx = ((s32)(a0 << 16)) >> 20;
    s32 s1 = tab[idx & 0xFFF];
    s32 m1 = a1 * s1;
    s32 m2 = a2 * s1;
    s32 s2 = tab[(idx - 0x400) & 0xFFF];
    s32 m3 = a1 * s2;
    s32 m4 = a2 * s2;

    *p5 = (m2 + m3) >> 14;
    *p4 = (m1 - m4) >> 14;
}
