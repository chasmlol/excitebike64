#include "ctx.h"

typedef struct { f32 x, y, z; } V3;

typedef struct {
    u8 pad0[0x18];
    s32 ang[3];       /* 0x18 */
    u8 pad24[0x48 - 0x24];
    V3 pos;           /* 0x48 */
    u8 pad54[0x78 - 0x54];
    f32 mtx[10];      /* 0x78 */
    f32 rot[3];       /* 0xA0 */
    V3 up;            /* 0xAC */
    u8 padB8[0x1BC - 0xB8];
    f32 f1BC;
    u8 pad1C0[0x47C - 0x1C0];
    f32 f47C;
    u8 pad480[0x520 - 0x480];
    f32 f520;
} Bike;

extern s32 D_800EFDBC;
extern f32 D_800C8E14;

void func_80013568(f32 *out, f32 *in);
void func_80012250(f32 *a, s32 *b, s32 *c, s32 *d);
f32 func_8001C550(V3 *a, V3 *b, f32 c, f32 d, f32 *e);
void func_8001B808(Bike *b);
void func_80013438(f32 *out, f32 x, f32 y, f32 z);

void func_8001C780(Bike *b)
{
    f32 tmp[8];
    f32 d[7];
    f32 r;
    f32 df;
    s32 i;
    s32 a;

    b->ang[0] = 0;
    b->ang[1] = 0;
    func_80013568(b->rot, b->mtx);
    func_80012250(b->mtx, &b->ang[0], &b->ang[1], &b->ang[2]);
    r = func_8001C550(&b->pos, &b->up, 0.0f, 0.002f, tmp);
    d[0] = b->up.x * -((r + 0.5f) - b->f1BC * 14.700001f / (D_800EFDBC == 6 ? D_800C8E14 * 20000.0f + D_800C8E14 * 20000.0f : 30000.0f));
    d[1] = b->up.y * -((r + 0.5f) - b->f1BC * 14.700001f / (D_800EFDBC == 6 ? D_800C8E14 * 20000.0f + D_800C8E14 * 20000.0f : 30000.0f));
    d[2] = b->up.z * -((r + 0.5f) - b->f1BC * 14.700001f / (D_800EFDBC == 6 ? D_800C8E14 * 20000.0f + D_800C8E14 * 20000.0f : 30000.0f));
    b->pos.x -= d[0];
    b->pos.y -= d[1];
    b->pos.z -= d[2];
    i = 16;
    do {
        func_80012250(b->mtx, &b->ang[0], &b->ang[1], &b->ang[2]);
        func_80013568(b->rot, b->mtx);
        func_8001B808(b);
        a = (b->f520 - b->f47C) / 2.5f * 32768.0f / 3.1415927f;
        if (a > 0x1000) a = 0x1000;
        if (a < -0x1000) a = -0x1000;
        b->ang[0] -= a;
        func_80013438(b->mtx, (f32)b->ang[0] * 3.1415927f * 3.0517578e-05f,
                      (f32)b->ang[1] * 3.1415927f * 3.0517578e-05f,
                      (f32)b->ang[2] * 3.1415927f * 3.0517578e-05f);
        df = b->f520 - b->f47C;
    } while ((0.0f < df ? 0.02f < df : 0.02f < -df) && i++ < 16);
    func_8001B808(b);
    r = (((b->f520 + b->f47C) * 0.5f) - b->f1BC * 14.700001f / (D_800EFDBC == 6 ? D_800C8E14 * 20000.0f + D_800C8E14 * 20000.0f : 30000.0f));
    d[0] = b->up.x * -r;
    d[1] = b->up.y * -r;
    d[2] = b->up.z * -r;
    b->pos.x -= d[0];
    b->pos.y -= d[1];
    b->pos.z -= d[2];
}
