#include "game.h"

typedef struct {
    s32 x0, y0, x1, y1;
    s32 f10, f14, f18, f1C, f20, f24;
    s32 n;
    s32 f2C;
    u16 ang;
    s16 arr[3];
    u8 idx[4];
    void *buf;
} Track_14;

extern Track_14 sGravelPitMovers[2];

extern TrackHack *sTrackHacks[];

void hacks_init(void)
{
    s32 key;
    TrackHack **pp;
    s32 i;

    key = gCurTrack_7 - gTracks_3;
    sCurTrackHack = NULL;
    for (i = 0; sTrackHacks[i] != NULL; i++) {
        if (sTrackHacks[i]->track == key) {
            sCurTrackHack = sTrackHacks[i];
            break;
        }
    }
    if (sCurTrackHack != NULL) {
        sHackBuffers[0] = NULL;
        sHackBuffers[1] = NULL;
        sHackResources = NULL;
        if (sCurTrackHack->numBufs != 0) {
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
} Res_4;

void hack_gravel_pit_init(void)
{
    s32 i, j;
    s32 r, cx, cy, t;
    Track_14 *p;
    Res_4 *res;
    s32 sp18;
    s32 d, e;

    gGravelPitMoverTimer = 0;
    for (i = 0; i < 2; i++) {
        p = &sGravelPitMovers[i];
        math_to_polar(p->x1 - p->x0, p->y1 - p->y0, &p->ang, &sp18);
        cx = (p->x0 + p->x1) / 2;
        cy = (p->y0 + p->y1) / 2;
        sp18 = p->n * 0x2CEC;
        p->f10 = cx + gCosTable[(((s16)p->ang >> 4) - 0x400) & 0xFFF] * sp18 / 0x4000;
        p->f18 = cy + gCosTable[p->ang >> 4] * sp18 / 0x4000;
        p->f1C = cx - gCosTable[(((s16)p->ang >> 4) - 0x400) & 0xFFF] * sp18 / 0x4000;
        p->f24 = cy - gCosTable[p->ang >> 4] * sp18 / 0x4000;
        p->f14 = height_get_ground(cx, 50000, cy);
        p->f20 = height_get_ground(cx, 50000, cy);
        p->f2C = gGroundArea;
        for (j = 0; j < p->n; j++) {
            p->arr[j] = (j << 8) / p->n;
            p->idx[j] = j;
        }
        p->buf = mem_alloc_dbg(p->n << 9, 0, "hacks.c", 0x116);
    }
    for (i = 0; i < 4; i++) {
        res = res_load(gResourceSet, *sGravelPitItemResIds[i]);
        sp18 = res->data - (s32)res;
        t = mem_get_size(res);
        d = 0x1E000;
        memcpy(gZBuffer + d, res->data, t - sp18);
        d = 0;
        mem_realloc_dbg(res, sp18, d, "hacks.c", 0x124);
        e = 0x1E000;
        res->data = gZBuffer + e;
        prim_alloc_dl(&sGravelPitModels[i], res, sObjectSegs, 0xC);
    }
}

typedef struct {
    u8 pad0[0x28];
    s32 count;
    s32 id;
    u8 pad30[2];
    s16 t[3];
    u8 v[8];
} Ent_62;

extern Ent_62 sGravelPitMovers_2[];

void hack_gravel_pit_update(void)
{
    s32 j;
    s32 i;
    Ent_62 *e;

    for (j = 0; j < 2; j++) {
        e = &sGravelPitMovers_2[j];
        for (i = 0; i < e->count; i++) {
            if (++e->t[i] >= 0x100) {
                e->t[i] = 0;
                e->v[i] = random_byte() & 3;
            }
        }
    }

    for (i = 0; i < 8; i++) {
        if (gRiders[i].inactive == 0 && gRiders[i].isCpu == 0 && gRiders[i].anim != -1 && gRiders[i].anim < 16) {
            if (gRiderSection[i] == sGravelPitMovers_2[0].id || gRiderSection[i] == sGravelPitMovers_2[1].id) {
                gGravelPitMoverTimer = 300;
            }
        }
    }

    if (gGravelPitMoverTimer > 0) {
        gGravelPitMoverTimer--;
    }
}

typedef struct {
    u32 pad0[4];
    s32 x0;
    s32 y0;
    s32 z0;
    s32 x1;
    s32 y1;
    s32 z1;
    s32 cnt;
    s32 idx;
    s16 sh30;
    s16 sk[3];
    u8 bt[4];
    Mtx *base;
} Cell_6;

Gfx *hack_gravel_pit_draw_pass2(Gfx *gdl, s32 unused, s32 p3, s32 p4)
{
    s32 i, k;
    s32 *v;
    s32 *tab;
    s32 sh;
    Cell_6 *c;
    Mtx *m;
    Gfx **tbl;
    s32 x, y, z;

    sh = p4 * 2;
    gDPPipeSync(gdl++);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    gDPSetRenderMode(gdl++, 0x00443078, 0);
    gDPSetCombineMode(gdl++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);

    i = 0;
    tab = gCamPos;
    v = tab + (sh + p4);
    for (; i < 2; i++) {
        c = &sGravelPitMovers[i];
        if ((gTrackSections->e[c->idx].mask >> sh) & 1) {
            k = 0;
            if (c->cnt > 0) {
                tbl = sGravelPitModels;
                do {
                    x = c->x0 + (c->x1 - c->x0) * c->sk[k] / 256;
                    z = c->z0 + (c->z1 - c->z0) * c->sk[k] / 256;
                    y = c->y0;
                    m = &c->base[p3 * (c->cnt << 2) + p4 * c->cnt + k];
                    mtx_from_angles_pos_scale(m, 0, 0, 0x8000 - c->sh30, x - v[0], y - v[1], z - v[2], 0x100);
                    gSPMatrix(gdl++, m, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPDisplayList(gdl++, tbl[c->bt[k]]);
                    k++;
                } while (k < c->cnt);
            }
        }
    }
    return gdl;
}

void hack_mill_update(void)
{
    s32 c, s;
    s32 i, j, m;
    u32 k;
    Rider *b, *bb, *base;
    V4 *p;
    Vec3i *e;
    s32 dx, dz, a, v, t, r, w;
    s32 ang;

    k = gInstanceItems[0].rot >> 4;
    c = gCosTable[k | 0xC00];
    s = gCosTable[k];
    for (m = 0; m < sMillObjCount; m++) {
        sMillObjState[m].z = 0;
    }
    for (m = 0; m < 8; m++) {
        if (gRiders[m].inactive != 0 || gRiders[m].rearWheel.airTicks >= 0x15 || gRiders[m].frontWheel.airTicks >= 0x15) continue;
        for (i = 0; i < sMillObjCount; i++) {
            dx = (s32)(gRiders[m].frontWheel.contact.x * 1000.0f) - gInstanceItems[i].x;
            if ((u32)(dx + 0x10000) > 0x20000) continue;
            dz = (s32)(gRiders[m].frontWheel.contact.z * 1000.0f) - gInstanceItems[i].z;
            if ((u32)(dz + 0x10000) > 0x20000) continue;
            a = (dx * c + dz * s) >> 14;
            dx = (s32)(gRiders[m].rearWheel.contact.x * 1000.0f) - gInstanceItems[i].x;
            if ((u32)(dx + 0x10000) > 0x20000) continue;
            dz = (s32)(gRiders[m].rearWheel.contact.z * 1000.0f) - gInstanceItems[i].z;
            if ((u32)(dz + 0x10000) > 0x20000) continue;
            v = (dx * c + dz * s) >> 14;
            if (a > 0 && v < 0) {
                r = 0x100;
            } else if (a < 0 && v > 0) {
                r = 0x100;
            } else {
                if (a > 0) {
                    r = v;
                    if (a < r) r = a;
                    v = 400;
                    v -= r;
                    r = (v << 8) / 400;
                } else if (a < 0) {
                    r = v;
                    if (r < a) r = a;
                    v = r + 400;
                    r = (v << 8) / 400;
                } else {
                    goto store;
                }
                if (r < 0) r = 0;
                if (r >= 0x100) r = 0xFF;
            }
store:
            if (sMillObjState[i].z < r) sMillObjState[i].z = r;
        }
    }
    i = 0;
    if (sMillObjCount > 0) {
        do {
            e = &sMillObjState[i];
            t = e->z;
            w = (gCosTable[(((gRaceFrame << 9) + i * 0x1000) >> 4) & 0xFFF] * (0xFF - t)) >> 15;
            w -= t;
            w /= 2;
            w -= 200;
            if (w < e->y - 16) {
                e->y = e->y - 16;
            } else {
                e->y = w;
            }
            ((MtxS *)gInstanceMtx)[i].i[3][1] = (e->x + e->y) >> 4; /* the y translation */
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

s32 hacks_draw_pass2(s32 a0, Dyn *arg1, s32 arg2, s32 arg3)
{
    if (sCurTrackHack != 0) {
        a0 = sCurTrackHack->drawPass2(a0);
    }
    return a0;
}

s32 hacks_draw_pass1(s32 x, Dyn *arg1, s32 arg2, s32 arg3)
{
    if (sCurTrackHack != 0) {
        x = sCurTrackHack->drawPass1(x);
    }
    return x;
}

typedef struct {
    s32 val;
    s32 zero;
    s32 pad;
} Rec_34;

void hack_mill_init(void)
{
    s32 i;
    s32 n;
    s32 cnt;
    Rec_34 *out;
    Instance *e;

    sMillObjCount = gInstanceHeader->numItems;
    sMillObjState = (s32 *)mem_alloc_dbg(sMillObjCount * 12, 0, sStrHacksFile, 0x1B1);
    i = 0;
    n = sMillObjCount;
    if (n > 0) {
        cnt = n;
        out = (Rec_34 *)sMillObjState;
        e = gInstanceItems;
        do {
            s32 v = e->y;
            e++;
            i++;
            out->zero = 0;
            out->val = v;
            out++;
        } while (i < cnt);
    }
}

void hack_mill_exit(void)
{
    mem_free(sMillObjState);
}

s32 hack_mill_draw_pass1(s32 x)
{
    return x;
}

void *hack_mill_draw_pass2(void *p)
{
    return p;
}

typedef struct {
    u8 pad[0x3C];
    u32 v;
} Blk_17;

void hack_gravel_pit_exit(void) {
    s32 i = 0;
    Blk_17 *p = sGravelPitMovers;
    u32 *q;

    do {
        u32 v = p->v;
        p++;
        i++;
        mem_free(v);
    } while (i < 2);

    i = 0;
    q = sGravelPitModels;
    do {
        u32 v = *q;
        q++;
        i++;
        mem_free(v);
    } while (i < 4);
}

s32 hack_gravel_pit_draw_pass1(s32 x)
{
    return x;
}
