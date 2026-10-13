#include "ctx.h"

typedef struct {
    u8 pad[0x42C];
    f32 f42C;
    u8 pad2[0x638 - 0x430];
    f32 f638;
    u8 pad3[0x6E8 - 0x63C];
    u8 b6E8;
} Obj;

s32 func_80064FE0(Obj **p, f32 x)
{
    f32 d, k;
    Obj *o;

    do {
        o = *p;
        if (o->b6E8 != 0) {
            d = o->f42C;
            k = 0.999f;
            d = d - x;
        } else {
            d = o->f638;
            k = 0.999f;
            d = d - x;
        }
        p++;
        if (d < k && -k < d) {
            return 1;
        }
    } while (*p != 0);
    return 0;
}
