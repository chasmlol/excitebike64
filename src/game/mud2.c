#include "game.h"

/* A frame of the particles: the starting dust cloud, ageing, motion (gravity on mud and sparks, drag on splashes),
   and the exhaust puff timers. */
void particles_update(void)
{
    Particle *p;
    Particle *base;
    s32 i;
    s32 type;
    s32 *timer;

    if (gDebugFreeze == 0 && gNumScreens < 4 && gPauseMenu == 0) {
        particles_emit_dust_cloud();
        i = 0;
        base = sParticles;
        do {
            p = (Particle *)((s32)base + i * sizeof(Particle));
            if (p->life >= 0) {
                p->life = p->life - 1;
                type = p->type;
                p->pos.x = p->pos.x + p->vel.x;
                p->pos.y = p->pos.y + p->vel.y;
                p->pos.z = p->pos.z + p->vel.z;
                if (type == PARTICLE_MUD) {
                    p->vel.y = p->vel.y - 0.008166667f; /* 29.4 m/s^2 at 60 frames per second */
                } else if (type == PARTICLE_SPLASH) {
                    p->vel.x = p->vel.x * 0.99f;
                    p->vel.z = p->vel.z * 0.99f;
                } else if (type == PARTICLE_SPARK) {
                    p->vel.y = p->vel.y - 0.0020416668f;
                }
            }
            i = i + 1;
        } while (i < PARTICLE_COUNT);
        i = 0;
        timer = sExhaustPuffTimers;
        do {
            i = i + 1;
            if (*timer != 0) {
                *timer = *timer - 1;
            }
            timer = timer + 1;
        } while (i < 2 * 8);
    }
}

/* Starts a particle in the next free slot of the ring (none when the ring is full): its life depends on the type;
   a mud clod lives until it falls back to where it started. */
void particle_spawn(s32 type, s32 surface, Vec3f *pos, Vec3f *vel, u8 *color)
{
    s32 i;
    Particle *p;

    for (i = sParticleLast + 1; ; i++) {
        if (i >= PARTICLE_COUNT) i = 0;
        if (sParticles[i].life <= 0) {
            sParticles[i].pos = *pos;
            sParticles[i].vel = *vel;
            sParticles[i].surface = surface;
            if (type == PARTICLE_MUD) {
                sParticles[i].life = (s16)(vel->y * 122.448975f + 1.0f) * 2;
            } else if (type == PARTICLE_SPARK) {
                sParticles[i].life = 15;
            } else if (type == PARTICLE_SPARKLE) {
                sParticles[i].life = 10;
            } else if (type == PARTICLE_SPLASH) {
                sParticles[i].life = 20;
            } else if (type == 4) { /* not spawned anywhere */
                sParticles[i].life = 2;
            } else if (type == PARTICLE_SMOKE) {
                sParticles[i].life = 120;
            } else {
                sParticles[i].life = 15;
            }
            p = (Particle *)(i * sizeof(Particle) + (u32)sParticles);
            p->type = type;
            p->maxLife = p->life;
            sParticles[i].r = color[0];
            sParticles[i].g = color[1];
            sParticles[i].b = color[2];
            sParticleLast = i;
            return;
        }
        if (i == sParticleLast) return;
    }
}

/* While the player's spark timer runs (one view only), a spark a frame from the point where the bike scraped. */
void particles_emit_sparks(Rider *rider)
{
    f32 pos[3];
    f32 vel[3];
    u32 rnd;
    s32 push;

    if (gNumScreens == 1 && rider->sparkTimer != 0) {
        rider->sparkTimer = rider->sparkTimer - 1;
        if (rider == gRiders) {
            vec3_mul_mtx3f(&rider->sparkDir.x, &rider->bike.right.x, pos);
            pos[0] = rider->bike.pos.x + pos[0];
            pos[1] = rider->bike.pos.y + pos[1];
            pos[2] = rider->bike.pos.z + pos[2];
            vel[0] = rider->bike.vel.x * 0.016666668f;
            vel[1] = rider->bike.vel.y * 0.016666668f;
            vel[2] = rider->bike.vel.z * 0.016666668f;
            rnd = random_byte();
            if (0.0f < rider->sparkDir.x) {
                push = rnd + 0x80;
            } else {
                push = rnd - 0x80;
            }
            vel[0] = vel[0] + rider->bike.right.x * ((f32)push * 0.00012207031f);
            vel[2] = vel[2] + rider->bike.right.z * ((f32)push * 0.00012207031f);
            rnd = random_byte();
            vel[1] = vel[1] + (f32)(s32)(rnd - 0x40) * 0.02f * 0.0078125f;
            particle_spawn(PARTICLE_SPARK, 0, (Vec3f *)pos, (Vec3f *)vel, gParticleWhite);
        }
    }
    return;
}

/* Loads the particle textures: puffs (made white, their intensity moved into alpha), mud clods, sparkles, splashes
   and exhaust puffs. */
