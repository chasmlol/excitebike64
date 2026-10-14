#include "game.h"

#define DIAL_VTX_COUNT 25
#define DIAL_TICK_COUNT 7

/* A vertex of the engine dial (the layout of a Vtx). */
typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 flag;
    s32 tc;
    s32 color;
} DialVtx;

typedef struct {
    DialVtx vtx[DIAL_VTX_COUNT];
} DialVtxSet;

typedef struct {
    DialVtxSet cur;
    DialVtxSet prev;
} DialVtxBuffers;

extern DialVtxBuffers sEngineDialVtx;
extern DialVtxSet D_800E9080[]; /* sEngineDialVtx.prev */

/* Builds the engine dial's vertices: seven tick triangles at 45-degree steps over a 270-degree arc of an ellipse,
   copied to the second buffer. */
void engine_init_dial_vtx(void)
{
    s32 i;
    s32 angle;
    s32 baseA;
    s32 baseB;

    mem_fill((u8 *)&sEngineDialVtx, 0, sizeof(sEngineDialVtx));
    for (i = 0; i < DIAL_VTX_COUNT; i++) {
        sEngineDialVtx.cur.vtx[i].z = 0x32;
    }
    for (i = 0; i < DIAL_TICK_COUNT; i++) {
        angle = i * 0x2000 + 0xA000;
        /* the tip (x from the sine, y from the cosine), then the two corners of the base */
        sEngineDialVtx.cur.vtx[i * 3].x = (gCosTable[((angle >> 4) - 0x400) & 0xFFF] * 15) >> 12;
        sEngineDialVtx.cur.vtx[i * 3].y = (gCosTable[(angle >> 4) & 0xFFF] * 5) >> 11;
        baseA = (angle + 0x200) >> 4;
        sEngineDialVtx.cur.vtx[i * 3 + 1].x = (gCosTable[(baseA - 0x400) & 0xFFF] * 0x41) >> 14;
        baseB = (angle - 0x200) >> 4;
        sEngineDialVtx.cur.vtx[i * 3 + 1].y = (gCosTable[baseA & 0xFFF] * 0x2D) >> 14;
        sEngineDialVtx.cur.vtx[i * 3 + 2].x = (gCosTable[(baseB - 0x400) & 0xFFF] * 0x41) >> 14;
        sEngineDialVtx.cur.vtx[i * 3 + 2].y = (gCosTable[baseB & 0xFFF] * 0x2D) >> 14;
    }
    D_800E9080[0] = D_800E9080[-1]; /* prev = cur */
}

/* The rider's drive: 2 turbo (Z), 1 throttle (A), 0 neither. */
static inline s32 drive_input(Rider *rider)
{
    u32 throttle;

    if (rider->input & INPUT_Z) {
        return 2;
    }
    throttle = rider->input & INPUT_A;
    return throttle != 0;
}

/* Heats or cools the engine by sEngineHeatRates[hot][drive] (turbo heating divided by the rider's turbo stat); it
   overheats at 12000 and recovers at 7000. */
void engine_update_heat(Rider *rider)
{
    s32 hot;
    s32 drive;
    f32 rate;

    u32 unused;

    drive = drive_input(rider);
    hot = 0;
    if (rider->engine.heat > 7000.0f) {
        hot = 1;
    }
    if (rider->engine.overheated) {
        hot = drive = 0;
    }
    rate = sEngineHeatRates[hot][drive];
    if ((rider->input & INPUT_Z) && rate > 0.0f) {
        rate = rate / rider->turbo;
    }
    rider->engine.heat = rider->engine.heat + rate;
    if (rider->engine.heat < 0.0f) {
        rider->engine.heat = 0.0f;
    }
    if (rider->engine.heat > 12000.0f) {
        rider->engine.heat = 12000.0f;
    }
    if (rider->engine.overheated == 0) {
        if (rider->engine.heat >= 12000.0f) {
            rider->engine.overheated = 1;
            rider->engine.heat = 12000.0f;
        }
    } else if (rider->engine.heat <= 7000.0f) {
        rider->engine.overheated = 0;
    }
}

/* Runs the engine for ticks 60 Hz steps: heat, rpm (from the rear wheel on the ground, revving freely in the air or
   while the timer is paused), the automatic gearbox, the drive torque, and the wheels' spin angles. */
