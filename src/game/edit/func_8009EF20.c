#include "ctx.h"

typedef struct {
    u8 pad0[0x174];
    u32 f174;
    u8 pad1[0x16E0 - 0x178];
    s32 cur;
    s32 e4;
    s32 state;
} Ctx;

extern Ctx *D_800CEA40;

void func_8009EF20(s32 a0)
{
    Ctx *st = D_800CEA40;
    if (st->cur != a0) {
        s32 s = st->state;
        if (s == 1) {
            st->cur = a0;
            goto tail;
        }
        if (s == 0) goto c0;
        if (s == 2) goto c2;
        if (s == 3) goto c3;
        goto tail;
c0:
        st->state = 1;
        st->e4 = a0;
        goto tail;
c2:
        st->state = 3;
        st->e4 = a0;
        goto tail;
c3:
        st->state = 1;
        st->cur = a0;
tail:
        D_800CEA40->f174 &= 0xFFFEFFFF;
    }
}