void particles_load_textures(void *resSet)
{
    s32 size;
    s32 j;
    s32 i;
    u8 *px;

    sPuffTextures = (TexSet *)res_load(resSet, sPuffTexResId);
    size = sPuffTextures->tex[0].width * (sPuffTextures->tex[0].height << 3);
    if (sPuffTextures->count != 0) {
        i = 0;
        do {
            j = 0;
            if (size != 0) {
                do {
                    px = sPuffTextures->tex[i].data;
                    px = px + j;
                    *px = *px >> 4;
                    px = sPuffTextures->tex[i].data;
                    px = px + j;
                    *px = *px & 0xf;
                    px = sPuffTextures->tex[i].data;
                    px = px + j;
                    j++;
                    *px = *px | 0xf0;
                } while (j < size);
            }
            i++;
        } while (i < sPuffTextures->count);
    }
    sClodTextures = (TexSet *)res_load(resSet, sClodTexResId);
    sSparkleTextures = (TexSet *)res_load(resSet, sSparkleTexResId);
    i = 0;
    if (sSparkleTextures->count != 0) {
        do {
            size = sSparkleTextures->tex[i].width * (sSparkleTextures->tex[i].height << 3);
            j = 0;
            if (size != 0) {
                do {
                    px = sSparkleTextures->tex[i].data;
                    px = px + j;
                    j++;
                    *px = *px | 0xf0;
                } while (j < size);
            }
            i++;
        } while (i < sSparkleTextures->count);
    }
    sSplashTextures = (TexSet *)res_load(resSet, sSplashTexResId);
    prim_flip_images(sSplashTextures);
    sExhaustTextures = (TexSet *)res_load(resSet, sExhaustTexResId);
    prim_flip_images(sExhaustTextures);
    gWaterSplashTextures = (TexSet *)res_load(resSet, sWaterSplashTexResId);
    prim_flip_images(gWaterSplashTextures);
}

/* gRiderGroundSurface as bytes: bits 4-7 of the low byte are the gSurfaceTable index */
typedef struct {
    u8 b[4];
} Col;

extern Col gRiderGroundSurface_2[];

/* Every fourth frame: dust behind each bike on a dusty surface (bursts on landing, turbo or a spray), and mud clods
   thrown up by the turbo or a landing on mud. */
void particles_emit_bike_dirt(void)
{
    V3 pad0;
    Vec3f vel;
    Vec3f pos;
    V3 pad1;
    V3 pad2;
    s32 i, j;
    s32 surface;
    s32 spray, burst;
    s32 heading;
    f32 sinH, cosH, latAcc;
    f32 spread;

    if (gDebugFreeze != 0 || gNumScreens >= 4 || gPauseMenu != 0) {
        return;
    }
    if (++sDirtFrameCount & 3) {
        return;
    }
    spread = 12800.0f;
    for (i = 0; i < 8; i++) {
        Rider *rider = &gRiders[i];
        if (rider->inactive != 0) continue;
        if (gRiderViewLod[i] == 0) continue;
        surface = gRiderGroundSurface_2[i].b[3] >> 4;
        if (rider->crashState != 0) continue;
        if (rider->rearWheel.airTicks <= 0 && (rider->rearWheel.surface->flags & SURFACE_DUST)) {
            vel.x = 0.0f;
            vel.y = 0.0f;
            vel.z = 0.0f;
            pos = rider->rearWheel.contact;
            pos.y += 0.3f;
            if (rider->speed < 8.0f) {
                pos.x -= (8.0f - rider->speed) * rider->bike.fwd.x * 0.125f;
                pos.z -= (8.0f - rider->speed) * rider->bike.fwd.z * 0.125f;
            }
            burst = 0;
            spray = 0;
            if ((rider->input & INPUT_Z) || rider->dirtSpray == 1) {
                spray = 1;
            }
            if (gRaceFrame - rider->landingFrame < 15) {
                burst = 1;
            } else if (gRaceFrame - rider->turboPressFrame < 10) {
                burst = 1;
            } else if (rider->dirtSpray == 2) {
                burst = 1;
            }
            if (spray && (gCamZoneState[0].flags & 1)) {
                burst = 1;
            }
            if (burst) {
                if (gEvent.state >= 5 || (gRaceFrame & 1)) {
                    vel.y = 0.07f;
                    vel.z += (s32)(random_byte() - 0x80) / spread;
                    vel.x += (s32)(random_byte() - 0x80) / spread;
                    heading = (s16)rider->heading >> 4;
                    sinH = -gCosTable[(heading - 0x400) & 0xFFF] * 6.1035156e-05f;
                    cosH = gCosTable[heading & 0xFFF] * 6.1035156e-05f;
                    latAcc = rider->bike.acc.x * cosH - rider->bike.acc.z * sinH;
                    vel.x -= cosH * latAcc / 250.0f;
                    vel.z += sinH * latAcc / 250.0f;
                    particle_spawn(PARTICLE_DUST, surface, (Vec3f *)&pos, (Vec3f *)&vel, sRiderLightColor[i]);
                }
            } else if (spray) {
                particle_spawn(PARTICLE_DUST, surface, (Vec3f *)&pos, (Vec3f *)&vel, sRiderLightColor[i]);
            }
        }
        if (((rider->input & INPUT_Z) || gRaceFrame - rider->landingFrame < 10) && (rider->rearWheel.surface->flags & SURFACE_CLODS)) {
            if (rider->isCpu == 0 || (random_byte() & 1)) {
                for (j = 0; j < 2; j++) {
                    Wheel *wheel;
                    vel = gRiders[i].bike.vel;
                    vel.x = vel.x / 80.0f + (((s32)random_byte() - 128.0f) * 0.01f) * 0.0078125f;
                    vel.y = ((s32)random_byte() / 50.0f + 5.0f) / 100.0f;
                    vel.z = vel.z / 80.0f + (((s32)random_byte() - 128.0f) * 0.01f) * 0.0078125f;
                    wheel = &gRiders[i].rearWheel;
                    particle_spawn(PARTICLE_MUD, surface, (Vec3f *)&wheel->contact, (Vec3f *)&vel, sRiderLightColor[i]);
                }
            }
        }
    }
}

