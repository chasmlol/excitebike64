#include "game.h"

/* Blizzard Blitz with one view: allocates the 128 snowflakes and scatters them ahead of the camera. */
void snow_init(void)
{
    s32 i;

    if (gCurTrack_2 - gTracks == TRACK_BLIZZARD_BLITZ && gNumViews == 1) {
        sSnowflakes = mem_alloc_dbg(SNOW_NUM_FLAKES * sizeof(Snowflake), 0, (s32)sStrSnowC, 0x6B);
        sSnowTextures = (TexSet *)res_load(gResourceSet, sSnowTexResId);
        for (i = 0; i < SNOW_NUM_FLAKES; i++) {
            snow_respawn_flake(&sSnowflakes[i]);
        }
        sSnowSplatTimer = 0;
    }
}

/* Draws the flakes as textured rectangles (smaller and fainter with distance, with a sine wobble) and, while its
   timer runs, the flake splatted on the lens. dyn is the frame's matrices (Dyn). */
Gfx *snow_draw(Gfx *gdl, s32 dyn, s32 unused)
{
    Snowflake *flakes;
    s32 i, halfW, halfH, wobble, x, y, z, alpha, radius;

    flakes = sSnowflakes;
    if (flakes == NULL) {
        return gdl;
    }
    gDPPipeSync(gdl++);
    gSPSetOtherMode(gdl++, G_SETOTHERMODE_L, G_MDSFT_RENDERMODE, 29, 0x404240);
    gDPSetCombine(gdl++, 0xFF97FF, 0xFF2DFEFF);
    gSPTexture(gdl++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
    /* gDPSetTextureLUT(G_TT_NONE) and gDPSetTexturePersp(G_TP_NONE): w1 is 0, written as an expression of the
       packet's address in the original */
    { Gfx *g = gdl++; g->words.w0 = 0xE3000C00; g->words.w1 = ((u32)g & 1) >> 1; }
    { Gfx *g = gdl++; g->words.w0 = 0xE3001001; g->words.w1 = ((u32)g & 1) >> 1; }
    {
        s32 w = gScreenWidth;
        s32 h = gScreenHeight;
        halfW = w / 2;
        halfH = h / 2;
    }
    gDPLoadTextureBlock(gdl++, sSnowTextures->tex[1].data, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                        G_TX_CLAMP, G_TX_CLAMP, 3, 3, G_TX_NOLOD, G_TX_NOLOD);
    for (i = 0; i < SNOW_NUM_FLAKES; i++) {
        if (flakes[i].visible != 0) {
            wobble = gCosTable[(((gRaceFrame & 0xFFFFFF) * 256 + i * 512) >> 4) & 0xFFF] / 2;
            z = flakes[i].z;
            x = flakes[i].x + wobble;
            x = halfW + x * halfW / z;
            y = flakes[i].y + wobble;
            y = halfH - y * halfH / z;
            alpha = 0xFF - (z >> 8);
            if (alpha < 0x20) {
                alpha = 0x20;
            }
            radius = 0x80 / (z >> 8);
            radius = (radius >> 1) + 1;
            if (radius > 4) {
                radius = 4;
            }
            gDPPipeSync(gdl++);
            gDPSetColor(gdl++, G_SETPRIMCOLOR, alpha | 0xFFFFFF00);
            gSPTextureRectangle(gdl++, (x - radius) << 2, (y - radius) << 2, (x + radius) << 2, (y + radius) << 2,
                                G_TX_RENDERTILE, 0, 0, sSnowflakeTexScale[radius].b, sSnowflakeTexScale[radius].b);
        }
    }
    if (sSnowSplatTimer != 0) {
        gDPSetColor(gdl++, G_SETPRIMCOLOR, (sSnowSplatTimer * 255 / 60) | 0xFFFFFF00);
        gDPLoadTextureBlock(gdl++, sSnowTextures->tex[0].data, G_IM_FMT_IA, G_IM_SIZ_8b,
                            sSnowTextures->tex[0].width * 8, sSnowTextures->tex[0].height, 0,
                            G_TX_CLAMP, G_TX_CLAMP, 8, 8, G_TX_NOLOD, G_TX_NOLOD);
        gDPPipeSync(gdl++);
        gDPSetTexturePersp(gdl++, G_TP_PERSP);
        /* Dyn.m3, the orthographic projection */
        gSPMatrix(gdl++, dyn + 0x300, G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
        gSPVertex(gdl++, sSnowSplatVtx, 4, 0);
        gSP2Triangles(gdl++, 0, 2, 1, 0, 0, 3, 2, 0);
    }
    gDPPipeSync(gdl++);
    return gdl;
}

/* snow_respawn_flake written inline (snow_update's copy) */
static inline void respawn(Snowflake *flake, s32 index) {
    if (gCamSegFlags[0] & CAM_SEG_COVERED) {
        flake->visible = 0;
    } else {
        flake->visible = 1;
    }
    flake->z = random_byte() * 192 + 0x4000;
    flake->x = ((s32)random_byte() - 128) * flake->z / 128;
    if (random_byte() & 1) {
        flake->y = ((s32)random_byte() - 128) * flake->z / 256;
    } else {
        flake->y = flake->z;
    }
    flake->fallSpeed = random_byte() + 0x100;
}

/* Moves the flakes against the camera's motion and turn, respawns those that leave the view and, when one hits
   the lens, starts the splat at its screen position. */
void snow_update(void) {
    s32 moveX, moveZ, yaw, cosYaw, sinYaw;
    s32 sideMove, fwdMove, yawDelta, moveY, liftY;
    s32 i;
    s32 x, y, z, screenX, screenY, halfW, halfH;
    s32 a, cornerX, cornerY;
    s32 w, h;

    if (sSnowflakes == NULL || gPauseMenu != 0 || (gPodiumMode != 0 && gDesaturation == 0x100)) {
        return;
    }
    moveZ = gCamPos[0].z - sSnowLastCamPos.z;
    yaw = -gCamYaw[0] >> 4;
    cosYaw = gCosTable[yaw & 0xFFF];
    sinYaw = gCosTable[(yaw - 0x400) & 0xFFF];
    moveX = gCamPos[0].x - sSnowLastCamPos.x;
    sSnowSplatX = -1;
    moveY = gCamPos[0].y - sSnowLastCamPos.y;
    fwdMove = (cosYaw * moveZ + sinYaw * moveX) >> 14;
    sideMove = (sinYaw * moveZ - cosYaw * moveX) >> 14;
    yawDelta = (gCamYaw[0] - sSnowLastCamYaw) / 64;
    i = 0;
    liftY = -moveY * 3;
    sSnowLastCamPos.x = gCamPos[0].x;
    sSnowLastCamPos.y = gCamPos[0].y;
    sSnowLastCamPos.z = gCamPos[0].z;
    sSnowLastCamYaw = gCamYaw[0];
    h = gScreenHeight;
    for (; i < SNOW_NUM_FLAKES; i++) {
        { s32 t = sSnowflakes[i].z; sSnowflakes[i].z = t - fwdMove * 3; }
        sSnowflakes[i].y += liftY - sSnowflakes[i].fallSpeed;
        { s32 t = sSnowflakes[i].x; sSnowflakes[i].x = t + sideMove * 3; }
        sSnowflakes[i].x += yawDelta * sSnowflakes[i].z / 256;
        z = sSnowflakes[i].z;
        if (z < 0x1000) {
            if (sSnowflakes[i].x < z && -z < sSnowflakes[i].x) {
                if (sSnowflakes[i].y < z && -z < sSnowflakes[i].y && z > 0 && sSnowflakes[i].visible != 0) {
                    halfW = gScreenWidth / 2;
                    screenX = halfW + sSnowflakes[i].x * halfW / z;
                    halfH = gScreenHeight / 2;
                    screenY = halfH - sSnowflakes[i].y * halfH / z;
                    if (screenX >= 20 && screenX < gScreenWidth - 20 && screenY >= 20 && screenY < gScreenHeight - 20) {
                        sSnowSplatX = screenX;
                        sSnowSplatY = screenY;
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
        cornerY = gCosTable[(i >> 4) & 0xFFF] >> 10;
        cornerX = gCosTable[((i >> 4) - 0x400) & 0xFFF] >> 10;
        x = sSnowSplatX;
        y = sSnowSplatY;
        sSnowSplatTimer = 60;
        sSnowSplatVtx[0].v.ob[0] = (x + cornerX - 160) * gScreenWidth / 320;
        sSnowSplatVtx[0].v.ob[1] = (y + cornerY - 120) * gScreenHeight / 240;
        sSnowSplatVtx[1].v.ob[0] = (x + cornerY - 160) * gScreenWidth / 320;
        sSnowSplatVtx[1].v.ob[1] = (y - cornerX - 120) * gScreenHeight / 240;
        sSnowSplatVtx[2].v.ob[0] = (x - cornerX - 160) * gScreenWidth / 320;
        sSnowSplatVtx[2].v.ob[1] = (y - cornerY - 120) * gScreenHeight / 240;
        sSnowSplatVtx[3].v.ob[0] = (x - cornerY - 160) * gScreenWidth / 320;
        sSnowSplatVtx[3].v.ob[1] = (y + cornerX - 120) * gScreenHeight / 240;
    }
}

/* Puts a flake at a random point ahead of the camera, hidden in covered areas. */
void snow_respawn_flake(Snowflake *flake)
{
    s32 v;
    u32 rnd;

    if (gCamSegFlags[0] & CAM_SEG_COVERED) {
        flake->visible = 0;
    } else {
        flake->visible = 1;
    }
    rnd = random_byte();
    flake->z = rnd * 0xc0 + 0x4000;
    rnd = random_byte();
    v = ((s32)rnd - 0x80) * flake->z;
    if (v < 0) {
        v += 0x7f;
    }
    flake->x = v >> 7;
    rnd = random_byte();
    if (rnd & 1) {
        rnd = random_byte();
        v = ((s32)rnd - 0x80) * flake->z;
        if (v < 0) {
            v += 0xff;
        }
        v = v >> 8;
    } else {
        v = flake->z;
    }
    flake->y = v;
    rnd = random_byte();
    flake->fallSpeed = rnd + 0x100;
}

/* Frees the snowflakes. */
void snow_free(void)
{
    if (sSnowflakes != 0) {
        mem_free(sSnowflakes);
        sSnowflakes = 0;
    }
}
