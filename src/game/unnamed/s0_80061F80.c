#include "game.h"

s32 ai_route_lap_ge(s8 *p, s16 v)
{
    return *(s16 *)(p + 0x660) >= v;
}

s32 ai_route_lap_le(u8 *a0, s16 a1)
{
    return *(s16 *)(a0 + 0x660) <= a1;
}

u8 ai_route_toggle(void)
{
    gAiRouteToggle = gAiRouteToggle ^ 1;
    return gAiRouteToggle;
}

s32 ai_route_lap_gt0(u8 *a0)
{
    return *(s16 *)(a0 + 0x660) > 0;
}

u8 ai_route_toggle_from_lap(s8 *p, s16 v)
{
    if (*(s16 *)(p + 0x660) < v) {
        return 0;
    }
    gAiRouteToggle = gAiRouteToggle ^ 1;
    return gAiRouteToggle;
}

s32 ai_route_random(void) {
    return D_800E76D3 & 1;
}

#define ABS(x) ((x) > 0.0f ? (x) : -(x))

struct Obj_43 {
    u8 pad[0x42C];
    f32 f42C;
    u8 pad2[0x660 - 0x430];
    s16 x660;
};

s32 ai_route_lap_lt_or_wide(Obj_43 *p, s16 s) {
    return p->x660 < s || ABS(p->f42C) > 0.5f;
}

s32 ai_route_never(void)
{
    return 0;
}