/* A bike in water: splashes either side where its body meets the water line, and on water it cannot ride through
   the big splash and a crash, else a splash sound at most every two seconds. */
void particles_emit_water_splash(Rider *rider)
{
    volatile s32 pad[4];
    f32 point[3];
    f32 dir[3];
    f32 front[3];
    f32 back[3];
    s32 water[2];
    s32 idx;
    s32 surface;
    f32 waterY;
    f32 t;

    if (rider->inactive != 0) goto done;
    idx = rider - gRiders;
    surface = rider->surfaceIdx;
    if (!(1.0f < rider->speed)) goto done;
    if (!(gSurfaceTable[surface].flags & SURFACE_WATER)) goto done;

    *(Vec3f *)point = rider->bike.pos;
    if (!tube_get_water_level((s32)(point[0] * 1000.0f), (s32)(point[2] * 1000.0f), water)) goto done;

    waterY = (f32)water[0] / 1000.0f;
    dir[0] = 0.0f;
    dir[1] = 0.8f;
    dir[2] = 1.0f;
    vec3_mul_mtx3f(dir, &rider->bike.right.x, front);
    front[0] = front[0] + rider->bike.pos.x;
    front[1] = front[1] + rider->bike.pos.y;
    front[2] = front[2] + rider->bike.pos.z;
    if (front[1] < waterY) goto done;

    dir[0] = 0.0f;
    dir[1] = -0.3f;
    dir[2] = 1.0f;
    vec3_mul_mtx3f(dir, &rider->bike.right.x, back);
    back[0] = back[0] + rider->bike.pos.x;
    back[1] = back[1] + rider->bike.pos.y;
    back[2] = back[2] + rider->bike.pos.z;
    if (waterY < back[1]) {
        dir[0] = 0.0f;
        dir[1] = -0.3f;
        dir[2] = -1.0f;
        vec3_mul_mtx3f(dir, &rider->bike.right.x, front);
        front[0] = front[0] + rider->bike.pos.x;
        front[1] = front[1] + rider->bike.pos.y;
        front[2] = front[2] + rider->bike.pos.z;
        if (waterY < front[1]) goto done;
    }

    t = (waterY - back[1]) / (front[1] - back[1]);
    point[0] = back[0] + t * (front[0] - back[0]);
    point[1] = waterY;
    point[2] = back[2] + t * (front[2] - back[2]);

    if (gRiderViewLod[idx] != 0 && (gRaceFrame & 3) == 0) {
        dir[0] = (f32)*(s16 *)((u8 *)gCosTable + ((rider->drawRot[2] >> 3) & 0x1FFE)) / 655360.0f;
        dir[1] = 0.0f;
        dir[2] = (f32)*(s16 *)((u8 *)gCosTable + ((((rider->drawRot[2] >> 4) - 0x400) & 0xFFF) << 1)) / 655360.0f;
        particle_spawn(PARTICLE_SPLASH, surface, (Vec3f *)point, (Vec3f *)dir, gParticleWhite);
        dir[0] = -(f32)*(s16 *)((u8 *)gCosTable + ((rider->drawRot[2] >> 3) & 0x1FFE)) / 655360.0f;
        dir[1] = 0.0f;
        dir[2] = -(f32)*(s16 *)((u8 *)gCosTable + ((((rider->drawRot[2] >> 4) - 0x400) & 0xFFF) << 1)) / 655360.0f;
        particle_spawn(PARTICLE_SPLASH, surface, (Vec3f *)point, (Vec3f *)dir, gParticleWhite);
    }

    /* water the rider cannot ride through: the big splash and a crash */
    if ((gSurfaceTable[surface].flags & (SURFACE_CRASH_ON_TOUCH | SURFACE_OUT_OF_BOUNDS)) != 0) {
        if (gRiderViewLod[idx] != 0) {
            tube_spawn_splash(idx, (s32)(point[0] * 1000.0f), water[0], (s32)(point[2] * 1000.0f));
        }
        if (rider->pendingCrash == 0) {
            rider->pendingCrash = 0x1c; /* forwards, now */
            cam_set_frozen(idx, 1);
        }
    } else {
        if ((s32)(gRaceFrame - sSplashSoundTime[idx]) > 0x78) {
            snd_play_rider_sfx(3, idx, 0xff); /* the splash sound */
            sSplashSoundTime[idx] = gRaceFrame;
        }
    }

done:
    return;
}

