#include "ctx.h"

typedef struct { f32 x, y, z; } V3;
typedef struct {
    V3 pos;
    f32 f0C;
    u8 pad10[4];
    f32 f14;
    f32 f18;
    u8 pad1C[0x46 - 0x1C];
    s16 next;
    u8 pad48[0x58 - 0x48];
    f32 f58;
    f32 f5C;
    u8 pad60[0x66 - 0x60];
    u16 h66;
    u8 pad68[0x6C - 0x68];
} Rec;

extern s32 D_800EE490;
extern Rec *D_800EE498;
extern f32 D_800EE48C;

void func_80030ECC(s32 a0, s32 a1, s32 a2, s32 a3);
void func_80030658(Rec *a0, V3 *a1, f32 *a2, f32 *a3, s32 a4);

void func_8004A334(void)
{
    s32 i;
    V3 v;
    f32 st[4];
    f32 l[2];
    f32 acc;
    Rec *r;

    for (i = 0; i < D_800EE490; i++) {
        func_80030ECC((s32)((D_800EE498[i].pos.x - 8.0f) * 1000.0f),
                      (s32)((D_800EE498[i].pos.z - 8.0f) * 1000.0f),
                      (s32)((D_800EE498[i].pos.x + 8.0f) * 1000.0f),
                      (s32)((D_800EE498[i].pos.z + 8.0f) * 1000.0f));
        v = D_800EE498[i].pos;
        v.x -= D_800EE498[i].f14 * 8.0f;
        v.z -= D_800EE498[i].f18 * 8.0f;
        l[0] = 1.0f;
        func_80030658(&D_800EE498[i], &v, st, l, 0);
        D_800EE498[i].f58 = l[0] * 8.0f;
        v = D_800EE498[i].pos;
        v.x += D_800EE498[i].f14 * 8.0f;
        v.z += D_800EE498[i].f18 * 8.0f;
        l[0] = 1.0f;
        func_80030658(&D_800EE498[i], &v, st, l, 0);
        D_800EE498[i].f5C = l[0] * 8.0f;
    }

    acc = 0.0f;
    i = 1;
    if (D_800EE498[i].next != 1) {
        r = D_800EE498;
        do {
            r[i].h66 = ((D_800EE48C - acc) * 32768.0f) / D_800EE48C;
            acc += r[i].f0C;
            i = r[i].next;
        } while (r[i].next != 1);
    }
}
