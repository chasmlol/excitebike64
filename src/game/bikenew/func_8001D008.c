#include "ctx.h"

typedef struct {
    u8 pad0[0x1BC];
    f32 f1BC;
    u8 pad1C0[0x424 - 0x1C0];
    f32 speed;
    u8 pad428[0x448 - 0x428];
    f32 out;
    u8 pad44C[0x458 - 0x44C];
    s32 f458;
    u8 pad45C[0x498 - 0x45C];
    f32 f498;
    u8 pad49C[0x4FC - 0x49C];
    s32 f4FC;
    u8 pad500[0x53C - 0x500];
    f32 f53C;
    u8 pad540[0x5C0 - 0x540];
    s32 state;
    u8 pad5C4[0x5D2 - 0x5C4];
    s8 f5D2;
    u8 pad5D3[0x64C - 0x5D3];
    f32 f64C;
    u8 pad650[0x7D8 - 0x650];
    f32 f7D8;
} Bike;

extern s32 D_800EFDBC;

void func_8001D008(Bike *p)
{
    f32 a, b, t, x;

    a = p->f64C;
    b = p->speed;
    if (a < 0.0f || (p->state >= 1 && p->state <= 4)) {
        if (0.0f < b) {
            p->out = p->f1BC * -2.0f;
        } else {
            p->out = 0.0f;
        }
        return;
    }
    if (b <= 1.0f) {
        b = 1.0f;
    }
    t = (a * p->f7D8) / b;
    p->out = t;
    if (p->f4FC < 0 && p->f458 < 0 && b < 10.0f) {
        x = p->f53C - p->f498;
        if (0.15f < x) {
            if (D_800EFDBC == 7) {
                if (p->f5D2 >= 0) {
                    return;
                }
                p->out = t * (1.0f - (x - 0.15f) * (p->f5D2 * 0.125f));
            } else {
                p->out = t * ((x - 0.15f) * 2.0f + 1.0f);
            }
        }
    }
}
