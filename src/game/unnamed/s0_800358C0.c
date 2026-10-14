#include "game.h"

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
    s32 unk8;
    s32 unkC;
} Rec_18;

typedef struct {
    Rec_18 r[25];
} RecSet;

typedef struct {
    RecSet cur;
    RecSet prev;
} Both;

extern Both D_800E8EF0;
extern RecSet D_800E9080[];

void func_800358C0(void)
{
    s32 i;
    s32 a;
    s32 u1;
    s32 u3;

    mem_fill((u8 *)&D_800E8EF0, 0, 800);
    for (i = 0; i < 25; i++) {
        D_800E8EF0.cur.r[i].z = 0x32;
    }
    for (i = 0; i < 7; i++) {
        a = i * 0x2000 + 0xA000;
        D_800E8EF0.cur.r[i * 3].x = (gCosTable[((a >> 4) - 0x400) & 0xFFF] * 15) >> 12;
        D_800E8EF0.cur.r[i * 3].y = (gCosTable[(a >> 4) & 0xFFF] * 5) >> 11;
        u1 = (a + 0x200) >> 4;
        D_800E8EF0.cur.r[i * 3 + 1].x = (gCosTable[(u1 - 0x400) & 0xFFF] * 0x41) >> 14;
        u3 = (a - 0x200) >> 4;
        D_800E8EF0.cur.r[i * 3 + 1].y = (gCosTable[u1 & 0xFFF] * 0x2D) >> 14;
        D_800E8EF0.cur.r[i * 3 + 2].x = (gCosTable[(u3 - 0x400) & 0xFFF] * 0x41) >> 14;
        D_800E8EF0.cur.r[i * 3 + 2].y = (gCosTable[u3 & 0xFFF] * 0x2D) >> 14;
    }
    D_800E9080[0] = D_800E9080[-1];
}

static inline s32 sel(Rider *o)
{
    u32 t;

    if (o->input & 0x20000) {
        return 2;
    }
    t = o->input & 0x10;
    return t != 0;
}

void engine_update_heat(Rider *o)
{
    s32 h;
    s32 g;
    f32 f;

    u32 t;

    g = sel(o);
    h = 0;
    if (o->engine.heat > 7000.0f) {
        h = 1;
    }
    if (o->engine.overheated) {
        h = g = 0;
    }
    f = sEngineHeatRates[h][g];
    if ((o->input & 0x20000) && f > 0.0f) {
        f = f / o->turbo;
    }
    o->engine.heat = o->engine.heat + f;
    if (o->engine.heat < 0.0f) {
        o->engine.heat = 0.0f;
    }
    if (o->engine.heat > 12000.0f) {
        o->engine.heat = 12000.0f;
    }
    if (o->engine.overheated == 0) {
        if (o->engine.heat >= 12000.0f) {
            o->engine.overheated = 1;
            o->engine.heat = 12000.0f;
        }
    } else if (o->engine.heat <= 7000.0f) {
        o->engine.overheated = 0;
    }
}

void engine_update(s32 idx, s32 dt)
{
    Rider *o;
    f32 fs0, v, fv;
    s32 a1;

    o = &gRiders[idx];
    engine_update_heat(o);
    {
        f32 z = 0.0f;
        o->engine.torque = z;
        fs0 = z;
    }
    if (o->crashState != 0) {
        o->input &= ~0x20010;
    }
    if (o->rearWheel.airTicks < 1 && (gEventFlags & 0x10) == 0) {
        if (o->speed < 4.5f && (o->input & 0x20010) != 0) {
            v = o->engine.rpm;
            if (v < 6500.0f) {
                o->engine.rpm = v + (f32)(dt * 1000);
                if (6500.0f < o->engine.rpm) {
                    o->engine.rpm = 6500.0f;
                }
            } else {
                o->engine.rpm = v - (f32)(dt * 1000);
                if (o->engine.rpm < 6500.0f) {
                    o->engine.rpm = 6500.0f;
                }
            }
        } else {
            o->engine.rpm = o->rearWheel.spin * sGearRatios[o->engine.gear];
        }
        fs0 = engine_torque_at_rpm(o->engine.rpm);
        if (gRaceFrame - o->engine.shiftFrame >= 16) {
            if (o->engine.gear != 0 && o->engine.rpm < 6500.0f) {
                fv = engine_torque_at_rpm(o->speed * sGearRatios[o->engine.gear - 1]);
                if (fs0 < fv) {
                    o->engine.gear--;
                    o->engine.shiftFrame = gRaceFrame;
                    fs0 = fv;
                }
            }
            if (o->engine.gear < 3 && 9000.0f < o->engine.rpm) {
                fv = engine_torque_at_rpm(o->speed * sGearRatios[o->engine.gear + 1]);
                if (fs0 < fv) {
                    o->engine.gear++;
                    o->engine.shiftFrame = gRaceFrame;
                    fs0 = fv;
                }
            }
        }
        if (o->rearWheel.surface->flags & 0x200) {
            o->engine.heat = 0.0f;
        }
    } else if (o->input & 0x20000) {
        o->engine.rpm = o->engine.rpm + (f32)(dt * 10000 / 15);
    } else if (o->input & 0x10) {
        o->engine.rpm = o->engine.rpm + (f32)(dt * 10000 / 30);
    } else {
        o->engine.rpm = o->engine.rpm - (f32)(dt * 10000 / 60);
    }
    o->engine.throttle = (o->input & 0x20010) != 0;
    if (o->engine.rpm < 1000.0f) {
        o->engine.rpm = 1000.0f;
    }
    if (14500.0f < o->engine.rpm) {
        o->engine.rpm = 14500.0f;
    }
    if ((o->input & 0x20010) != 0 && (gEventFlags & 0x10) == 0) {
        if (o->rearWheel.airTicks < 1) {
            if (o->engine.overheated != 0) {
                fv = sOverheatPowerScale;
            } else if (o->input & 0x20000) {
                fv = sTurboPowerScale;
            } else {
                fv = sThrottlePowerScale;
            }
            fs0 = fs0 * fv;
            o->engine.torque = fs0;
        }
    } else if (1.0f < o->speed) {
        o->engine.torque = -3728.4995f;
    } else {
        o->engine.torque = 0.0f;
    }
    a1 = o->fwdSpeed * 65536.0f / 188.49556f;
    if (a1 > 0x1000) {
        a1 = random_byte() << 8;
    }
    o->frontSpinAngle += a1;
    if ((o->input & 0x20010) != 0 || 1000.0f < o->engine.rpm) {
        a1 = o->engine.rpm / sGearRatios[o->engine.gear] * 65536.0f / 188.49556f;
        if (a1 > 0x1000) {
            a1 = random_byte() << 8;
        }
    }
    o->rearSpinAngle += a1;
}

f32 engine_torque_at_rpm(f32 x)
{
    f32 r;
    f32 lo;
    s32 i;

    if (x < 0.0f) {
        return 0.0f;
    }
    if (14500.0f < x) {
        x = 14500.0f;
    }
    i = (s32)((x * 28.0f) / 14500.0f);
    lo = sEngineTorqueCurve[i];
    r = lo;
    if (i < 0x1C) {
        r = r + ((x - (f32)((i * 0x38A4) / 0x1C)) * (sEngineTorqueCurve[i + 1] - r)) / 517.0f;
    }
    return r;
}

void engine_reset(RiderEngine *p)
{
    mem_fill(p, 0, 4);
    p->rpm = p->torque = 0.0f;
}
