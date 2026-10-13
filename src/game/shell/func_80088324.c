#include "ctx.h"

typedef struct {
    u8 pad0[0x4C];
    s32 f4C;
    u8 pad1[0x410 - 0x50];
    s32 f410;
} State;

typedef struct {
    u8 pad0[0x14];
    s16 h14;
} Obj;

extern State D_800EFD70;
extern u8 D_800F1BC0;

s32 func_80088324(Obj *o)
{
    s32 mode;
    s32 t;

    if (D_800EFD70.f4C == 8 || D_800EFD70.f4C == 0) {
        mode = D_800F1BC0;
    } else {
        mode = D_800EFD70.f410;
    }
    if (mode == 2) {
        goto two;
    }
    if (mode < 3) {
        if (mode == 1) {
            goto one;
        }
        goto ret0;
    }
    if (mode == 3) {
        goto ret1;
    }
    goto ret0;
one:
    t = o->h14 < 10;
tail:
    if (t == 0) {
        goto ret0;
    }
ret1:
    return 1;
two:
    t = o->h14 < 0xF;
    goto tail;
ret0:
    return 0;
}
