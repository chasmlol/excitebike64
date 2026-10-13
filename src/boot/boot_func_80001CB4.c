#include "ctx.h"

u32 boot_func_80001BF0(void *p);

u32 boot_func_80001CB4(u32 a) {
    u32 v;
    u32 sh;

    sh = (~a & 3) << 3;
    v = boot_func_80001BF0((void *)(a & ~3));
    return (v >> sh) & 0xFF;
}
