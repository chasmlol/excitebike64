#include "game.h"

void ai_update_throttle(Rider *p)
{
    s32 iVar1;
    f32 fVar2;
    f32 fVar3;
    u32 ev;
    u8 b;

    iVar1 = p->segment + 1;
    fVar2 = *(f32 *)(gPathNodes + p->segment * 0x6C + 0x10) * (1.0f - p->segmentT)
          + *(f32 *)(gPathNodes + (iVar1 % gPathNodeCount) * 0x6C + 0x10) * p->segmentT;
    ev = gGameMode;
    if (ev == 10) goto L_e4;
    if (ev < 11) {
        if (ev == 7) goto L_e4;
    } else if (ev == 0xF) {
        fVar2 = 50.0f;
    }
    goto L_gen;

L_e4:
    if (!(0.5f < p->fwdSpeed)) goto ZERO;
STORE20:
    p->input = 0x20;
    return;
ZERO:
    p->input = 0;
    return;

L_gen:
    b = p->finishState;
    if (b == 1) goto C1;
    if (b == 2) goto C2;
    goto CS;
C1:
    fVar2 = 0.0f;
    goto CS;
C2:
    fVar2 = 5.0f;
CS:
    fVar3 = p->speed - fVar2;
    if (0.5f < fVar3) {
        p->frontWheel.brakeRamp = 6;
        p->rearWheel.brakeRamp = 6;
        goto STORE20;
    }
    if (fVar3 < -0.5f && p->airFrames < 3 && p->laneBlocked == 0) {
        if (fVar3 < -5.0f && p->engine.heat < 9500.0f) {
            if ((p->prevInput & 0x20000) == 0) {
                p->turboPressFrame = gRaceFrame;
            }
            p->input = 0x20000;
            return;
        }
        p->input = 0x10;
    }
    return;

}

void ai_set_steering(Rider *b, f32 maxv) {
    f32 speed = b->speed;
    s32 i = 0;
    f32 t;
    f32 r;

    while (sSteerSpeedSteps[i + 1] < speed) {
        i++;
    }
    if (speed < 0.0f) {
        t = 0.0f;
    } else {
        t = (sSteerSpeedSteps[i + 1] - speed) / (sSteerSpeedSteps[i + 1] - sSteerSpeedSteps[i]);
    }
    r = (sSteerMaxBySpeed[i] * t + sSteerMaxBySpeed[i + 1] * (1.0f - t)) * 0.18849557638168335f * 0.5f;
    if (r < maxv) {
        maxv = r;
    } else if (r < -maxv) {
        maxv = -r;
    }
    b->steer = maxv;
    b->stickX = (s32)((maxv * 13.0f) / r);
}

typedef struct {
    Vec3f pos;
    u8 pad0C[8];
    f32 dx;
    f32 dz;
    u8 pad1C[0x45 - 0x1C];
    s8 unk45;
    s16 next;
    s16 alt;
    u8 pad4A[0x58 - 0x4A];
    f32 unk58;
    f32 unk5C;
    u8 pad60[4];
    u8 flags;
    u8 pad65[0x6C - 0x65];
} Node_15;

extern Node_15 *gPathNodes_26;

