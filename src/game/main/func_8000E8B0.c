#include "ctx.h"

extern u8 D_800E2138[];
extern s32 D_800C2930;
extern void func_800ACD70(void *a, u32 b, s32 c);

void func_8000E8B0(s32 param_1)
{
    u8 *p = D_800E2138;
    s32 flag = 0;
    if (param_1 != 0) {
        flag = -1;
    }
    func_800ACD70(p, 0xFF70000, flag);
    D_800C2930 = param_1;
}