/* For 30 frames after particles_start_dust_cloud: dust around its point. */
void particles_emit_dust_cloud(void)
{
    s32 i;
    s32 surface;
    s32 rnd;
    Vec3f pos;
    Vec3f vel;

    if (gNumScreens < 4) {
        if (gRaceFrame < sDustCloudStart) {
            sDustCloudStart = -1000;
        }
        if (gRaceFrame - sDustCloudStart < 0x1F) {
            surface = 0;
            i = surface;
            vel.x = 0.0f;
            vel.y = 0.0f;
            vel.z = 0.0f;
            do {
                i = i + 1;
                rnd = random_byte();
                pos.x = sDustCloudPos.x + ((f32)rnd - 128.0f) * 0.00390625f;
                rnd = random_byte();
                pos.y = sDustCloudPos.y + ((f32)rnd - 128.0f) * 0.0009765625f;
                rnd = random_byte();
                pos.z = sDustCloudPos.z + ((f32)rnd - 128.0f) * 0.00390625f;
                particle_spawn(PARTICLE_DUST, surface, &pos, &vel, sRiderLightColor[sDustCloudBike]);
                sParticles[sParticleLast].maxLife = 30;
            } while (i <= 0);
        }
    }
}

/* particles_project_point written inline: a point in metres to the screen (stadium coordinates are absolute, the
   others relative to the camera). Returns project_to_screen's result. */
static inline s32 project(Vec3f *p, s32 view, s32 mtx, s32 *sx, s32 *sy, s32 *sz, s32 *w) {
    if (gCurTrack_4->f2C == TRACK_ENV_STADIUM) {
        return project_to_screen(p->x * 1000.0f * 0.0625f, p->y * 1000.0f * 0.0625f, p->z * 1000.0f * 0.0625f, sx, sy,
                             sz, w, mtx, view);
    } else {
        return project_to_screen(p->x * 1000.0f * 0.0625f - (gCamPos[view].x >> 4),
                             p->y * 1000.0f * 0.0625f - (gCamPos[view].y >> 4),
                             p->z * 1000.0f * 0.0625f - (gCamPos[view].z >> 4), sx, sy, sz, w, mtx, view);
    }
}

/* Draws the particles of a view, one pass per type, as sprites sized by distance and fading with age: dust, smoke,
   splashes, exhaust puffs, mud, sparkles; sparks are single pixels. Particles off the screen of the only view are
   killed. */