void ai_update_steering(Rider *b) {
    f32 unused[8];
    s16 ang0;
    s32 ratio;
    s16 ang1;
    s32 idx;
    s32 cnt;
    s32 k;
    s32 j;
    s32 nx;
    f32 dist;
    f32 amp;
    f32 amp2;
    f32 amp3;
    f32 px, pz;
    f32 p0x, p0z;
    f32 ex;
    f32 f;
    f32 dx, dz;
    f32 len, l3;
    f32 ax, az, cx, cz, ez;
    f32 d, d3;
    f32 tx, tz, tl;
    f32 qx, qz, wz, wx;
    f32 vx, vz, sp;
    f32 r, steer;
    f32 ddx, ddz;
    f32 ang, s, c, q, sn;
    Vec3f *pos;
    Node_15 *n;

    switch (gGameMode_2) {
    case 6:
        excite3d_cpu_steer(b);
        break;
    case 5:
        desert_ai_steer(b);
        return;
    case 10:
        ai_set_steering(b, 0.0f);
        b->stickX = 0;
        break;
    }

    switch (b->powerSlide) {
    case 0: {
        s32 idx = b->segment;
        s32 a;
        if (!(gPathNodes_26[idx].flags & 4) || gRaceFrame - b->aiFrame < 300) {
            break;
        }
        math_to_polar(-(b->bike.fwd.x * 16384.0f), b->bike.fwd.z * 16384.0f, &ang0, &ratio);
        math_to_polar(gPathNodes_26[idx].dz * 16384.0f, gPathNodes_26[idx].dx * 16384.0f, &ang1, &ratio);
        ang1 -= ang0;
        a = ang1 < 0 ? -ang1 : ang1;
        if (a > 0x1000) {
            break;
        }
        b->powerSlide = 1;
    }
    case 1: {
        s32 idx, j, a;
        s16 d;
        if (b->speed < 5.0f) {
            b->powerSlide = 2;
            break;
        }
        idx = b->segment;
        if (!(gPathNodes_26[idx].flags & 4)) {
            b->powerSlide = 2;
            break;
        }
        j = idx;
        do {
            j = gPathNodes_26[j].next;
        } while (gPathNodes_26[j].flags & 4);
        math_to_polar(-(b->bike.fwd.x * 16384.0f), b->bike.fwd.z * 16384.0f, &ang0, &ratio);
        math_to_polar(gPathNodes_26[j].dz * 16384.0f, gPathNodes_26[j].dx * 16384.0f, &ang1, &ratio);
        ang1 -= ang0;
        d = ang1;
        a = d < 0 ? -d : d;
        if (a > 0x400) {
            steer = gPathNodes_26[j].pos.x - b->bike.pos.x;
            steer = (steer * gCosTable[(ang0 >> 4) & 0xFFF]
                 + (gPathNodes_26[j].pos.z - b->bike.pos.z) * gCosTable[((ang0 >> 4) - 0x400) & 0xFFF]) * (1.0f / 16384.0f);
            if (d < 0) {
                b->stickX = -13;
                steer = -steer;
            } else {
                b->stickX = 13;
            }
            if (steer > -2.0f) {
                b->stickX = 0;
            }
            b->powerSlide = 1;
            b->input = (b->input & ~0x10) | 0x20;
        } else {
            b->powerSlide = 2;
        }
        steer = b->stickX > 0 ? 0.3f : -0.3f;
        ai_set_steering(b, steer);
        return;
    }
    case 2:
        b->powerSlide = 0;
        b->aiFrame = gRaceFrame;
        b->laneOffset = b->lateral;
        break;
    }

    dist = b->laneOffset;
    if (b->airFrames < 3) {
        pos = &b->bike.pos;
        idx = b->segment;
        amp = b->segmentT;
        cnt = 2;
        if (gRaceFrame - b->aiFrame < 300) {
            cnt = 4;
        } else {
            k = gPathNodes_26[idx].unk45;
            if (cnt < k) {
                amp = 0.0f;
                cnt = k;
            }
        }
    } else if (gGameMode_2 != 6) {
        pos = b->landingPick ? &b->landingPos1 : &b->landingPos2;
        idx = b->landingPick ? b->landingSegment1 : b->landingSegment2;
        r = path_get_lateral_offset(pos, idx);
        if (!(gPathNodes_26[idx].unk5C < r)) {
            if (!(r < -gPathNodes_26[idx].unk58)) {
                amp = gPathSegmentT;
                cnt = 2;
                if (cnt < gPathNodes_26[idx].unk45) {
                    amp = 0.0f;
                    cnt = gPathNodes_26[idx].unk45;
                }
            } else {
                b->stickX = -8;
                return;
            }
        } else {
            b->stickX = 8;
            return;
        }
    } else {
        b->stickX = 0;
        return;
    }

    if ((u32)((u8)b->crestAhead - 1) < 2 && b->speed > 2.0f) {
        idx = b->segment;
        for (k = b->crestAhead; k > 0; k--) {
            idx = gPathNodes_26[idx].next;
        }
        f = (b->bike.vel.x * gPathNodes_26[idx].dx + b->bike.vel.z * gPathNodes_26[idx].dz) / b->speed;
        b->stickX = f * 32.0f;
        if (b->stickX >= 16) {
            b->stickX = 15;
        } else if (b->stickX < -15) {
            b->stickX = -15;
        }
        ai_set_steering(b, f * 8.0f / (b->speed + 5.0f));
        if (b == &gRiders[gCamTargetOverride]) {
            D_800EE150 = gPathNodes_26[idx].pos;
            D_800EE160.z = 2.0f * gPathNodes_26[idx].dx;
            D_800EE160.y = 0;
            D_800EE160.x = 2.0f * -gPathNodes_26[idx].dz;
        }
        return;
    }

    switch ((s32)b->pathSearch) {
    case 2:
        j = idx;
        while (cnt != 0) {
            cnt--;
            j = gPathNodes_26[j].alt >= 0 ? gPathNodes_26[j].alt : gPathNodes_26[j].next;
        }
        break;
    case 0:
    case 1:
    default:
        j = idx;
        while (cnt != 0) {
            cnt--;
            j = gPathNodes_26[j].next;
        }
        break;
    }

    b->laneOffset = dist;
    nx = gPathNodes_26[j].next;
    p0x = gPathNodes_26[j].pos.x + gPathNodes_26[j].dx * dist;
    p0z = gPathNodes_26[j].pos.z + gPathNodes_26[j].dz * dist;
    dx = (gPathNodes_26[nx].pos.x + gPathNodes_26[nx].dx * dist) - p0x;
    dz = (gPathNodes_26[nx].pos.z + gPathNodes_26[nx].dz * dist) - p0z;
    amp2 = amp * amp;
    amp3 = amp2 * amp;
    len = __builtin_sqrtf(dx * dx + dz * dz);

    {
        f32 t1, s1, t2, s2, a1, b1, acx, acz, ds1;
        t1 = gPathNodes_26[j].dz * -3.0f;
        s1 = gPathNodes_26[nx].dz + gPathNodes_26[j].dz;
        d = len / 3.0f;
        ds1 = d * s1;
        cx = t1 * d;
        t2 = gPathNodes_26[j].dx * 3.0f;
        s2 = d * (gPathNodes_26[nx].dx + gPathNodes_26[j].dx);
        cz = t2 * d;
        a1 = (dx + ds1) * 3.0f;
        b1 = (dz - s2) * 3.0f;
        ax = a1 - cx;
        ex = (dx - ax) - cx;
        az = b1 - cz;
        ez = (dz - az) - cz;
        px = p0x + amp3 * ex + amp2 * ax + amp * cx;
        pz = p0z + amp3 * ez + amp2 * az + amp * cz;
    }
    d = __builtin_sqrtf((px - pos->x) * (px - pos->x) + (pz - pos->z) * (pz - pos->z));

    qx = amp2 * 3.0f * ex + 2.0f * amp * ax + cx;
    qz = amp2 * 3.0f * ez + 2.0f * amp * az + cz;
    sp = __builtin_sqrtf(qx * qx + qz * qz);
    qx /= sp;
    qz /= sp;

    vx = b->bike.vel.x;
    vz = b->bike.vel.z;
    sp = __builtin_sqrtf(vx * vx + vz * vz);
    if (sp < 2.0f) {
        vx = b->bike.fwd.x;
        vz = b->bike.fwd.z;
        sp = __builtin_sqrtf(vx * vx + vz * vz);
    }
    vx /= sp;
    d3 = d / 3.0f;
    ddx = pos->x + vx * d3;
    vz /= sp;
    ddz = pos->z + vz * d3;
    {
        f32 e1, e2;
        e1 = px - qx * d3;
        e2 = pz - qz * d3;
        wx = d * vx;
        tx = (e1 - ddx) * 3.0f;
        wz = d * vz;
        tz = (e2 - ddz) * 3.0f;
    }
    d = wx * wx + wz * wz;
    r = ((tz - wz) * wx - (tx - wx) * wz) / (d * __builtin_sqrtf(d));

    if (b->airFrames < 4) {
        dx = b->frontWheel.contact.x - b->rearWheel.contact.x;
        dz = b->frontWheel.contact.z - b->rearWheel.contact.z;
        dx = r * __builtin_sqrtf(dx * dx + dz * dz);
        dx += math_atan2f(wx, wz);
        sn = cosf(dx);
        c = sinf(dx);
        q = b->bike.right.z * sn - b->bike.right.x * c;
        tl = b->bike.fwd.x * c - b->bike.fwd.z * sn;
        steer = 0.0f;
        if (!(r * tl < 0.0f)) {
            steer = math_atan2f(-q, tl);
        }
    } else {
        steer = r * 1.25f;
    }
    if (b->airFrames < 4) {
        ai_set_steering(b, steer);
    } else {
        k = steer * 192.0f;
        if (k >= 16) {
            k = 15;
        } else if (k < -15) {
            k = -15;
        }
        b->stickX = k / 2;
    }
}


