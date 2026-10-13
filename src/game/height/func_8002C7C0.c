#include "ctx.h"

typedef struct {
    u8 pre[0x48];
    Vec3f pos;
    u8 post[0x7E0 - 0x48 - 12];
} Node;

extern Node D_800E37C0[];
extern s32 D_800E8090[];

extern f32 func_800ADFB0(f32 x);
extern void func_80030ECC(s32 a, s32 b, s32 c, s32 d);
extern u8 func_80030658(Vec3f *p, f32 *pt, f32 *q, f32 *tt, s32 e);
extern void func_8002D1F0(s32 a, s32 b);

void func_8002C7C0(s32 a, s32 b, s32 c)
{
    Node *nd, *nd0;
    s32 *tb;
    Vec3f p;
    f32 pt[3];
    f32 n[3];
    f32 q[4];
    f32 tt[2];
    f32 sum, len, L, t;
    s32 *dst;

    nd0 = D_800E37C0;
    nd = nd0 + a;
    tb = D_800E8090;
    p = nd->pos;
    dst = &tb[b * 3];
    pt[0] = (f32)dst[0] / 1000.0f;
    pt[1] = (f32)dst[1] / 1000.0f;
    pt[2] = (f32)dst[2] / 1000.0f;
    pt[0] = pt[0] - p.x;
    pt[1] = pt[1] - p.y;
    pt[2] = pt[2] - p.z;
    sum = pt[0] * pt[0] + pt[1] * pt[1] + pt[2] * pt[2];
    L = __builtin_sqrtf(sum);
    if (0.0f < L) {
        n[0] = pt[0] / L;
        n[1] = pt[1] / L;
        n[2] = pt[2] / L;
    }
    L = L + 0.2f;
    pt[0] = p.x + n[0] * L;
    pt[1] = p.y + n[1] * L;
    pt[2] = p.z + n[2] * L;
    func_80030ECC((s32)((p.x - L) * 1000.0f), (s32)((p.z - L) * 1000.0f),
                  (s32)((p.x + L) * 1000.0f), (s32)((p.z + L) * 1000.0f));
    tt[0] = 1.0f;
    if (func_80030658(&p, pt, q, tt, c)) {
        t = tt[0] * L - 0.2f;
        pt[0] = p.x + t * n[0];
        pt[1] = p.y + t * n[1];
        pt[2] = p.z + t * n[2];
        dst[0] = (s32)(pt[0] * 1000.0f);
        dst[1] = (s32)(pt[1] * 1000.0f);
        tt[0] = t;
        dst[2] = (s32)(pt[2] * 1000.0f);
    }
    func_8002D1F0(a, b);
}
