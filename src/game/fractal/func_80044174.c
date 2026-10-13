#include "ctx.h"

extern s32 D_800E76D0;
extern s32 D_800CA3B4;

typedef struct { s32 x, y, z, a, b, kind, ref; } Ent;
typedef struct { u8 pad[0x320]; s32 count; Ent e[8]; } Tile;
extern Tile *D_800EE138;

u32 func_8000EFE8(void);
void func_80036374(s32 a, s32 b, f32 *c, f32 *d, u8 *e);

void func_80044174(void)
{
    f32 pos[3];
    f32 vel[3];
    u8 col[4];
    s32 i, j;
    Ent *p;

    if ((D_800E76D0 & 7) != 0) {
        return;
    }
    col[0] = 0;
    col[1] = 0;
    col[2] = 0;
    col[3] = 0;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < D_800EE138[i].count; j++) {
            p = &D_800EE138[i].e[j];
            if (p->kind == 2 && p->ref == D_800CA3B4) {
                pos[0] = (f32)p->x / 1000.0f;
                pos[1] = (f32)p->y / 1000.0f + 1.0f;
                pos[2] = (f32)p->z / 1000.0f;
                vel[0] = (f32)(s32)(func_8000EFE8() - 0x80) / 10000.0f;
                vel[1] = 0.1f;
                vel[2] = (f32)(s32)(func_8000EFE8() - 0x80) / 10000.0f;
                func_80036374(6, 0, pos, vel, col);
            }
        }
    }
}
