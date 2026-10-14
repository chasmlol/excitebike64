#include "game.h"

void snow_init(void)
{
    s32 i;

    if (gCurTrack_2 - gTracks == 0x13 && gNumViews == 1) {
        sSnowflakes = mem_alloc_dbg(0xE00, 0, (s32)D_800D9190, 0x6B);
        sSnowTextures = res_load(D_800E7728, D_800D1000);
        for (i = 0; i < 0x80; i++) {
            snow_respawn_flake(&sSnowflakes[i]);
        }
        sSnowSplatTimer = 0;
    }
}

typedef struct { s32 x, y, z, pad[3]; s32 on; } Star;
typedef struct { u8 pad[8]; u8 w, h; u8 pad2[2]; void *img2; u8 pad3[4]; void *img; } TexInfo_3;

extern TexInfo_3 *sSnowTextures_2;

Gfx *snow_draw(Gfx *gdl, s32 mtx, s32 arg2)
{
    Star *p;
    s32 i, cx, cy, s, x, y, z, a, r;

    p = sSnowflakes;
    if (p == NULL) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    gSPSetOtherMode(gdl++, G_SETOTHERMODE_L, G_MDSFT_RENDERMODE, 29, 0x404240);
    gDPSetCombine(gdl++, 0xFF97FF, 0xFF2DFEFF);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    { Gfx *g = gdl++; g->words.w0 = 0xE3000C00; g->words.w1 = ((u32)g & 1) >> 1; }
    { Gfx *g = gdl++; g->words.w0 = 0xE3001001; g->words.w1 = ((u32)g & 1) >> 1; }
    {
        s32 w = gScreenWidth;
        s32 h = gScreenHeight;
        cx = w / 2;
        cy = h / 2;
    }
    gDPLoadTextureBlock(gdl++, sSnowTextures_2->img, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
    for (i = 0; i < 0x80; i++) {
        if (p[i].on != 0) {
            s = gCosTable[(((gRaceFrame & 0xFFFFFF) * 256 + i * 512) >> 4) & 0xFFF] / 2;
            z = p[i].z;
            x = p[i].x + s;
            x = cx + x * cx / z;
            y = p[i].y + s;
            y = cy - y * cy / z;
            a = 0xFF - (z >> 8);
            if (a < 0x20) {
                a = 0x20;
            }
            r = 0x80 / (z >> 8);
            r = (r >> 1) + 1;
            if (r > 4) {
                r = 4;
            }
            gDPPipeSync(gdl++);
            gDPSetColor(gdl++, G_SETPRIMCOLOR, a | 0xFFFFFF00);
            gSPTextureRectangle(gdl++, (x - r) << 2, (y - r) << 2, (x + r) << 2, (y + r) << 2,
                                G_TX_RENDERTILE, 0, 0, sSnowflakeTexScale[r].b, sSnowflakeTexScale[r].b);
        }
    }
    if (sSnowSplatTimer != 0) {
        gDPSetColor(gdl++, G_SETPRIMCOLOR, (sSnowSplatTimer * 255 / 60) | 0xFFFFFF00);
        gDPLoadTextureBlock(gdl++, sSnowTextures_2->img2, G_IM_FMT_IA, G_IM_SIZ_8b,
                            sSnowTextures_2->w * 8, sSnowTextures_2->h, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 8, 8, G_TX_NOLOD, G_TX_NOLOD);
        gDPPipeSync(gdl++);
        gDPSetTexturePersp(gdl++, G_TP_PERSP);
        gSPMatrix(gdl++, mtx + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
        gSPVertex(gdl++, sSnowSplatVtx, 4, 0);
        gSP2Triangles(gdl++, 0, 2, 1, 0, 0, 3, 2, 0);
    }
    gDPPipeSync(gdl++);
    return gdl;
}

typedef struct {
    s32 x, y, z;
    s32 pad[2];
    s32 f14;
    s32 f18;
} Part_6;

extern Part_6 *sSnowflakes_2;

static inline void respawn(Part_6 *p, s32 k) {
    if (gCamSegFlags & 0x40) {
        p->f18 = 0;
    } else {
        p->f18 = 1;
    }
    p->z = random_byte() * 192 + 0x4000;
    p->x = ((s32)random_byte() - 128) * p->z / 128;
    if (random_byte() & 1) {
        p->y = ((s32)random_byte() - 128) * p->z / 256;
    } else {
        p->y = p->z;
    }
    p->f14 = random_byte() + 0x100;
}

void snow_update(void) {
    s32 dx, dz, ang, s, c;
    s32 mx, mz, rot, dy, ny;
    s32 i;
    s32 x, y, z, sx, sy, hw, hh;
    s32 a, t3, t2;
    s32 w, h;

    if (sSnowflakes_2 == NULL || gPauseMenu != 0 || (gPodiumMode != 0 && gDesaturation == 0x100)) {
        return;
    }
    dz = gCamPos_2.z - sSnowLastCamPos.z;
    ang = -gCamYaw_2 >> 4;
    s = gCosTable[ang & 0xFFF];
    c = gCosTable[(ang - 0x400) & 0xFFF];
    dx = gCamPos_2.x - sSnowLastCamPos.x;
    sSnowSplatX = -1;
    dy = gCamPos_2.y - sSnowLastCamPos.y;
    mz = (s * dz + c * dx) >> 14;
    mx = (c * dz - s * dx) >> 14;
    rot = (gCamYaw_2 - sSnowLastCamYaw) / 64;
    i = 0;
    ny = -dy * 3;
    sSnowLastCamPos.x = gCamPos_2.x;
    sSnowLastCamPos.y = gCamPos_2.y;
    sSnowLastCamPos.z = gCamPos_2.z;
    sSnowLastCamYaw = gCamYaw_2;
    h = gScreenHeight;
    for (; i < 128; i++) {
        { s32 t = sSnowflakes_2[i].z; sSnowflakes_2[i].z = t - mz * 3; }
        sSnowflakes_2[i].y += ny - sSnowflakes_2[i].f14;
        { s32 t = sSnowflakes_2[i].x; sSnowflakes_2[i].x = t + mx * 3; }
        sSnowflakes_2[i].x += rot * sSnowflakes_2[i].z / 256;
        z = sSnowflakes_2[i].z;
        if (z < 0x1000) {
            if (sSnowflakes_2[i].x < z && -z < sSnowflakes_2[i].x) {
                if (sSnowflakes_2[i].y < z && -z < sSnowflakes_2[i].y && z > 0 && sSnowflakes_2[i].f18 != 0) {
                    hw = gScreenWidth / 2;
                    sx = hw + sSnowflakes_2[i].x * hw / z;
                    hh = gScreenHeight / 2;
                    sy = hh - sSnowflakes_2[i].y * hh / z;
                    if (sx >= 20 && sx < gScreenWidth - 20 && sy >= 20 && sy < gScreenHeight - 20) {
                        sSnowSplatX = sx;
                        sSnowSplatY = sy;
                    }
                }
            }
            respawn(&sSnowflakes_2[i], i);
        } else if (sSnowflakes_2[i].y < -z) {
            respawn(&sSnowflakes_2[i], i);
        } else if (sSnowflakes_2[i].x < -z) {
            respawn(&sSnowflakes_2[i], i);
        } else if (z < sSnowflakes_2[i].x) {
            respawn(&sSnowflakes_2[i], i);
        } else if (z > 0x10000) {
            respawn(&sSnowflakes_2[i], i);
        }
    }
    if (sSnowSplatTimer != 0) {
        sSnowSplatTimer--;
    }
    random_byte();
    random_byte();
    if (sSnowSplatTimer == 0 && sSnowSplatX != -1) {
        i = random_byte() << 5;
        t2 = gCosTable[(i >> 4) & 0xFFF] >> 10;
        t3 = gCosTable[((i >> 4) - 0x400) & 0xFFF] >> 10;
        x = sSnowSplatX;
        y = sSnowSplatY;
        sSnowSplatTimer = 60;
        sSnowSplatVtx[0].v.ob[0] = (x + t3 - 160) * gScreenWidth / 320;
        sSnowSplatVtx[0].v.ob[1] = (y + t2 - 120) * gScreenHeight / 240;
        sSnowSplatVtx[1].v.ob[0] = (x + t2 - 160) * gScreenWidth / 320;
        sSnowSplatVtx[1].v.ob[1] = (y - t3 - 120) * gScreenHeight / 240;
        sSnowSplatVtx[2].v.ob[0] = (x - t3 - 160) * gScreenWidth / 320;
        sSnowSplatVtx[2].v.ob[1] = (y - t2 - 120) * gScreenHeight / 240;
        sSnowSplatVtx[3].v.ob[0] = (x - t2 - 160) * gScreenWidth / 320;
        sSnowSplatVtx[3].v.ob[1] = (y + t3 - 120) * gScreenHeight / 240;
    }
}

void snow_respawn_flake(s32 *param_1)
{
    s32 x;
    u32 u;

    if (gCamSegFlags & 0x40) {
        param_1[6] = 0;
    } else {
        param_1[6] = 1;
    }
    u = random_byte();
    param_1[2] = u * 0xc0 + 0x4000;
    u = random_byte();
    x = ((s32)u - 0x80) * param_1[2];
    if (x < 0) {
        x += 0x7f;
    }
    param_1[0] = x >> 7;
    u = random_byte();
    if (u & 1) {
        u = random_byte();
        x = ((s32)u - 0x80) * param_1[2];
        if (x < 0) {
            x += 0xff;
        }
        x = x >> 8;
    } else {
        x = param_1[2];
    }
    param_1[1] = x;
    u = random_byte();
    param_1[5] = u + 0x100;
}

void snow_free(void)
{
    if (sSnowflakes != 0) {
        mem_free(sSnowflakes);
        sSnowflakes = 0;
    }
}