Gfx *particles_draw(Gfx *gdl, s32 view, s32 dyn) {
    s32 mtx[9];
    s32 ofs[3];
    s32 out[3];
    Vec3f pos;
    s32 sx, sy, sz, w;
    s32 i, size, grow, height, dsdx, dtdy, alpha, lastFrame;
    s32 aux; /* a particle's surface, or the smoke's fade with distance */
    Rider *rider;

    if (gNumScreens >= 4) {
        return gdl;
    }

    gDPPipeSync(gdl++);
    gDPSetColorDither(gdl++, G_CD_NOISE);
    gDPSetTexturePersp(gdl++, G_TP_NONE);
    gDPSetTextureLUT(gdl++, G_TT_NONE);
    gDPSetCombine(gdl++, 0x30B261, 0xFF67FFFF);
    gDPSetRenderMode(gdl++, 0x404A50, 0);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    gDPSetDepthSource(gdl++, G_ZS_PRIM);
    gDPLoadTextureBlock(gdl++, sPuffTextures->tex[0].data, G_IM_FMT_IA, G_IM_SIZ_8b, sPuffTextures->tex[0].width * 8,
                        sPuffTextures->tex[0].height, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

    for (i = 0; i < PARTICLE_COUNT; i++) {
        if (sParticles[i].life >= 0 && sParticles[i].type == PARTICLE_DUST) {
            if (project(&sParticles[i].pos, view, dyn, &sx, &sy, &sz, &w) != 0) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else if (sx < 0 || gScreenWidth < sx || sy < 0 || gScreenHeight < sy) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else if (w < 5001 && w != 0) {
                size = 15000 / w;
                if (gNumViews == 2) {
                    size = size * 2 / 3;
                } else if (gNumViews >= 3) {
                    size /= 2;
                }
                if (size > 48) {
                    size = 48;
                }
                if (sParticles[i].vel.y > 0.0f) {
                    grow = ((sParticles[i].maxLife - sParticles[i].life) << 10) / sParticles[i].maxLife + 0x80;
                    if (grow > 0x400) {
                        grow = 0x400;
                    }
                    if (gPauseMenu == 0) {
                        sParticles[i].vel.y -= 0.005f;
                    }
                } else {
                    grow = ((sParticles[i].maxLife - sParticles[i].life) << 9) / sParticles[i].maxLife + 0x100;
                    if (grow > 0x200) {
                        grow = 0x200;
                    }
                }
                size = size * grow / 256;
                if (size < 3) {
                    size = 3;
                }
                dsdx = (sPuffTextures->tex[0].width << 13) / size;
                aux = sParticles[i].surface;
                height = size * 2 / 3;
                alpha = 0xFF - gSurfaceTable[aux].color[3];
                alpha = (((sParticles[i].life << 7) / sParticles[i].maxLife + 0x7F) * alpha) >> 8;
                dtdy = (sPuffTextures->tex[0].height << 10) / height;
                if (alpha != 0) {
                    gDPSetPrimColor(gdl++, 0, 0, (gSurfaceTable[aux].color[0] * sParticles[i].r) >> 8,
                                    (gSurfaceTable[aux].color[1] * sParticles[i].g) >> 8, (gSurfaceTable[aux].color[2] * sParticles[i].b) >> 8,
                                    alpha);
                    gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                    gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - height / 2) << 2, (sx + size / 2) << 2,
                                        (sy + height / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                }
            }
        }
    }

    for (i = 0; i < PARTICLE_COUNT; i++) {
        if (sParticles[i].life >= 0 && sParticles[i].type == PARTICLE_SMOKE) {
            if (project(&sParticles[i].pos, view, dyn, &sx, &sy, &sz, &w) != 0) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else if (sx < 0 || gScreenWidth < sx || sy < 0 || gScreenHeight < sy) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else if (w != 0) {
                grow = ((sParticles[i].maxLife - sParticles[i].life) * 0x2800) / sParticles[i].maxLife + 0x500;
                size = 15000 / w;
                if (grow > 0x400) {
                    grow = 0x400;
                }
                size = size * grow / 256;
                if (size < 20) {
                    size = 20;
                }
                if (size > 100) {
                    size = 100;
                }
                dsdx = (sPuffTextures->tex[0].width << 13) / size;
                alpha = (sParticles[i].life * 0xC0) / sParticles[i].maxLife + 0x3F;
                height = size * 2 / 3;
                aux = 0xFF - (w * 0xFF) / 15000;
                dtdy = (sPuffTextures->tex[0].height << 10) / height;
                if (aux < 0) {
                    aux = 0;
                }
                if (aux > 255) {
                    aux = 255;
                }
                alpha = (alpha * aux) >> 8;
                if (alpha != 0) {
                    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, alpha);
                    gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                    gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - height / 2) << 2, (sx + size / 2) << 2,
                                        (sy + height / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                }
            }
        }
    }

    for (i = 0; i < PARTICLE_COUNT; i++) {
        if (sParticles[i].life >= 0 && sParticles[i].type == PARTICLE_SPLASH) {
            if (project(&sParticles[i].pos, view, dyn, &sx, &sy, &sz, &w) != 0) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else if (sx < 0 || gScreenWidth < sx || sy < 0 || gScreenHeight < sy) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else if (w != 0) {
                size = 7500 / w;
                if (gNumViews >= 3) {
                    size /= 2;
                }
                if (size > 48) {
                    size = 48;
                }
                grow = ((sParticles[i].maxLife - sParticles[i].life) << 10) / sParticles[i].maxLife + 0x100;
                if (grow > 0x400) {
                    grow = 0x400;
                }
                size = size * grow / 256;
                if (size < 3) {
                    size = 3;
                }
                dsdx = (sSplashTextures->tex[0].width << 13) / size;
                height = size * 2 / 3;
                dtdy = (sSplashTextures->tex[0].height << 10) / height;
                gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, (sParticles[i].life * 245) / sParticles[i].maxLife + 20);
                grow = (u32)(i * 123 + sParticles[i].maxLife - sParticles[i].life) % sSplashTextures->count;
                gDPLoadTextureBlock(gdl++, sSplashTextures->tex[grow].data, G_IM_FMT_IA, G_IM_SIZ_8b, sSplashTextures->tex[grow].width * 8,
                                    sSplashTextures->tex[grow].height, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
                gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - height / 2) << 2, (sx + size / 2) << 2,
                                    (sy + height / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
            }
        }
    }

    for (i = 0; i < 2 * 8; i++) { /* two puffs per bike */
        if (sExhaustPuffTimers[i] != 0) {
            grow = ((15 - sExhaustPuffTimers[i]) * sExhaustTextures->count) / 15;
            if (grow < 0) {
                grow = 0;
            }
            if (grow >= sExhaustTextures->count) {
                grow = sExhaustTextures->count - 1;
            }
            gDPLoadTextureBlock(gdl++, sExhaustTextures->tex[grow].data, G_IM_FMT_IA, G_IM_SIZ_8b, sExhaustTextures->tex[grow].width * 8,
                                sExhaustTextures->tex[grow].height, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
            rider = &gRiders[i / 2];
            /* behind the exhaust, moving back as the puff ages */
            mtx3i_from_angles(mtx, rider->drawRot[0], rider->drawRot[1], rider->drawRot[2]);
            ofs[0] = 100;
            ofs[1] = 700;
            ofs[2] = (sExhaustPuffTimers[i] - 15) * 40 - 650;
            vec3i_mul_mtx3i(ofs, mtx, out);
            pos.x = (f32)(out[0] + rider->drawPos.x) / 1000.0f;
            pos.y = (f32)(out[1] + rider->drawPos.y) / 1000.0f;
            pos.z = (f32)(out[2] + rider->drawPos.z) / 1000.0f;
            if (project(&pos, view, dyn, &sx, &sy, &sz, &w) != 0) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else if (sx < 0 || gScreenWidth < sx || sy < 0 || gScreenHeight < sy) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else if (w != 0) {
                size = 10000 / w;
                if (gNumViews >= 2) {
                    size /= 2;
                }
                grow = ((15 - sExhaustPuffTimers[i]) << 8) / 15;
                grow += 0x100;
                size = size * grow / 256;
                if (size > 48) {
                    size = 48;
                }
                if (size < 3) {
                    size = 3;
                }
                if (gRiders[i / 2].engine.overheated != 0) {
                    size = size * 3 / 2;
                    grow = (sExhaustPuffTimers[i] * 63) / 15 + 0xC0;
                    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, grow);
                } else {
                    if (gRiders[i / 2].input & INPUT_Z) {
                        size = size * 3 / 2;
                    }
                    grow = (sExhaustPuffTimers[i] * 127) / 15 + 0x80;
                    gDPSetPrimColor(gdl++, 0, 0, 0x60, 0x60, 0x80, grow);
                }
                dsdx = (sExhaustTextures->tex[0].width << 13) / size;
                height = size * 2 / 3;
                dtdy = (sExhaustTextures->tex[0].height << 10) / height;
                gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - height / 2) << 2, (sx + size / 2) << 2,
                                    (sy + height / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
            }
        }
    }

    for (i = 0; i < PARTICLE_COUNT; i++) {
        if (sParticles[i].life >= 0 && sParticles[i].type == PARTICLE_MUD) {
            if (project(&sParticles[i].pos, view, dyn, &sx, &sy, &sz, &w) != 0) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else if (sx < 0 || gScreenWidth < sx || sy < 0 || gScreenHeight < sy) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else {
                gDPLoadTextureBlock(gdl++, sClodTextures->tex[i & 3].data, G_IM_FMT_IA, G_IM_SIZ_8b,
                                    sClodTextures->tex[i & 3].width * 8, sClodTextures->tex[i & 3].height, 0, G_TX_CLAMP, G_TX_CLAMP, 5,
                                    5, G_TX_NOLOD, G_TX_NOLOD);
                if (w != 0) {
                    size = 15000 / w;
                    size /= 6;
                    if (gNumViews >= 3) {
                        size /= 2;
                    }
                    if (size > 24) {
                        size = 24;
                    }
                    grow = ((sParticles[i].maxLife - sParticles[i].life) << 9) / sParticles[i].maxLife + 0x100;
                    if (grow > 400) {
                        grow = 400;
                    }
                    size = size * grow / 256;
                    if (size < 3) {
                        size = 3;
                    }
                    dsdx = (sClodTextures->tex[0].width << 13) / size;
                    height = size * 2 / 3;
                    dtdy = (sClodTextures->tex[0].height << 10) / height;
                    aux = sParticles[i].surface;
                    gDPSetPrimColor(gdl++, 0, 0, (gSurfaceTable[aux].color[4] * sParticles[i].r) >> 8,
                                    (gSurfaceTable[aux].color[5] * sParticles[i].g) >> 8, (gSurfaceTable[aux].color[6] * sParticles[i].b) >> 8,
                                    (sParticles[i].life << 7) / sParticles[i].maxLife + 0x40);
                    gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                    gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - height / 2) << 2, (sx + size / 2) << 2,
                                        (sy + height / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                }
            }
        }
    }

    lastFrame = -1;
    if (gNumViews == 1) {
        gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
        for (i = 0; i < PARTICLE_COUNT; i++) {
            if (sParticles[i].life >= 0 && sParticles[i].type == PARTICLE_SPARKLE) {
                if (project(&sParticles[i].pos, view, dyn, &sx, &sy, &sz, &w) != 0) {
                    if (gNumViews == 1) {
                        sParticles[i].life = -1;
                    }
                } else if (sx < 0 || gScreenWidth < sx || sy < 0 || gScreenHeight < sy) {
                    if (gNumViews == 1) {
                        sParticles[i].life = -1;
                    }
                } else {
                    /* the sparkle's frame: 0-2 then back down */
                    alpha = (sParticles[i].maxLife - sParticles[i].life) / 2;
                    if (alpha >= 3) {
                        alpha = 5 - alpha;
                    }
                    if (alpha >= 0) {
                        if (lastFrame != alpha) {
                            lastFrame = alpha;
                            gDPLoadTextureBlock(gdl++, sSparkleTextures->tex[alpha].data, G_IM_FMT_IA, G_IM_SIZ_8b,
                                                sSparkleTextures->tex[alpha].width * 8, sSparkleTextures->tex[alpha].height, 0, G_TX_CLAMP, G_TX_CLAMP,
                                                5, 5, G_TX_NOLOD, G_TX_NOLOD);
                        }
                        if (w != 0) {
                            size = 30000 / w;
                            size /= 6;
                            if (gNumViews >= 3) {
                                size /= 2;
                            }
                            if (size > 24) {
                                size = 24;
                            }
                            if (size < 15) {
                                size = 15;
                            }
                            dsdx = (sSparkleTextures->tex[alpha].width << 13) / size;
                            height = size * 2 / 3;
                            dtdy = (sSparkleTextures->tex[alpha].height << 10) / height;
                            gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                            gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - height / 2) << 2, (sx + size / 2) << 2,
                                                (sy + height / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                        }
                    }
                }
            }
        }
    }

    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x442230, 0);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    for (i = 0; i < PARTICLE_COUNT; i++) {
        if (sParticles[i].life >= 0 && sParticles[i].type == PARTICLE_SPARK) {
            if (project(&sParticles[i].pos, view, dyn, &sx, &sy, &sz, &w) != 0) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else if (sx < 0 || gScreenWidth < sx || sy < 0 || gScreenHeight < sy) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else {
                w = 0x7F - (w << 7) / 2000;
                if (w < 0) {
                    w = 0;
                }
                if (w > 127) {
                    w = 127;
                }
                gDPSetPrimColor(gdl++, 0, 0, w + 0x80, w + 0x80, 0, 255);
                gDPSetPrimDepth(gdl++, sz - 0x100, 0);
                gDPFillRectangle(gdl++, sx, sy, sx + 1, sy + 1);
            }
        }
    }

    gDPPipeSync(gdl++);
    gDPSetTexturePersp(gdl++, G_TP_PERSP);
    gDPSetCombine(gdl++, 0xFFFFFF, 0xFFFCF87C);
    gDPSetTextureLUT(gdl++, G_TT_RGBA16);
    gDPSetDepthSource(gdl++, G_ZS_PIXEL);
    gDPSetColorDither(gdl++, G_CD_MAGICSQ);
    return gdl;
}

