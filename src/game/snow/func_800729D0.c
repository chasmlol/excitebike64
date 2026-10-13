#include "ctx.h"

extern u32 D_800E7FF0;
extern u32 func_8000EFE8(void);

void func_800729D0(s32 *param_1)
{
    s32 x;
    u32 u;

    if (D_800E7FF0 & 0x40) {
        param_1[6] = 0;
    } else {
        param_1[6] = 1;
    }
    u = func_8000EFE8();
    param_1[2] = u * 0xc0 + 0x4000;
    u = func_8000EFE8();
    x = ((s32)u - 0x80) * param_1[2];
    if (x < 0) {
        x += 0x7f;
    }
    param_1[0] = x >> 7;
    u = func_8000EFE8();
    if (u & 1) {
        u = func_8000EFE8();
        x = ((s32)u - 0x80) * param_1[2];
        if (x < 0) {
            x += 0xff;
        }
        x = x >> 8;
    } else {
        x = param_1[2];
    }
    param_1[1] = x;
    u = func_8000EFE8();
    param_1[5] = u + 0x100;
}
