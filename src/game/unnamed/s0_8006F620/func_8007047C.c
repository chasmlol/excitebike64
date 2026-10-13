#include "ctx.h"

typedef struct {
    u8 pad0[0x3BA];
    s16 f3ba;
    u8 pad1[0x3C4 - 0x3BC];
    s32 f3c4;
    u8 pad2[0x3D0 - 0x3C8];
    u8 f3d0;
    u8 pad3[0x3E4 - 0x3D1];
    u8 f3e4[4];
} Obj;

typedef struct {
    u8 pad[0x20];
    s32 f20;
    u8 rest[0x3C - 0x24];
} Ent;

extern Ent D_800C9A40[];

void func_8007047C(Obj *o) {
    s32 base;
    s8 idx;
    s32 k;
    s32 t;
    if (o->f3d0 >= 16) {
        o->f3c4 = 0;
        return;
    }
    idx = (s8)o->f3d0;
    base = D_800C9A40[idx].f20;
    base += o->f3ba * 2;
    for (k = 0; k < 4; k++) {
        if (idx == o->f3e4[k]) {
            break;
        }
    }
    base = base * (k + 4);
    t = base;
    if (base < 0) {
        t = base + 7;
    }
    base = t >> 3;
    o->f3c4 += base;
}
