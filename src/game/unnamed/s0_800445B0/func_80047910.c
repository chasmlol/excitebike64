#include "ctx.h"

typedef struct {
    u8 pad0[0x3AC];
    s32 w3ac;
    u8 pad1[0x3CC - 0x3B0];
    s32 w3cc;
    u8 b3d0;
    u8 b3d1;
} Obj;

void func_8006FE84(void *a, s32 b);

void func_80047910(Obj *o, s32 b) {
    o->b3d1 = o->b3d0;
    o->w3cc = o->w3ac;
    func_8006FE84(o, b);
}
