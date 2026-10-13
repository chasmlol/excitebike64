#include "ctx.h"

extern void func_800BFEA4(u32 *p);
extern void func_800BCB04(void *p);

void func_800BFDD4(s32 *param_1)
{
    s32 i;
    s32 *pp;
    s32 *n;
    s32 *nx;

    i = 0;
    if (param_1[0] > 0) {
        do {
            func_800BFEA4((u32 *)(param_1[1] + i * 4));
            i = i + 1;
        } while (i < param_1[0]);
    }
    i = 0;
    pp = param_1;
    do {
        n = (s32 *)pp[2];
        while (n != 0) {
            nx = (s32 *)n[0];
            func_800BCB04((void *)n[1]);
            func_800BCB04(n);
            n = nx;
        }
        i = i + 1;
        pp = pp + 1;
    } while (i < 0x100);
    if ((param_1[0x145] & 1) == 0) {
        func_800BCB04(param_1);
    }
}
