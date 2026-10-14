#include "game.h"

void snow_init(void)
{
    s32 i;

    if (gCurTrack_2 - gTracks == 0x13 && gNumViews == 1) {
        sSnowflakes = mem_alloc_dbg(0xE00, 0, (s32)D_800D9190, 0x6B);
        sSnowTextures = (TexSet *)res_load(D_800E7728, D_800D1000);
        for (i = 0; i < 0x80; i++) {
            snow_respawn_flake(&sSnowflakes[i]);
        }
        sSnowSplatTimer = 0;
    }
}

Gfx *snow_draw(Gfx *gdl, s32 mtx, s32 arg2)
{
    Snowflake *p;
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
    gDPLoadTextureBlock(gdl++, sSnowTextures->tex[1].data, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
    for (i = 0; i < 0x80; i++) {
        if (p[i].visible != 0) {
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
        gDPLoadTextureBlock(gdl++, sSnowTextures->tex[0].data, G_IM_FMT_IA, G_IM_SIZ_8b,
                            sSnowTextures->tex[0].width * 8, sSnowTextures->tex[0].height, 0,
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

static inline void respawn(Snowflake *p, s32 k) {
    if (gCamSegFlags & 0x40) {
        p->visible = 0;
    } else {
        p->visible = 1;
    }
    p->z = random_byte() * 192 + 0x4000;
    p->x = ((s32)random_byte() - 128) * p->z / 128;
    if (random_byte() & 1) {
        p->y = ((s32)random_byte() - 128) * p->z / 256;
    } else {
        p->y = p->z;
    }
    p->fallSpeed = random_byte() + 0x100;
}

void snow_update(void) {
    s32 dx, dz, ang, s, c;
    s32 mx, mz, rot, dy, ny;
    s32 i;
    s32 x, y, z, sx, sy, hw, hh;
    s32 a, t3, t2;
    s32 w, h;

    if (sSnowflakes == NULL || gPauseMenu != 0 || (gPodiumMode != 0 && gDesaturation == 0x100)) {
        return;
    }
    dz = gCamPos[0].z - sSnowLastCamPos.z;
    ang = -gCamYaw[0] >> 4;
    s = gCosTable[ang & 0xFFF];
    c = gCosTable[(ang - 0x400) & 0xFFF];
    dx = gCamPos[0].x - sSnowLastCamPos.x;
    sSnowSplatX = -1;
    dy = gCamPos[0].y - sSnowLastCamPos.y;
    mz = (s * dz + c * dx) >> 14;
    mx = (c * dz - s * dx) >> 14;
    rot = (gCamYaw[0] - sSnowLastCamYaw) / 64;
    i = 0;
    ny = -dy * 3;
    sSnowLastCamPos.x = gCamPos[0].x;
    sSnowLastCamPos.y = gCamPos[0].y;
    sSnowLastCamPos.z = gCamPos[0].z;
    sSnowLastCamYaw = gCamYaw[0];
    h = gScreenHeight;
    for (; i < 128; i++) {
        { s32 t = sSnowflakes[i].z; sSnowflakes[i].z = t - mz * 3; }
        sSnowflakes[i].y += ny - sSnowflakes[i].fallSpeed;
        { s32 t = sSnowflakes[i].x; sSnowflakes[i].x = t + mx * 3; }
        sSnowflakes[i].x += rot * sSnowflakes[i].z / 256;
        z = sSnowflakes[i].z;
        if (z < 0x1000) {
            if (sSnowflakes[i].x < z && -z < sSnowflakes[i].x) {
                if (sSnowflakes[i].y < z && -z < sSnowflakes[i].y && z > 0 && sSnowflakes[i].visible != 0) {
                    hw = gScreenWidth / 2;
                    sx = hw + sSnowflakes[i].x * hw / z;
                    hh = gScreenHeight / 2;
                    sy = hh - sSnowflakes[i].y * hh / z;
                    if (sx >= 20 && sx < gScreenWidth - 20 && sy >= 20 && sy < gScreenHeight - 20) {
                        sSnowSplatX = sx;
                        sSnowSplatY = sy;
                    }
                }
            }
            respawn(&sSnowflakes[i], i);
        } else if (sSnowflakes[i].y < -z) {
            respawn(&sSnowflakes[i], i);
        } else if (sSnowflakes[i].x < -z) {
            respawn(&sSnowflakes[i], i);
        } else if (z < sSnowflakes[i].x) {
            respawn(&sSnowflakes[i], i);
        } else if (z > 0x10000) {
            respawn(&sSnowflakes[i], i);
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

void snow_respawn_flake(Snowflake *flake)
{
    s32 x;
    u32 u;

    if (gCamSegFlags & 0x40) {
        flake->visible = 0;
    } else {
        flake->visible = 1;
    }
    u = random_byte();
    flake->z = u * 0xc0 + 0x4000;
    u = random_byte();
    x = ((s32)u - 0x80) * flake->z;
    if (x < 0) {
        x += 0x7f;
    }
    flake->x = x >> 7;
    u = random_byte();
    if (u & 1) {
        u = random_byte();
        x = ((s32)u - 0x80) * flake->z;
        if (x < 0) {
            x += 0xff;
        }
        x = x >> 8;
    } else {
        x = flake->z;
    }
    flake->y = x;
    u = random_byte();
    flake->fallSpeed = u + 0x100;
}

void snow_free(void)
{
    if (sSnowflakes != 0) {
        mem_free(sSnowflakes);
        sSnowflakes = 0;
    }
}
