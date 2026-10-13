#include "ctx.h"

typedef struct {
    u16 count;
    u16 best;
    s16 v[3][3];
    s16 x3, z3;
} Rec;

typedef struct {
    u32 count;
    s32 pad4;
    s32 x0, z0, x1, z1;
    s32 pad18, pad1C;
} Ent;

extern s32 D_800D1590;
extern Rec *D_800F14F0;
extern Ent *D_800C9644;
extern void *D_800F14F8;
extern s32 D_800F14F4;
extern u8 D_800D8E20[];

u32 func_8005AED8(s32 key);
void *func_800BC908(u32 a, s32 b, void *c, s32 d);

void func_8006AC40(void)
{
    Rec *list;
    Rec *r;
    Rec *recs;
    s32 i;
    u32 j;
    s32 cx, cz, dx, dz, d;
    s32 best;
    s32 bi;

    D_800F14F0 = list = (Rec *)func_8005AED8(D_800D1590);
    if (list == NULL) {
        return;
    }
    i = 0;
    if (i >= list->count) goto end;
    recs = list;
    for (; i < recs->count; i++) {
        r = &recs[i];
        cx = (r->v[0][0] + r->v[1][0] + r->v[2][0] + r->x3) / 4;
        cz = (r->v[0][2] + r->v[1][2] + r->v[2][2] + r->z3) / 4;
        for (j = 0; j < D_800C9644->count; j++) {
            dx = (D_800C9644[j].x0 + D_800C9644[j].x1) / 2 - cx;
            dz = (D_800C9644[j].z0 + D_800C9644[j].z1) / 2 - cz;
            dx /= 4;
            dz /= 4;
            dx = dx * dx + dz * dz;
            if (dx < best || j == 0) {
                best = dx;
                bi = j;
            }
        }
        r->best = bi;
    }
end:
    D_800F14F8 = func_800BC908(D_800F14F0->count << 2, 0, D_800D8E20, 0x6F);
    D_800F14F4 = 0;
}
