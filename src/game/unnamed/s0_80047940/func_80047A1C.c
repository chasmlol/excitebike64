#include "ctx.h"

typedef struct {
    s32 x, y, z;
} Vec3i;

extern Vec3f D_800CA6F0[];
extern Vec3i D_800EE068[];
extern s32 D_800CA410;

void func_80047A1C(void) {
    s32 i;
    D_800CA410 = 10;
    for (i = 0; i < 10; i++) {
        D_800EE068[i].x = D_800CA6F0[i].x * 1000.0f;
        D_800EE068[i].z = D_800CA6F0[i].y * 1000.0f;
        D_800EE068[i].y = D_800CA6F0[i].z * 1000.0f;
    }
}
