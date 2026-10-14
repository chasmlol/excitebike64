#include "game.h"

typedef struct { u8 pad[0x10]; void *p; } Obj_3;

extern Obj_3 *sPodiumModel;
extern Obj_3 *D_800E1B78;

typedef struct Rec_7 {
    s16 x, y, z, a, b, c;
} Rec_7;

typedef struct Seg {
    s32 f00;
    s32 len;
    s32 f08;
    s32 f0c;
    Rec_7 *data;
} Seg;

typedef struct Entry_3 {
    s32 w0;
    Seg *seg;
} Entry_3;

extern Entry_3 sPodiumCameraClips[];

void podium_update_camera(void)
{
    s32 t = sPodiumFrame;
    s32 n = 0;
    Rec_7 *p;
    s16 *tp;
    s32 o;
    u16 *ap;
    s32 ang, s, c, A, B, X, Y, as, bs, y2, x2;

    while (t >= sPodiumCameraClips[n].seg->len) {
        t -= sPodiumCameraClips[n].seg->len;
        n++;
        if (sPodiumCameraClips[n].w0 == 0) {
            n--;
            t = sPodiumCameraClips[n].seg->len - 1;
            break;
        }
    }

    o = t * 12;
    p = (Rec_7 *)((u8 *)sPodiumCameraClips[n].seg->data + o);
    A = p->x;
    gCamPos_5.x = A;
    B = p->z;
    B = (s16)B;
    gCamPos_5.z = B;
    tp = gCosTable;
    ang = (s16)sPodiumYaw >> 4;
    s = tp[ang & 0xFFF];
    c = tp[(ang - 0x400) & 0xFFF];
    as = A * s;
    bs = B * s;
    Y = (bs + A * c) >> 14;
    X = (as - B * c) >> 14;
    gCamPos_5.z = Y;
    x2 = sPodiumPos_2.x;
    y2 = Y + ((volatile Vec *)&sPodiumPos_2)->z;
    gCamPos_5.x = X;
    gCamPos_5.x = X + x2;
    gCamPos_5.z = y2;
    gCamPos_5.y = p->y + sPodiumPos_2.y;
    gCamPitch_2 = p->a;
    gCamRoll_2 = p->b;
    gCamYaw_2 = p->c + sPodiumYaw;
}

typedef struct { s32 *a; s32 *c; Obj_3 *b; Obj_3 *d; } Rec_8;
typedef struct { s32 *a; Obj_3 *b; } Rec2;

extern Rec_8 *sPodiumRiderClips[];
extern Rec2 sPodiumCameraClips_2[];
extern Obj_3 *sPodiumTexture;

