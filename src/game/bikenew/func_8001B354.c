#include "ctx.h"

typedef struct {
    u8 pad0[0x45];
    u8 b45;
    u8 pad46[0x48 - 0x46];
    Vec3f pos;      /* 0x48 */
    Vec3f pos2;     /* 0x54 */
    u8 pad60[0xAC - 0x60];
    Vec3f dir;      /* 0xAC */
    Vec3f vel;      /* 0xB8 */
    u8 padC4[0x43C - 0xC4];
    Vec3f save;     /* 0x43C */
    u8 pad448[0x458 - 0x448];
    s32 n458;
} Bike;

typedef struct {
    s32 w0;
    u8 pad4[0x24 - 0x4];
    f32 f24;
    f32 f28;
    f32 f2C;
    u8 pad30[0x40 - 0x30];
    Vec3f prev;     /* 0x40 */
    Vec3f d1;       /* 0x4C */
    Vec3f d2;       /* 0x58 */
    u8 pad64[0x6C - 0x64];
    Vec3f n;        /* 0x6C */
    u8 pad78[0x90 - 0x78];
    u8 *p90;
    s32 n94;
} Wheel;

extern s32 D_800E80D4;
extern s32 D_800E8958;
extern s32 D_800E84F8;
extern s32 D_800E8508;
extern u8 D_800C9080[];

f32 func_80013408(f32 *a, f32 *b);
void func_8002F4B0(f32 *param_1);
s32 func_8002E920(s32 x, s32 y, s32 z);
f32 func_8001B05C(Bike *o, f32 *p, f32 p3);

void func_8001B354(Bike *b, Wheel *w, f32 dt, u8 flag)
{
    f32 P[3];
    Vec3f Q;
    f32 A[4];
    f32 L;
    f32 t, dif, base, thr, k, ts, ds;
    s32 i;

    D_800E80D4 = w->prev.y * 1000.0f;
    P[0] = b->pos.x + b->vel.x * dt;
    P[1] = b->pos.y + b->vel.y * dt;
    P[2] = b->pos.z + b->vel.z * dt;
    t = w->f28;
    D_800E8958 = w->w0;
    Q.x = P[0] + t * b->dir.x;
    base = P[1] + t * b->dir.y;
    Q.z = P[2] + t * b->dir.z;
    i = 0;
    Q.y = b->pos.y;
    L = (f32)func_8002E920(Q.x * 1000.0f, Q.y * 1000.0f, Q.z * 1000.0f) / 1000.0f;
    dif = L - base;
    thr = 0.002f;
    k = -1.0f;
    if (flag) thr = 0.1f;
    if (b->n458 > 0 && (dif < 0.0f || 0.5f < t)) thr = 0.05f;
    while ((0.0f < dif) ? (thr < dif) : (thr < -dif)) {
        ts = t;
        ds = dif;
        t -= k * dif;
        if (t < -32.0f || 8.0f < t) {
            t = func_8001B05C(b, P, thr);
            break;
        }
        Q.x = P[0] + t * b->dir.x;
        base = P[1] + t * b->dir.y;
        Q.z = P[2] + t * b->dir.z;
        L = (f32)func_8002E920(Q.x * 1000.0f, Q.y * 1000.0f, Q.z * 1000.0f) / 1000.0f;
        dif = L - base;
        if (i++ >= 9) {
            if (i >= 17) {
                t = w->f28 - 0.5f;
                break;
            }
            func_8002F4B0(A);
            if (-0.2f < func_80013408(A, &b->dir.x)) {
                k = (t - ts) / ((dif - ds) * 2.0f);
            } else {
                k = -1.0f;
            }
        } else {
            k = (t - ts) / (dif - ds);
        }
    }
    t += 0.5f;
    Q.y = L;
    if (D_800E8508) {
        w->p90 = D_800C9080 + ((D_800E84F8 << 2) & 0x3C0);
        w->n94 = D_800E84F8;
        w->w0 = D_800E8958;
        func_8002F4B0(&w->n.x);
        if (0.25f < w->f2C && 0.5f <= w->n.y && i >= 17) {
            w->f2C = 0.0f;
            b->b45 = 0x19;
            b->save = b->pos2;
            return;
        }
    } else {
        w->n.x = w->n.z = 0.0f;
        w->n.y = 1.0f;
    }
    if (w->f28 < w->f24) w->f2C = t - w->f24;
    else w->f2C = t - w->f28;
    w->f28 = t;
    w->d1.x = Q.x - b->pos.x;
    w->d1.y = Q.y - b->pos.y;
    w->d1.z = Q.z - b->pos.z;
    w->d2.x = Q.x - w->prev.x;
    w->d2.y = Q.y - w->prev.y;
    w->d2.z = Q.z - w->prev.z;
    w->prev = Q;
}
