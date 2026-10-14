#include "game.h"

void particles_update(void)
{
    Particle *p;
    Particle *base;
    s32 i;
    s32 t;
    s32 *q;

    if (gDebugFreeze == 0 && gNumScreens < 4 && gPauseMenu == 0) {
        particles_emit_dust_cloud();
        i = 0;
        base = sParticles;
        do {
            p = (Particle *)((s32)base + i * 0x24);
            if (p->life >= 0) {
                p->life = p->life - 1;
                t = p->type;
                p->pos.x = p->pos.x + p->vel.x;
                p->pos.y = p->pos.y + p->vel.y;
                p->pos.z = p->pos.z + p->vel.z;
                if (t == PARTICLE_MUD) {
                    p->vel.y = p->vel.y - 0.008166667f;
                } else if (t == PARTICLE_SPLASH) {
                    p->vel.x = p->vel.x * 0.99f;
                    p->vel.z = p->vel.z * 0.99f;
                } else if (t == PARTICLE_SPARK) {
                    p->vel.y = p->vel.y - 0.0020416668f;
                }
            }
            i = i + 1;
        } while (i < 0x60);
        i = 0;
        q = sExhaustPuffTimers;
        do {
            i = i + 1;
            if (*q != 0) {
                *q = *q - 1;
            }
            q = q + 1;
        } while (i < 0x10);
    }
}

