#include "game.h"

/* gCurTrack as this file sees it: the CPU riders' time scale per difficulty */
typedef struct {
    u8 pad0[0x20];
    f32 timeScale[1];
} Ply;

extern Ply *gCurTrack_31;

/* A debug display whose printing is stubbed out: C-up pages through tuning values, the last impact, and the CPU
   riders' time scale and skill. */
void debug_update_value_display(void)
{
    s32 i;
    char buf[0x28];

    if (gPlayerInputs_2 & INPUT_C_UP) {
        if (gPlayerInputsPrev == 0) {
            sDebugPage++;
            if (sDebugPage == 4) {
                sDebugPage = 0;
            }
        }
    }
    for (i = 0x13; i >= 0; i--) {
    }
    switch (sDebugPage) {
    case 1:
        if (*(s32 *)((u8 *)gCurTrack_31 + 0x2C) == TRACK_ENV_DESERT) {
            break;
        }
        if (D_800C8D08 != 0 || D_800C8D10 != 0 || D_800C8D00 != 0 || D_800C8D18 != 0) {
            break;
        }
        debug_print_float(sStrDebugImpact, gDebugValue0);
        debug_print_float(sStrDebugExit, gDebugValue1);
        break;
    case 0:
        debug_print_float(sStrDebug0, gDebugValue0);
        debug_print_float(sStrDebug1, gDebugValue1);
        debug_print_float(sStrDebug2, gDebugValue2);
        debug_print_float(sStrDebug3, gDebugValue3);
        debug_print_float(sStrDebug4, gDebugValue4);
        debug_print_float(sStrDebug5, gDebugValue5);
        break;
    case 2:
        for (i = 0; i < 6; i++) {
            debug_ftoa(buf, gRiders[i].timeScale / gCurTrack_31->timeScale[gGame.difficulty - 1], 4);
            debug_print_float(buf, gRiders[i].aiSkill);
        }
        break;
    }
}

/* The text buffers of debug_dump_spline, in the order of its stack frame */
typedef struct {
    u8 x[16];
    u8 y[16];
    u8 z[16];
    u8 count[16];
} SplineText;

/* Writes the recorded spline as text to the (stubbed-out) debug log. */
void debug_dump_spline(void)
{
    SplineText text;
    s32 k;
    u32 groundY;

    debug_log(sStrSplineFileHeader);
    int_to_str(sSplineNodeCount, text.count);
    debug_log(text.count);
    debug_log(sStrSplineCountComment);
    for (k = 1; k <= sSplineNodeCount; k++) {
        int_to_str((s32)(sSplineNodes[k].x * 1000.0f), text.x);
        int_to_str((s32)(sSplineNodes[k].z * 1000.0f), text.z);
        groundY = height_get_ground((s32)(sSplineNodes[k].x * 1000.0f), (s32)(sSplineNodes[k].y * 1000.0f), (s32)(sSplineNodes[k].z * 1000.0f));
        int_to_str(groundY, text.y);
        debug_log(text.x);
        debug_log(sStrSplineSep);
        debug_log(text.z);
        debug_log(sStrSplineNodeTail);
    }
    debug_log(sStrSplineFileFooter);
}

/* A rider as this file sees it: the bike's position (Rider.bike.pos) */
struct Obj_40 {
    u8 pad[0x48];
    Vec3f pos;
};

/* Adds the rider's position to the spline recorder (up to 2000 nodes) when it moved more than a metre. */
void debug_record_spline_node(Obj_40 *body)
{
    s32 n;
    s32 newCount;
    Vec3f **nodes;
    Vec3f *last;
    Vec3f *next;
    f32 dx;
    f32 dy;
    f32 dz;

    if (sSplineNodeCount < 2000) {
        nodes = &sSplineNodes;
        n = sSplineNodeCount;
        last = (Vec3f *)((u32)(n * 12) + (u32)*nodes);
        dx = last->x - body->pos.x;
        dy = last->y - body->pos.y;
        dz = last->z - body->pos.z;
        if (1.0f < dx * dx + dy * dy + dz * dz) {
            newCount = n + 1;
            next = (Vec3f *)((u32)(newCount * 12) + (u32)*nodes);
            sSplineNodeCount = newCount;
            *next = body->pos;
        }
    }
}

