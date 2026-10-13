#include "ctx.h"

typedef struct { f32 x, y, z; } Vec3;
typedef struct {
    s16 life;
    s16 life0;
    u8 type;
    u8 b;
    u8 col[3];
    u8 pad;
    Vec3 pos;
    Vec3 vel;
} Part;

extern s32 D_800E9210;
extern Part *D_800E9214;

void func_80036374(s32 type, u8 b, Vec3 *pos, Vec3 *vel, u8 *col)
{
    s32 i;
    Part *p;

    for (i = D_800E9210 + 1; ; i++) {
        if (i >= 0x60) i = 0;
        if (D_800E9214[i].life <= 0) {
            D_800E9214[i].pos = *pos;
            D_800E9214[i].vel = *vel;
            D_800E9214[i].b = b;
            if (type == 1) {
                D_800E9214[i].life = (s16)(vel->y * 122.448975f + 1.0f) * 2;
            } else if (type == 2) {
                D_800E9214[i].life = 15;
            } else if (type == 5) {
                D_800E9214[i].life = 10;
            } else if (type == 3) {
                D_800E9214[i].life = 20;
            } else if (type == 4) {
                D_800E9214[i].life = 2;
            } else if (type == 6) {
                D_800E9214[i].life = 120;
            } else {
                D_800E9214[i].life = 15;
            }
            p = (Part *)(i * sizeof(Part) + (u32)D_800E9214);
            p->type = type;
            p->life0 = p->life;
            D_800E9214[i].col[0] = col[0];
            D_800E9214[i].col[1] = col[1];
            D_800E9214[i].col[2] = col[2];
            D_800E9210 = i;
            return;
        }
        if (i == D_800E9210) return;
    }
}