void engine_update(s32 riderIdx, s32 ticks)
{
    Rider *rider;
    f32 torque, rpm, tmp;
    s32 spinStep;

    rider = &gRiders[riderIdx];
    engine_update_heat(rider);
    {
        f32 z = 0.0f;
        rider->engine.torque = z;
        torque = z;
    }
    if (rider->crashState != CRASH_STATE_RIDING) {
        rider->input &= ~(INPUT_Z | INPUT_A);
    }
    if (rider->rearWheel.airTicks < 1 && (gEvent.flags & EVENT_STOPPED) == 0) {
        if (rider->speed < 4.5f && (rider->input & (INPUT_Z | INPUT_A)) != 0) {
            /* pulling away: the clutch slips, the engine holds 6500 rpm */
            rpm = rider->engine.rpm;
            if (rpm < 6500.0f) {
                rider->engine.rpm = rpm + (f32)(ticks * 1000);
                if (6500.0f < rider->engine.rpm) {
                    rider->engine.rpm = 6500.0f;
                }
            } else {
                rider->engine.rpm = rpm - (f32)(ticks * 1000);
                if (rider->engine.rpm < 6500.0f) {
                    rider->engine.rpm = 6500.0f;
                }
            }
        } else {
            rider->engine.rpm = rider->rearWheel.spin * sGearRatios[rider->engine.gear];
        }
        torque = engine_torque_at_rpm(rider->engine.rpm);
        /* four gears, at most one change every 16 frames, to the gear with more torque */
        if (gRaceFrame - rider->engine.shiftFrame >= 16) {
            if (rider->engine.gear != 0 && rider->engine.rpm < 6500.0f) {
                tmp = engine_torque_at_rpm(rider->speed * sGearRatios[rider->engine.gear - 1]);
                if (torque < tmp) {
                    rider->engine.gear--;
                    rider->engine.shiftFrame = gRaceFrame;
                    torque = tmp;
                }
            }
            if (rider->engine.gear < 3 && 9000.0f < rider->engine.rpm) {
                tmp = engine_torque_at_rpm(rider->speed * sGearRatios[rider->engine.gear + 1]);
                if (torque < tmp) {
                    rider->engine.gear++;
                    rider->engine.shiftFrame = gRaceFrame;
                    torque = tmp;
                }
            }
        }
        if (rider->rearWheel.surface->flags & SURFACE_COOLS_ENGINE) {
            rider->engine.heat = 0.0f;
        }
    } else if (rider->input & INPUT_Z) {
        rider->engine.rpm = rider->engine.rpm + (f32)(ticks * 10000 / 15);
    } else if (rider->input & INPUT_A) {
        rider->engine.rpm = rider->engine.rpm + (f32)(ticks * 10000 / 30);
    } else {
        rider->engine.rpm = rider->engine.rpm - (f32)(ticks * 10000 / 60);
    }
    rider->engine.throttle = (rider->input & (INPUT_Z | INPUT_A)) != 0;
    if (rider->engine.rpm < 1000.0f) {
        rider->engine.rpm = 1000.0f;
    }
    if (14500.0f < rider->engine.rpm) {
        rider->engine.rpm = 14500.0f;
    }
    if ((rider->input & (INPUT_Z | INPUT_A)) != 0 && (gEvent.flags & EVENT_STOPPED) == 0) {
        if (rider->rearWheel.airTicks < 1) {
            if (rider->engine.overheated != 0) {
                tmp = sOverheatPowerScale;
            } else if (rider->input & INPUT_Z) {
                tmp = sTurboPowerScale;
            } else {
                tmp = sThrottlePowerScale;
            }
            torque = torque * tmp;
            rider->engine.torque = torque;
        }
    } else if (1.0f < rider->speed) {
        rider->engine.torque = -3728.4995f; /* engine braking */
    } else {
        rider->engine.torque = 0.0f;
    }
    /* angle steps of a 0.5 m wheel (0x10000 per turn, 60 frames a second); too fast to see turn: a random angle */
    spinStep = rider->fwdSpeed * 65536.0f / 188.49556f;
    if (spinStep > 0x1000) {
        spinStep = random_byte() << 8;
    }
    rider->frontSpinAngle += spinStep;
    if ((rider->input & (INPUT_Z | INPUT_A)) != 0 || 1000.0f < rider->engine.rpm) {
        spinStep = rider->engine.rpm / sGearRatios[rider->engine.gear] * 65536.0f / 188.49556f;
        if (spinStep > 0x1000) {
            spinStep = random_byte() << 8;
        }
    }
    rider->rearSpinAngle += spinStep;
}

/* The engine's torque at rpm: sEngineTorqueCurve holds 29 points from 0 to 14500 rpm, interpolated linearly. */
f32 engine_torque_at_rpm(f32 rpm)
{
    f32 torque;
    f32 base;
    s32 step;

    if (rpm < 0.0f) {
        return 0.0f;
    }
    if (14500.0f < rpm) {
        rpm = 14500.0f;
    }
    step = (s32)((rpm * 28.0f) / 14500.0f);
    base = sEngineTorqueCurve[step];
    torque = base;
    if (step < 28) {
        torque = torque + ((rpm - (f32)((step * 14500) / 28)) * (sEngineTorqueCurve[step + 1] - torque)) / 517.0f;
    }
    return torque;
}

/* Clears the engine's rpm and torque (the fill covers only rpm). */
void engine_reset(RiderEngine *engine)
{
    mem_fill(engine, 0, 4);
    engine->rpm = engine->torque = 0.0f;
}