typedef struct {
    u8 c[4];
} S4_2;

extern S4_2 sStrFtoaBig;

/* Formats a float with up to `digits` digits ("BIG" past a million). */
void debug_ftoa(char *out, f32 value, s32 digits)
{
    s32 intPart;
    s32 pow10;
    s32 d;

    if (value < 0.0f) {
        *out++ = '-';
        value = -value;
    } else {
        *out++ = ' ';
    }
    if (value >= 1.0f) {
        if (value > 1000000.0f) {
            *(S4_2 *)out = sStrFtoaBig;
            return;
        }
        pow10 = 10;
        intPart = (s32)value;
        value -= (f32)intPart;
        while (intPart >= pow10) {
            pow10 *= 10;
        }
        pow10 /= 10;
        do {
            digits--;
            *out++ = intPart / pow10 + '0';
            intPart -= (intPart / pow10) * pow10;
            pow10 /= 10;
        } while (pow10 != 0);
    } else {
        *out++ = '0';
    }
    if (digits > 0) {
        if (value > 0.0001f) {
            *out++ = '.';
        }
        do {
            digits--;
            {
                f32 t = value;
                if (!(t > 0.0001f)) break;
            }
            intPart = (s32)(value * 10.0f + 0.0001f);
            *out++ = intPart + '0';
            value = value * 10.0f - (f32)intPart;
        } while (digits > 0);
    }
    *out = '\0';
}

f32 func_8005CEFC(f32 x)
{
    if (!(0.0f < x)) {
        x = -x;
    }
    return x / 200.0f;
}

/* Rounds to three significant digits. */
f32 debug_round_sig3(f32 x)
{
    f32 mag;
    f32 pow10;
    f32 orig;
    f32 res;

    orig = x;
    mag = x;
    if (0.0f < orig) {
        mag = x;
    } else {
        mag = -x;
    }
    pow10 = 1.0f;
    for (; pow10 < mag; pow10 = pow10 * 10.0f) {
    }
    for (; mag < pow10; pow10 = pow10 / 10.0f) {
    }
    res = mag;
    if (pow10 == 0.0f) {
        return res;
    }
    mag = ((f32)(s32)((mag * 100.0f) / pow10 + 0.5f) * pow10) / 100.0f;
    if (0.0f < orig) {
        return mag;
    }
    return -mag;
}

/* The debug printers: each formats its line into a buffer and prints nothing. */
void debug_print_float(char *label, f32 value)
{
    char buf[40];
    s32 len;
    str_copy(buf, label);
    len = strlen(buf);
    debug_ftoa(buf + len, value, 3);
}

void debug_print_float2(char *label, f32 a, f32 b)
{
    char buf[40];
    s32 n;
    s32 i;

    mem_fill((u8 *)buf, 0x20, 0x1E);
    str_copy(buf, label);
    n = strlen(buf);
    debug_ftoa(buf + n, a, 3);
    i = 0;
    if (0 < n + 6) {
        do {
            if (buf[i] == 0) {
                buf[i] = ' ';
            }
            i++;
        } while (i < n + 6);
    }
    debug_ftoa(buf + 6 + n, b, 3);
}

void debug_print_vec3(s32 unused, f32 *v)
{
    char bx[24];
    char by[24];
    char bz[24];

    strcpy(bx, "\nx=");
    debug_ftoa(bx + strlen(bx), v[0], 6);
    strcpy(by, "y=");
    debug_ftoa(by + strlen(by), v[1], 6);
    strcpy(bz, "z=");
    debug_ftoa(bz + strlen(bz), v[2], 6);
}

void debug_print_int(char *label, s32 value) {
    char pad[16];
    char buf[40];
    char *b;

    b = buf;
    str_copy(b, label);
    int_to_str(value, b + strlen(b));
}