void podium_init(void)
{
    s32 i;
    Rec_8 *r;
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
    tunes_play_song(D_800D11E4);
    sPodiumTexture = res_load(D_800E7728, D_800D10CC);
    sPodiumModel = res_load(D_800E7728, D_800D0F30);
    sPodiumMtxBuf = mem_alloc_dbg(0x200, i, D_800D6000, 0xFE);
    D_800E1B78 = res_load(D_800E7728, D_800D0E34);

    for (i = 0; i < 3; i++) {
        for (r = sPodiumRiderClips[i]; r->a != 0; r++) {
            r->b = res_load(D_800E7728, *r->a);
            r->b->p = (u8 *)r->b + 0x14;
            if (r->c != 0) {
                r->d = res_load(D_800E7728, *r->c);
                r->d->p = (u8 *)r->d + 0x1C;
            }
        }
    }
    for (i = 0; sPodiumCameraClips_2[i].a != 0; i++) {
        sPodiumCameraClips_2[i].b = res_load(D_800E7728, *sPodiumCameraClips_2[i].a);
        sPodiumCameraClips_2[i].b->p = (u8 *)sPodiumCameraClips_2[i].b + 0x1C;
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
    for (; i < gEvent.n; i++) {
        render_make_world_mtx((s32)&((Mtx *)(sPodiumMtxBuf + gGfxFrameIndex * 0x100))[i + 1], D_800E1B88[i][0], D_800E1B88[i][1], D_800E1B88[i][2], D_800E1BC8[i][0], D_800E1BC8[i][1], D_800E1BC8[i][2], 0x10, 0);
        W2(gdl++, 0xDA380003, (s32)&((Mtx *)(sPodiumMtxBuf + gGfxFrameIndex * 0x100))[i + 1]);
        W2(gdl++, 0xFD100000, gRiderGfx_8[i].tex);
        W2(gdl++, 0xE8000000, 0);
        W2(gdl++, 0xF5000100, 0x07000000);
        W2(gdl++, 0xE6000000, 0);
        W2(gdl++, 0xF0000000, 0x073FC000);
        W2(gdl++, 0xE7000000, 0);
        gdl = prim_build_dl(0, D_800E1B78, &gRiderGfx_8[i], gdl);
    }
    return gdl;
}

#define LO9C (cam[1])

typedef struct { s32 f0; s32 len; s32 pad8[2]; void *data; } Clip;
typedef struct { s32 f0; s32 f4; Clip *f8; Clip *fC; } Seg_2;
typedef struct { s16 x, y, z; Rgb c[15]; u8 pad; } Rec34;
typedef struct { s16 x, y, z; u8 c[3]; u8 pad[3]; } Rec12;

typedef struct {
    u8 pad0[4];
    s32 f4, f8, fC;
    u8 pad10[0x134 - 0x10];
    s32 pos[3];
    Mtx3 rot;
} Bike_2;

typedef struct {
    s32 f0, f4, f8;
    s32 fC, f10, f14;
    u8 pad18[0x44 - 0x18];
    u8 f44;
    u8 pad45[0x65C - 0x45];
    Bike_2 *bike;
    u8 pad660[0x7A5 - 0x660];
    u8 f7A5;
    u8 pad7A6[0x7E0 - 0x7A6];
} Player_6;

extern Seg_2 *sPodiumRiderClips_2[];
extern Player_6 gRiders_10[];

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
    Spot *e;
    Player_6 *p;
    Bike_2 *b;
    Rec34 *r;
    u8 *cp;
    u16 *cam;

    sPodiumPos[0] = (s32)((f32)(s32)gPathNodes_2[sPodiumTrackPoint].x * 1000.0f);
    sPodiumPos[2] = (s32)((f32)(s32)gPathNodes_2[sPodiumTrackPoint].z * 1000.0f);
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
    for (i = 0; i < gEventNumRiders; i++) {
        cam = (u16 *)&sPodiumYaw;
        gRiders_10[i].f7A5 = 0;
        gRiders_10[i].f44 = 1;
        t = sPodiumFrame;
        b = gRiders_10[i].bike;
        k = 0;
        while (t >= sPodiumRiderClips_2[i][k].f8->len) {
            t -= sPodiumRiderClips_2[i][k].f8->len;
            k++;
            if (sPodiumRiderClips_2[i][k].f0 == 0) {
                k--;
                t %= sPodiumRiderClips_2[i][k].f8->len;
                if (gDesaturation == 0) {
                    gDesaturation = 1;
                    tunes_queue_crowd(2);
                }
                break;
            }
        }
        { s32 off = t * sizeof(Rec34); r = (Rec34 *)((u8 *)sPodiumRiderClips_2[i][k].f8->data + off); }
        { Rgb *cc = r->c; for (j = 0; j < 15; j++) {
            s32 *dst = (s32 *)((u8 *)b + j * 0x80 + 0x10);
            s32 x, y, z;
            if (j == 0) dst = (s32 *)((u8 *)b + 0x40);
            x = cc[j].r << 8; y = cc[j].g << 8; z = cc[j].b << 8;
            if (j == 0) z += sPodiumYaw;
            mtx3i_from_angles(dst, x, y, z);
        } }
        ang = (s32)(LO9C << 16) >> 20;
        b->f4 = 0;
        b->f8 = 0;
        b->fC = 0;
        vx = r->x;
        vz = r->z;
        rot(&vx, &vz, ang);
        gRiders_10[i].f0 = gRiders_10[i].fC = sPodiumPos[0] + vx;
        gRiders_10[i].f4 = gRiders_10[i].f10 = sPodiumPos[1] + r->y;
        gRiders_10[i].f8 = gRiders_10[i].f14 = sPodiumPos[2] + vz;
        skel_update_world_transforms((s32 *)b, 15);
        if (k != 0) {
            k = 1;
            t = sPodiumFrame;
            t -= sPodiumRiderClips_2[i][0].f8->len;
            while (t >= sPodiumRiderClips_2[i][k].fC->len) {
                t -= sPodiumRiderClips_2[i][k].fC->len;
                k++;
                if (sPodiumRiderClips_2[i][k].f4 == 0) {
                    k--;
                    if (i == 0) {
                        t = sPodiumRiderClips_2[i][k].fC->len - 1;
                    } else {
                        t %= sPodiumRiderClips_2[i][k].fC->len;
                    }
                    break;
                }
            }
            { s32 off = t * sizeof(Rec12); r = (Rec34 *)((u8 *)sPodiumRiderClips_2[i][k].fC->data + off); }
            {
                s32 *px = &D_800E1BC8[i][0], *pz = &D_800E1BC8[i][2];
                s32 x = ((Rec12 *)r)->x, z = ((Rec12 *)r)->z;
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
            D_800E1BC8[i][1] = ((Rec12 *)r)->y;
            D_800E1BC8[i][0] += sPodiumPos[0];
            D_800E1BC8[i][1] += sPodiumPos[1];
            D_800E1BC8[i][2] += sPodiumPos[2];
            cp = (u8 *)((Rec12 *)r)->c;
            D_800E1B88[i][0] = cp[0];
            D_800E1B88[i][1] = cp[1];
            D_800E1B88[i][2] = cp[2] + LO9C;
        } else {
            s32 *q = D_800E1BC8[i];
            q[0] = b->pos[0] + gRiders_10[i].fC;
            q[1] = b->pos[1] + gRiders_10[i].f10;
            q[2] = b->pos[2] + gRiders_10[i].f14;
            q[0] += (b->rot.m[3] * 200) >> 15;
            q[1] += (b->rot.m[4] * 200) >> 15;
            q[2] += (b->rot.m[5] * 200) >> 15;
            q[0] += (b->rot.m[6] * 50) >> 15;
            q[1] += (b->rot.m[7] * 50) >> 15;
            q[2] += (b->rot.m[8] * 50) >> 15;
            m = b->rot;
            mtx3i_to_angles(m.m, &o0, &o1, &o2);
            D_800E1B88[i][0] = o0;
            D_800E1B88[i][1] = o1;
            D_800E1B88[i][2] = o2;
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
