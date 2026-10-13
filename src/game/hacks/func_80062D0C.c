#include "ctx.h"

extern u8 *D_800F0B58;

void func_80062D0C(void)
{
    if (D_800F0B58 != 0) {
        ((void (*)(void))(*(u32 *)(D_800F0B58 + 0xC)))();
    }
}