void debug_print_hex(char *label, u32 value) {
    char buf[56];
    char *s = buf + 16;
    str_copy(s, label);
    u32_to_hex_str(value, s + strlen(s));
}

/* Allocates the 128 tire-track quads and their vertices, and builds the 8x1 IA8 texture of a track. */
void decal_init(void)
{
    s32 i;

    sTireTracks = mem_alloc_dbg(DECAL_NUM_TRACKS * sizeof(TireTrack), 0, (s32)sStrDecalC, 0x67);
    sTireTrackVtx = mem_alloc_dbg(DECAL_NUM_TRACKS * 4 * sizeof(Vtx), 0, (s32)sStrDecalC, 0x68);
    sTireTrackNext = 0;
    mem_fill((u8 *)sTireTrackCur, 0, 0x20);
    mem_fill((u8 *)gTireTrackTimers, 0, 0x20);
    mem_fill((u8 *)sTireTrackLastY, 0, 0x20);
    mem_fill((u8 *)sTireTrackMudTimer, 0, 0x20);
    mem_fill((u8 *)sTireTracks, 0, DECAL_NUM_TRACKS * sizeof(TireTrack));
    mem_fill((u8 *)sTireTrackSurface, 0, 0x20);
    mem_fill((u8 *)sTireTrackMudSurface, 0, 0x20);
    for (i = 0; i < DECAL_NUM_TRACKS; i++) {
        sTireTracks[i].active = 0;
    }
    for (i = 0; i < DECAL_NUM_TRACKS * 4; i += 4) {
        sTireTrackVtx[i].v.tc[0] = 0;
        sTireTrackVtx[i].v.tc[1] = 0;
        sTireTrackVtx[i + 1].v.tc[0] = 0x200;
        sTireTrackVtx[i + 1].v.tc[1] = 0;
        sTireTrackVtx[i + 2].v.tc[0] = 0;
        sTireTrackVtx[i + 2].v.tc[1] = 0;
        sTireTrackVtx[i + 3].v.tc[0] = 0x200;
        sTireTrackVtx[i + 3].v.tc[1] = 0;
    }
    sTireTrackTexture = mem_alloc_dbg(8, 0, (s32)sStrDecalC, 0x85);
    sTireTrackTexture[0] = 0xFF;
    sTireTrackTexture[1] = 0xDC;
    sTireTrackTexture[2] = 0x88;
    sTireTrackTexture[3] = 0x58;
    sTireTrackTexture[4] = 0x38;
    sTireTrackTexture[5] = 0x58;
    sTireTrackTexture[6] = 0x8C;
    sTireTrackTexture[7] = 0x1F;
}

/* Lays tire tracks under the rear wheel of each skidding rider (and for 45 frames after leaving mud): the current
   quad stretches while the track keeps its direction and area, else a new one starts from its far edge. */
