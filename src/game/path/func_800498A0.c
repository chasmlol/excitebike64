#include "ctx.h"

typedef struct {
    u8 pad0[0x10];
    f32 f10;
    u8 pad1[0x8];
    f32 f1c;
    f32 f20;
    u8 pad2[0x40];
    u8 b64;
    s8 b65;
    u8 rest[6];
} Ent;

extern Ent *D_800EE498;

void func_800498A0(s32 idx)
{
    f32 f;
    f32 w;
    f32 v;

    f = D_800EE498[idx].f1c;
    if ((f > 0.0f) ? (f > 0.005f) : (-f > 0.005f)) {
        w = D_800EE498[idx].f1c;
        if (w > 0.0f) {
            v = D_800EE498[idx].f20 * 14.700001f + 19.11f;
            w = v / w;
        } else {
            v = 19.11f - D_800EE498[idx].f20 * 14.700001f;
            v = -v;
            w = v / w;
        }
        if (w < 1.0f) {
            w = 1.0f;
        }
        D_800EE498[idx].f10 = __builtin_sqrtf(w);
    } else {
        D_800EE498[idx].f10 = 50.0f;
    }
    if (D_800EE498[idx].b64 & 4) {
        D_800EE498[idx].f10 += D_800EE498[idx].f10;
    }
    D_800EE498[idx].f10 *= D_800EE498[idx].b65 / 10.0f + 1.0f;
}
