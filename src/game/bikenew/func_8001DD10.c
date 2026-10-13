#include "ctx.h"

typedef struct Bike {
    /* 0x000 */ u8 pad0[0x54];
    /* 0x054 */ Vec3f pos;
    /* 0x060 */ u8 pad60[0x88 - 0x60];
    /* 0x088 */ Vec3f v88;
    /* 0x094 */ u8 pad94[0xA0 - 0x94];
    /* 0x0A0 */ Vec3f mA0;
    /* 0x0AC */ Vec3f mAC;
    /* 0x0B8 */ Vec3f mB8;
    /* 0x0C4 */ u8 padC4[0x1BC - 0xC4];
    /* 0x1BC */ f32 f1BC;
    /* 0x1C0 */ u8 padC0[0x416 - 0x1C0];
    /* 0x416 */ s16 s416;
    /* 0x418 */ u8 pad418[0x448 - 0x418];
    /* 0x448 */ f32 f448;
    /* 0x44C */ u8 pad44C[0x458 - 0x44C];
    /* 0x458 */ s32 n458;
    /* 0x45C */ Vec3f v45C;
    /* 0x468 */ f32 f468;
    /* 0x46C */ u8 pad46C[0x4A0 - 0x46C];
    /* 0x4A0 */ Vec3f v4A0;
    /* 0x4AC */ u8 pad4AC[0x4BC - 0x4AC];
    /* 0x4BC */ f32 f4BC;
    /* 0x4C0 */ Vec3f v4C0;
    /* 0x4CC */ Vec3f v4CC;
    /* 0x4D8 */ Vec3f v4D8;
    /* 0x4E4 */ u8 pad4E4[0x4FC - 0x4E4];
    /* 0x4FC */ s32 n4FC;
    /* 0x500 */ Vec3f v500;
    /* 0x50C */ f32 f50C;
    /* 0x510 */ u8 pad510[0x544 - 0x510];
    /* 0x544 */ Vec3f v544;
    /* 0x550 */ u8 pad550[0x560 - 0x550];
    /* 0x560 */ f32 f560;
    /* 0x564 */ Vec3f v564;
    /* 0x570 */ Vec3f v570;
    /* 0x57C */ Vec3f v57C;
    /* 0x588 */ u8 pad588[0x5A0 - 0x588];
    /* 0x5A0 */ f32 f5A0;
    /* 0x5A4 */ u8 pad5A4[0x5D0 - 0x5A4];
    /* 0x5D0 */ s8 c5D0;
    /* 0x5D1 */ u8 pad5D1[0x7D0 - 0x5D1];
    /* 0x7D0 */ f32 f7D0;
} Bike;

extern f32 D_800E76C8;
f32 func_800ADFC0(f32);
f32 func_800AEA30(f32);
f32 func_80013408(f32 *a, f32 *b);
void func_8001327C(f32 *v, f32 *m, f32 *o);

#define CROSS(r, a, b) \
    (r).x = (a).y * (b).z - (b).y * (a).z; \
    (r).y = (a).z * (b).x - (b).z * (a).x; \
    (r).z = (a).x * (b).y - (b).x * (a).y

#define NORM(v) { \
    f32 len = __builtin_sqrtf((v).x * (v).x + (v).y * (v).y + (v).z * (v).z); \
    if (len != 0.0f) { (v).x /= len; (v).y /= len; (v).z /= len; } }