void decal_update_tire_tracks(void)
{
    s32 contact[3];
    s32 edgeA[3];
    s32 edgeB[3];
    s32 half[3];
    s32 rot[10];
    s32 prevA[3];
    s32 prevB[3];
    s32 blk[4];
    s32 i, changed, area, surface, originX, originZ, dx, dz, quadX, quadZ, len, curArea;
    Rider *rider;
    Surface *prevSurface;
    TireTrack *cur;
    s32 n;
    s32 dist;

    for (i = 0; i < 8; i++) {
        if (gRiders[i].inactive != 0) continue;
        rider = &gRiders[i];
        if (sTireTrackMudTimer[i] != 0) sTireTrackMudTimer[i]--;
        if (gRiders[i].rearWheel.slip != 0.0f) {
            gTireTrackTimers[i] = 1;
        } else if (gTireTrackTimers[i] != 0) {
            gTireTrackTimers[i]--;
        }
        if (!(rider->rearWheel.surface->flags & 1)) gTireTrackTimers[i] = 0; /* a surface that takes tracks */
        prevSurface = sTireTrackSurface[i];
        if (rider->rearWheel.surface != prevSurface) {
            changed = 1;
            if (prevSurface != NULL && (prevSurface->flags & 0x40) && rider->rearWheel.airTicks <= 0) { /* left mud */
                sTireTrackMudSurface[i] = prevSurface;
                gTireTrackTimers[i] = 45;
                sTireTrackMudTimer[i] = 45;
            }
        } else {
            changed = 0;
        }
        sTireTrackSurface[i] = rider->rearWheel.surface;
        if (rider->crashState != 0) gTireTrackTimers[i] = 0;
        if (gDebugFreeze != 0) gTireTrackTimers[i] = 0;
        if (gTireTrackTimers[i] == 0) {
            sTireTrackCur[i] = NULL;
            continue;
        }
        contact[0] = rider->rearWheel.contact.x * 1000.0f;
        contact[1] = rider->rearWheel.contact.y * 1000.0f;
        contact[2] = rider->rearWheel.contact.z * 1000.0f;
        quat_to_mtx3i(rot, rider->drawQuat);
        half[0] = -rider->rearWheel.contactDelta.z * 1000.0f;
        half[2] = rider->rearWheel.contactDelta.x * 1000.0f;
        len = isqrt_u32_2(half[0] * half[0] + half[2] * half[2]);
        if (len == 0) {
            half[1] = 0;
            half[0] = rot[0];
            half[2] = rot[2];
            len = isqrt_u32_2(rot[0] * rot[0] + rot[2] * rot[2]);
        }
        if (len != 0) {
            half[0] = half[0] * 50 / len;
            half[2] = half[2] * 50 / len;
        }
        edgeA[0] = contact[0] - half[0];
        edgeA[2] = contact[2] - half[2];
        edgeB[0] = contact[0] + half[0];
        edgeB[2] = contact[2] + half[2];
        edgeA[1] = height_get_ground(edgeA[0], contact[1], edgeA[2]);
        edgeB[1] = height_get_ground(edgeB[0], contact[1], edgeB[2]);
        if (gGroundFound == 0) continue;
        area = gGroundArea;
        if (gCurTrack_5->mode == TRACK_ENV_DESERT) {
            /* the desert's area: its block of 8000 units (128 m), x << 16 | z */
            divmod_floor(contact[0], 128000, &blk[0], &blk[1]);
            divmod_floor(contact[2], 128000, &blk[2], &blk[3]);
            area = (blk[0] << 16) | (u16)blk[2];
        }
        /* a step of more than half a metre ends the track */
        if (sTireTrackCur[i] != NULL && (u32)(edgeA[1] - sTireTrackLastY[i] + 500) > 1000) {
            sTireTrackCur[i] = NULL;
            gTireTrackTimers[i] = 0;
            continue;
        }
        surface = rider->rearWheel.surface - gSurfaceTablePtr;
        if (sTireTrackMudTimer[i] != 0) surface = sTireTrackMudSurface[i] - gSurfaceTablePtr;
        if (sTireTrackCur[i] != NULL) {
            curArea = sTireTrackCur[i]->area;
            if (gCurTrack_5->mode == TRACK_ENV_OUTDOOR) {
                originX = (sHeightAreas->e[curArea].x0 + sHeightAreas->e[curArea].x1) / 2;
                originZ = (sHeightAreas->e[curArea].z0 + sHeightAreas->e[curArea].z1) / 2;
            } else if (gCurTrack_5->mode == TRACK_ENV_DESERT) {
                originX = (curArea >> 16) * 8000 + 4000;
                originZ = (s16)curArea * 8000 + 4000;
            } else {
                originX = originZ = 0;
            }
            cur = sTireTrackCur[i];
            dx = contact[0] - cur->x;
            dz = contact[2] - cur->z;
            quadX = cur->dirX;
            quadZ = cur->dirZ;
            dist = isqrt_u32_2(dx * dx + dz * dz);
            if (dist != 0) {
                dx = (dx << 14) / dist;
                dz = (dz << 14) / dist;
            }
            /* turned by more than about 10 degrees (cos 0.984), another area or surface: a new quad */
            if ((quadX * dx + quadZ * dz) >> 14 < 0x3F07 || area != sTireTrackCur[i]->area || changed) {
                Vtx *v;
                s16 *q, *q2;
                len = sTireTrackCur[i] - sTireTracks;
                v = (Vtx *)((len << 6) + (u32)sTireTrackVtx);
                q = v[2].v.ob;
                prevA[0] = q[0] + originX;
                prevA[1] = q[1];
                prevA[2] = q[2] + originZ;
                q2 = v[3].v.ob;
                prevB[0] = q2[0] + originX;
                prevB[1] = q2[1];
                prevB[2] = q2[2] + originZ;
                prevA[0] *= 16;
                prevA[1] *= 16;
                prevA[2] *= 16;
                prevB[0] *= 16;
                prevB[1] *= 16;
                prevB[2] *= 16;
                sTireTrackCur[i] = decal_add_quad(area, prevA, prevB, edgeA, edgeB, surface);
                sTireTrackLastY[i] = edgeA[1];
            } else {
                sTireTrackCur[i]->x = contact[0];
                sTireTrackCur[i]->z = contact[2];
                {
                Vtx *v;
                len = sTireTrackCur[i] - sTireTracks;
                v = &sTireTrackVtx[len * 4];
                v[2].v.ob[0] = edgeA[0] / 16 - originX;
                v[2].v.ob[1] = edgeA[1] / 16;
                v[2].v.ob[2] = edgeA[2] / 16 - originZ;
                v[3].v.ob[0] = edgeB[0] / 16 - originX;
                v[3].v.ob[1] = edgeB[1] / 16;
                v[3].v.ob[2] = edgeB[2] / 16 - originZ;
                }
                sTireTrackLastY[i] = edgeA[1];
            }
        } else {
            sTireTrackCur[i] = decal_add_quad(area, edgeA, edgeB, edgeA, edgeB, surface);
            sTireTrackLastY[i] = edgeA[1];
        }
    }
}

