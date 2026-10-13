#include "ctx.h"

typedef struct { u8 pad[2]; u8 flags; u8 pad2[5]; } Ent;
typedef struct { u32 count; u32 pad; Ent e[1]; } Tbl;
extern Tbl *D_800E77F8;
extern s32 *D_800E7830;
extern s32 *D_800E782C;
extern s32 D_800C29E0;

void func_80027AB4(void)
{
    s32 i, start, k;
    Tbl *t;

    D_800E7830 = (s32 *)(D_800C29E0 + 0x18000);
    D_800E782C = D_800E7830 + D_800E77F8->count;
    start = 0;
    for (i = 0; i < D_800E77F8->count; i++) {
        if (!(D_800E77F8->e[i].flags & 0x34)) {
            for (k = start; k <= i; k++) {
                D_800E7830[k] = start;
                D_800E782C[k] = i - start + 1;
            }
            start = i + 1;
        }
    }
}
