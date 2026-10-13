#include "ctx.h"

typedef struct {
    u8 pad0[0x419];
    u8 b419;
    u8 b41a;
    u8 b41b;
    u8 pad1[4];
    f32 f420;
    u8 pad2[0x5B0 - 0x424];
    u32 w5b0;
    u8 pad3[4];
    s32 w5b8;
    u8 pad4[0x5CF - 0x5BC];
    u8 b5cf;
} Obj;

extern s32 D_800E76D0;

void func_80022B50(Obj *o) {
    o->b5cf = 0;
    if (5.0f < o->f420) {
        if (o->b41b != 0 || ((o->w5b0 & 0x20000) != 0 && (D_800E76D0 - o->w5b8 < 0x1E))) {
            o->b5cf = 2;
            return;
        }
        if ((*(volatile u8 *)&o->b41a | *(volatile u8 *)&o->b419) != 0) {
            o->b5cf = 1;
        }
    }
}
