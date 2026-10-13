#include "ctx.h"

extern s32 D_800F55A4;
extern s32 D_800F5574;
extern s32 D_800F5578;
extern s32 D_800F4F88;
extern s32 *D_800CE1F0[];
extern u8 *D_800CEA40;
void func_800C0258(s32 a0);
s32 func_800BF79C(s32 a0, s32 a1);

void func_8008E76C(s32 param_1, s32 param_2, s32 param_3)
{
    s32 cur;
    s32 *w;
    s32 *q;
    s32 *slot;
    u8 *p;

    cur = D_800F55A4;
    D_800F5574 = param_2;
    D_800F5578 = param_3;
    if (cur != param_1) {
        if (cur == -1) {
            D_800F4F88 = func_800BF79C(*(s32 *)(D_800CEA40 + 0x1720), *D_800CE1F0[param_1]);
        } else {
            func_800C0258(D_800F4F88);
            func_800BF79C(*(s32 *)(D_800CEA40 + 0x1720), *D_800CE1F0[param_1]);
            func_800C0258(0);
            p = D_800CEA40;
            *(s32 *)(((s32 *)*(s32 *)(p + 0x1720))[1] + *D_800CE1F0[D_800F55A4] * 4) = 0;
            w = D_800CE1F0[param_1];
            q = (s32 *)*(s32 *)(p + 0x1720);
            slot = (s32 *)(*w * 4 + q[1]);
            *slot = *slot & 0xFFFFFFFC;
        }
        D_800F55A4 = param_1;
    }
}