void particle_spawn(s32 type, s32 surface, Vec3f *pos, Vec3f *vel, u8 *color)
{
    s32 i;
    Particle *p;

    for (i = sParticleLast + 1; ; i++) {
        if (i >= 0x60) i = 0;
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
            } else if (type == 4) {
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

void particles_emit_sparks(Rider *rider)
{
    f32 o[3];
    f32 t[3];
    u32 r;
    s32 iv;

    if (gNumScreens == 1 && rider->sparkTimer != 0) {
        rider->sparkTimer = rider->sparkTimer - 1;
        if (rider == gRiders) {
            vec3_mul_mtx3f(&rider->sparkDir.x, &rider->bike.right.x, o);
            o[0] = rider->bike.pos.x + o[0];
            o[1] = rider->bike.pos.y + o[1];
            o[2] = rider->bike.pos.z + o[2];
            t[0] = rider->bike.vel.x * 0.016666668f;
            t[1] = rider->bike.vel.y * 0.016666668f;
            t[2] = rider->bike.vel.z * 0.016666668f;
            r = random_byte();
            if (0.0f < rider->sparkDir.x) {
                iv = r + 0x80;
            } else {
                iv = r - 0x80;
            }
            t[0] = t[0] + rider->bike.right.x * ((f32)iv * 0.00012207031f);
            t[2] = t[2] + rider->bike.right.z * ((f32)iv * 0.00012207031f);
            r = random_byte();
            t[1] = t[1] + (f32)(s32)(r - 0x40) * 0.02f * 0.0078125f;
            particle_spawn(PARTICLE_SPARK, 0, (Vec3f *)o, (Vec3f *)t, gParticleWhite);
        }
    }
    return;
}

void particles_load_textures(void *file)
{
    s32 n;
    s32 j;
    s32 i;
    u8 *pb;

    sPuffTextures = (TexSet *)res_load(file, sPuffTexResId);
    n = sPuffTextures->tex[0].width * (sPuffTextures->tex[0].height << 3);
    if (sPuffTextures->count != 0) {
        i = 0;
        do {
            j = 0;
            if (n != 0) {
                do {
                    pb = sPuffTextures->tex[i].data;
                    pb = pb + j;
                    *pb = *pb >> 4;
                    pb = sPuffTextures->tex[i].data;
                    pb = pb + j;
                    *pb = *pb & 0xf;
                    pb = sPuffTextures->tex[i].data;
                    pb = pb + j;
                    j++;
                    *pb = *pb | 0xf0;
                } while (j < n);
            }
            i++;
        } while (i < sPuffTextures->count);
    }
    sClodTextures = (TexSet *)res_load(file, sClodTexResId);
    sSparkleTextures = (TexSet *)res_load(file, sSparkleTexResId);
    i = 0;
    if (sSparkleTextures->count != 0) {
        do {
            n = sSparkleTextures->tex[i].width * (sSparkleTextures->tex[i].height << 3);
            j = 0;
            if (n != 0) {
                do {
                    pb = sSparkleTextures->tex[i].data;
                    pb = pb + j;
                    j++;
                    *pb = *pb | 0xf0;
                } while (j < n);
            }
            i++;
        } while (i < sSparkleTextures->count);
    }
    sSplashTextures = (TexSet *)res_load(file, sSplashTexResId);
    prim_flip_images(sSplashTextures);
    sExhaustTextures = (TexSet *)res_load(file, sExhaustTexResId);
    prim_flip_images(sExhaustTextures);
    gWaterSplashTextures = (TexSet *)res_load(file, sWaterSplashTexResId);
    prim_flip_images(gWaterSplashTextures);
}

typedef struct {
    u8 b[4];
} Col;

extern Col gRiderGroundSurface_2[];

void particles_emit_bike_dirt(void)
{
    V3 pad0;
    Vec3f A;
    Vec3f B;
    V3 pad1;
    V3 pad2;
    s32 i, j;
    s32 bA;
    s32 bB, bC;
    s32 ang;
    f32 s, c, d;
    f32 kr;

    if (gDebugFreeze != 0 || gNumScreens >= 4 || gPauseMenu != 0) {
        return;
    }
    if (++sDirtFrameCount & 3) {
        return;
    }
    kr = 12800.0f;
    for (i = 0; i < 8; i++) {
        Rider *b = &gRiders[i];
        if (b->inactive != 0) continue;
        if (gRiderViewLod[i] == 0) continue;
        bA = gRiderGroundSurface_2[i].b[3] >> 4;
        if (b->crashState != 0) continue;
        if (b->rearWheel.airTicks <= 0 && (b->rearWheel.surface->flags & 2)) {
            A.x = 0.0f;
            A.y = 0.0f;
            A.z = 0.0f;
            B = b->rearWheel.contact;
            B.y += 0.3f;
            if (b->speed < 8.0f) {
                B.x -= (8.0f - b->speed) * b->bike.fwd.x * 0.125f;
                B.z -= (8.0f - b->speed) * b->bike.fwd.z * 0.125f;
            }
            bC = 0;
            bB = 0;
            if ((b->input & 0x20000) || b->dirtSpray == 1) {
                bB = 1;
            }
            if (gRaceFrame - b->landingFrame < 15) {
                bC = 1;
            } else if (gRaceFrame - b->turboPressFrame < 10) {
                bC = 1;
            } else if (b->dirtSpray == 2) {
                bC = 1;
            }
            if (bB && (gCamZoneState[0].flags & 1)) {
                bC = 1;
            }
            if (bC) {
                if (gEvent.state >= 5 || (gRaceFrame & 1)) {
                    A.y = 0.07f;
                    A.z += (s32)(random_byte() - 0x80) / kr;
                    A.x += (s32)(random_byte() - 0x80) / kr;
                    ang = (s16)b->heading >> 4;
                    s = -gCosTable[(ang - 0x400) & 0xFFF] * 6.1035156e-05f;
                    c = gCosTable[ang & 0xFFF] * 6.1035156e-05f;
                    d = b->bike.acc.x * c - b->bike.acc.z * s;
                    A.x -= c * d / 250.0f;
                    A.z += s * d / 250.0f;
                    particle_spawn(PARTICLE_DUST, bA, (Vec3f *)&B, (Vec3f *)&A, sRiderLightColor[i]);
                }
            } else if (bB) {
                particle_spawn(PARTICLE_DUST, bA, (Vec3f *)&B, (Vec3f *)&A, sRiderLightColor[i]);
            }
        }
        if (((b->input & 0x20000) || gRaceFrame - b->landingFrame < 10) && (b->rearWheel.surface->flags & 4)) {
            if (b->isCpu == 0 || (random_byte() & 1)) {
                for (j = 0; j < 2; j++) {
                    Wheel *sp;
                    A = gRiders[i].bike.vel;
                    A.x = A.x / 80.0f + (((s32)random_byte() - 128.0f) * 0.01f) * 0.0078125f;
                    A.y = ((s32)random_byte() / 50.0f + 5.0f) / 100.0f;
                    A.z = A.z / 80.0f + (((s32)random_byte() - 128.0f) * 0.01f) * 0.0078125f;
                    sp = &gRiders[i].rearWheel;
                    particle_spawn(PARTICLE_MUD, bA, (Vec3f *)&sp->contact, (Vec3f *)&A, sRiderLightColor[i]);
                }
            }
        }
    }
}

void particles_emit_water_splash(Rider *p)
{
    volatile s32 pad[4];
    f32 pt[3];
    f32 A[3];
    f32 B[3];
    f32 C[3];
    s32 loc[2];
    s32 idx;
    s32 mv;
    f32 lim;
    f32 t;

    if (p->inactive != 0) goto done;
    idx = p - gRiders;
    mv = p->surfaceIdx;
    if (!(1.0f < p->speed)) goto done;
    if (!(gSurfaceTable[mv].flags & 8)) goto done;

    *(Vec3f *)pt = p->bike.pos;
    if (!tube_get_water_level((s32)(pt[0] * 1000.0f), (s32)(pt[2] * 1000.0f), loc)) goto done;

    lim = (f32)loc[0] / 1000.0f;
    A[0] = 0.0f;
    A[1] = 0.8f;
    A[2] = 1.0f;
    vec3_mul_mtx3f(A, &p->bike.right.x, B);
    B[0] = B[0] + p->bike.pos.x;
    B[1] = B[1] + p->bike.pos.y;
    B[2] = B[2] + p->bike.pos.z;
    if (B[1] < lim) goto done;

    A[0] = 0.0f;
    A[1] = -0.3f;
    A[2] = 1.0f;
    vec3_mul_mtx3f(A, &p->bike.right.x, C);
    C[0] = C[0] + p->bike.pos.x;
    C[1] = C[1] + p->bike.pos.y;
    C[2] = C[2] + p->bike.pos.z;
    if (lim < C[1]) {
        A[0] = 0.0f;
        A[1] = -0.3f;
        A[2] = -1.0f;
        vec3_mul_mtx3f(A, &p->bike.right.x, B);
        B[0] = B[0] + p->bike.pos.x;
        B[1] = B[1] + p->bike.pos.y;
        B[2] = B[2] + p->bike.pos.z;
        if (lim < B[1]) goto done;
    }

    t = (lim - C[1]) / (B[1] - C[1]);
    pt[0] = C[0] + t * (B[0] - C[0]);
    pt[1] = lim;
    pt[2] = C[2] + t * (B[2] - C[2]);

    if (gRiderViewLod[idx] != 0 && (gRaceFrame & 3) == 0) {
        A[0] = (f32)*(s16 *)((u8 *)gCosTable + ((p->drawRot[2] >> 3) & 0x1FFE)) / 655360.0f;
        A[1] = 0.0f;
        A[2] = (f32)*(s16 *)((u8 *)gCosTable + ((((p->drawRot[2] >> 4) - 0x400) & 0xFFF) << 1)) / 655360.0f;
        particle_spawn(PARTICLE_SPLASH, mv, (Vec3f *)pt, (Vec3f *)A, gParticleWhite);
        A[0] = -(f32)*(s16 *)((u8 *)gCosTable + ((p->drawRot[2] >> 3) & 0x1FFE)) / 655360.0f;
        A[1] = 0.0f;
        A[2] = -(f32)*(s16 *)((u8 *)gCosTable + ((((p->drawRot[2] >> 4) - 0x400) & 0xFFF) << 1)) / 655360.0f;
        particle_spawn(PARTICLE_SPLASH, mv, (Vec3f *)pt, (Vec3f *)A, gParticleWhite);
    }

    if ((gSurfaceTable[mv].flags & 0x180) != 0) {
        if (gRiderViewLod[idx] != 0) {
            tube_spawn_splash(idx, (s32)(pt[0] * 1000.0f), loc[0], (s32)(pt[2] * 1000.0f));
        }
        if (p->pendingCrash == 0) {
            p->pendingCrash = 0x1c;
            cam_set_frozen(idx, 1);
        }
    } else {
        if ((s32)(gRaceFrame - sSplashSoundTime[idx]) > 0x78) {
            snd_play_rider_sfx(3, idx, 0xff);
            sSplashSoundTime[idx] = gRaceFrame;
        }
    }

done:
    return;
}

void particles_emit_dust_cloud(void)
{
    s32 i;
    s32 z;
    s32 v;
    Vec3f pos;
    Vec3f vel;

    if (gNumScreens < 4) {
        if (gRaceFrame < sDustCloudStart) {
            sDustCloudStart = -1000;
        }
        if (gRaceFrame - sDustCloudStart < 0x1F) {
            z = 0;
            i = z;
            vel.x = 0.0f;
            vel.y = 0.0f;
            vel.z = 0.0f;
            do {
                i = i + 1;
                v = random_byte();
                pos.x = sDustCloudPos.x + ((f32)v - 128.0f) * 0.00390625f;
                v = random_byte();
                pos.y = sDustCloudPos.y + ((f32)v - 128.0f) * 0.0009765625f;
                v = random_byte();
                pos.z = sDustCloudPos.z + ((f32)v - 128.0f) * 0.00390625f;
                particle_spawn(PARTICLE_DUST, z, &pos, &vel, sRiderLightColor[sDustCloudBike]);
                sParticles[sParticleLast].maxLife = 30;
            } while (i <= 0);
        }
    }
}

static inline s32 project(Vec3f *p, s32 view, s32 arg2, s32 *sx, s32 *sy, s32 *sz, s32 *w) {
    if (gCurTrack_4->f2C == 0) {
        return project_to_screen(p->x * 1000.0f * 0.0625f, p->y * 1000.0f * 0.0625f, p->z * 1000.0f * 0.0625f, sx, sy,
                             sz, w, arg2, view);
    } else {
        return project_to_screen(p->x * 1000.0f * 0.0625f - (gCamPos[view].x >> 4),
                             p->y * 1000.0f * 0.0625f - (gCamPos[view].y >> 4),
                             p->z * 1000.0f * 0.0625f - (gCamPos[view].z >> 4), sx, sy, sz, w, arg2, view);
    }
}

Gfx *particles_draw(Gfx *gdl, s32 view, s32 arg2) {
    s32 mtx[9];
    s32 v[3];
    s32 out[3];
    Vec3f pos;
    s32 sx, sy, sz, w;
    s32 i, size, k, hh, dsdx, dtdy, alpha, a, f, last;
    Rider *b;

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

    for (i = 0; i < 96; i++) {
        if (sParticles[i].life >= 0 && sParticles[i].type == PARTICLE_DUST) {
            if (project(&sParticles[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
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
                    k = ((sParticles[i].maxLife - sParticles[i].life) << 10) / sParticles[i].maxLife + 0x80;
                    if (k > 0x400) {
                        k = 0x400;
                    }
                    if (gPauseMenu == 0) {
                        sParticles[i].vel.y -= 0.005f;
                    }
                } else {
                    k = ((sParticles[i].maxLife - sParticles[i].life) << 9) / sParticles[i].maxLife + 0x100;
                    if (k > 0x200) {
                        k = 0x200;
                    }
                }
                size = size * k / 256;
                if (size < 3) {
                    size = 3;
                }
                dsdx = (sPuffTextures->tex[0].width << 13) / size;
                a = sParticles[i].surface;
                hh = size * 2 / 3;
                alpha = 0xFF - gSurfaceTable[a].color[3];
                alpha = (((sParticles[i].life << 7) / sParticles[i].maxLife + 0x7F) * alpha) >> 8;
                dtdy = (sPuffTextures->tex[0].height << 10) / hh;
                if (alpha != 0) {
                    gDPSetPrimColor(gdl++, 0, 0, (gSurfaceTable[a].color[0] * sParticles[i].r) >> 8,
                                    (gSurfaceTable[a].color[1] * sParticles[i].g) >> 8, (gSurfaceTable[a].color[2] * sParticles[i].b) >> 8,
                                    alpha);
                    gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                    gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                        (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                }
            }
        }
    }

    for (i = 0; i < 96; i++) {
        if (sParticles[i].life >= 0 && sParticles[i].type == PARTICLE_SMOKE) {
            if (project(&sParticles[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else if (sx < 0 || gScreenWidth < sx || sy < 0 || gScreenHeight < sy) {
                if (gNumViews == 1) {
                    sParticles[i].life = -1;
                }
            } else if (w != 0) {
                k = ((sParticles[i].maxLife - sParticles[i].life) * 0x2800) / sParticles[i].maxLife + 0x500;
                size = 15000 / w;
                if (k > 0x400) {
                    k = 0x400;
                }
                size = size * k / 256;
                if (size < 20) {
                    size = 20;
                }
                if (size > 100) {
                    size = 100;
                }
                dsdx = (sPuffTextures->tex[0].width << 13) / size;
                alpha = (sParticles[i].life * 0xC0) / sParticles[i].maxLife + 0x3F;
                hh = size * 2 / 3;
                a = 0xFF - (w * 0xFF) / 15000;
                dtdy = (sPuffTextures->tex[0].height << 10) / hh;
                if (a < 0) {
                    a = 0;
                }
                if (a > 255) {
                    a = 255;
                }
                alpha = (alpha * a) >> 8;
                if (alpha != 0) {
                    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, alpha);
                    gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                    gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                        (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                }
            }
        }
    }

    for (i = 0; i < 96; i++) {
        if (sParticles[i].life >= 0 && sParticles[i].type == PARTICLE_SPLASH) {
            if (project(&sParticles[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
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
                k = ((sParticles[i].maxLife - sParticles[i].life) << 10) / sParticles[i].maxLife + 0x100;
                if (k > 0x400) {
                    k = 0x400;
                }
                size = size * k / 256;
                if (size < 3) {
                    size = 3;
                }
                dsdx = (sSplashTextures->tex[0].width << 13) / size;
                hh = size * 2 / 3;
                dtdy = (sSplashTextures->tex[0].height << 10) / hh;
                gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, (sParticles[i].life * 245) / sParticles[i].maxLife + 20);
                k = (u32)(i * 123 + sParticles[i].maxLife - sParticles[i].life) % sSplashTextures->count;
                gDPLoadTextureBlock(gdl++, sSplashTextures->tex[k].data, G_IM_FMT_IA, G_IM_SIZ_8b, sSplashTextures->tex[k].width * 8,
                                    sSplashTextures->tex[k].height, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
                gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                    (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
            }
        }
    }

    for (i = 0; i < 16; i++) {
        if (sExhaustPuffTimers[i] != 0) {
            k = ((15 - sExhaustPuffTimers[i]) * sExhaustTextures->count) / 15;
            if (k < 0) {
                k = 0;
            }
            if (k >= sExhaustTextures->count) {
                k = sExhaustTextures->count - 1;
            }
            gDPLoadTextureBlock(gdl++, sExhaustTextures->tex[k].data, G_IM_FMT_IA, G_IM_SIZ_8b, sExhaustTextures->tex[k].width * 8,
                                sExhaustTextures->tex[k].height, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);
            b = &gRiders[i / 2];
            mtx3i_from_angles(mtx, b->drawRot[0], b->drawRot[1], b->drawRot[2]);
            v[0] = 100;
            v[1] = 700;
            v[2] = (sExhaustPuffTimers[i] - 15) * 40 - 650;
            vec3i_mul_mtx3i(v, mtx, out);
            pos.x = (f32)(out[0] + b->drawPos.x) / 1000.0f;
            pos.y = (f32)(out[1] + b->drawPos.y) / 1000.0f;
            pos.z = (f32)(out[2] + b->drawPos.z) / 1000.0f;
            if (project(&pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
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
                k = ((15 - sExhaustPuffTimers[i]) << 8) / 15;
                k += 0x100;
                size = size * k / 256;
                if (size > 48) {
                    size = 48;
                }
                if (size < 3) {
                    size = 3;
                }
                if (gRiders[i / 2].engine.overheated != 0) {
                    size = size * 3 / 2;
                    k = (sExhaustPuffTimers[i] * 63) / 15 + 0xC0;
                    gDPSetPrimColor(gdl++, 0, 0, 0, 0, 0, k);
                } else {
                    if (gRiders[i / 2].input & 0x20000) {
                        size = size * 3 / 2;
                    }
                    k = (sExhaustPuffTimers[i] * 127) / 15 + 0x80;
                    gDPSetPrimColor(gdl++, 0, 0, 0x60, 0x60, 0x80, k);
                }
                dsdx = (sExhaustTextures->tex[0].width << 13) / size;
                hh = size * 2 / 3;
                dtdy = (sExhaustTextures->tex[0].height << 10) / hh;
                gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                    (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
            }
        }
    }

    for (i = 0; i < 96; i++) {
        if (sParticles[i].life >= 0 && sParticles[i].type == PARTICLE_MUD) {
            if (project(&sParticles[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
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
                    k = ((sParticles[i].maxLife - sParticles[i].life) << 9) / sParticles[i].maxLife + 0x100;
                    if (k > 400) {
                        k = 400;
                    }
                    size = size * k / 256;
                    if (size < 3) {
                        size = 3;
                    }
                    dsdx = (sClodTextures->tex[0].width << 13) / size;
                    hh = size * 2 / 3;
                    dtdy = (sClodTextures->tex[0].height << 10) / hh;
                    a = sParticles[i].surface;
                    gDPSetPrimColor(gdl++, 0, 0, (gSurfaceTable[a].color[4] * sParticles[i].r) >> 8,
                                    (gSurfaceTable[a].color[5] * sParticles[i].g) >> 8, (gSurfaceTable[a].color[6] * sParticles[i].b) >> 8,
                                    (sParticles[i].life << 7) / sParticles[i].maxLife + 0x40);
                    gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                    gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                        (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                }
            }
        }
    }

    last = -1;
    if (gNumViews == 1) {
        gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, 255);
        for (i = 0; i < 96; i++) {
            if (sParticles[i].life >= 0 && sParticles[i].type == PARTICLE_SPARKLE) {
                if (project(&sParticles[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
                    if (gNumViews == 1) {
                        sParticles[i].life = -1;
                    }
                } else if (sx < 0 || gScreenWidth < sx || sy < 0 || gScreenHeight < sy) {
                    if (gNumViews == 1) {
                        sParticles[i].life = -1;
                    }
                } else {
                    alpha = (sParticles[i].maxLife - sParticles[i].life) / 2;
                    if (alpha >= 3) {
                        alpha = 5 - alpha;
                    }
                    if (alpha >= 0) {
                        if (last != alpha) {
                            last = alpha;
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
                            hh = size * 2 / 3;
                            dtdy = (sSparkleTextures->tex[alpha].height << 10) / hh;
                            gDPSetPrimDepth(gdl++, sz - 0x40, 0);
                            gSPTextureRectangle(gdl++, (sx - size / 2) << 2, (sy - hh / 2) << 2, (sx + size / 2) << 2,
                                                (sy + hh / 2) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);
                        }
                    }
                }
            }
        }
    }

    gDPPipeSync(gdl++);
    gDPSetRenderMode(gdl++, 0x442230, 0);
    gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    for (i = 0; i < 96; i++) {
        if (sParticles[i].life >= 0 && sParticles[i].type == PARTICLE_SPARK) {
            if (project(&sParticles[i].pos, view, arg2, &sx, &sy, &sz, &w) != 0) {
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

#define SIN(a) gCosTable[((a) >> 4) & 0xFFF]
#define COS(a) gCosTable[(((a) >> 4) - 0x400) & 0xFFF]

Gfx *rain_draw(Gfx *gdl, s32 mtx, s32 buf) {
    s32 dx, dz, a, s, c;
    s32 fwd, side, turn;
    s32 i, j, k, row;
    s32 h, w, y, r, q, t, u;
    s32 rad, off, x, z;
    Vtx *src, *dst, *end;
    Vec3f pos;
    Vec3i vel;
    volatile s32 vv;

    if (sRainEnabled == 0 || (!(gCamZoneState[0].flags & 1) && (gCamSegFlags[0] & 0x40))) {
        return gdl;
    }
    gdl = render_set_viewport_2(gdl, mtx, 0);

    t = gCamPos[0].x - sRainLastCamPos.x;
    r = gCamPos[0].z - sRainLastCamPos.z;
    fwd = (((t) * SIN(gCamYaw[0])) >> 14) + (((r) * COS(gCamYaw[0])) >> 14);
    fwd = -fwd;
    side = (((r) * SIN(gCamYaw[0])) >> 14) - (((t) * COS(gCamYaw[0])) >> 14);
    side *= 2;
    turn = gCamYaw[0] - sRainLastCamYaw;
    if (fwd > 0x200) fwd = 0x200;
    if (fwd < -0x200) fwd = -0x200;
    if (side > 0x200) side = 0x200;
    if (side < -0x200) side = -0x200;
    if (turn > 0x200) turn = 0x200;
    if (turn < -0x200) turn = -0x200;

    fwd = turn;
    if (gPauseMenu == 0 && (gPodiumMode == 0 || gDesaturation != 0x100)) {
        for (i = 0; i < 64; i++) {
            x = random_byte_2() * 2;
            x -= 0x100;
            x = x * gScreenWidth / 512;
            row = i >> 3;
            r = row * 32;
            h = r * 10 / 256 + 5;
            h = h * gScreenHeight / 240;
            w = ((r >> 1) + 0x80) * gScreenHeight;
            if (w < 0) w += 255;
            w >>= 8;
            r = random_byte_2() * w;
            if (r < 0) r += 255;
            y = r >> 8;
            y = gScreenHeight / 2 - y;
            q = -(x * 32) / gScreenWidth;
            w = 0xFBFF - (row << 8);
            sRainVtx[buf][i * 4].v.ob[0] = x;
            sRainVtx[buf][i * 4].v.ob[1] = y;
            sRainVtx[buf][i * 4].v.ob[2] = w;
            t = (h * (-fwd * 16 / 500)) >> 4;
            t += (q * side) >> 9;
            sRainVtx[buf][i * 4 + 1].v.ob[0] = x + t;
            sRainVtx[buf][i * 4 + 1].v.ob[1] = y + h;
            sRainVtx[buf][i * 4 + 2].v.ob[0] = x + 2;
            sRainVtx[buf][i * 4 + 2].v.ob[1] = y;
            u = t + 2 + (((u32)h & 1) >> 1);
            sRainVtx[buf][i * 4 + 3].v.ob[0] = x + u;
            sRainVtx[buf][i * 4 + 3].v.ob[1] = y + h;
        }
    } else {
        src = sRainVtx[buf ^ 1];
        dst = sRainVtx[buf];
        end = src + 256;
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
        g->words.w0 = 0xDA380007;
        g->words.w1 = mtx + 0x300;
    }
    gSPTexture(gdl++, 0, 0, 0, 0, G_OFF);
    for (i = 0; i < 64; i += 8) {
        gDPSetPrimColor(gdl++, 0, 0, 255, 255, 255, i * 5 * 4 / 64 + 16);
        gDPSetPrimDepth(gdl++, sRainVtx[buf][i * 4].v.ob[2], 0);
        gSPVertex(gdl++, &sRainVtx[buf][i * 4], 32, 0);
        for (j = 0; j < 8; j++) {
            gSP2Triangles(gdl++, j * 4, j * 4 + 1, j * 4 + 3, 0, j * 4, j * 4 + 3, j * 4 + 2, 0);
        }
    }
    gDPPipeSync(gdl++);
    {
        Gfx *g = gdl++;
        g->words.w0 = 0xDA380007;
        g->words.w1 = mtx;
    }
    sRainLastCamPos = gCamPos[0];
    sRainLastCamYaw = gCamYaw[0];
    if (gPauseMenu == 0) {
        w = (random_byte_2() << 7) + 8000;
        x = random_byte_2() * w / 256 - w / 2;
        t = ((x * SIN(gCamYaw[0])) >> 14) - ((w * COS(gCamYaw[0])) >> 14);
        r = ((w * SIN(gCamYaw[0])) >> 14) + ((x * COS(gCamYaw[0])) >> 14);
        x = t + gCamPos[0].x;
        w = r + gCamPos[0].z;
        y = height_get_ground(x, 0x8000, w);
        r = (gGroundSurface >> 4) & 0xF;
        if (gGroundFound != 0 && (gCurTrack_2 - gTracks != 11 || r != 6)) {
            vel.x = 0;
            vel.y = 0;
            vel.z = 0;
            pos.x = (f32)x / 1000.0f;
            pos.y = (f32)(y + 10) / 1000.0f;
            pos.z = (f32)w / 1000.0f;
            particle_spawn(3, 0, &pos, &vel, gParticleWhite);
        }
    }
    gSPSetGeometryMode(gdl++, gCullMode);
    return gdl;
}

void particles_init(Surface *surfaces)
{
    sParticles = mem_alloc_dbg(0xD80, 0, sStrMud2C, 0x60);
    mem_fill((u8 *)sParticles, 0, 0xD80);
    sParticleLast = 0;
    mem_fill(sExhaustPuffTimers, 0, 0x40);
    mem_fill(sSplashSoundTime, 0, 0x20);
    gSurfaceTablePtr = surfaces;
    particles_update();
}

void particles_free(void) {
    mem_free(sParticles);
}

void particles_start_dust_cloud(Vec3f *pos, s32 rider)
{
    Vec3f *dst = &sDustCloudPos;
    *dst = *pos;
    sDustCloudBike = rider;
    sDustCloudStart = gRaceFrame;
}

void rain_init(void)
{
    s32 i;

    sRainEnabled = 0;
    if ((((gCurTrack_6 - (s32)gTracks) * -0x33333333) >> 5) == 0xb &&
        (i = 0, gNumScreens == 1)) {
        for (i = 0; i < 2; i++) {
            sRainVtx[i] = mem_alloc_dbg(0x1000, 0, sStrMud2C, 0x512);
        }
        sRainEnabled = 1;
    }
}

void rain_free(void)
{
    s32 i;
    u32 *p;
    if (sRainEnabled != 0) {
        i = 0;
        p = &sRainVtx;
        do {
            u32 v = p[0];
            p++;
            i++;
            mem_free(v);
        } while (i < 2);
        sRainEnabled = 0;
    }
}

void particles_start_sparks(Rider *p)
{
    f32 buf[8];

    if (gNumScreens == 1) {
        buf[0] = gCollidePoint.x - p->bike.pos.x;
        buf[1] = gCollidePoint.y - p->bike.pos.y;
        buf[2] = gCollidePoint.z - p->bike.pos.z;
        mtx3f_mul_vec3(buf, &p->bike.right.x, &p->sparkDir.x);
        p->sparkTimer = 0xF;
    }
}

void particles_project_point(f32 *v, s32 *a1, s32 *a2, s32 *a3, s32 *a4, s32 a5, s32 idx)
{
    s32 x, y, z;
    Vec3i *t;

    if (gCurTrack_3[11] == 0) {
        x = v[0] * 1000.0f * 0.0625f;
        y = v[1] * 1000.0f * 0.0625f;
        z = v[2] * 1000.0f * 0.0625f;
        project_to_screen(x, y, z, a1, a2, a3, a4, a5, idx);
    } else {
        x = v[0] * 1000.0f * 0.0625f - (f32)(gCamPos[idx].x >> 4);
        y = v[1] * 1000.0f * 0.0625f - (f32)(gCamPos[idx].y >> 4);
        z = v[2] * 1000.0f * 0.0625f - (f32)(gCamPos[idx].z >> 4);
        project_to_screen(x, y, z, a1, a2, a3, a4, a5, idx);
    }
}

void particles_start_exhaust_puff(s32 rider)
{
    u32 r = random_byte();
    sExhaustPuffTimers[rider * 2 + (r & 1)] = 0xF;
}
