#include "ctx.h"

s32 boot_func_80001B50(void);
s32 boot_func_80002808(u8 *arg0);
void boot_func_80002010(void *arg0, u8 *arg1, s32 arg2);
s32 boot_func_80002828(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void boot_func_80001B88(s32 arg0);

s32 boot_func_800029BC(u8 *arg0, s32 arg1) {
    s32 mask;
    s32 len;
    s32 ret;

    mask = boot_func_80001B50();
    len = boot_func_80002808(arg0);
    boot_func_80002010((void *)0xB1FF0000, arg0, len);
    ret = boot_func_80002828(0x303, len, arg1, 0);
    if (ret & 0x8000) {
        ret |= 0xFFFF0000;
    }
    boot_func_80001B88(mask);
    return ret;
}