typedef struct { s32 pad[11]; s32 mode; } Ctl_17;

extern Ctl_17 *gCurTrack_32;

/* Starts a quad from the near edge a0-a1 to the far edge b0-b1 (mm) and returns it. */
TireTrack *decal_add_quad(s32 area, s32 *a0, s32 *a1, s32 *b0, s32 *b1, s32 surface)
{
    s32 i, j, mode;
    s32 originX, originZ;
    s32 farZ, dx, dz;
    u32 len;
    Vtx *vtx;
    Vtx *q;
    TireTrack *track;
    TireTrack *same;
    TireTrack *tracks;

    for (i = 0; i < 4; i++) {
        if (&sTireTracks[sTireTrackNext] == sTireTrackCur[i]) sTireTrackCur[i] = 0;
    }

    mode = gCurTrack_32->mode;
    /* vertices relative to the centre of the area */
    if (mode == TRACK_ENV_OUTDOOR) {
        originX = (sHeightAreas->e[area].x0 + sHeightAreas->e[area].x1) / 2;
        originZ = (sHeightAreas->e[area].z0 + sHeightAreas->e[area].z1) / 2;
    } else if (mode == TRACK_ENV_DESERT) {
        originX = (s16)(area >> 16) * 8000 + 4000;
        originZ = (s16)area * 8000 + 4000;
    } else {
        originX = originZ = 0;
    }

    j = sTireTrackNext * 4;
    vtx = (Vtx *)((u32)(j * 16) + (u32)sTireTrackVtx);
    vtx[0].v.ob[0] = (a0[0] >> 4) - originX;
    vtx[0].v.ob[1] = a0[1] >> 4;
    vtx[0].v.ob[2] = (a0[2] >> 4) - originZ;
    vtx[1].v.ob[0] = (a1[0] >> 4) - originX;
    q = &vtx[1];
    q->v.ob[1] = a1[1] >> 4;
    q->v.ob[2] = (a1[2] >> 4) - originZ;
    vtx[2].v.ob[0] = (b0[0] >> 4) - originX;
    q = &vtx[2];
    q->v.ob[1] = b0[1] >> 4;
    q->v.ob[2] = (b0[2] >> 4) - originZ;
    vtx[3].v.ob[0] = (b1[0] >> 4) - originX;
    vtx = &vtx[3];
    vtx->v.ob[1] = b1[1] >> 4;
    vtx->v.ob[2] = (b1[2] >> 4) - originZ;

    j = j / 4;
    tracks = sTireTracks;
    track = (TireTrack *)((s32)(j * 24) + (s32)tracks);
    track->x = (b0[0] + b1[0]) / 2;
    track->z = farZ = (b0[2] + b1[2]) / 2;
    dx = track->x - (a0[0] + a1[0]) / 2;
    dz = farZ - (a0[2] + a1[2]) / 2;
    len = isqrt_u32_3(dx * dx + dz * dz);
    if (len != 0) {
        dx = (dx * 0x4000) / (s32)len;
        dz = (dz * 0x4000) / (s32)len;
    }

    same = (TireTrack *)((u32)(j * 24) + (u32)sTireTracks); /* the same quad, through a second pointer */
    same->active = 1;
    same->dirX = dx;
    same->dirZ = dz;
    same->area = area;
    sTireTracks[j].surface = surface;

    sTireTrackNext = (sTireTrackNext + 1) % DECAL_NUM_TRACKS;
    return &sTireTracks[j];
}