/* Cosine and sine of a 16-bit angle, 1.0 = 0x4000 */
#define COS16(a) gCosTable[((a) >> 4) & 0xFFF]
#define SIN16(a) gCosTable[(((a) >> 4) - 0x400) & 0xFFF]

/* Rainforest Run's rain (one view): 64 streaks in screen space, in 8 rows of depth, slanted by the camera's turn and
   sideways motion (kept as they were while paused), and a splash on the ground ahead each frame. */

Gfx *rain_draw(Gfx *gdl, s32 dyn, s32 fb) {
    s32 dx, dz, a, s, c;
    s32 fwd, side, turn;
    s32 i, j, k, depthRow;
    s32 len, w, y, r, q, t, u;
    s32 rad, off, x, z;
    Vtx *src, *dst, *end;
    Vec3f pos;
    Vec3i vel;
    volatile s32 vv;

    if (sRainEnabled == 0 || (!(gCamZoneState[0].flags & 1) && (gCamSegFlags[0] & CAM_SEG_COVERED))) {
        return gdl;
    }
    gdl = render_set_viewport_2(gdl, dyn, 0);

    t = gCamPos[0].x - sRainLastCamPos.x;
    r = gCamPos[0].z - sRainLastCamPos.z;
    fwd = (((t) * COS16(gCamYaw[0])) >> 14) + (((r) * SIN16(gCamYaw[0])) >> 14);
    fwd = -fwd;
    side = (((r) * COS16(gCamYaw[0])) >> 14) - (((t) * SIN16(gCamYaw[0])) >> 14);
    side *= 2;
    turn = gCamYaw[0] - sRainLastCamYaw;
    if (fwd > 0x200) fwd = 0x200;
    if (fwd < -0x200) fwd = -0x200;
    if (side > 0x200) side = 0x200;
    if (side < -0x200) side = -0x200;
    if (turn > 0x200) turn = 0x200;
    if (turn < -0x200) turn = -0x200;

    fwd = turn; /* the forward motion is not used */
    if (gPauseMenu == 0 && (gPodiumMode == 0 || gDesaturation != 0x100)) {
        for (i = 0; i < RAIN_NUM_STREAKS; i++) {
            x = random_byte_2() * 2;
            x -= 0x100;
            x = x * gScreenWidth / 512;
            depthRow = i >> 3;
            r = depthRow * 32;
            len = r * 10 / 256 + 5;
            len = len * gScreenHeight / 240;
            w = ((r >> 1) + 0x80) * gScreenHeight;
            if (w < 0) w += 255;
            w >>= 8;
            r = random_byte_2() * w;
            if (r < 0) r += 255;
            y = r >> 8;
            y = gScreenHeight / 2 - y;
            q = -(x * 32) / gScreenWidth;
            w = 0xFBFF - (depthRow << 8);
            sRainVtx[fb][i * 4].v.ob[0] = x;
            sRainVtx[fb][i * 4].v.ob[1] = y;
            sRainVtx[fb][i * 4].v.ob[2] = w;
            t = (len * (-fwd * 16 / 500)) >> 4;
            t += (q * side) >> 9;
            sRainVtx[fb][i * 4 + 1].v.ob[0] = x + t;
            sRainVtx[fb][i * 4 + 1].v.ob[1] = y + len;
            sRainVtx[fb][i * 4 + 2].v.ob[0] = x + 2;
            sRainVtx[fb][i * 4 + 2].v.ob[1] = y;
            u = t + 2 + (((u32)len & 1) >> 1);
            sRainVtx[fb][i * 4 + 3].v.ob[0] = x + u;
            sRainVtx[fb][i * 4 + 3].v.ob[1] = y + len;
        }
    } else {
        src = sRainVtx[fb ^ 1];
        dst = sRainVtx[fb];
        end = src + RAIN_NUM_STREAKS * 4;
        do {
            *dst++ = *src++;
        } while (src != end);
    }

    gDPPipeSync(gdl++);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPSetGeometryMode(gdl++, G_ZBUFFER);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xE200001C;
        g->words.w1 = 0x404A50;
    }
    gDPSetDepthSource(gdl++, G_ZS_PRIM);
    gSPClearGeometryMode(gdl++, G_CULL_BOTH);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xDA380007; /* gSPMatrix: Dyn.m3, the orthographic projection */
        g->words.w1 = dyn + 0x300;
    }
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    for (i = 0; i < RAIN_NUM_STREAKS; i += 8) {
        gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, i * 5 * 4 / 64 + 16);
        gDPSetPrimDepth(gdl++, sRainVtx[fb][i * 4].v.ob[2], 0);
        gSPVertex(gdl++, &sRainVtx[fb][i * 4], 32, 0);
        for (j = 0; j < 8; j++) {
            gSP2Triangles(gdl++, j * 4, j * 4 + 1, j * 4 + 3, 0, j * 4, j * 4 + 3, j * 4 + 2, 0);
        }
    }
    gDPPipeSync(gdl++);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xDA380007; /* gSPMatrix: back to the perspective */
        g->words.w1 = dyn;
    }
    sRainLastCamPos = gCamPos[0];
    sRainLastCamYaw = gCamYaw[0];
    if (gPauseMenu == 0) {
        w = (random_byte_2() << 7) + 8000;
        x = random_byte_2() * w / 256 - w / 2;
        t = ((x * COS16(gCamYaw[0])) >> 14) - ((w * SIN16(gCamYaw[0])) >> 14);
        r = ((w * COS16(gCamYaw[0])) >> 14) + ((x * SIN16(gCamYaw[0])) >> 14);
        x = t + gCamPos[0].x;
        w = r + gCamPos[0].z;
        y = height_get_ground(x, 0x8000, w);
        r = (gGroundSurface >> 4) & 0xF;
        if (gGroundFound != 0 && (gCurTrack_2 - gTracks != TRACK_RAINFOREST_RUN || r != 6)) {
            vel.x = 0;
            vel.y = 0;
            vel.z = 0;
            pos.x = (f32)x / 1000.0f;
            pos.y = (f32)(y + 10) / 1000.0f;
            pos.z = (f32)w / 1000.0f;
            particle_spawn(PARTICLE_SPLASH, 0, &pos, &vel, gParticleWhite);
        }
    }
    gSPSetGeometryMode(gdl++, gCullMode);
    return gdl;
}

