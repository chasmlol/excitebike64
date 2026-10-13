#include "ctx.h"

typedef struct {
    u8 pad[0x3C];
    u32 v;
} Blk;

extern Blk D_800CCB90[];
extern u32 D_800F0B48[];
void func_800BCB04(u32 x);

void func_80062ED0(void) {
    s32 i = 0;
    Blk *p = D_800CCB90;
    u32 *q;

    do {
        u32 v = p->v;
        p++;
        i++;
        func_800BCB04(v);
    } while (i < 2);

    i = 0;
    q = D_800F0B48;
    do {
        u32 v = *q;
        q++;
        i++;
        func_800BCB04(v);
    } while (i < 4);
}