#define RAW(pkt, a, b) { Gfx *_g = (Gfx *)(pkt); _g->words.w0 = (a); _g->words.w1 = (b); }

/* Draws the tire tracks of a view, area by area (desert blocks, outdoor sections, or all at once in the
   stadium), in the surface's colour with an alpha that fades with age; the 16 oldest are not drawn. */
Gfx *decal_draw(Gfx *gdl, s32 fb, Mtx *dyn, s32 view) {
    s32 i, j, k, m, age, idx, mtxLoaded, v, hi, lo;

    if (gDebugFreeze != 0) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    RAW(gdl++, 0xE200001C, 0x00404A50);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    RAW(gdl++, 0xFC119623, 0xFF2FFFFF);
    gDPLoadTextureBlock(gdl++, sTireTrackTexture, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 1, 0,
                        G_TX_CLAMP, G_TX_NOMIRROR | G_TX_WRAP, 3, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    if (gCurTrack_5->mode == TRACK_ENV_DESERT) {
        for (i = 0; i < sDesBlockCount; i++) {
            mtxLoaded = 0;
            for (j = 0; j < DECAL_NUM_TRACKS; j++) {
                if (!sTireTracks[j].active) continue;
                v = sTireTracks[j].area;
                hi = v >> 16;
                lo = (s16)v;
                if (hi == gDesBlockX[i] && lo == gDesBlockZ[i]) {
                    k = j + 1;
                    if (!mtxLoaded) {
                        gSPMatrix(gdl++, &sDesBlockMtx[fb][i], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                        mtxLoaded = 1;
                    }
                    for (; k < DECAL_NUM_TRACKS; k++) {
                        if (k >= j + 8) break;
                        if (!sTireTracks[k].active) break;
                        v = sTireTracks[k].area;
                        hi = v >> 16;
                        lo = (s16)v;
                        if (hi != gDesBlockX[i] || lo != gDesBlockZ[i]) break;
                    }
                    k -= j;
                    gSPVertex(gdl++, &sTireTrackVtx[j * 4], k * 4, 0);
                    for (m = 0; m < k; m++) {
                        idx = m + j;
                        age = idx - sTireTrackNext;
                        if (age < 0) age += DECAL_NUM_TRACKS;
                        if (age >= 16) {
                            gDPSetPrimColor(gdl++, 0, 0, gSurfaceTablePtr[sTireTracks[idx].surface].color[8], gSurfaceTablePtr[sTireTracks[idx].surface].color[9], gSurfaceTablePtr[sTireTracks[idx].surface].color[10], age);
                            gSP2Triangles(gdl++, m * 4, m * 4 + 1, m * 4 + 2, 0, m * 4 + 2, m * 4 + 1, m * 4 + 3, 0);
                        }
                    }
                    j += k - 1;
                }
            }
        }
    } else if (gCurTrack_5->mode == TRACK_ENV_OUTDOOR) {
        TrackModelList *l = sHeightAreas;
        u32 bit;
        {
            for (i = 0; i < sHeightAreas->count; i++) {
                if (l->e[i].mask & TRACK_MODEL_VISIBLE(view)) {
                mtxLoaded = 0;
                for (j = 0; j < DECAL_NUM_TRACKS; j++) {
                    if (sTireTracks[j].active && sTireTracks[j].area == i) {
                        k = j + 1;
                        if (!mtxLoaded) {
                            gSPMatrix(gdl++, gSectionMtxs[i * 4 + view], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                            mtxLoaded = 1;
                        }
                        for (; k < DECAL_NUM_TRACKS; k++) {
                            if (k >= j + 8) break;
                            if (!sTireTracks[k].active) break;
                            if (sTireTracks[k].area != i) break;
                        }
                        k -= j;
                        gSPVertex(gdl++, &sTireTrackVtx[j * 4], k * 4, 0);
                        for (m = 0; m < k; m++) {
                            idx = m + j;
                            age = idx - sTireTrackNext;
                            if (age < 0) age += DECAL_NUM_TRACKS;
                            if (age >= 16) {
                                gDPSetPrimColor(gdl++, 0, 0, gSurfaceTablePtr[sTireTracks[idx].surface].color[8], gSurfaceTablePtr[sTireTracks[idx].surface].color[9], gSurfaceTablePtr[sTireTracks[idx].surface].color[10], age);
                                gSP2Triangles(gdl++, m * 4, m * 4 + 1, m * 4 + 2, 0, m * 4 + 2, m * 4 + 1, m * 4 + 3, 0);
                            }
                        }
                        j += k - 1;
                    }
                }
                }
            }
        }
    } else {
        for (i = 0; i < DECAL_NUM_TRACKS; i += 8) {
            gSPVertex(gdl++, &sTireTrackVtx[i * 4], 32, 0);
            for (j = 0; j < 8; j++) {
                idx = i + j;
                if (sTireTracks[idx].active) {
                    if (sHeightAreas->e[sTireTracks[idx].area].mask & TRACK_MODEL_VISIBLE(view)) {
                        age = idx - sTireTrackNext;
                        if (age < 0) age += DECAL_NUM_TRACKS;
                        if (age >= 16) {
                            gDPSetPrimColor(gdl++, 0, 0, gSurfaceTablePtr[sTireTracks[idx].surface].color[8], gSurfaceTablePtr[sTireTracks[idx].surface].color[9], gSurfaceTablePtr[sTireTracks[idx].surface].color[10], age);
                            gSP2Triangles(gdl++, j * 4, j * 4 + 1, j * 4 + 2, 0, j * 4 + 2, j * 4 + 1, j * 4 + 3, 0);
                        }
                    }
                }
            }
        }
    }
    gSPMatrix(gdl++, dyn + 16 /* Dyn.m4 */, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gDPPipeSync(gdl++);
    return gdl;
}

/* Frees the tire tracks. */
void decal_free(void)
{
    mem_free(sTireTrackTexture);
    mem_free(sTireTrackVtx);
    mem_free(sTireTracks);
}

/* Ends a rider's current track: the next one starts a new quad. */
void decal_end_track(s32 rider)
{
    sTireTrackCur[rider] = 0;
}

/* Removes all tire tracks. */
void decal_clear_all(void) {
    s32 i;
    for (i = 0; i < 8; i++) {
        sTireTrackCur[i] = 0;
        gTireTrackTimers[i] = 0;
    }
    for (i = 0; i < DECAL_NUM_TRACKS; i++) {
        sTireTracks[i].active = 0;
    }
}
