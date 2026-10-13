#include "ctx.h"

typedef struct { s32 x, y, z; } V3i;
extern V3i D_800E8090[];
void func_80012DC8(s32 *param_1, s32 *param_2, s32 *param_3);
void func_80012D94(s32 *p);

typedef struct {
    u8 pad0[0x34];
    s32 v34[3];
    s32 m40[16];
} Rec;

s32 func_8003C588(Rec *recs, s32 n, s32 *out, s32 *mat, s32 *pos, s32 idx)
{
    s32 rel[3];
    s32 wp[3];
    s32 d, sum;
    s32 i;

    d = pos[0] - D_800E8090[idx].x;
    if (d < 0) d = -d;
    sum = d;
    d = pos[1] - D_800E8090[idx].y;
    if (d < 0) d = -d;
    sum += d;
    d = pos[2] - D_800E8090[idx].z;
    if (d < 0) d = -d;
    sum += d;
    if (sum > 0x8000) return 1;
    for (i = 0; i < n; i++) {
        func_80012DC8(recs[i].v34, mat, wp);
        wp[0] += pos[0];
        wp[1] += pos[1];
        wp[2] += pos[2];
        rel[0] = (D_800E8090[idx].x >> 6) - wp[0];
        rel[1] = (D_800E8090[idx].y >> 6) - wp[1];
        rel[2] = (D_800E8090[idx].z >> 6) - wp[2];
        func_80012D94(recs[i].m40);
        func_80012DC8(rel, recs[i].m40, &out[i * 3]);
        func_80012D94(recs[i].m40);
    }
    return 0;
}
