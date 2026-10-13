#include "ctx.h"

extern u8 D_800F0C90[];
u32 func_8000EFE8(void);
void func_80064A8C(s32 arg0, s32 count);

void func_80064DF4(s32 arg0, s32 count)
{
    s32 i, k;

    for (i = 0; i < count; i++) {
        D_800F0C90[i] = (((s32)func_8000EFE8() * count) >> 8) + arg0;
        for (k = 0; k < i; k++) {
            if (D_800F0C90[k] == D_800F0C90[i]) {
                i--;
                break;
            }
        }
    }
    func_80064A8C(arg0, count);
}
