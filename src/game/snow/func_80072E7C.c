#include "ctx.h"

typedef struct {
    s32 pad[4];
    s32 f10;
    s32 f14;
} Ent;

extern s32 D_800F21B0;
extern Ent D_800F21B8[];
extern void func_800BCB04(s32 a0);

void func_80072E7C(void)
{
    s32 i = 0;
    if (0 < D_800F21B0) {
        do {
            if (D_800F21B8[i].f10 != 0) {
                func_800BCB04(D_800F21B8[i].f14);
            }
            i++;
        } while (D_800F21B0 > i);
    }
}