/* Allocates the particle ring; surfaces is the race's surface table. */
void particles_init(Surface *surfaces)
{
    sParticles = mem_alloc_dbg(PARTICLE_COUNT * sizeof(Particle), 0, sStrMud2C, 0x60);
    mem_fill((u8 *)sParticles, 0, PARTICLE_COUNT * sizeof(Particle));
    sParticleLast = 0;
    mem_fill(sExhaustPuffTimers, 0, 2 * 8 * sizeof(s32));
    mem_fill(sSplashSoundTime, 0, 0x20);
    gSurfaceTablePtr = surfaces;
    particles_update();
}

/* Frees the particle ring. */
void particles_free(void) {
    mem_free(sParticles);
}

/* Starts a dust cloud at pos (metres) in the colour of the rider's ground. */
void particles_start_dust_cloud(Vec3f *pos, s32 rider)
{
    Vec3f *dst = &sDustCloudPos;
    *dst = *pos;
    sDustCloudBike = rider;
    sDustCloudStart = gRaceFrame;
}

/* Rain on Rainforest Run with one screen: two frames of streak vertices. */
void rain_init(void)
{
    s32 i;

    sRainEnabled = 0;
    /* (gCurTrack_6 - gTracks) / 0xA0, the track index, as a multiplication */
    if ((((gCurTrack_6 - (s32)gTracks) * -0x33333333) >> 5) == TRACK_RAINFOREST_RUN &&
        (i = 0, gNumScreens == 1)) {
        for (i = 0; i < 2; i++) {
            sRainVtx[i] = mem_alloc_dbg(RAIN_NUM_STREAKS * 4 * sizeof(Vtx), 0, sStrMud2C, 0x512);
        }
        sRainEnabled = 1;
    }
}

