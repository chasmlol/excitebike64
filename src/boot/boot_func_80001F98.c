#include "ctx.h"

u32 boot_func_80001BF0();
void boot_func_80001BB8();

void boot_func_80001F98(u32 addr, u8 val) {
    u32 sh;
    u32 v;

    sh = (~addr & 3) << 3;
    addr = addr & 0xFFFFFFFC;
    v = boot_func_80001BF0(addr);
    boot_func_80001BB8(addr, (v & ~(0xFFu << sh)) | (val << sh));
}
