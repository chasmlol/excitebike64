#include "ctx.h"

extern s32 *D_800CE164;

s32 *func_800C1E60(s32 *param_1, s32 param_2, s32 param_3, s32 param_4, s32 param_5);
void func_800C1F2C(s32 *a, void *b);
void func_800C0ACC(s32 *a, s32 b, s32 c);
void func_800C1394(void *a);

void func_8008C9B8(void *param_1, void *param_2, s32 param_3, s32 param_4, s32 param_5, s32 param_6)
{
    func_800C1E60(D_800CE164, 0x80, param_3, param_4, 0);
    func_800C1F2C(D_800CE164, param_2);
    func_800C0ACC(D_800CE164, param_5 - D_800CE164[6], param_6);
    func_800C1394(param_1);
}
