#include "ctx.h"

void func_800BC3D0(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

void func_8000ECC4(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    func_800BC3D0(arg0, arg1, arg2, arg3);
    __asm__ volatile("break 0x407");
    while (1) {
    }
}
