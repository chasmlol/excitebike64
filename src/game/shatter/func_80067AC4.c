#include "ctx.h"

/* struct stride 0x30, field w at 0x2c */
typedef struct {
    s32 pad[11];
    s32 w;
} Ent30;

extern s32 D_800CCCE0;
extern Ent30 *D_800F1444;

void func_800BCB04(s32 a);

void func_80067AC4(void)
{
    s32 i;

    if (D_800CCCE0 != 0) {
        for (i = 0; i < D_800CCCE0; i++) {
            func_800BCB04(D_800F1444[i].w);
        }
        func_800BCB04((s32)D_800F1444);
        D_800CCCE0 = 0;
    }
}
