#include "ctx.h"

void boot_func_80001DD0(u32 addr, u32 val)
{
    u32 sh = (~addr & 3) << 3;
    u32 *p = (u32 *)(addr & ~3u);
    *p = (*p & ~(0xFFu << sh)) | ((val & 0xFFu) << sh);
}
