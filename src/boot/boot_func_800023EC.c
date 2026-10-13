#include "ctx.h"

u32 boot_func_80001BF0(u32 a);
void boot_func_80001BB8(u32 a, u32 v);
void boot_func_80001C2C(s32 ms);

void boot_func_800023EC(void)
{
    u8 buf[8];
    u32 *src;
    u32 size;
    u32 i;

    src = (u32 *)(boot_func_80001BF0(0xB1FFFFF4) & 0xB1FFFFFC);
    size = boot_func_80001BF0(0xB1FFFFF8) & 0x01FFFFFC;
    boot_func_80001BB8(0xB1FFFFFC, 0);
    while (boot_func_80001BF0(0xB0000010) == 0) {
        boot_func_80001C2C(500);
    }
    for (i = 0; i < size >> 2; i++) {
        boot_func_80001BB8((u32)&((u32 *)((u32)src & 0xB07FFFFF))[i], boot_func_80001BF0((u32)&src[i]));
    }
    boot_func_80001C2C(2000);
    boot_func_80001BB8(0xB1FFFFF4, 0);
}
