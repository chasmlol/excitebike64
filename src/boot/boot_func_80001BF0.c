#include "ctx.h"

u32 boot_func_80001BF0(u32 *param_1) {
    while ((*(volatile u32 *)0xA4600010 & 3) != 0) {
    }
    return *param_1;
}