void bike_update_steering(Rider *p, s32 m)
{
    s32 dir, i;
    u8 b;
    f32 x, lim, r, w, T, S, g;

    dir = p->stickX;
    if (p->camMode == 4) {
        excite3d_player_steer(p);
    } else {
        b = (s32)(p->bike.pos.x / 2.178f);
        p->gridSlot = b;
        if (p->crashState != 0) {
            p->laneChange = 0;
        } else {
            p->laneChange = (s32)((p->bike.pos.x - b * 2.178f) * (gLaneChangeFrames / 2.178f));
            if (p->gridSlot == 0 && p->laneChange < 0) {
                p->laneChange = 0;
            }
        }
    }
    if (p->airFrames >= 8) {
        dir = 0;
    }
    x = p->speed;
    i = 0;
    while (sSteerSpeedSteps[i + 1] < x) {
        i++;
    }
    if (x < 0.0f) {
        r = 0.0f;
    } else {
        r = (sSteerSpeedSteps[i + 1] - x) / (sSteerSpeedSteps[i + 1] - sSteerSpeedSteps[i]);
    }
    S = 0.012566371f;
    T = dir * S;
    w = (sSteerMaxBySpeed[i] * r + sSteerMaxBySpeed[i + 1] * (1.0f - r)) * p->turnRadius;
    T *= w;
    lim = 0.18849556f;
    lim *= w;
    S = (sSteerRateBySpeed[i] * r + sSteerRateBySpeed[i + 1] * (1.0f - r)) * 0.327f * m * S;
    if (p->reversing != 0) {
        T *= 0.25f;
    }
    if (T == 0.0f || T * p->steer < 0.0f) {
        S *= 5.0f;
    }
    if (p->hardTurn != 0) {
        T = sHardTurnSteerAngle;
        if (p->stickX <= 0) {
            T = -T;
        }
    } else if (lim < T) {
        S = 0.1f;
        T = lim;
    } else {
        w = -lim;
        if (T < w) {
            S = 0.1f;
            T = w;
        }
    }
    if (gGameMode == 7) {
        T += (p->bike.fwd.x * p->frontWheel.normal.z - p->bike.fwd.z * p->frontWheel.normal.x) * 16.0f * S;
    }
    g = p->steer;
    if (T < g) {
        p->steer = g - S;
        if (p->steer < T) {
            p->steer = T;
        }
    } else {
        p->steer = g + S;
        if (T < p->steer) {
            p->steer = T;
        }
    }
}

void func_8005C8F4(void)
{
}

void func_8005C8FC(void)
{
}
