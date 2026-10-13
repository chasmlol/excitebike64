#include "ctx.h"

typedef struct {
    u8 pad0[0x5B0];
    u32 flags;      /* 0x5B0 */
    u8 pad1[0x648 - 0x5B4];
    f32 val;        /* 0x648 */
    u8 pad2[0x654 - 0x64C];
    u8 stopped;     /* 0x654 */
    u8 pad3[0x7C0 - 0x655];
    f32 div;        /* 0x7C0 */
} Obj;

extern f32 D_800C9954[2][3];

static inline s32 sel(Obj *o)
{
    u32 t;

    if (o->flags & 0x20000) {
        return 2;
    }
    t = o->flags & 0x10;
    return t != 0;
}

void func_80035A84(Obj *o)
{
    s32 h;
    s32 g;
    f32 f;

    u32 t;

    g = sel(o);
    h = 0;
    if (o->val > 7000.0f) {
        h = 1;
    }
    if (o->stopped) {
        h = g = 0;
    }
    f = D_800C9954[h][g];
    if ((o->flags & 0x20000) && f > 0.0f) {
        f = f / o->div;
    }
    o->val = o->val + f;
    if (o->val < 0.0f) {
        o->val = 0.0f;
    }
    if (o->val > 12000.0f) {
        o->val = 12000.0f;
    }
    if (o->stopped == 0) {
        if (o->val >= 12000.0f) {
            o->stopped = 1;
            o->val = 12000.0f;
        }
    } else if (o->val <= 7000.0f) {
        o->stopped = 0;
    }
}
