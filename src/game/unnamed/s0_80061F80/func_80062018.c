#include "ctx.h"

typedef struct {
    u8 pad[0x42C];
    f32 f42C;
    u8 pad2[0x660 - 0x430];
    s16 x660;
} Obj;

#define ABS(x) ((x) > 0.0f ? (x) : -(x))

s32 func_80062018(Obj *p, s16 s) {
    return p->x660 < s || ABS(p->f42C) > 0.5f;
}
