#include "ctx.h"

extern u32 D_800C29E0;
extern u8 D_800D6154[];

u32 func_80013D70(u32 a0, u32 *a1, s32 a2, u32 a3);
void *func_800BC908(u32 a0, u32 a1, u8 *a2, u32 a3);

void func_80014AE4(void **param_1, u32 *param_2, s32 param_3, u32 param_4)
{
    u32 r;
    u32 saved;

    r = func_80013D70(param_4 | 0x21, param_2, param_3, D_800C29E0);
    *param_1 = func_800BC908(r - D_800C29E0, 0, D_800D6154, 0x323);
    saved = param_2[1];
    param_2[1] = 0x01000000;
    func_80013D70(param_4 | 0x21, param_2, param_3, (u32)*param_1);
    param_2[1] = saved;
}
