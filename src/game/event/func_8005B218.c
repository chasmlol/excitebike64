#include "ctx.h"

typedef struct {
    s32 state;
    u8 pad[0x1E];
    s16 count;
    s16 f24;
} Ctl;

extern Ctl D_800F05A8;
extern s16 D_800E3E20[];
void func_80064D28(s32 a, s32 b);

void func_8005B218(void) {
    Ctl *c = &D_800F05A8;
    s32 i;
    s16 *t;

    if (c->state == 8) {
        func_80064D28(2, 0);
    }
    i = 0;
    c->f24 = 0;
    if (c->count > 0) {
        s16 neg = -1;
        Ctl *d = c;
        t = D_800E3E20;
        do {
            t[4] = neg;
            t += 0x3F0;
        } while (++i < d->count);
    }
}
