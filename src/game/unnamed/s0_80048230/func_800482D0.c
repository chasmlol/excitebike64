#include "ctx.h"

extern s32 D_800CA770;
extern u8 D_800CA784[];
extern u8 D_800CA79C[];

void func_8000ECB8(s32) __attribute__((noreturn));
u32 func_800C1DEC(void);
s32 func_800C06D0(void *a, void *b, void *c);

void func_800482D0(u32 *param_1)
{
    if (D_800CA770 != 1) {
        if (D_800CA770 != 0 && func_800C1DEC() != 0) {
            func_8000ECB8(1);
        }
        if (func_800C06D0(param_1, D_800CA784, D_800CA79C) != 0) {
            func_8000ECB8(1);
        }
        D_800CA770 = 1;
    }
}
