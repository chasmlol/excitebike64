#include "ctx.h"

typedef struct {
    s32 w0, w1, w2, w3, w4;
} Ent;

extern s32 D_800F14C0;
extern Ent *D_800F14D0;
extern s32 D_800F14D4;
extern s32 D_800F14E4;
extern void func_800BCB04(s32 a);

void func_8006A944(void)
{
    s32 i;

    for (i = 0; i < D_800F14C0; i++) {
        func_800BCB04(D_800F14D0[i].w0);
    }
    D_800F14D4 = 0;
    D_800F14E4 = 0;
    D_800F14C0 = 0;
    func_800BCB04((s32)D_800F14D0);
}