void func_8001DD10(Bike *b)
{
    Vec3f out, in, t, c1, c2, v, c3, c4, u, s, q, r, p, w;
    f32 ra, rb, k, A, B, C, D, r0, x, y, det, d, e, f, g;

    b->v45C.z = 0.0f;
    b->v45C.y = 0.0f;
    b->v45C.x = 0.0f;
    b->v500.z = 0.0f;
    b->v500.y = 0.0f;
    b->v500.x = 0.0f;
    in.x = func_800ADFC0(b->f5A0);
    in.y = 0.0f;
    in.z = func_800AEA30(b->f5A0);
    func_8001327C(&in.x, &b->mA0.x, &out.x);
    CROSS(t, out, b->v564);
    NORM(t);
    b->v57C = t;
    CROSS(b->v570, b->v564, t);
    CROSS(t, b->mA0, b->v4C0);
    NORM(t);
    b->v4D8 = t;
    CROSS(b->v4CC, b->v4C0, t);
    if (b->n4FC <= 0) {
        if (b->n458 <= 0) {
            p.x = b->pos.x + (b->v500.x + b->v45C.x) * (D_800E76C8 / b->f1BC);
            p.y = b->pos.y + (b->v500.y + b->v45C.y) * (D_800E76C8 / b->f1BC) - D_800E76C8 * 14.700001f;
            p.z = b->pos.z + (b->v500.z + b->v45C.z) * (D_800E76C8 / b->f1BC);
            CROSS(q, b->v544, b->v500);
            CROSS(r, b->v4A0, b->v45C);
            k = D_800E76C8 / 250.0f;
            w.x = b->v88.x + (q.x + r.x) * k;
            w.y = b->v88.y + (q.y + r.y) * k;
            w.z = b->v88.z + (q.z + r.z) * k;
            CROSS(s, w, b->v4A0);
            s.x += p.x;
            s.y += p.y;
            s.z += p.z;
            ra = func_80013408(&s.x, &b->v4CC.x);
            CROSS(u, w, b->v544);
            u.x += p.x;
            u.y += p.y;
            u.z += p.z;
            rb = func_80013408(&u.x, &b->v570.x);
            CROSS(t, b->v544, b->v570);
            d = func_80013408(&t.x, &b->mB8.x);
            t.x -= d * b->mB8.x;
            t.y -= d * b->mB8.y;
            t.z -= d * b->mB8.z;
            CROSS(c1, t, b->v544);
            CROSS(c2, t, b->v4A0);
            CROSS(v, b->v4A0, b->v4CC);
            d = func_80013408(&v.x, &b->mB8.x);
            v.x -= d * b->mB8.x;
            v.y -= d * b->mB8.y;
            v.z -= d * b->mB8.z;
            CROSS(c3, v, b->v544);
            CROSS(c4, v, b->v4A0);
            r0 = func_80013408(&b->v4CC.x, &b->v570.x);
            A = 1.0f / b->f1BC + func_80013408(&c1.x, &b->v570.x) / 250.0f;
            B = r0 / b->f1BC + func_80013408(&c3.x, &b->v570.x) / 250.0f;
            C = r0 / b->f1BC + func_80013408(&c2.x, &b->v4CC.x) / 250.0f;
            D = 1.0f / b->f1BC + func_80013408(&c4.x, &b->v4CC.x) / 250.0f;
            if (b->n458 > 0) {
                x = -rb / A;
                y = 0.0f;
            } else if (b->n4FC > 0) {
                x = 0.0f;
                y = -ra / D;
            } else {
                x = (B * ra - D * rb) / (A * D - B * C);
                y = (C * rb - A * ra) / (A * D - B * C);
            }
            y /= D_800E76C8;
            x /= D_800E76C8;
            b->f468 = y;
            b->f50C = x;
            b->v45C.x = b->v4CC.x * y + b->v4D8.x * (b->f448 - b->f4BC);
            b->v45C.y = b->v4CC.y * y + b->v4D8.y * (b->f448 - b->f4BC);
            b->v45C.z = b->v4CC.z * y + b->v4D8.z * (b->f448 - b->f4BC);
            b->v500.x = b->v570.x * x - b->v57C.x * b->f560;
            b->v500.y = b->v570.y * x - b->v57C.y * b->f560;
            b->v500.z = b->v570.z * x - b->v57C.z * b->f560;
        }
    } else if (b->n458 <= 0 && b->s416 != 0) {
        b->v45C.x = b->v4D8.x * (b->f448 - b->f4BC);
        b->v45C.y = b->v4D8.y * (b->f448 - b->f4BC);
        b->v45C.z = b->v4D8.z * (b->f448 - b->f4BC);
        e = func_80013408(&b->pos.x, &b->v4CC.x) * (b->f1BC / D_800E76C8);
        b->v45C.x -= b->v4CC.x * e;
        b->v45C.y -= b->v4CC.y * e;
        b->v45C.z -= b->v4CC.z * e;
        f = func_80013408(&b->pos.x, &b->v4D8.x);
        if (f > 0.0f) {
            g = 1.0f / (f + 20.0f);
        } else {
            g = 1.0f / (-f + 20.0f);
        }
        f = g * (b->f7D0 / 0.57f);
        b->v88.y += -(b->c5D0 * f) - b->v88.y * 0.5f;
        b->c5D0 *= 0.25f;
    }
}
