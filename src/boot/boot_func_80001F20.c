#include "ctx.h"

u32 boot_func_80001BF0(void *p);
void boot_func_80001BB8(void *p, u32 v);

void boot_func_80001F20(u32 param_1, u8 param_2) {
    u32 shift;
    u32 v;

    shift = (~param_1 & 3) << 3;
    param_1 = param_1 & 0xFFFFFFFC;
    v = boot_func_80001BF0((void *)param_1);
    boot_func_80001BB8((void *)param_1, (v & ~(0xFFU << shift)) | ((u32)param_2 << shift));
}
