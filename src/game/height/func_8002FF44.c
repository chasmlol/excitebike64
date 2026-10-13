#include "ctx.h"

typedef struct {
    u8 pad[4];
    s16 x, z;
} Ent;

typedef struct {
    u8 pad[0x1C];
    u8 *base;
} Hdr;

extern void func_80030ECC(s32 a, s32 b, s32 c, s32 d);
extern s32 D_800E84FC;
extern Ent *D_800E8538[];
extern Hdr *D_800E8D6C;
extern u8 *D_800C9800;

void func_8002FF44(f32 *pos)
{
    s32 i;
    Ent *e;
    f32 f;

    func_80030ECC((pos[0] - 1.0f) * 1000.0f, (pos[2] - 1.0f) * 1000.0f,
                  (pos[0] + 1.0f) * 1000.0f, (pos[2] + 1.0f) * 1000.0f);
    for (i = 0; i < D_800E84FC; i++) {
        e = D_800E8538[i];
        if (!(D_800C9800[((u8 *)e - D_800E8D6C->base) >> 3] & 0x20)) {
            f = __builtin_sqrtf(e->x * e->x + e->z * e->z);
            pos[0] += e->x / f;
            pos[2] += e->z / f;
            return;
        }
    }
}
