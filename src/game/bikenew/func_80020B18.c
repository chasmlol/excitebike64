#include "ctx.h"

f32 func_80013408(f32 *a, f32 *b);
s32 func_80013010(s32 x);
void func_8002F4B0(f32 *param_1);
s32 func_8002E920(s32 x, s32 y, s32 z);

typedef struct {
    u8 pad0[0x48];
    Vec3f pos;      /* 0x48 */
    Vec3f vel;      /* 0x54 */
    u8 pad60[0x88 - 0x60];
    Vec3f rot;      /* 0x88 */
    u8 pad94[0xA0 - 0x94];
    Vec3f fA0;      /* 0xA0 */
    Vec3f fAC;      /* 0xAC */
    u8 padB8[0x6F4 - 0xB8];
    f32 dt;         /* 0x6F4 */
} Bike;

void func_80020B18(Bike *b)
{
    Vec3f n;
    Vec3f c;
    Vec3f u;
    Vec3f v;
    f32 len;
    f32 q;
    s32 t, w;
    f32 th, qd;

    v = b->pos;
    th = v.x + b->vel.x * b->dt;
    v.x = th;
    qd = v.z + b->vel.z * b->dt;
    v.z = qd;
    if (b == 0) q = 1.0f; else if (t == w) q = 2.0f; else q = 3.0f;
    func_8002E920((s32)(v.x * 1000.0f), (s32)(v.y * 1000.0f), (s32)(v.z * 1000.0f));
    func_8002F4B0(&n.x);
    if (n.y < 0.5f) {
        return;
    }
    c.x = b->fAC.y * n.z - n.y * b->fAC.z;
    c.y = b->fAC.z * n.x - n.z * b->fAC.x;
    c.z = b->fAC.x * n.y - n.x * b->fAC.y;
    len = func_80013408(&c.x, &b->fA0.x);
    c.x = b->fA0.x * len;
    c.y = b->fA0.y * len;
    c.z = b->fA0.z * len;
    if (len == 0.0f) {
        return;
    }
    t = func_80013010((s32)(len * 16384.0f));
    q = (f32)(s16)t * 3.1415927f * 3.0517578e-05f;
    th = b->dt;
    th += 0.1f;
    qd = (q + q) / th;
    if (qd > 2.0f) {
        qd = 2.0f;
    } else if (qd < -2.0f) {
        qd = -2.0f;
    }
    qd = qd / len;
    b->rot.x = c.x * qd;
    b->rot.y = c.y * qd;
    b->rot.z = c.z * qd;
}
