#include "ctx.h"

typedef struct {
    Vec3f pos;
    u8 unkC[0x46 - 0xC];
    s16 next;
    s16 branch;
    s16 prev;
    s16 unk4C;
    s16 alt;
    u8 unk50[0x6C - 0x50];
} Seg;

extern Seg *D_800EE498;

s32 func_8004A788(Vec3f *pos, s32 start, s32 count, u8 mode) {
    f32 best = 100000000.0f;
    s32 bestIdx;
    s32 seg;
    s32 i;
    f32 dx;
    f32 dz;
    f32 d;

    seg = start;
    for (i = 0; i < count; i++) {
        dx = pos->x - D_800EE498[seg].pos.x;
        dz = pos->z - D_800EE498[seg].pos.z;
        d = dx * dx + dz * dz;
        if (d < best) {
            best = d;
            bestIdx = seg;
        }
        if (D_800EE498[seg].branch >= 0 && mode == 2) {
            seg = D_800EE498[seg].branch;
        } else {
            seg = D_800EE498[seg].next;
        }
    }
    seg = start;
    for (i = 0; i < count; i++) {
        dx = pos->x - D_800EE498[seg].pos.x;
        dz = pos->z - D_800EE498[seg].pos.z;
        d = dx * dx + dz * dz;
        if (d < best) {
            best = d;
            bestIdx = seg;
        }
        seg = D_800EE498[seg].prev;
    }
    if (mode == 0) {
        seg = D_800EE498[bestIdx].alt;
        if (seg >= 0) {
            dx = pos->x - D_800EE498[seg].pos.x;
            dz = pos->z - D_800EE498[seg].pos.z;
            d = dx * dx + dz * dz;
            if (d < best) {
                bestIdx = seg;
            }
        }
    }
    return bestIdx;
}