/* Frees the rain's vertices. */
void rain_free(void)
{
    s32 i;
    u32 *bufs;
    if (sRainEnabled != 0) {
        i = 0;
        bufs = &sRainVtx;
        do {
            u32 vtx = bufs[0];
            bufs++;
            i++;
            mem_free(vtx);
        } while (i < 2);
        sRainEnabled = 0;
    }
}

/* The bike scraped at gCollidePoint: sparks from that point (in the bike's frame) for 15 frames. */
void particles_start_sparks(Rider *rider)
{
    f32 ofs[8];

    if (gNumScreens == 1) {
        ofs[0] = gCollidePoint.x - rider->bike.pos.x;
        ofs[1] = gCollidePoint.y - rider->bike.pos.y;
        ofs[2] = gCollidePoint.z - rider->bike.pos.z;
        mtx3f_mul_vec3(ofs, &rider->bike.right.x, &rider->sparkDir.x);
        rider->sparkTimer = 0xF;
    }
}

/* A point in metres to the screen through view's matrix of mtx (see project_to_screen). */
void particles_project_point(f32 *pos, s32 *sx, s32 *sy, s32 *sz, s32 *w, s32 mtx, s32 view)
{
    s32 x, y, z;
    Vec3i *t;

    if (gCurTrack_3[11] == TRACK_ENV_STADIUM) { /* gTracks +0x2C */
        x = pos[0] * 1000.0f * 0.0625f;
        y = pos[1] * 1000.0f * 0.0625f;
        z = pos[2] * 1000.0f * 0.0625f;
        project_to_screen(x, y, z, sx, sy, sz, w, mtx, view);
    } else {
        x = pos[0] * 1000.0f * 0.0625f - (f32)(gCamPos[view].x >> 4);
        y = pos[1] * 1000.0f * 0.0625f - (f32)(gCamPos[view].y >> 4);
        z = pos[2] * 1000.0f * 0.0625f - (f32)(gCamPos[view].z >> 4);
        project_to_screen(x, y, z, sx, sy, sz, w, mtx, view);
    }
}

/* Starts one of the rider's two exhaust puffs at random. */
void particles_start_exhaust_puff(s32 rider)
{
    u32 rnd = random_byte();
    sExhaustPuffTimers[rider * 2 + (rnd & 1)] = 0xF;
}
