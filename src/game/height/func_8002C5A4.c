#include "ctx.h"

typedef struct {
    u8 pad0[0x14];
    f32 fa;
    f32 fb;
    u8 pad1[0x50];
} Rec;

extern s32 D_800EE490;
extern Rec *D_800EE498;
void func_8001254C(s32 x, s32 y, u16 *p, s32 *q);

void func_8002C5A4(s16 p1, f32 p2, u16 *p3)
{
    s32 i1, i2;
    s32 aux[2];
    f32 xa, xb;
    if (0.0f <= p2) {
        i2 = p1;
        i1 = i2 + 1;
        if (i1 >= D_800EE490) i1 -= D_800EE490;
    } else {
        s32 t = p1;
        i2 = t - 1;
        i1 = t;
        if (i2 < 0) i2 += D_800EE490;
        p2 = p2 + 1.0f;
    }
    xb = (D_800EE498[i2].fb * (1.0f - p2) + D_800EE498[i1].fb * p2) * 16384.0f;
    xa = (D_800EE498[i2].fa * (1.0f - p2) + D_800EE498[i1].fa * p2) * 16384.0f;
    func_8001254C((s32)xb, (s32)xa, p3, aux);
}
