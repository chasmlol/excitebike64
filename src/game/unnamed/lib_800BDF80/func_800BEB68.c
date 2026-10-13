#include "ctx.h"

extern s32 D_8010069C;
extern s32 D_801009A4;
void func_800AD7E0(void *a, s32 b);
void func_800AEC34(void *a, void *b, s32 c);
s32 func_800BEC20(s32 d, s32 e, void *f, void *g, s32 h);
void func_800BEFB4(s32 x);

void func_800BEB68(void *param_1, void *param_2, s32 param_3)
{
    s32 r;
    func_800AD7E0(param_2, param_3);
    if ((D_8010069C & 0xF0000000) == 0x80000000) {
        func_800AEC34(param_2, param_1, param_3);
    } else {
        do {
            r = func_800BEC20(D_801009A4, 0, param_1, param_2, param_3);
        } while (r < 0);
        func_800BEFB4(r);
        func_800AD7E0(param_2, param_3);
    }
}
