#include "game.h"

/* Route predicates at the branches of the racing line (the per-track sAiRouteBranches* tables pair a path node with
   one of these). The rider arrives as a byte pointer: 0x660 is its race.lap. */

/* True once the rider has completed at least lap laps. */
s32 ai_route_lap_ge(s8 *rider, s16 lap)
{
    return *(s16 *)(rider + 0x660) >= lap;
}

/* True while the rider has completed at most lap laps. */
s32 ai_route_lap_le(u8 *rider, s16 lap)
{
    return *(s16 *)(rider + 0x660) <= lap;
}

/* Alternates between the two routes, each call. */
u8 ai_route_toggle(void)
{
    gAiRouteToggle = gAiRouteToggle ^ 1;
    return gAiRouteToggle;
}

/* True after the first lap. */
s32 ai_route_lap_gt0(u8 *rider)
{
    return *(s16 *)(rider + 0x660) > 0;
}

/* False before lap laps, then alternates like ai_route_toggle. */
u8 ai_route_toggle_from_lap(s8 *rider, s16 lap)
{
    if (*(s16 *)(rider + 0x660) < lap) {
        return 0;
    }
    gAiRouteToggle = gAiRouteToggle ^ 1;
    return gAiRouteToggle;
}

/* Picks a route at random (the low bit of the race frame). */
s32 ai_route_random(void)
{
    return ((u8 *)&gRaceFrame)[3] & 1;
}

#define ABS(x) ((x) > 0.0f ? (x) : -(x))

/* The fields of Rider the next predicate reads. */
struct Obj_43 {
    u8 pad[0x42C];
    f32 laneOffset;             /* Rider.laneOffset */
    u8 pad2[0x660 - 0x430];
    s16 lap;                    /* Rider.race.lap */
};

/* True before lap laps, or when the rider's lane is more than half a metre off the racing line. */
s32 ai_route_lap_lt_or_wide(Obj_43 *rider, s16 lap)
{
    return rider->lap < lap || ABS(rider->laneOffset) > 0.5f;
}

/* Never takes the branch. */
s32 ai_route_never(void)
{
    return 0;
}
