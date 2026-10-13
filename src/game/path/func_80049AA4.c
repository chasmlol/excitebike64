#include "ctx.h"

typedef struct Node {
    u8 pad0[0xc];
    f32 f0c;
    f32 f10;
    u8 pad14[0x8];
    f32 f1c;
    f32 f20;
    f32 f24;
    u8 pad28[0x20];
    s16 s48;
    s16 s4a;
    s16 s4c;
    u8 pad4e[0x6c - 0x4e];
} Node;

extern Node *D_800EE498;
f32 sqrtf(f32);
#define sqrtf __builtin_sqrtf

void func_80049AA4(s32 end, s32 i)
{
    f32 v;
    f32 t;
    f32 q;
    f32 sm;
    f32 r;
    f32 a;
    s32 nx;
    s32 c;
    s32 p;

    for (; end != i; i = D_800EE498[i].s4a) {
        if (i == 0) {
            return;
        }
        v = D_800EE498[i].f10;
        t = v * v * D_800EE498[i].f1c - D_800EE498[i].f20 * 14.700001f;
        t = t * t;
        nx = D_800EE498[i].s4a;
        a = D_800EE498[i].f0c;
        if (t > 365.39212f) {
            q = 0.0f;
        } else {
            q = sqrtf(365.39212f - t);
        }
        if (q > 10.29f) {
            q = 10.29f;
        }
        q = q + D_800EE498[i].f24 * 14.700001f;
        sm = (q + q) * a + v * v;
        if (sm < 1.0f) {
            sm = 1.0f;
        }
        sm = sqrtf(sm);
        if (sm < D_800EE498[nx].f10) {
            D_800EE498[nx].f10 = sm;
        }
        p = D_800EE498[i].s4c;
        if (p >= 0) {
            while (c = D_800EE498[p].s4a, D_800EE498[c].s48 == -1) {
                p = c;
            }
            func_80049AA4(p, D_800EE498[i].s4c);
        }
    }
}
