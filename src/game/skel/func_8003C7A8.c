#include "ctx.h"

extern void func_80011A58(void *a, s32 b, s32 c, s32 d);
extern void func_8003C4A4(s32 *p, s32 n);

void func_8003C7A8(s32 *param_1, s32 param_2)
{
    s32 i;
    s32 *p;

    i = 0;
    if (param_2 > 0) {
        p = param_1;
        do {
            func_80011A58(p + 4, 0, 0, 0);
            func_80011A58(p + 0x10, 0, 0, 0);
            i++;
            p += 0x20;
        } while (i < param_2);
    }
    func_8003C4A4(param_1, param_2);
}
