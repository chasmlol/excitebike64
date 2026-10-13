#include "ctx.h"

extern u8 D_800DB4D0[];
u8 *func_800C1F04(u8 *p);
s32 *func_800BD730(s32 a0, s32 a1, s32 a2, s32 a3);

s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5)
{
    s32 t;

    if (param_2 == 0) {
        param_2 = 0x40;
    }
    t = (param_2 + 0xF) & -0x10;
    if (param_1 == 0) {
        param_1 = func_800BD730(t + 0x20, 4, (s32)D_800DB4D0, 0x22A);
        if (param_1 == 0) {
            return 0;
        }
    }
    if (param_3 != 0) {
        param_1[0] = param_3;
    }
    if (param_4 != 0) {
        param_1[1] = param_4;
    }
    if (param_5 != 0) {
        param_1[2] = param_5;
    }
    func_800C1F04((u8 *)param_1);
    return param_1;
}
