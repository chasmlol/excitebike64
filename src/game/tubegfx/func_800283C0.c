#include "ctx.h"

extern s32 D_800E76D0;
extern s32 D_800E78AC;
extern s32 D_800C2358;
extern s32 D_800E1B70;
extern s32 D_800E77F8;
extern s32 D_800E7824;
extern s32 D_800C8C74;
void func_800147C4(s32, s32);

void func_800283C0(void)
{
    volatile s32 pad[10];
    s32 v = D_800E76D0 / 2;
    if (v == D_800E78AC) return;
    D_800E78AC = v;
    if (D_800C2358 != 0 && D_800E1B70 == 0x100) return;
    if (D_800E77F8 != 0) {
        func_800147C4(D_800E77F8, v);
    }
    if (D_800E7824 != 0) {
        D_800C8C74 = 1;
        func_800147C4(D_800E7824, D_800E78AC);
        D_800C8C74 = 0;
    }
}
