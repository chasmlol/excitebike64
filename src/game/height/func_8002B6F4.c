#include "ctx.h"

typedef struct {
    s32 x, y, z, w, u, v;
} S;

extern s32 D_800C9684;
extern S D_800E7998[][16];
extern S D_800E8030[];
extern s32 D_800E7988[];

void func_8002B6F4(S *p, f32 *v)
{
    s32 i;
    s32 sx, sy, sz;

    sx = sy = sz = 0;
    for (i = 15; i >= 0; i--) {
        D_800E7998[D_800C9684][i] = *p;
        sx += p->x;
        sy += p->y;
        sz += p->z;
        p->x -= (s32)(v[0] * 16.666666f);
        p->y -= (s32)(v[1] * 16.666666f);
        p->z -= (s32)(v[2] * 16.666666f);
    }
    D_800E8030[D_800C9684] = *p;
    D_800E8030[D_800C9684].x = sx;
    D_800E8030[D_800C9684].y = sy;
    D_800E8030[D_800C9684].z = sz;
    D_800E7988[D_800C9684] = 0;
}
