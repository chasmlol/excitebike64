#include "ctx.h"

extern s32 D_800FE8F0[];
extern s32 *D_800D5604;
void func_8000ED10(u8 *a0, u8 a1, s32 a2);
void func_800BFEA4(u32 *p);
void func_800BCB04();

void func_800BB4EC(u32 p)
{
    s32 *w;
    s32 *e;
    if (p < 0x101) {
        w = D_800FE8F0;
        e = &w[p * 7];
        if (e[1] != 0) {
            if (e[6] != -1) {
                func_800BFEA4((u32 *)(D_800D5604[1] + e[6] * 4));
            } else {
                func_800BCB04(e[0]);
            }
            func_8000ED10((u8 *)&D_800FE8F0[p * 7], 0, 0x1c);
        }
    }
}
