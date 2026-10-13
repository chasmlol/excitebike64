#include "ctx.h"

typedef struct {
    u8 pad0[0x6C];
    f32 f6c;
    u8 pad1[4];
    f32 f74;
} Obj;

typedef struct {
    s32 w0, w4, w8, wC, w10, w14;
} Out;

extern void func_8002B348(Out *o);

void func_8002D528(Obj *o, Out *out) {
    out->w0 = o->f6c * 1000.0f + -28000.0f;
    out->w4 = 0x5BCC;
    out->w8 = o->f74 * 1000.0f - 10000.0f;
    out->wC = 0x1800;
    out->w14 = -0x4000;
    out->w10 = 0;
    func_8002B348(out);
}
