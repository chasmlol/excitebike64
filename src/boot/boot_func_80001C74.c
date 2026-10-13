#include "ctx.h"

u32 boot_func_80001BF0(u32 addr);

u32 boot_func_80001C74(u32 addr)
{
    u32 sh = (~addr & 3) << 3;
    u32 v = boot_func_80001BF0(addr & ~3u);
    return (v >> sh) & 0xFF;
}
