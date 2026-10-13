#include "ctx.h"

extern s32 *D_800C2354;
extern u8 D_800D6170[];
extern s32 D_800E7728;
extern s32 D_800D088C;
extern s32 D_800D09B0;
extern s32 D_800D085C;
extern s32 D_800EFDBC;
extern s32 *D_800CA930;
extern s32 D_800C8D68[];

s32 *func_800BC908(s32 a, s32 b, void *c, s32 d);
s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
s32 func_800BF79C(s32 a, s32 b);

void func_800166CC(void)
{
    s32 i;
    s32 r;

    D_800C2354 = func_800BC908(0x148, 0, D_800D6170, 0x52C);
    i = 0;
    do {
        *(s32 *)((u8 *)D_800C2354 + (i << 2)) = (s32)func_800C1E60((s32 *)0, 0x40, 1, 2, 0x20);
        i = i + 1;
    } while (i < 0x4a);
    r = func_800BF79C(D_800E7728, D_800D088C);
    D_800C2354[0x4a] = r;
    r = func_800BF79C(D_800E7728, D_800D09B0);
    D_800C2354[0x50] = r;
    if (D_800EFDBC == 1) {
        i = 0;
        do {
            r = func_800BF79C(D_800E7728, *(s32 *)D_800C8D68[*(s16 *)((u8 *)*(s16 **)D_800CA930 + (i << 1) + 8)]);
            *(s32 *)((u8 *)D_800C2354 + (i << 2) + 0x12C) = r;
            i = i + 1;
        } while (i < 5);
    }
    r = func_800BF79C(D_800E7728, D_800D085C);
    D_800C2354[0x51] = r;
}
